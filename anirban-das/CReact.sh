#!/bin/bash

mingw32-make.exe
./transpiler $1
if [ -f "output.c" ]; then
    gcc output.c -o output
    ./output
    cp output.c target.c
    rm output.c
    rm output
fi