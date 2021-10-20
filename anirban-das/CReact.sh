#!/bin/bash

mingw32-make.exe
./transpiler $1
gcc output.c -o output
./output