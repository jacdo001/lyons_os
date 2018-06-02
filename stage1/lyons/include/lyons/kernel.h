#ifndef _KERNEL_H
#define _KERNEL_H 1

#include <stddef.h>

size_t kprintf(const char* __restrict, ...);
void kpanic(const char* msg);

#endif
