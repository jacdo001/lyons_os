#include "lyons/pmem_manager.h"

#include <lyons/kernel.h>
#include <lyons/string.h>

#include <stddef.h>
#include <stdbool.h>

#define PMM_MAP_SIZE 0x20000

static uint8_t bitmask(uint8_t bit);

static const size_t PAGE_SIZE = 0x1000; /* 4KB pages */

static bool initialized = false;
static uint64_t memory_limit = 0;
static uint8_t page_map[PMM_MAP_SIZE];

void k_pmem_initialize(multiboot_uint32_t mmap_addr, multiboot_uint32_t mmap_length)
{
    if(initialized)
    {
        kpanic("PMem Manager cannot be re-initialized");
    }
    /* Set physical memory limits */
    multiboot_uint32_t offset = 0;
    do
    {
        multiboot_memory_map_t* entry = (multiboot_memory_map_t*)(mmap_addr + offset);
        uint64_t segment_limit = entry->addr + entry->len;
        if(segment_limit > memory_limit && entry->type == MULTIBOOT_MEMORY_AVAILABLE)
        {
            memory_limit = segment_limit;
        }
        offset += entry->size + sizeof entry->size;
    }
    while (offset < mmap_length);

    uint64_t page_limit = memory_limit / PAGE_SIZE;

    memset(page_map, 0xFF, PMM_MAP_SIZE);

    uint64_t map_page_limit = PMM_MAP_SIZE * 8; /* Maximum pages which can be tracked */

    if(page_limit > map_page_limit)
    {
        kprintf("PMM error: requested tracking for %lld pages but PMM can only "
            "track %lld pages\n",
            page_limit, map_page_limit);
        kpanic("Failed to initialize PMem Manager.");
    }

    /* Initialized avaliable memory */
    offset = 0;
    do
    {
        multiboot_memory_map_t* entry = (multiboot_memory_map_t*)(mmap_addr + offset);
        if(entry->type == MULTIBOOT_MEMORY_AVAILABLE)
        {
            uint64_t page = entry->addr / PAGE_SIZE;
            uint64_t end_page = page + (entry->len / PAGE_SIZE);
            while(page < end_page)
            {
                page_map[page / 8] &= ~bitmask(page % 8);
                page ++;
            }
        }
        offset += entry->size + sizeof entry->size;
    }
    while (offset < mmap_length);

    /* Initialized reserved memory */
    offset = 0;
    do
    {
        multiboot_memory_map_t* entry = (multiboot_memory_map_t*)(mmap_addr + offset);
        if(entry->type != MULTIBOOT_MEMORY_AVAILABLE)
        {
            uint64_t page = entry->addr / PAGE_SIZE;
            uint64_t end_page = page + (entry->len / PAGE_SIZE);
            while(page < end_page)
            {
                page_map[page / 8] |= bitmask(page % 8);
                page ++;
            }
        }
        offset += entry->size + sizeof entry->size;
    }
    while (offset < mmap_length);

    initialized = 1;
}

void k_pmem_reserve(pmem_addr_t page_base, size_t page_count)
{
    uint64_t page = page_base / PAGE_SIZE;
    uint64_t end_page = page + page_count;
    while(page < end_page)
    {
        page_map[page / 8] |= bitmask(page % 8);
        page ++;
    }
}

void k_pmem_debug_print()
{
    if(initialized != false)
    {
        kprintf("Physical memory manager is initialized!\n");
        kprintf("Page limit: %#llX\n", memory_limit / PAGE_SIZE);
        uint64_t page_count = 0;
        for(uint64_t i = 0; i < memory_limit / PAGE_SIZE; i ++)
        {
            if((page_map[i / 8] & bitmask(i % 8)) != 0)
            {
                page_count ++;
            }
        }
        kprintf("Pages reserved: %lld\n", page_count);
        page_count = 0;
        for(uint64_t i = 0; i < memory_limit / PAGE_SIZE; i ++)
        {
            if((page_map[i / 8] & bitmask(i % 8)) == 0)
            {
                page_count ++;
            }
        }
        kprintf("Pages free: %lld\n", page_count);
    }
    else
    {
        kprintf("Physical memory manager is not initialized\n");
    }
}

pmem_addr_t k_pmem_alloc()
{
    for(uint64_t page = 0; page < memory_limit / PAGE_SIZE; page ++)
    {
        if((page_map[page / 8] & bitmask(page % 8)) == 0)
        {
            page_map[page / 8] |= bitmask(page % 8);
            kprintf("PMM returned page @ 0x%X\n", page * PAGE_SIZE);
            return page * PAGE_SIZE;
        }
    }
    kpanic("Out of memory");
    return 0;
}

uint8_t bitmask(uint8_t bit)
{
    return 0x80 >> bit;
}
