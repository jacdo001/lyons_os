#include "lyons/syscall.h"
#include "lyons/kernel.h"
#include "lyons/interrupt.h"
#include "lyons/string.h"

#define SYSCALL_COUNT 3

struct syscall_entry
{
    syscall_arg_out_t (*func)(syscall_arg_in_t);
};
typedef struct syscall_entry syscall_entry_t;

void do_syscall ( void );

void syscall_isr( void );
__asm__
(
".align 4;"
"syscall_isr:"
    "call do_syscall;"
    "iret;"
);

static syscall_entry_t syscall_table[SYSCALL_COUNT];

void k_syscall_initialize( void )
{
    memset(syscall_table, 0x0, sizeof(syscall_entry_t) * SYSCALL_COUNT);
    k_interrupt_install_isr(0x80, syscall_isr, INTERRUPT_USER);
}

void k_syscall_register(syscall_id_t id, syscall_arg_out_t (*func_p)(syscall_arg_in_t))
{
    if(id >= SYSCALL_COUNT)
    {
        kpanic("Syscall id out of bounds when registering.");
    }
    if(syscall_table[id].func != 0)
    {
        kpanic("Syscall registered to id which already has a call.");
    }
    syscall_table[id].func = func_p;
}

void do_syscall()
{
    uint32_t id = 0;
    asm( "mov %%eax, %[output]" : [output] "=r" (id) : );
    uint32_t arg_b = 0;
    asm( "mov %%ebx, %[output]" : [output] "=r" (arg_b) : );

    if(id >= SYSCALL_COUNT)
    {
        kpanic("Syscall id out of bounds.");
    }
    if(syscall_table[id].func == 0)
    {
        kpanic("No syscall registered for id.");
    }
    syscall_arg_in_t in;
    in.arg_a = id;
    in.arg_b = arg_b;
    syscall_arg_out_t out = syscall_table[id].func(in);
}
