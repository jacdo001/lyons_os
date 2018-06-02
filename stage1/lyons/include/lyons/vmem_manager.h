#ifndef _VMEM_MANAGER_H
#define _VMEM_MANAGER_H 1

#include "stdint.h"

typedef uint8_t page_access_t;

static const page_access_t \
VMEM_ACCESS_USER = 0x4,
VMEM_ACCESS_SUPER = 0x0,
VMEM_ACCESS_READ_O = 0x0,
VMEM_ACCESS_WRITE = 0x2;

void k_vmem_initialize(uint32_t kernel_physical_base, uint32_t kernel_virtual_base);
void k_vmem_map(uint32_t physical_base, uint32_t virtual_base, page_access_t access_flags);

#endif // VMEM_MANAGER_H
