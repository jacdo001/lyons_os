#ifndef _EXCEPTION_H
#define _EXCEPTION_H 1

#include "stdint.h"

/* parse elf structure from base to base + limit. Returns entrypoint addr */
uint32_t k_elf_load(uint32_t base, uint32_t limit);

#endif//_EXCEPTION_H
