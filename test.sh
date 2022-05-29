#!/bin/bash

size=${#1}
cpus=$((2*($size-1)))

mpic++ --prefix /usr/local/share/OpenMPI -o pro pro.cpp 

mpirun --prefix /usr/local/share/OpenMPI -oversubscribe -np $cpus pro $1
rm -f pro 