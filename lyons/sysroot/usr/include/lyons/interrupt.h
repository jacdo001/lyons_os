#ifndef _INTERRUPT_H
#define _INTERRUPT_H 1

#include <stdint.h>

typedef uint8_t interrupt_caller_level_t;

static const interrupt_caller_level_t \
INTERRUPT_USER = 1,
INTERRUPT_SYSTEM = 0;

typedef uint8_t interrupt_vector_t;

static const interrupt_vector_t \
INTERRUPT_DIV_0 = 0x00,
INTERRUPT_DEBUG = 0x01,
INTERRUPT_NMI = 0x02,
INTERRUPT_BREAK = 0x03,
INTERRUPT_OVERFLOW = 0x04,
INTERRUPT_RANGE = 0x05,
INTERRUPT_INV_OP = 0x06,
INTERRUPT_DEV_NOT_AVL = 0x07,
INTERRUPT_DBL_FAULT = 0x08,
INTERRUPT_INVALID_TSS = 0x0A,
INTERRUPT_SEG_NOT_PRESENT = 0x0B,
INTERRUPT_STACK_SEG_FAULT = 0x0C,
INTERRUPT_GP = 0x0D,
INTERRUPT_PAGE_FAULT = 0x0E,
INTERRUPT_X87_FP = 0x10,
INTERRUPT_ALIGN = 0x11,
INTERRUPT_MACHINE = 0x12,
INTERRUPT_SIMD_FP = 0x13,
INTERRUPT_VIRT = 0x14,
INTERRUPT_SECURITY = 0x1E;

void k_interrupt_initialize();
void k_interrupt_install_isr(interrupt_vector_t vector, void (*isr) (void), interrupt_caller_level_t required_caller_level);

#endif
