#!/bin/bash

make
if [ -f "output.c" ]; then
rm "output.c"
fi
if [ -f "output" ]; then
rm "output"
fi
./transpiler1 $1
if [ -f "output.c" ]; then
gcc output.c -o output
./output
fi

exit