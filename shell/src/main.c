#include "stdint.h"

void print_call(char c)
{
    __asm__("mov $0x01, %%eax; mov %0, %%ebx; int $0x80;"
        :
        : "m" (c)
        : "eax", "ebx");
}

void yield_call()
{
    __asm__("mov $0x02, %%eax; int $0x80;"
        :
        :
        : "eax", "ebx");
}

void print_string(char* s)
{
    char *p = s;
    while(*p){ print_call(*(p++)); }
}

static inline uint8_t read_port_8(uint16_t port)
{
    uint8_t value;
    asm volatile ( "inb %1, %0"
                    : "=a"(value)
                    : "Nd"(port) );
    return value;
}

int main(int argc, char **argv)
{
    print_string("LyonShell Ready\n");
    print_string("> ");

    while(1)
    {
        print_string("Hi\n");
        yield_call();
    }
}
