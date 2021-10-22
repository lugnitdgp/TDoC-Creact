#!/bin/bash

python ./python-transpiler.py $1
gcc output.c -o output
./output