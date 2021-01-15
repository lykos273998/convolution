#!/bin/bash

cd roba_varia/convolution
module load openmpi-4.0.5+gnu-9.3.0

in_file="image.pgm"
out_file="out.pgm"

./compile.sh
gcc read_write_pgm_image.c -o rw

kernel_size=101
kernel_type="1"
w="0.2"
mpi_path_="timing/mpi/weak/"
omp_path_="timing/omp/weak/"

./rw 65535 2000 2000
procs=1

time_file=${omp_path_}"w".$procs
export OMP_NUM_THREADS=$procs
/usr/bin/time ./blur_omp $kernel_type $kernel_size $w $in_file $out_file 2> $time_file

time_file=${mpi_path_}"w".$procs
/usr/bin/time mpirun -np $procs --mca btl '^openib' blur_mpi $kernel_type $kernel_size $w $in_file $out_file 2> $time_file

./rw 65535 2000 4000
procs=2

time_file=${omp_path_}"w".$procs
export OMP_NUM_THREADS=$procs
/usr/bin/time ./blur_omp $kernel_type $kernel_size $w $in_file $out_file 2> $time_file

time_file=${mpi_path_}"w".$procs
/usr/bin/time mpirun -np $procs --mca btl '^openib' blur_mpi $kernel_type $kernel_size $w $in_file $out_file 2> $time_file

./rw 65535 4000 4000
procs=4

time_file=${omp_path_}"w".$procs
export OMP_NUM_THREADS=$procs
/usr/bin/time ./blur_omp $kernel_type $kernel_size $w $in_file $out_file 2> $time_file

time_file=${mpi_path_}"w".$procs
/usr/bin/time mpirun -np $procs --mca btl '^openib' blur_mpi $kernel_type $kernel_size $w $in_file $out_file 2> $time_file

./rw 65535 4000 8000
procs=8

time_file=${omp_path_}"w".$procs
export OMP_NUM_THREADS=$procs
/usr/bin/time ./blur_omp $kernel_type $kernel_size $w $in_file $out_file 2> $time_file

time_file=${mpi_path_}"w".$procs
/usr/bin/time mpirun -np $procs --mca btl '^openib' blur_mpi $kernel_type $kernel_size $w $in_file $out_file 2> $time_file

./rw 65535 4000 10000
procs=10

time_file=${omp_path_}"w".$procs
export OMP_NUM_THREADS=$procs
/usr/bin/time ./blur_omp $kernel_type $kernel_size $w $in_file $out_file 2> $time_file

time_file=${mpi_path_}"w".$procs
/usr/bin/time mpirun -np $procs --mca btl '^openib' blur_mpi $kernel_type $kernel_size $w $in_file $out_file 2> $time_file

./rw 65535 8000 6000
procs=12

time_file=${omp_path_}"w".$procs
export OMP_NUM_THREADS=$procs
/usr/bin/time ./blur_omp $kernel_type $kernel_size $w $in_file $out_file 2> $time_file

time_file=${mpi_path_}"w".$procs
/usr/bin/time mpirun -np $procs --mca btl '^openib' blur_mpi $kernel_type $kernel_size $w $in_file $out_file 2> $time_file

./rw 65535 14000 4000
procs=14

time_file=${omp_path_}"w".$procs
export OMP_NUM_THREADS=$procs
/usr/bin/time ./blur_omp $kernel_type $kernel_size $w $in_file $out_file 2> $time_file

time_file=${mpi_path_}"w".$procs
/usr/bin/time mpirun -np $procs --mca btl '^openib' blur_mpi $kernel_type $kernel_size $w $in_file $out_file 2> $time_file

./rw 65535 8000 8000
procs=16

time_file=${omp_path_}"w".$procs
export OMP_NUM_THREADS=$procs
/usr/bin/time ./blur_omp $kernel_type $kernel_size $w $in_file $out_file 2> $time_file

time_file=${mpi_path_}"w".$procs
/usr/bin/time mpirun -np $procs --mca btl '^openib' blur_mpi $kernel_type $kernel_size $w $in_file $out_file 2> $time_file

./rw 65535 18000 4000
procs=18

time_file=${omp_path_}"w".$procs
export OMP_NUM_THREADS=$procs
/usr/bin/time ./blur_omp $kernel_type $kernel_size $w $in_file $out_file 2> $time_file

time_file=${mpi_path_}"w".$procs
/usr/bin/time mpirun -np $procs --mca btl '^openib' blur_mpi $kernel_type $kernel_size $w $in_file $out_file 2> $time_file

./rw 65535 20000 4000
procs=20

time_file=${omp_path_}"w".$procs
export OMP_NUM_THREADS=$procs
/usr/bin/time ./blur_omp $kernel_type $kernel_size $w $in_file $out_file 2> $time_file

time_file=${mpi_path_}"w".$procs
/usr/bin/time mpirun -np $procs --mca btl '^openib' blur_mpi $kernel_type $kernel_size $w $in_file $out_file 2> $time_file

./rw 65535 22000 4000
procs=22

time_file=${omp_path_}"w".$procs
export OMP_NUM_THREADS=$procs
/usr/bin/time ./blur_omp $kernel_type $kernel_size $w $in_file $out_file 2> $time_file

time_file=${mpi_path_}"w".$procs
/usr/bin/time mpirun -np $procs --mca btl '^openib' blur_mpi $kernel_type $kernel_size $w $in_file $out_file 2> $time_file

./rw 65535 12000 8000
procs=24

time_file=${omp_path_}"w".$procs
export OMP_NUM_THREADS=$procs
/usr/bin/time ./blur_omp $kernel_type $kernel_size $w $in_file $out_file 2> $time_file

time_file=${mpi_path_}"w".$procs
/usr/bin/time mpirun -np $procs --mca btl '^openib' blur_mpi $kernel_type $kernel_size $w $in_file $out_file 2> $time_file