.globl set_tss			/* set_tss(uint32_t tss_selector) */
set_tss:
	mov 4(%esp), %eax	/* ax = tss_index */
	ltr %ax
	ret
