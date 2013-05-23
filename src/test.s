	.file	"test.c"
	.section .rdata,"dr"
LC0:
	.ascii "abcdef\0"	
    .section .rdata,"dr"
LC1:
	.ascii "abcdef\0"
	.text
.globl _f
	.def	_f;	.scl	2;	.type	32;	.endef
_f:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$16, %esp
	movl	$LC0, -4(%ebp)
	leave
	ret
