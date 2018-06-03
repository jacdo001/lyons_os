#include "lyons/string.h"

void *memset(void *dst_ptr, int value, size_t length)
{
    unsigned char *dst = dst_ptr;
    while(dst < (unsigned char*) (dst_ptr + length))
    {
        *dst = (unsigned char)value;
        dst ++;
    }
    return dst_ptr;
}

size_t strlen(const char *str)
{
    size_t result = 0;
    while ( str [result] != '\0' )
    {
        result ++;
    }
    return result;
}


void * memcpy ( void * destination, const void * source, size_t num )
{
    unsigned char *dst = destination;
    unsigned char *src = source;
    while(dst < (unsigned char*) (destination + num))
    {
        *dst = *src;
        dst ++;
        src ++;
    }
    return destination;
}
