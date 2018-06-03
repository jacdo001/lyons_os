#include "lyons/vmem_manager.h"
#include "lyons/pmem_manager.h"
#include "lyons/kernel.h"
#include "lyons/string.h"

struct page_directory_entry
{
    uint32_t present        : 1;
    uint32_t read_write     : 1; // 0 for read only
    uint32_t user_super     : 1; // 0 for supervisor only, 1 for user and supervisor
    uint32_t write_through  : 1;
    uint32_t cache_disable  : 1;
    uint32_t accessed       : 1;
    uint32_t none_0         : 1; // make 0
    uint32_t size           : 1; // 1 for 4MiB, 0 for 4KiB
    uint32_t none_g         : 1; // not used
    uint32_t avaliable      : 3; // not used by the cpu, can be any value
    uint32_t address        : 20;
} __attribute__((packed));
typedef struct page_directory_entry page_directory_entry_t;

struct page_table_entry
{
    uint32_t present        : 1;
    uint32_t read_write     : 1; // 0 for read only
    uint32_t user_super     : 1; // 0 for supervisor only, 1 for user and supervisor
    uint32_t write_through  : 1;
    uint32_t cache_disable  : 1;
    uint32_t accessed       : 1;
    uint32_t dirty          : 1;
    uint32_t none_0         : 1;
    uint32_t global         : 1;
    uint32_t avaliable      : 3; // not used by the cpu, can be any value
    uint32_t address        : 20;
} __attribute__((packed));
typedef struct page_table_entry page_table_entry_t;

static page_directory_entry_t initial_page_directory[1024] __attribute__((aligned(4096)));
static page_table_entry_t initial_proc_data_table[1024] __attribute__((aligned(4096)));

#define PROCESS_ADMIN_DATA_SIZE (4 * 1024 * 255) // 1MiB - 4KiB

struct process_admin_space
{
    uint8_t admin_data[PROCESS_ADMIN_DATA_SIZE];
    page_directory_entry_t page_directory[1024];
    page_table_entry_t page_tables[768][1024];
}__attribute__((packed));
typedef struct process_admin_space process_admin_space_t;

#define PROCESS_ADMIN_SPACE_ADDR (0xC0000000 - (4 * 1024 * 1024)) // 4MiB set aside for admin data relating to this process

static process_admin_space_t *process_admin_space = (process_admin_space_t*) PROCESS_ADMIN_SPACE_ADDR;

static void map_big(page_directory_entry_t* entry, uint32_t physical_address, page_access_t access_flags);
void set_cr3(uint32_t dir_addr);

void k_vmem_initialize(uint32_t kernel_physical_base, uint32_t kernel_virtual_base)
{
    memset(initial_page_directory, 0x0, sizeof(page_directory_entry_t) * 1024);
    // Get the page directory base of the kernel virtual space.
    uint32_t kernel_page_directory_index = kernel_virtual_base / (4 * 1024 * 1024);
    // Map the kernel static data as a big page.
    map_big(&initial_page_directory[kernel_page_directory_index], kernel_physical_base, VMEM_ACCESS_SUPER | VMEM_ACCESS_WRITE);

    // Setup process admin space

    // Initialize page table describing process admin space
    memset(initial_proc_data_table, 0x0, sizeof(page_table_entry_t) * 1024);

    // Setup directory entry, pointing to page table describing process admin space.
    uint32_t proc_data_dir_index = kernel_page_directory_index - 1;
    initial_page_directory[proc_data_dir_index].present = 1;
    initial_page_directory[proc_data_dir_index].address = (((uint32_t)initial_proc_data_table) - 0xC0000000) >> 12;
    initial_page_directory[proc_data_dir_index].read_write = 1;

    // Map physical page containing page directory into process admin space
    initial_proc_data_table[256 - 1].present = 1;
    initial_proc_data_table[256 - 1].read_write = 1;
    initial_proc_data_table[256 - 1].address = (((uint32_t)initial_page_directory) - 0xC0000000) >> 12;

    // Map physical page containing page table into process admin space
    initial_proc_data_table[256 + proc_data_dir_index].present = 1;
    initial_proc_data_table[256 + proc_data_dir_index].read_write= 1;
    initial_proc_data_table[256 + proc_data_dir_index].address = (((uint32_t)initial_proc_data_table) - 0xC0000000) >> 12;

    set_cr3(((uint32_t)initial_page_directory) - 0xC0000000);
}

void k_vmem_map(uint32_t physical_base, uint32_t virtual_base, page_access_t access_flags)
{
    kprintf("Mapping p_page at 0x%X to v_page at 0x%X\n", physical_base, virtual_base);
    uint32_t directory_index = virtual_base >> 22;
    uint32_t page_table_index = virtual_base >> 12 & 0x03FF;

    page_directory_entry_t *directory_entry = &process_admin_space->page_directory[directory_index];
    if(directory_entry->present == 0)
    {
        uint32_t table_p_addr = 0;
        if(directory_index >= 768)
        {
            kpanic("Kernel mapping not implemented");
        }
        else
        {
            // Allocate a new page, and map it into the process admin space.
            pmem_addr_t new_page = k_pmem_alloc();
            page_table_entry_t *dst_v_page = &process_admin_space->page_tables[directory_index][0];
            uint32_t new_page_dir_index = (uint32_t)dst_v_page >> 22;
            uint32_t new_page_table_index = (uint32_t)dst_v_page >> 12 & 0x03FF;
            page_directory_entry_t *admin_dir_entry = &process_admin_space->page_directory[new_page_dir_index];
            if(admin_dir_entry->present == 0)
            {
                kpanic("Structural failure in page mapping. The directory entry for the page table is not present.");
            }
            if(admin_dir_entry->size == 1)
            {
                kpanic("Structural failure in page mapping. The page directory entry for the page table is marked as 4MiB page.");
            }
            page_table_entry_t *admin_space_table = &process_admin_space->page_tables[new_page_dir_index][new_page_table_index];
            if(admin_space_table->present == 1)
            {
                kpanic("Trying to map a new page into the process admin space, but there is already a mapping there.");
            }
            admin_space_table->present = 1;
            admin_space_table->read_write = 1;
            admin_space_table->address = new_page >> 12;
            set_cr3(((uint32_t)initial_page_directory) - 0xC0000000);
            memset(dst_v_page, 0x0, sizeof(page_table_entry_t) * 1024);
            table_p_addr = new_page;
        }
        directory_entry->present = 1;
        directory_entry->user_super = 1;
        directory_entry->read_write = 1;
        directory_entry->address = table_p_addr >> 12;
    }
    if(directory_entry->present == 1 && directory_entry->size == 1)
    {
        kpanic("Attempted to clobber 4MiB page");
    }

    page_table_entry_t *table_entry = 0;

    if(directory_index >= 768)
    {
        kpanic("Kernel mapping not implemented");
    }
    else
    {
        table_entry = &process_admin_space->page_tables[directory_index][page_table_index];
    }

    if(table_entry->present == 1)
    {
        kpanic("Attempted to clobber 4KiB page");
    }
    table_entry->present = 1;
    table_entry->read_write = 1;
    table_entry->user_super = 1;
    if(access_flags & VMEM_ACCESS_WRITE != 0)
    {
        table_entry->read_write = 1;
    }
    if(access_flags & VMEM_ACCESS_USER != 0)
    {
        table_entry->user_super = 1;
    }
    table_entry->address = physical_base >> 12;

    set_cr3(((uint32_t)initial_page_directory) - 0xC0000000);
}

void map_big(page_directory_entry_t *entry, uint32_t physical_address, page_access_t access_flags)
{
    if((physical_address & 0xFFFFF) != 0)
    {
        kpanic("VMM error: tried to map a physical address that is not correctly aligned");
    }
    memset(entry, 0x0, sizeof(page_directory_entry_t));
    entry->present = 1;
    entry->size = 1;
    entry->address = physical_address >> 12;
    if(access_flags & VMEM_ACCESS_USER)
    {
        entry->user_super = 1;
    }
    if(access_flags & VMEM_ACCESS_WRITE)
    {
        entry->read_write = 1;
    }
}

asm // implementation of void set_cr3(uint32_t dir_addr)
(
"set_cr3:"
"   mov 4(%esp), %eax;"
"   mov %eax, %cr3;"
"   ret;"
);
