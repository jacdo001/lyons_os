#ifndef _STRING_H
#define _STRING_H 1

#include <stddef.h>

void* memset(void* dst_ptr, int value, size_t length);
size_t strlen(const char*);
void * memcpy ( void * destination, const void * source, size_t num );

#endif
