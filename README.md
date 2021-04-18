# Convolution
## The code written is higly experimental and bad written, not mantained
Assignement 02 for the HPC course @ University of Trieste, DSSC master degree
## Project
This project is an implemetation of a convolution algorithm using openmpi and openmp
## How to compile
To compile use the script provided or the makefile
These are the commands:

- *MPI version*: `mpicc mpi_qq_scatter.c -lm`

- *openMP version*: `gcc -fopenmp omp_conv.c -lm`

- complie it by running `compile.sh`, produces 2 executables, `blur_mpi`, `blur_mp`

## How to RUN

- *MPI version*: `mpirun mpi_executable KERNEL_TYPE KERNEL_SIZE INPUT_FILE OUPUT_FILE`

- *openMP version*: `omp_executable KERNEL_TYPE KERNEL_SIZE INPUT_FILE OUPUT_FILE`
### Note on kernels
By now are implemented 3 types of kernels:

- **MEAN kernel** -> CODE 0 size arbitrary

- **GAUSSIAN kernel** -> CODE 1 size arbitrary

- **SHARPEN kernel** -> CODE 2 (N.B.: kernel size bounded to 3 x 3 )
