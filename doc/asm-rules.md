<link href="http://kevinburke.bitbucket.org/markdowncss/markdown.css" rel="stylesheet"></link>



This files describes the details of manner in which PCAT compiler is compiling AST into assembly code.

# Overview

The main aim is to compile AST into assembly code. To accomplish this goal, the PCAT compiler will generate file `prog.S` in `gas` format, which will later turn into `prog.o` by gcc.
The entrance and supporting routines would be written in `sup.c` then turned into `sup.o` by gcc.
Finally, a call to `gcc` would generate the executable file `prog`.

# Assembly

In this part we are mainly concerning about how to transfer AST into Assembly code.

There's not so-called global variables. All variables are allocated on stack.
To make things simpler, now only (signed) integer and real, both occupying 4-bytes, are supported. Boolean is treated as a 4-byte integer, with 0 representing `FALSE` and non-zero values `TRUE`.


The code follows the __cdecl__ calling conversion. The static link is passed as the first paramenter to the callee in form of a pointer to the destination frame.
To avoid register allocation, all temporary variables are also stored on the stack, which means
each calculation would take operants from stack, compute, and store the result to the stack.

## Arithmetic operations
In the following examples, assume that `a` is stored at `-4(%ebp)` while `b` at `-8(%ebp)`
 
    #c=a+b
        movl	-8(%ebp), %eax
        movl	-4(%ebp), %edx 
        addl	%edx, %eax
        movl	%eax, -12(%ebp)
    #b=a-b
        movl	-8(%ebp), %eax
        movl	-4(%ebp), %edx
        subl	%eax, %edx
        movl	%edx, %eax
        movl	%eax, -8(%ebp)
    #c=a*b
        movl	-4(%ebp), %eax
        imull	-8(%ebp), %eax
        movl	%eax, -12(%ebp)
    #c=a div b
        movl	-4(%ebp), %eax
        cltd
        idivl	-8(%ebp)
        movl	%eax, -12(%ebp)
    #c=a mod b
        movl	-4(%ebp), %eax
        cltd
        idivl	-8(%ebp)
        movl	%edx, -8(%ebp)
    #int->float
        fildl	-4(%ebp)
        fstps	-12(%ebp)
    #c=a/b with(int->float) a=16(%esp) b=12(%esp)
        fildl	16(%esp)
        fildl	12(%esp)
        fdivrp	%st, %st(1)
        fstps	8(%esp)

The integer-to-real conversion can be done by `fildl` instruction.

## Frame
The __cdecl__ calling conversion says that
> In cdecl, subroutine arguments are passed on the stack. 
> 
> Integer values and memory addresses are returned in the EAX register, floating point values—in the ST0 x87 register. 
> 
> Registers EAX, ECX, and EDX are caller-saved, and the rest are callee-saved. 
> 
> The x87 floating point registers ST0 to ST7 must be empty (popped or freed) when calling a new function, and ST1 to ST7 must be empty on exiting a function.



# Supporting routing

TODO: the READ/WRITE

# TODO list
+ Support for array
+ support for record