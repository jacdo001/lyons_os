#ifndef _EXCEPTION_H
#define _EXCEPTION_H 1

#include <stdint.h>

typedef uint32_t except_handle_t;
typedef uint32_t except_class_t;

static const except_class_t \
EXCEPT_DIV_0 = 0x00,
EXCEPT_DEBUG = 0x01,
EXCEPT_NMI = 0x02,
EXCEPT_BREAK = 0x03,
EXCEPT_OVERFLOW = 0x04,
EXCEPT_RANGE = 0x05,
EXCEPT_INV_OP = 0x06,
EXCEPT_DEV_NOT_AVL = 0x07,
EXCEPT_DBL_FAULT = 0x08,
EXCEPT_INVALID_TSS = 0x0A,
EXCEPT_SEG_NOT_PRESENT = 0x0B,
EXCEPT_STACK_SEG_FAULT = 0x0C,
EXCEPT_GP = 0x0D,
EXCEPT_PAGE_FAULT = 0x0E,
EXCEPT_X87_FP = 0x10,
EXCEPT_ALIGN = 0x11,
EXCEPT_MACHINE = 0x12,
EXCEPT_SIMD_FP = 0x13,
EXCEPT_VIRT = 0x14,
EXCEPT_SECURITY = 0x1E;

struct exception_data
{
    uint8_t error_code;
};
typedef struct exception_data exception_data_t;

void k_exception_initialize();
except_handle_t k_exception_install(except_class_t except_class, void* func);

#endif
