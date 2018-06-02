#include "lyons/proc.h"
#include <lyons/string.h>
#include <lyons/kernel.h>

#include <stdint.h>

struct gdt_entry
{
    uint32_t limit_low	:16;
    uint32_t base_low	:24;
    uint32_t accessed	:1 ;
    uint32_t read_write	:1 ;
    uint32_t bd_flag 	:1 ; 	/* Expand down (data) / 32-bit ops (code). */
    uint32_t code		:1 ;
    uint32_t not_sys	:1 ;	/* Clear for system, set is data or code. */
    uint32_t dpl		:2 ;
    uint32_t present	:1 ;
    uint32_t limit_high	:4 ;
    uint32_t available	:1 ;
    uint32_t exe_64		:1 ;	/* Set if code in segment is IA_64 */
    uint32_t big		:1 ;	/* Set for 32-bit clear for 16-bit */
    uint32_t gran		:1 ;	/* Set if limit is in 4KB chunks */
    uint32_t base_high	:8 ;
} __attribute__((packed));
typedef struct gdt_entry gdt_entry_t;

struct gdt_register
{
    uint16_t limit;
    uint32_t base;
} __attribute__ ((packed));
typedef struct gdt_register gdtr_t;

struct task_state_segment
{
    uint32_t prev_tss;
    uint32_t esp0;
    uint32_t ss0;
    uint32_t unused[22];
    uint16_t trap;
    uint16_t iomap_base;
} __attribute__((packed));
typedef struct task_state_segment task_state_segment_t;

typedef uint32_t seg_selector_t;

/* ASM functions */
extern void set_gdt(gdtr_t *gdtr);
extern void set_code(seg_selector_t selector);
extern void set_data(seg_selector_t selector);
extern void set_tss(seg_selector_t selector);

task_state_segment_t tss;
gdt_entry_t gdt[6];
gdtr_t gdtr;

void gdt_null(gdt_entry_t* dst)
{
    memset(dst, 0, sizeof(gdt_entry_t));
}

void gdt_kernel_code(gdt_entry_t *dst)
{
    gdt_null(dst);
    dst->limit_low = 0xFFFF;
    dst->base_low = 0;
    dst->accessed = 0;
    dst->read_write = 1;
    dst->bd_flag = 0;
    dst->code = 1;
    dst->not_sys = 1;
    dst->dpl = 0;
    dst->present = 1;
    dst->limit_high = 0xF;
    dst->available = 0;
    dst->exe_64 = 0;
    dst->big = 1;
    dst->gran = 1;
    dst->base_high = 0;
}

void gdt_kernel_data(gdt_entry_t *dst)
{
    gdt_null(dst);
    dst->limit_low = 0xFFFF;
    dst->base_low = 0;
    dst->accessed = 0;
    dst->read_write = 1;
    dst->bd_flag = 0;
    dst->code = 0;
    dst->not_sys = 1;
    dst->dpl = 0;
    dst->present = 1;
    dst->limit_high = 0xF;
    dst->available = 0;
    dst->exe_64 = 0;
    dst->big = 1;
    dst->gran = 1;
    dst->base_high = 0;
}

void gdt_usr_code(gdt_entry_t *dst)
{
    gdt_null(dst);
    dst->limit_low = 0xFFFF;
    dst->base_low = 0;
    dst->accessed = 0;
    dst->read_write = 1;
    dst->bd_flag = 0;
    dst->code = 1;
    dst->not_sys = 1;
    dst->dpl = 3;
    dst->present = 1;
    dst->limit_high = 0xF;
    dst->available = 0;
    dst->exe_64 = 0;
    dst->big = 1;
    dst->gran = 1;
    dst->base_high = 0;
}

void gdt_usr_data(gdt_entry_t *dst)
{
    gdt_null(dst);
    dst->limit_low = 0xFFFF;
    dst->base_low = 0;
    dst->accessed = 0;
    dst->read_write = 1;
    dst->bd_flag = 0;
    dst->code = 0;
    dst->not_sys = 1;
    dst->dpl = 3;
    dst->present = 1;
    dst->limit_high = 0xF;
    dst->available = 0;
    dst->exe_64 = 0;
    dst->big = 1;
    dst->gran = 1;
    dst->base_high = 0;
}

void gdt_tss(gdt_entry_t *dst, task_state_segment_t *tss)
{
    uint32_t base = (uint32_t) tss;
    uint32_t limit = sizeof(task_state_segment_t);

    gdt_null(dst);
    dst->limit_low = limit & 0xFFFF;
    dst->base_low = base & 0xFFFFFF;
    dst->accessed = 1;
    dst->read_write = 0;
    dst->bd_flag = 0;
    dst->code = 1;
    dst->not_sys = 0;
    dst->dpl = 3;
    dst->present = 1;
    dst->limit_high = (limit & 0xF0000) >> 16;
    dst->available = 1;
    dst->big = 0;
    dst->gran = 1;
    dst->base_high = (base & 0xFF000000) >> 24;
}

void k_proc_initialize_state()
{
    gdt_null(&gdt[0]);
    gdt_kernel_code(&gdt[1]);
    gdt_kernel_data(&gdt[2]);
    gdt_usr_code(&gdt[3]);
    gdt_usr_data(&gdt[4]);
    gdt_tss(&gdt[5], &tss);

    gdtr.limit = (sizeof(gdt_entry_t) * 6) - 1;
    gdtr.base = (uint32_t) gdt;
    memset(&tss, 0, sizeof(task_state_segment_t));
    tss.esp0 = 0x0;
    tss.ss0 = (2 << 3 | 0);

    set_gdt(&gdtr);
    set_code(1 << 3 | 0);
    set_data(2 << 3 | 0);
    set_tss(5 << 3 | 3);
}

void k_proc_set_int_esp(void *esp)
{
    tss.esp0 = (uint32_t)esp;
}
