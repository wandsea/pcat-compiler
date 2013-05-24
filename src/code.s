	 .text
	 .globl _MainEntry

_MainEntry:
	 pushl %ebp
	 movl %esp, %ebp
	 subl $20, %esp
	 andl $-16, %esp
	 movl $2, %eax
	 movl %eax, -4(%ebp)
	 movl $1, %eax
	 movl %eax, -8(%ebp)
	 movl $1, %eax
	 movl %eax, -4(%ebp)
label_1:
	 movl -4(%ebp), %eax
	 movl $6, %ecx
	 cmpl %ecx, %eax
	 jg label_2
	 movl -4(%ebp), %eax
	 movl %eax, -12(%ebp)
	 movl -12(%ebp), %eax
	 push %eax
	 call _print_int
	 addl $4, %esp
	 call _print_line
	 movl -4(%ebp), %eax
	 movl $1, %ecx
	 addl %ecx, %eax
	 movl %eax, -4(%ebp)
	 jmp label_1
label_2:
	 leave
	 ret
	 movl $1, %eax
	 movl %eax, -4(%ebp)
label_3:
	 movl -4(%ebp), %eax
	 movl $6, %ecx
	 cmpl %ecx, %eax
	 jg label_4
	 movl -4(%ebp), %eax
	 movl %eax, -16(%ebp)
	 movl -16(%ebp), %eax
	 push %eax
	 call _print_int
	 addl $4, %esp
	 call _print_line
	 movl -4(%ebp), %eax
	 movl $2, %ecx
	 addl %ecx, %eax
	 movl %eax, -4(%ebp)
	 jmp label_3
label_4:
	 leave
	 ret
