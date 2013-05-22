	.file	"test.c"
	.text
.globl _f
	.def	_f;	.scl	2;	.type	32;	.endef
_f:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$16, %esp
	movl	8(%ebp), %eax
	negl	%eax
	movl	%eax, -4(%ebp)
	movl	-4(%ebp), %eax
	leave
	ret
