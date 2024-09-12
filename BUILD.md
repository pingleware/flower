# CMake build for FLOWER

```
mkdir build
cd build
cmake ..
make
```

to run,

```
bin/flower-macos # for MacOS
bin/flower-linux # for Linux
bin/flower-windows # for Windows
```

## CLI Options

```
flower 04.00
Syntax:

        flower [-abct] file1.c [file2.c] [...] [-x exFunc1 [exFunc2] [...]]
  or
        flower [-abct] @filelist.txt [-x exFunc1 [exFunc2] [...]]
  or
        flower [-abct] @filelist.txt [-x @exFileNm]

Writes a function cross reference to standard output

Options:
        -a all reports
        -b functions and which functions call them
        -c functions and which functions they call
        -t call tree
        -x exclude these functions

Default is all three reports
```
