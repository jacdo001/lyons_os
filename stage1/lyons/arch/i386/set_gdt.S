.globl set_gdt	/* set_gdt(gdtr_t *gdtr) */
set_gdt:
	movl 4(%esp), %eax	/* eax = gdtr */
	lgdt (%eax)
	ret
