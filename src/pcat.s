	 .text
	 .globl MainEntry
MainEntry:
	 pushl %ebp
	 movl %esp, %ebp
	 andl $-16, %esp
	 subl $-12, %esp
	 andl $-16, %esp
	 movl -4(%ebp), %eax
	 movl %eax, -4(%ebp)
	 movl -4(%ebp), %eax
	 movl %eax, -8(%ebp)
	 movl -8(%ebp), %eax
	 push %eax
	 call print_int
	 addl $4, %esp
	 call print_line
	 leave
	 ret
