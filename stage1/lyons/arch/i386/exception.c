#include "lyons/exception.h"

#include <lyons/interrupt.h>
#include <lyons/kernel.h>

void do_div_0(void);
void do_overflow(void);
void do_range(void);
void do_inv_op(void);
void do_gp(void);
void do_pf(void);

void isr_div_0(void);
void isr_overflow(void);
void isr_range(void);
void isr_inv_op(void);
void isr_gp(void);
void isr_pf(void);

void k_exception_initialize()
{
    k_interrupt_install_isr(INTERRUPT_DIV_0, isr_div_0, INTERRUPT_SYSTEM);
    k_interrupt_install_isr(INTERRUPT_OVERFLOW, isr_overflow, INTERRUPT_SYSTEM);
    k_interrupt_install_isr(INTERRUPT_RANGE, isr_range, INTERRUPT_SYSTEM);
    k_interrupt_install_isr(INTERRUPT_INV_OP, isr_inv_op, INTERRUPT_SYSTEM);
    k_interrupt_install_isr(INTERRUPT_GP, isr_gp, INTERRUPT_SYSTEM);
    k_interrupt_install_isr(INTERRUPT_PAGE_FAULT, isr_pf, INTERRUPT_SYSTEM);
}

asm(
".align 4;"
"isr_div_0: ;"
    "pushal;"
    "cld;"
    "call do_div_0;"
    "popal;"
    "iret;"
);

void do_div_0 (void)
{
    uint32_t esp = 0;
    asm("mov %%esp, %[output];" : [output] "=r" (esp) : );
    kprintf("esp = 0x%X\n", esp);
    kpanic("Divide By 0");
}

asm(
".align 4;"
"isr_overflow: ;"
    "pushal;"
    "cld;"
    "call do_overflow;"
    "popal;"
    "iret;"
);

void do_overflow (void)
{
    kpanic("Overflow");
}

asm(
".align 4;"
"isr_range: ;"
    "pushal;"
    "cld;"
    "call do_range;"
    "popal;"
    "iret;"
);

void do_range (void)
{
    kpanic("Bound Range Exceeded");
}

asm(
".align 4;"
"isr_inv_op: ;"
    "pushal;"
    "cld;"
    "call do_inv_op;"
    "popal;"
    "iret;"
);

void do_inv_op (void)
{
    kpanic("Invalid Operation");
}

asm(
".align 4;"
"isr_gp: ;"
    "pushal;"
    "cld;"
    "call do_gp;"
    "popal;"
    "iret;"
);

void do_gp (void)
{
    uint32_t err;
    asm ( "mov 4(%%esp), %[output];" : [output] "=r" (err) : );
    kpanic("General Protection Fault");
}

asm(
".align 4;"
"isr_pf: ;"
    "pushal;"
    "cld;"
    "call do_pf;"
    "popal;"
    "iret;"
);

void do_pf (void)
{
    kpanic("Page Fault");
}
