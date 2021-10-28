#!/bin/bash

mingw64-make.exe
./Transpiler1 $1
gcc output.c -o output
./output