.globl set_idt				/* set_idt(idtr_t *idtr) */
set_idt:
	movl 4(%esp), %eax	/* eax = idtr */
	lidt (%eax)					/* load idtr */
	ret
