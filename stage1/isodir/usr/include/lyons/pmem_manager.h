#ifndef _PMEM_MANAGER_H
#define _PMEM_MANAGER_H

#include <grub/multiboot.h>
#include <stdint.h>
#include <stddef.h>

typedef uint32_t pmem_addr_t;

void k_pmem_initialize(multiboot_uint32_t mmap_addr, multiboot_uint32_t mmap_length);
void k_pmem_reserve(pmem_addr_t page_base, size_t page_count);
pmem_addr_t k_pmem_alloc();
void k_pmem_dealloc(pmem_addr_t page_base);
void k_pmem_debug_print();

#endif
