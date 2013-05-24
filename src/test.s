	.file	"test.c"
	.text
.globl _f
	.def	_f;	.scl	2;	.type	32;	.endef
_f:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$16, %esp
	movl	$1, -4(%ebp)
	movl	$2, -8(%ebp)
	movl	-4(%ebp), %eax
	cmpl	-8(%ebp), %eax
	jge	L3
	movl	$10, -4(%ebp)
L3:
	leave
	ret
