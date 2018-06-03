#ifndef _SYSCALL_H
#define _SYSCALL_H 1

#include <stdint.h>

struct syscall_arg_in
{
    uint32_t arg_a;
    uint32_t arg_b;
};
typedef struct syscall_arg_in syscall_arg_in_t;

struct syscall_arg_out
{
    uint32_t arg_a;
};
typedef struct syscall_arg_out syscall_arg_out_t;

typedef uint32_t syscall_id_t;

void k_syscall_initialize( void );
void k_syscall_register( syscall_id_t id, syscall_arg_out_t (*func_p) (syscall_arg_in_t) );

#endif
