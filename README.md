# Convolution
Assignement 02 for the HPC course @ University of Trieste, DSSC master degree
## Project
This project is an implemetation of a convolution algorithm using openmpi and openmp.
## How to compile
To compile use the script provided or the makefile
These are the commands:

- *MPI version*: `mpicc mpi_conv.c -lm`

- *openMP version*: `gcc -fopenmp omp_conv.c -lm`

## How to RUN

- *MPI version*: `mpirun mpi_executable KERNEL_TYPE KERNEL_SIZE INPUT_FILE OUPUT_FILE`

- *openMP version*: `omp_executable KERNEL_TYPE KERNEL_SIZE INPUT_FILE OUPUT_FILE`
