.globl set_data
set_data:							# void set_data(int32 ds_selector)
	mov 4(%esp), %ax		# ax = ds_selector
	mov %ax, %ds				#	set segment registers
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	mov %ax, %ss
	ret									# return
