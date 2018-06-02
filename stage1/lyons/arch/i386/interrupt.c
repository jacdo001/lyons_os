#include "lyons/interrupt.h"

#include <lyons/string.h>
#include <lyons/kernel.h>

typedef uint8_t idt_flags_t;

static const idt_flags_t \
FLAGS_PRESENT = 0x80,
FLAGS_NOT_PRESENT = 0x0,
FLAGS_32BIT = 0x08,
FLAGS_INTERRUPT = 0x06,
FLAGS_USER_DPL = 0x3 << 5;

struct idt_entry
{
    uint16_t base_low;
    uint16_t selector;
    uint8_t  reserved; /* set 0 */
    uint8_t  flags;
    uint16_t base_high;
} __attribute__((packed));
typedef struct idt_entry idt_entry_t;

struct idt_register
{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));
typedef struct idt_register idtr_t;

extern void set_idt(idtr_t *idtr);

void install_isr(interrupt_vector_t vector, idt_flags_t flags, uint16_t selector, void (*isr) (void));
void null_isr(interrupt_vector_t vector);
static void dbl_fault_default() __attribute__((used));

void dbl_fault_default()
{
    kpanic("Double Fault");
}

void dbl_fault(void);
__asm__
(
    ".align 4;"
    "dbl_fault: ;"
        "pushal;"
        "cld;"
        "call dbl_fault_default;"
        "popal;"
        "iret;"
);

static idt_entry_t idt[256];
static idtr_t idtr;
static void (*dbl_fault_asm)(void) = dbl_fault;

void k_interrupt_initialize()
{
    idtr.base = (uint32_t) idt;
    idtr.limit = (sizeof(idt_entry_t) * 256) - 1;

    memset((void*)idt, 0, sizeof(idt_entry_t) * 256);
    install_isr(INTERRUPT_DBL_FAULT, FLAGS_PRESENT | FLAGS_32BIT | FLAGS_INTERRUPT, 0x08, dbl_fault_asm);

    set_idt(&idtr);
}

void k_interrupt_install_isr(interrupt_vector_t vector, void (*isr) (void), interrupt_caller_level_t required_caller_level)
{
    idt_flags_t flags = 0x0;
    if(required_caller_level == INTERRUPT_USER)
    {
        flags |= FLAGS_USER_DPL;
    }
    install_isr(vector, flags | FLAGS_PRESENT | FLAGS_32BIT | FLAGS_INTERRUPT, 0x08, isr);
}

void null_isr(interrupt_vector_t vector)
{
    idt[vector].base_low = 0;
    idt[vector].base_high = 0;
    idt[vector].reserved = 0;
    idt[vector].flags = FLAGS_NOT_PRESENT;
    idt[vector].selector = 0;
}

void install_isr(interrupt_vector_t vector, idt_flags_t flags, uint16_t selector, void (*isr) (void))
{
    idt[vector].base_low = ((uint32_t) isr) & 0xFFFF;
    idt[vector].base_high = (((uint32_t) isr) >> 16) & 0xFFFF;
    idt[vector].reserved = 0;
    idt[vector].flags = flags;
    idt[vector].selector = selector;
}
