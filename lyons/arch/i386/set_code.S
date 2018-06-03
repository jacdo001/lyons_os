.globl set_code
set_code:							# void set_code(int32 cs_selector)
	mov 4(%esp), %eax 	# Read cs selector from stack
	push %eax		 				# Push cs selector
	push $done		  		# Push done address as offset
	retf								# Far ret - reads selector:offset from stack, sets cs to selector 
done:
	ret 								# cs is set now
