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

There are four kinds of tokens, namely
literals, keywords, operators and special ones.

Literals can be one of following types
* Identifiers
* Integer
* Real
* String

Keywords are reserved words. Operators
(including delimiter) are used for
operating or seperating.

Tab and space (which is not in a string or comment)
are special ones, so are new lines (which are not in a comment)
and comments. They should be
totally ingored.

#### Dealing
Tokens are handles in the following order.
##### Identifiers and Keywords
Using `[a-zA-Z_][a-zA-Z_0-9]*` to catch
identifiers and keywords. Check if the token
is a keyword, and if so, treat it properly.
##### Reals
A little extension to language specification:
support `1.` or `.1` besided `1.1` using
`[0-9]+\.[0-9]+|[0-9]+\.|\.[0-9]+`.
##### Integers
Catch by `[0-9][0-9]*`
##### String
Catch by `\".*?(?:\"|\n|\r)` in order
to forced close an open string (one that
isn't properly closed)
##### Operators
Catch exactly what they look like.
##### New lines
Catch and ingore
##### Comments
Catch normal comments by
`\(\*(?:.|\n|\r)*?\*\)`, and then open comment
by `\(\*(?:.|\n|\r)*`.
##### Error Tokens
Anything failed to fall into at least one type
of tokens mentioned above is treated as error.
We will try to ingore it by moving the pointer to the current charactor by one.

#### Error Tolerance
Several error auto correctings are performed, 
including

* Stripping overlong string literals and identifiers
* Setting overflowed integer literals to `MAX_INT`
* Ignore illegal charactors in string literals
* Forcing closing open string literals and comments


### Syntax Analysis
TODO

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
