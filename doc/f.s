
.globl _f

_f:
    pushl   %ebp
	movl	%esp, %ebp
    
	andl	$-16, %esp
	subl	$32, %esp
    
    movl    $1,%eax
    movl    8(%ebp),%ebx
    addl    %ebx,%eax
    movl    %eax,-4(%esp)
    
    pushl   -4(%esp)
    call    _print_int
    addl    $4,%esp
    
    movl    %eax
    
	leave
	ret
    