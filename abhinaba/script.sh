#!/bin/bash

make
./transpiler $1
gcc output.c -o output
./output