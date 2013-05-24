	 .text
	 .globl _MainEntry

_MainEntry:
	 pushl %ebp
	 movl %esp, %ebp
	 subl $16, %esp
	 andl $-16, %esp
	 movl $0, %eax
	 movl %eax, -4(%ebp)
	 movl $4, %eax
	 pushl %eax
	 movl $1, %eax
	 pushl %eax
	 pushl %ebp
	 call _FACTORIAL
	 addl $12, %esp
	 movl %eax, -8(%ebp)
	 movl -8(%ebp), %eax
	 movl %eax, -4(%ebp)
	 movl $label_1, %eax
	 push %eax
	 call _print_str
	 addl $4, %esp
	 movl -4(%ebp), %eax
	 movl %eax, -12(%ebp)
	 movl -12(%ebp), %eax
	 push %eax
	 call _print_int
	 addl $4, %esp
	 movl $label_2, %eax
	 push %eax
	 call _print_str
	 addl $4, %esp
	 call _print_line
	 leave
	 ret

_FACTORIAL:
	 pushl %ebp
	 movl %esp, %ebp
	 subl $52, %esp
	 andl $-16, %esp
	 movl $0, %eax
	 movl %eax, -4(%ebp)
	 movl $label_3, %eax
	 push %eax
	 call _print_str
	 addl $4, %esp
	 movl $label_4, %eax
	 push %eax
	 call _print_str
	 addl $4, %esp
	 movl 12(%ebp), %eax
	 movl %eax, -8(%ebp)
	 movl -8(%ebp), %eax
	 push %eax
	 call _print_int
	 addl $4, %esp
	 movl $label_5, %eax
	 push %eax
	 call _print_str
	 addl $4, %esp
	 movl 16(%ebp), %eax
	 movl %eax, -12(%ebp)
	 movl -12(%ebp), %eax
	 push %eax
	 call _print_int
	 addl $4, %esp
	 call _print_line
	 movl 16(%ebp), %eax
	 movl %eax, -16(%ebp)
	 movl -16(%ebp), %eax
	 movl $0, %ecx
	 cmpl %ecx, %eax
	 setg %al
	 movzbl	%al, %eax
	 movl %eax, -20(%ebp)
	 movl -20(%ebp), %eax
	 cmpl $0, %eax
	 jne label_6
	 jmp label_7
label_6:
	 movl 12(%ebp), %eax
	 movl %eax, -24(%ebp)
	 movl 16(%ebp), %eax
	 movl %eax, -28(%ebp)
	 movl -28(%ebp), %eax
	 pushl %eax
	 movl -24(%ebp), %eax
	 pushl %eax
	 pushl %ebp
	 call _MULT
	 addl $12, %esp
	 movl %eax, -32(%ebp)
	 movl -32(%ebp), %eax
	 movl %eax, -4(%ebp)
	 movl -4(%ebp), %eax
	 movl %eax, -36(%ebp)
	 movl 16(%ebp), %eax
	 movl %eax, -40(%ebp)
	 movl -40(%ebp), %eax
	 movl $1, %ecx
	 subl %ecx, %eax
	 movl %eax, -44(%ebp)
	 movl -44(%ebp), %eax
	 pushl %eax
	 movl -36(%ebp), %eax
	 pushl %eax
	 movl (%ebp), %edx
	 pushl %edx
	 call _FACTORIAL
	 addl $12, %esp
	 movl %eax, -48(%ebp)
	 movl -48(%ebp), %eax
	 leave
	 ret
label_7:
	 leave
	 ret

_ADD:
	 pushl %ebp
	 movl %esp, %ebp
	 subl $16, %esp
	 andl $-16, %esp
	 movl 12(%ebp), %eax
	 movl %eax, -4(%ebp)
	 movl 16(%ebp), %eax
	 movl %eax, -8(%ebp)
	 movl -4(%ebp), %eax
	 movl -8(%ebp), %ecx
	 addl %ecx, %eax
	 movl %eax, -12(%ebp)
	 movl -12(%ebp), %eax
	 leave
	 ret
	 leave
	 ret

_MULT:
	 pushl %ebp
	 movl %esp, %ebp
	 subl $40, %esp
	 andl $-16, %esp
	 movl $0, %eax
	 movl %eax, -4(%ebp)
	 movl $0, %eax
	 movl %eax, -8(%ebp)
	 movl $1, %eax
	 movl %eax, -4(%ebp)
	 movl $0, %eax
	 movl %eax, -8(%ebp)
label_8:
	 movl 12(%ebp), %eax
	 movl %eax, -12(%ebp)
	 movl -8(%ebp), %eax
	 movl -12(%ebp), %ecx
	 cmpl %ecx, %eax
	 jg label_9
	 movl -8(%ebp), %eax
	 movl %eax, -16(%ebp)
	 movl 16(%ebp), %eax
	 movl %eax, -20(%ebp)
	 movl -20(%ebp), %eax
	 pushl %eax
	 movl -16(%ebp), %eax
	 pushl %eax
	 movl (%ebp), %edx
	 pushl %edx
	 call _ADD
	 addl $12, %esp
	 movl %eax, -24(%ebp)
	 movl -24(%ebp), %eax
	 movl %eax, -8(%ebp)
	 movl -4(%ebp), %eax
	 movl %eax, -28(%ebp)
	 movl -28(%ebp), %eax
	 movl $1, %ecx
	 addl %ecx, %eax
	 movl %eax, -32(%ebp)
	 movl -32(%ebp), %eax
	 movl %eax, -4(%ebp)
	 movl -8(%ebp), %eax
	 movl $0, %ecx
	 addl %ecx, %eax
	 movl %eax, -8(%ebp)
	 jmp label_8
label_9:
	 movl -8(%ebp), %eax
	 movl %eax, -36(%ebp)
	 movl -36(%ebp), %eax
	 leave
	 ret
	 leave
	 ret
