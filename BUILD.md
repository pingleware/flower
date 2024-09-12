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

## Building for Windows
From the root path .\flower, run the following commands in order,

```
cmake -G "Visual Studio 17 2022" -A Win32 -S . -B "build32"
cmake -G "Visual Studio 17 2022" -A x64 -S . -B "build64"
cmake --build build32 --config Release
cmake --build build64 --config Release
```

## Building for Linux (on WSL)

First update,

```
sudo apt update
sudo apt upgrade -y
sudo apt install build-essentials -y
```

then build,

```
mkdir build
cd build
sudo cmake ..
sudo make
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
