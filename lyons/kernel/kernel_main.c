#include <grub/multiboot.h>

#include <lyons/text_console.h>
#include <lyons/kernel.h>
#include <lyons/proc.h>
#include <lyons/interrupt.h>
#include <lyons/exception.h>
#include <lyons/pmem_manager.h>
#include <lyons/vmem_manager.h>
#include <lyons/syscall.h>

/* Check if the compiler thinks if we are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

#if !defined(__i386__)
#error "Must be compiled for 32-bit ix86"
#endif

#if defined(__cplusplus)
extern "C" /* Use C linkage for kernel_main. */
#endif

#define KERNEL_ADDR_OFFSET 0xC0000000

typedef struct
{
    uint edi;
    uint esi;
    uint ebx;
    uint ebp;
    uint eip;
}cpu_state;

typedef struct 
{
    uint32_t process_id;
    uint32_t page_dir_address; // CR3
}process;

typedef struct
{
    process *parent_proc;
    cpu_state *state;
}thread_context;

void kernel_early (void)
{
    k_console_initialize();
}

void update_mbd_pointers(multiboot_info_t* mbd)
{
    mbd->mmap_addr += KERNEL_ADDR_OFFSET;
    mbd->mods_addr += KERNEL_ADDR_OFFSET;
}

syscall_arg_out_t syscall_test(syscall_arg_in_t arg)
{
    kprintf("SYSCALL OK!\n");
    syscall_arg_out_t arg_out;
    arg_out.arg_a = 0;
    return arg_out;
}

syscall_arg_out_t syscall_print(syscall_arg_in_t arg)
{
    char c = arg.arg_b;
    kprintf("%c", c);
    syscall_arg_out_t arg_out;
    arg_out.arg_a = 0;
    return arg_out;
}

syscall_arg_out_t syscall_yield(syscall_arg_in_t arg)
{
    kprintf("YIELD\n");
    syscall_arg_out_t arg_out;
    arg_out.arg_a = 0;
    return arg_out;
}

void k_exec(void *executable_base, size_t executable_size)
{
    // Map user stack
    uint32_t user_stack_hi_g = KERNEL_ADDR_OFFSET - (8 * 1024 * 1024) - (4 * 1024);
    uint32_t user_stack = user_stack_hi_g - (8 * 1024);
    uint32_t user_stack_lo_g = user_stack - (4 * 1024);
    pmem_addr_t user_stack_page[2];
    user_stack_page[0] = k_pmem_alloc();
    user_stack_page[1] = k_pmem_alloc();
    k_vmem_map(user_stack_page[0], user_stack, VMEM_ACCESS_WRITE | VMEM_ACCESS_USER);
    k_vmem_map(user_stack_page[1], user_stack + (4 * 1024), VMEM_ACCESS_WRITE | VMEM_ACCESS_USER);
    // Map kernel stack
    uint32_t k_stack_hi_g = KERNEL_ADDR_OFFSET - (4 * 1024 * 1024) - (4 * 1024);
    uint32_t k_stack = k_stack_hi_g - (8 * 1024);
    uint32_t k_stack_lo_g = k_stack - (4 * 1024);
    pmem_addr_t k_stack_page[2];
    k_stack_page[0] = k_pmem_alloc();
    k_stack_page[1] = k_pmem_alloc();
    k_vmem_map(k_stack_page[0], k_stack, VMEM_ACCESS_WRITE | VMEM_ACCESS_SUPER);
    k_vmem_map(k_stack_page[1], k_stack + (4 * 1024), VMEM_ACCESS_WRITE | VMEM_ACCESS_SUPER);

    uint32_t entry_point = k_elf_load(executable_base, executable_size);
    if(entry_point == 0)
    {
        kpanic("Failed to parse & load ELF");
    }

    // Reset interrupt stack
    k_proc_set_int_esp((void*)(k_stack_hi_g - 4));
    // reti into executable
    asm
    (
    "push %%eax;"
    "mov $0x23, %%ax;"
    "mov %%ax, %%ds;"
    "mov %%ax, %%es;"
    "mov %%ax, %%fs;"
    "mov %%ax, %%gs;"
    "pop %%eax;"

    "push $0x23;"
    "push %[stack_ptr];"
    "pushf;"
    "push $0x1B;"
    "push %[entry_point];"

    "iret;"
    :
    : [stack_ptr] "r" (user_stack_hi_g - 4), [entry_point] "r" ((uint32_t)entry_point)
    );
}

uint32_t get_exe_base(multiboot_info_t* mbd)
{
    if(mbd->mods_count == 0)
    {
        kpanic("Module not loaded");
    }
    multiboot_module_t *module = mbd->mods_addr;
    return module->mod_start;
}

uint32_t get_exe_length(multiboot_info_t* mbd)
{
    if(mbd->mods_count == 0)
    {
        kpanic("Module not loaded");
    }
    multiboot_module_t *module = mbd->mods_addr;
    return module->mod_end - module->mod_start;
}

void kernel_main (multiboot_info_t* mbd, unsigned int mb_magic)
{
    kprintf("Hello, world!\n");

    if(mb_magic != MULTIBOOT_BOOTLOADER_MAGIC)
    {
        kprintf("Incorrect multiboot magic number. Was 0x%lX. Expected 0x%lX",
                mb_magic, MULTIBOOT_BOOTLOADER_MAGIC);
        kpanic("Invalid multiboot parameters");
    }
    if((mbd->flags & MULTIBOOT_INFO_MEM_MAP) == 0)
    {
        kpanic("No multiboot memory map");
    }
    update_mbd_pointers(mbd);

    k_proc_initialize_state();
    k_interrupt_initialize();
    k_exception_initialize();
    k_pmem_initialize(mbd->mmap_addr, mbd->mmap_length);
    k_pmem_reserve(0x0, 1024); /* reserve first 4MB physical pages for kernel static data.*/

    k_vmem_initialize(0x0, KERNEL_ADDR_OFFSET);

    k_syscall_initialize();
    k_syscall_register(0x0, syscall_test);
    k_syscall_register(0x1, syscall_print);
    k_syscall_register(0x2, syscall_yield);

    uint32_t exe_base = get_exe_base(mbd);
    uint32_t exe_len = get_exe_length(mbd);

    exe_base += KERNEL_ADDR_OFFSET;

    k_exec(exe_base, exe_len);

    kpanic("Done");
}
