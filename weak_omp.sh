#!/bin/bash

cd roba_varia/convolution
module load openmpi-4.0.5+gnu-9.3.0

in_file="images/earth-large.pgm"
out_file="out.pgm"

./compile.sh

ks=11
kernel_type="1"
w="0.2"
timing_path="timing/omp/weak"
for run in {1..3}
do



    for dim in 1 2 3 4 5; 
    do
    procs=dim*dim
    kernel_size=ks*dim
    time_file=$timing_path${run}_"w".$procs
    export OMP_NUM_THREADS=$procs
    echo "executing on ", ${procs}, "  processors"
    /usr/bin/time ./blur_omp $kernel_type $kernel_size $w $in_file $out_file 2> $time_file
    done

done
