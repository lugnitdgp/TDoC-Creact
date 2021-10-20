#!/bin/bash

make
./transpiler $1
gcc output.c -o output
echo -e "Executing $1\n"
./output
echo ""
