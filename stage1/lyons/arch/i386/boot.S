/* Values for the Multiboot header */
.set ALIGN,	1<<0
.set MEMINFO,	1<<1
.set FLAGS,	ALIGN | MEMINFO
.set MAGIC,	0x1BADB002
.set CHECKSUM,	-(MAGIC + FLAGS)

.set KERNEL_V_BASE,	0xC0000000 /* Base address of kernel in virtual mem */
.set KERNEL_PAGE_NO,	(KERNEL_V_BASE >> 22) /* Virtual page number of kernel */

/* Static page directory, the first 4M is mapped to KERNEL_V_BASE, where it will remain. 
	The first 4M is also identity mapped, because when paging is enabled, the IP
	will still be in this section. */ 
.section .data
.align 4096	/* Page align */
BootPageDirectory:
	.long 0x00000083			/* Identity mapped page 0 */
        .fill (KERNEL_PAGE_NO - 1), 4, 0x0	/* Nothing until kernel base */
	.long 0x00000083			/* Map physical page 0 to kernel base */
	.fill (1024 - KERNEL_PAGE_NO - 1), 4, 0x0 /* Fill rest of the directory */

.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

.section .bootstrap_stack, "aw", @nobits
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

.section .text
.global _start
.type _start, @function
_start:
	movl $(BootPageDirectory - KERNEL_V_BASE), %ecx
	movl %ecx, %cr3
	
	movl %cr4, %ecx
	orl $0x00000010, %ecx
	movl %ecx, %cr4

	movl %cr0, %ecx
	orl $0x80000000, %ecx
	movl %ecx, %cr0

	leal StartHigh, %ecx
	jmp *%ecx

StartHigh:
	movl $0, BootPageDirectory
	invlpg 0

	movl $stack_top, %esp

	pushl %eax /* Push magic value */
	addl $KERNEL_V_BASE, %ebx
	pushl %ebx /* Push pointer to multiboot info */

	call kernel_early

	call _init

	call kernel_main
	
	cli
.Lhang:
	hlt
	jmp .Lhang

.size _start, . - _start
