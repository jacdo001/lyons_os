#include "lyons/kernel.h"

void kpanic(const char *msg)
{
    if(msg == 0)
    {
        kprintf("Kernel Panic!");
    }
    else
    {
        kprintf("Kernel Panic! Cause: %s", msg);
    }
    asm ( "cli; hlt;" );
}
