#!/bin/bash

mingw32-make.exe
./transpile $1
gcc output.c -o output
./output