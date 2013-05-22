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


The code follows the __cdecl__ calling conversion. The static link is passed as the first parameter to the callee in form of a pointer to the destination frame.
To avoid register allocation, all temporary variables are also stored on the stack, which means
each calculation would take operands from stack, compute, and store the result to the stack.

## Constant values
All constant values should be placed at the end of asm code.

Integer constance can be represented as immediate number, the real one need some tricks like
    
    (global)
        .globl	a
        .align 4
        .type	a, @object
        .size	a, 4
    a:
        .long	1092616192

    (local)
        movl	.LC0, %eax
        movl	%eax, -4(%ebp)

        .align 4
    .LC0:
        .long	1093664768
      
For strings, we may need

        .globl	b
        .section	.rodata
    .LC0:
        .string	"10"
        .data
        .align 4
        .type	b, @object
        .size	b, 4
    b:
        .long	.LC0

## Arithmetic operations

See https://en.wikibooks.org/wiki/X86_Assembly for more examples

In the following examples, assume that integer `a` ,`b` and `c` are stored at `-4(%ebp)` `-8(%ebp)` and `-12(%ebp)` (recall that Boolean is also treated as integer),
float `d` , `e` and 'f' are stored at `-12(%ebp)` , `-16(%ebp)` and `-20(%ebp)`.

    #c=a+b
        movl	-8(%ebp), %eax
        movl	-4(%ebp), %edx 
        addl	%edx, %eax
        movl	%eax, -12(%ebp)
    #c=a-b
        movl	-8(%ebp), %eax
        movl	-4(%ebp), %edx
        subl	%eax, %edx
        movl	%edx, %eax
        movl	%eax, -12(%ebp)
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
    #f=d+e
        flds	-12(%ebp)
        fadds	-16(%ebp)
        fstps	-20(%ebp)
    #f=d-e
        flds	-12(%ebp)
        fsubs	-16(%ebp)
        fstps	-20(%ebp)
    #f=d*e
        flds	-12(%ebp)
        fmuls	-16(%ebp)
        fstps	-20(%ebp)
    #f=d/e
        flds	-12(%ebp)
        fdivs	-16(%ebp)
        fstps	-20(%ebp)
    #c=a&&b
        cmpl	$0, -4(%ebp)
        je	.L2
        cmpl	$0, -8(%ebp)
        je	.L2
        movl	$1, %eax
        jmp	.L3
    .L2:
        movl	$0, %eax
    .L3:
        movl	%eax, -12(%ebp)

    #c=a||b
        cmpl	$0, -4(%ebp)
        jne	.L2
        cmpl	$0, -8(%ebp)
        je	.L3
    .L2:
        movl	$1, %eax
        jmp	.L4
    .L3:
        movl	$0, %eax
    .L4:
        movl	%eax, -12(%ebp)

    #c=!a
        cmpl	$0, -4(%ebp)
        sete	%al
        movzbl	%al, %eax
        movl	%eax, -12(%ebp)

for floats see
http://www2.math.uni-wuppertal.de/~fpf/Uebungen/GdR-SS02/opcode_f.html


The integer-to-real conversion can be done by `fildl` instruction.

## Frame

The EBP of the caller is stored at `+0(%ebp)`, the return address is stored at `+4(%ebp)`. 
The first parameter (here static links) is stored at `+8(%ebp)`, 
the second is at `+12(%ebp)`, etc.
Local variables are stored at `-4(%ebp)`,`-8(%ebp)`, etc.

Following __cdecl__ calling conversion,
integer values are returned in the `%eax` register, floating point values—in the `ST0` x87 register. 
Registers `%eax`, `%ecx`, and `%edx` are caller-saved, and the rest are callee-saved. 
Float point registers other than `st0` is not used.

The static link is stored at `+8(%ebp)`. To set up static link for next frame,
first load it by `movl +8(%ebp),%eax`, then do `movl (%eax),%eax` for `scope_now-scope_next` times.
If `scope_next` > `scope_now`, set static link as `%ebp`

Register usage is: `%eax` for left operand, `%ecx` for right operand, `%edx` for memory dereference and expanding (like for `idivl`)
Loading local variable can be simply done by something like `movl -8(%ebp),%eax`.
To load variables into `%eax` via static link, 
first load it by `movl +8(%ebp),%eax`, then do `movl (%eax),%eax` for `scope_now-scope_next` times, 
finally add the offset to get the finally address for accessing by something like
`movl -4(%eax),eax`. 
The same holds for loading into `%edx`

Variable initialization is done upon the entry just like normal assignment.
But this value should be known at the compiling time.

All local variables (including declared and _temporary_) are allocated on
the stack. To make things simpler, temporary variables are not reused.

To make stack align to 16-byte boundary, a common trick is 

	andl	$-16, %esp
	subl	$32, %esp


### Example

so for a piece PCAT code

    (* callee is at scope 1 *)
    PROCEDURE callee(a : INTEGER, b : INTEGER, c : INTEGER ) : INTEGER is
    BEGIN
        RETURN a + b + c;
    END;
    (* x is at scope 1 *)
    VAR x : INTEGER; (*stored at `-4(%ebp)`)
            
    ...
    
            (* caller is at scope 3 *)
            PROCEDURE caller () : INTEGER is
                var ret : INTEGER;    (* stored at `-4(%ebp)` *)
                var x : INTEGER := 1; (* stored at `-8(%ebp)` *)
            BEGIN
                ret := callee(1,2,3);
                ret := x + ret;
                RETURN ret;
            END;

the generated code for caller should be

        .global caller
        .type	caller, @function
    caller:
        pushl   %ebp
        movl    %esp,%ebp
        # make some room
        subl	$16, %esp
        # variable initialization
        movl    $1,-8(%ebp)
        # push parameters
        pushl   $3
        pushl   $2
        pushl   $1
        # set up static link
        movl    +8(%ebp),%eax
        movl    (%eax),%eax
        movl    (%eax),%eax
        pushl   %eax
        # call
        call    callee
        # restore stack
        addl    $16,%esp
        # store the result
        movl    %eax,-4(%ebp)
        # load x
        movl    +8(%ebp),%eax
        movl    (%eax),%eax
        movl    (%eax),%eax
        movl    -4(%eax),%eax
        # load ret
        movl    -4(%ebp),%edx
        # plus
        addl    %edx,%eax
        # store
        movl    %eax,%-4(%ebp)
        # pick return value
        movl    %-4(%ebp),%eax
        leave
        ret


# Supporting routing

TODO: the READ/WRITE

# TODO list
+ Support for array
+ support for record