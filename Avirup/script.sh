#!/bin/bash
make
./day1 $1 $2
gcc -o output $2 
./output
