	.file	"test.c"
	.text
.globl _f
	.def	_f;	.scl	2;	.type	32;	.endef
_f:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$16, %esp
	cmpl	$0, 8(%ebp)
	je	L2
	cmpl	$0, 12(%ebp)
	je	L2
	movl	$1, %eax
	jmp	L3
L2:
	movl	$0, %eax
L3:
	movl	%eax, -4(%ebp)
	movl	-4(%ebp), %eax
	leave
	ret
