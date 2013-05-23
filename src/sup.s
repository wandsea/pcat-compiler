	.file	"sup.c"
	.section .rdata,"dr"
LC0:
	.ascii "%d\0"
	.text
.globl _read_int
	.def	_read_int;	.scl	2;	.type	32;	.endef
_read_int:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$40, %esp
	leal	-12(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC0, (%esp)
	call	_scanf
	movl	-12(%ebp), %eax
	leave
	ret
	.section .rdata,"dr"
LC1:
	.ascii "%f\0"
	.text
.globl _read_real
	.def	_read_real;	.scl	2;	.type	32;	.endef
_read_real:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$40, %esp
	leal	-16(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC1, (%esp)
	call	_scanf
	leal	-16(%ebp), %eax
	movl	(%eax), %eax
	movl	%eax, -12(%ebp)
	movl	-12(%ebp), %eax
	leave
	ret
.globl _print_int
	.def	_print_int;	.scl	2;	.type	32;	.endef
_print_int:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	8(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC0, (%esp)
	call	_printf
	leave
	ret
.globl _print_real
	.def	_print_real;	.scl	2;	.type	32;	.endef
_print_real:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$40, %esp
	leal	8(%ebp), %eax
	movl	(%eax), %eax
	movl	%eax, -12(%ebp)
	movl	8(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC0, (%esp)
	call	_printf
	leave
	ret
	.section .rdata,"dr"
LC2:
	.ascii "%s\0"
	.text
.globl _print_str
	.def	_print_str;	.scl	2;	.type	32;	.endef
_print_str:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$40, %esp
	movl	8(%ebp), %eax
	movl	%eax, -12(%ebp)
	movl	8(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC2, (%esp)
	call	_printf
	leave
	ret
.globl _print_line
	.def	_print_line;	.scl	2;	.type	32;	.endef
_print_line:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	$10, (%esp)
	call	_putchar
	leave
	ret
	.def	___main;	.scl	2;	.type	32;	.endef
.globl _main
	.def	_main;	.scl	2;	.type	32;	.endef
_main:
	pushl	%ebp
	movl	%esp, %ebp
	andl	$-16, %esp
	call	___main
	call	_MainEntry
	movl	$0, %eax
	movl	%ebp, %esp
	popl	%ebp
	ret
	.def	_scanf;	.scl	2;	.type	32;	.endef
	.def	_printf;	.scl	2;	.type	32;	.endef
	.def	_putchar;	.scl	2;	.type	32;	.endef
	.def	_MainEntry;	.scl	2;	.type	32;	.endef
