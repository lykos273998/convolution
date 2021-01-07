#!/bin/bash

mpi_source="mpi_qq.c"
omp_source="omp_double_3.c"

mpicc -O3 -march=native $mpi_source -o blur_mpi -lm
gcc -fopenmp -march=native -O3 $omp_source -o blur_omp -lm
