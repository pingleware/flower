# Flower

|                              |                                            |
| ---------------------------- | ------------------------------------------ |
| Copyright                    | (c) 1996, 1997 Craig Schneiderwent        |
| Author                       | Craig Schneiderwent                        |
|                              | 74631.165@compuserve.com                   |
| Date                         | 27-Feb-1997                                |
| Maintainer (this reposiotry) | PRESSPAGE ENTERTAINMENT INC dba PINGLEWARE |

## Purpose

I found a reference to cflow in the comp.lang.c FAQ and it sounded like what I needed: write each function name in a .c source file and the names of the functions it calls to stdout.  But since I couldn't find a working version of cflow, I wrote my own.  It's far from perfect, but it suits my needs - and perhaps yours.  Run without arguments for execution syntax.

Multiple source code files can be specified on the command line, thus

`flower main.c mniplist.c printrpt.c updtlist.c memfunc.c`

or, if you prefer

`flower @filelist.txt`

where filelist.txt is a text file containing names of files, one per line.  This is useful for specifying large numbers of source code files.

Flower prints any or all of three reports: a call tree, functions and the functions they call, and functions and which functions call them.

| OPTION | DESCRIPTION                             |
| :----: | --------------------------------------- |
|   -a   | all reports                             |
|   -b   | functions and which functions call them |
|   -c   | functions and which functions they call |
|   -t   | call tree                               |
|   -x   | exclude these functions                 |

The default is to output all three reports.  The reports are written to stdout.  Progress and error messages are written to stderr.

If you want to exclude functions (maybe they're only for debugging and are #ifdef'd) add -x func1 func2 [...] after your list of files.  If you have many of these  functions, add -x @exFileNm where exFileNm contains one function (to be excluded) per line.

This version was written for OS/2.  Unless I screwed up, it _should_ compile pretty much anywhere.  HA! I hear you say.  Try it.  You'll probably have to change the makefile to invoke your compiler (unless it's called ICC), and your linker (unless it's called LINK386).  There should be a file included in the original zip called cmplopt.txt that details the compile and link options I used (nothing fancy, mostly just checking for bugs).

Don't worry about the .cmd files, they're just OS/2's version of DOS .bat files in this case.

The basic algorithm:

1. Find a token (a parenthesis, a curly brace, a space or a line end)
2. If the token is an opening curly brace
   2a. Increment a counter
   2b. If the counter is 1, we're at the beginning of a function definition
   2b1. Back up to the outermost previous opening parenthesis.
   2b2. Back up to the previous token
   2b3. From present position to the position in 3a is the name of a function
   2b4. Print it
3. If the token is a closing curly brace
   3a. If the counter is > 0 decrement the counter
4. If the token is an opening parenthesis
   4a. If the counter is > 0 we're potentially at the beginning of a called function
   4a1. Back up to the previous token
   4a2. From present position to the position in 4a is potentially a called function - copy it
   4a3. Match the potentially called function against a list of reserved words - if it doesn't match, print it

## NOTE

I do check for circular references - recursion and the Function A calls Function B which calls Function C which calls Function A situation.  A notation is made in the call tree report if such situations are encountered.

## NOTE

Be advised, I ran flower with the -t option on one of my projects comprising about 3K of source lines and the output was over 17K lines.  Be careful when routing this report to a printer - my advice is to route all output to a file first.

## NOTE

Feel free to make use of the -x option.  By adding two debugging functions to be ignored, I reduced the above-mentioned 17K print lines to 4.5K lines.

## Other Limitations

If you do things like

```
#define BEGIN_STRUCT {
#define END_STRUCT   }
```

flower will not work properly.

flower does not understand C++ style comments (single line beginning with // (slash slash).  Giving source code containing this comment style to flower is a good way to force a register dump.

I would suggest you exclude (-x) any macros or pragmas that take arguments.  These, combined with struct declarations mixed in with your source, tend to confuse flower:

```
#pragma alloc_text( codeseg1, myVeryCoolFunction );
struct a {
    short w;
    char  x[ sizeof( w ) ];
    struct b {
        long y;
        long z;
    };
};
```

The functions are stored in linked lists, if you give flower enough source to process, you will run out of RAM.

Each source code file is read in completely before its processing begins.  If your source code files are large enough you will run out of RAM.

The makefile declares a stack of 20K.  If you have a very deep call tree, you may have to re-link with a larger stack.

Root entries for the call tree report are any function that is not (apparently) called.  If a function is only called by passing its address to another function, it will look to flower as if it is not called.

## Return Codes

| CODE | DESCRIPTION                                         |
| :--: | --------------------------------------------------- |
|  0  | Processing ended normally                           |
|  1  | Unable to open or close file                        |
|  2  | Error attempting to locate beginning or end of file |
|  3  | Error attempting to allocate memory                 |
|  4  | fseek position < 0                                  |

## Licensing

On the off-chance anyone cares, this program is free. Use it, abuse it, just don't charge for it.  Take a look at the code - who'd pay for it?

If you recompile, redistribute this stuff, please keep keep all the original source and documentation files together.

Given our litigious society:

## DISCLAIMER

Users of this program must accept this disclaimer of warranty:

"This program is supplied as is.  The original author disclaims all warranties, expressed or implied, including, without limitation, the warranties of merchantability and of fitness for any purpose. The original author assumes no liability for damages, direct or consequential, which may result from the use of this program."

This program was written because _I_ needed it.  I hope you find it useful, but please don't expect ANYTHING in the way of support.


# ZIP file contents

This zipfile should contain

| FILE        | DESCRIPTION                                |
| ----------- | ------------------------------------------ |
| main.c      | -mainline                                  |
| mniplist.c  | -linked list manipulations                 |
| mniplist.h  | -header for linked list manipulations      |
| memfunc.c   | -allocate/initialize structs               |
| memfunc.h   | -header for allocate/initialize structs    |
| printrpt.c  | -print the reports                         |
| printrpt.h  | -header for printing reports               |
| updtlist.c  | -update linked lists                       |
| updtlist.h  | -header for update linked lists            |
| structs.h   | -structs & some #defines                   |
| mydebug.h   | -debugging macros                          |
| cmplopt.txt | -explanation of compile and link options   |
| output.txt  | -sample output                             |
| makefile    | -makefile                                  |
| flower.exe  | -excutable                                 |
| readme.1st  | -you're looking at it                      |
| flower.doc  | -documentation                             |
| file_id.diz | -short description of function             |
| flow.cmd    | -executes flower using its own source code |
| makeme.cmd  | -executes nmake to build flower            |

Documentation on use is in flower.doc, or run flower.exe without any command-line arguments.
