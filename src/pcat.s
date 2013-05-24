	 .text
	 .globl _MainEntry

_MainEntry:
	 pushl %ebp
	 movl %esp, %ebp
	 subl $36, %esp
	 andl $-16, %esp
	 movl $1, %eax
	 movl %eax, -4(%ebp)
	 movl $1073741824, %eax
	 movl %eax, -8(%ebp)
	 movl -4(%ebp), %eax
	 movl %eax, -12(%ebp)
	 movl -12(%ebp), %eax
	 push %eax
	 call _print_int
	 addl $4, %esp
	 call _print_line
	 movl -8(%ebp), %eax
	 movl %eax, -16(%ebp)
	 movl -16(%ebp), %eax
	 push %eax
	 call _print_real
	 addl $4, %esp
	 call _print_line
	 movl $label_1, %eax
	 push %eax
	 call _print_str
	 addl $4, %esp
	 call _print_line
	 movl -4(%ebp), %eax
	 movl %eax, -20(%ebp)
	 movl -20(%ebp), %eax
	 push %eax
	 call _print_int
	 addl $4, %esp
	 movl -8(%ebp), %eax
	 movl %eax, -24(%ebp)
	 movl -24(%ebp), %eax
	 push %eax
	 call _print_real
	 addl $4, %esp
	 movl $label_2, %eax
	 push %eax
	 call _print_str
	 addl $4, %esp
	 call _print_line
	 call _print_line
	 call _read_int
	 movl %eax, -4(%ebp)
	 call _read_real
	 movl %eax, -8(%ebp)
	 movl -4(%ebp), %eax
	 movl %eax, -28(%ebp)
	 movl -28(%ebp), %eax
	 push %eax
	 call _print_int
	 addl $4, %esp
	 call _print_line
	 movl -8(%ebp), %eax
	 movl %eax, -32(%ebp)
	 movl -32(%ebp), %eax
	 push %eax
	 call _print_real
	 addl $4, %esp
	 call _print_line
	 leave
	 ret

	 .section .rdata,"dr"
label_1:
	 .ascii "hello\0"

	 .section .rdata,"dr"
label_2:
	 .ascii "hello\0"
