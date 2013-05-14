pcat-compiler
=============

PCAT compiler is a college course project assigned in the course ''Compiler''
by Prof. Bo Yan, offered in Fudan University.

Students are required to implement an compiler of PCAT language, including
Lexical, syntax, semantic analysis and IR generation.


Architecture
------------


Details
------------

### Lexical Analysis (Tokenizer)

#### Tokens
No more tweaks required. It is sufficient to follow the 
specification.

### Syntax Analysis

As part of the language specification, the syntax is also available at
http://web.cecs.pdx.edu/~apt/cs302_1999/pcat99/pcat99.html

Both concrete and abstract syntax is provided. In this part (syntax analysis),
we use concrete syntax to handle the tokens, and then arrange them into
AST in form of abstract syntax.

The concrete syntax (concrete-syntax-ebnf.txt) is provided in form of EBNF, thus we
convert it to BNF (concrete-syntax-bnf.txt) following directions from
http://web.eecs.utk.edu/~bvz/cs461/notes/bison/


### Semantic Analysis
TODO


### IR Generation

TODO



Resources
---------

We didn't receive enough information and supplimentary materials from TA,
so we must search for it manually.


A [similar course](http://web.cecs.pdx.edu/~harry/compilers/syllabus.html) 
with project on PCAT was offered by 
[Prof. Harry H.Porter](http://web.cecs.pdx.edu/~harry/) in 2005.
Although in Java without solution, its supplimentary materials
as well as lectures closely linked to the PCAT compiler, still mean a lot
to us.

Another [course](http://lambda.uta.edu/cse5317/spring02/)
offered by Prof Fegras also contains PCAT compiler implementation
as it course project.

Students are required to implement an compiler of PCAT language, including
Lexical, syntax, semantic analysis and IR generation.


### Misc

The [PCAT Language Specification](http://web.cecs.pdx.edu/~harry/compilers/PCATLangSpec.pdf)


### Syntax and Semantic Analysis

For Syntax Analysis,
the lectures involved is 
[SyntaxPart1](http://web.cecs.pdx.edu/~harry/compilers/slides/SyntaxPart1.pdf), 
[SyntaxPart2](http://web.cecs.pdx.edu/~harry/compilers/slides/SyntaxPart2.pdf) and 
[SyntaxPart3](http://web.cecs.pdx.edu/~harry/compilers/slides/SyntaxPart3.pdf).
For semantic analysis one may refer to
[SemanticsPart1](http://web.cecs.pdx.edu/~harry/compilers/slides/SemanticsPart1.pdf)
and 
[SemanticsPart2](http://web.cecs.pdx.edu/~harry/compilers/slides/SemanticsPart2.pdf).

Related Projects are [Project 4](http://web.cecs.pdx.edu/~harry/compilers/slides/Project4.pdf),
[Project 5](http://web.cecs.pdx.edu/~harry/compilers/slides/Project5.pdf)
and [Project 6](http://web.cecs.pdx.edu/~harry/compilers/slides/Project6.pdf).
One can also refer to [AST Summary](web.cecs.pdx.edu/~harry/compilers/p4/ASTSummary.pdf) and 
[PCAT Errors](http://web.cecs.pdx.edu/~harry/compilers/p6/PCATErrors.pdf)

### IR Generation

Prof. Porter covered the SPARC architecture in 3 courses
([Part 1](http://web.cecs.pdx.edu/~harry/compilers/slides/SPARC-Part1.pdf),
[Part 2](http://web.cecs.pdx.edu/~harry/compilers/slides/SPARC-Part2.pdf)
and
[Part 3](http://web.cecs.pdx.edu/~harry/compilers/slides/SPARC-Part3.pdf)),
then covered Code Generation([Part 1](http://web.cecs.pdx.edu/~harry/compilers/slides/CodeGen1.pdf),
[Part 2](http://web.cecs.pdx.edu/~harry/compilers/slides/CodeGen2.pdf),
[Part 3](http://web.cecs.pdx.edu/~harry/compilers/slides/CodeGen3.pdf)), 
[Tolmach's AST](http://web.cecs.pdx.edu/~harry/compilers/slides/TolmachsAST.pdf)
and [Parameter Passing](http://web.cecs.pdx.edu/~harry/compilers/slides/ParamPassing.pdf).

Related Projects are 
[Project 8](http://web.cecs.pdx.edu/~harry/compilers/slides/Proj8.pdf),
[Project 9](http://web.cecs.pdx.edu/~harry/compilers/slides/Proj9.pdf) and
[Project 10](http://web.cecs.pdx.edu/~harry/compilers/slides/Proj10.pdf).


Also refer to
[Review of AST](http://web.cecs.pdx.edu/~harry/compilers/p8/ReviewOfAST.pdf)



Authors
-------
Alan Tian (Tian Yingtao), Wandsea (Wang Xi), Edward Cheng (Cheng Munan), Jifan Chen
