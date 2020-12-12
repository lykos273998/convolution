#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>

void print_matrix(short int *matrix, unsigned int nrows, unsigned int ncols){
    for (unsigned int i = 0; i < nrows; i++){
        for (unsigned int j = 0; j < ncols; j++){
            printf("%hi ", matrix[i*ncols+j] );
        }
        printf("\n");
    }
}
void print_kernel(float *matrix, unsigned int nrows){
    unsigned int ncols = nrows;
    for (unsigned int i = 0; i < nrows; i++){
        for (unsigned int j = 0; j < ncols; j++){
            printf("%f ", matrix[i*ncols+j] );
        }
        printf("\n");
    }
}

void get_MEAN_kernel(float* kernel, unsigned int kernel_size){
    for(int i = 0; i<kernel_size; i++){
        for(int j = 0; j<kernel_size; j++){
            kernel[i*kernel_size+j] = 1./(kernel_size*kernel_size);
        }
    }
}





int main(int argc, char**argv){
    unsigned short int kernel_type;
    unsigned int kernel_size;
    float* kernel;
    char* input_file; 
    if(argc < 3){
        printf("usage: \n ./executable KERNEL_TYPE<0,1,..> KERNEL_SIZE INPUT_FILE \n ---PGM FILES ALLOWED ONLY--- \n");
        return 0;
    }
    kernel_type = atoi(argv[1]);
    kernel_size = atoi(argv[2]);
    input_file = argv[3];
    kernel = (float*)malloc(kernel_size*kernel_size*sizeof(float));
    switch (kernel_type){
        case 0:
        //mean kernel
        get_MEAN_kernel(kernel,kernel_size);
        break;

        default:
        printf("Unknown kernel\nThis is the list of Implemented kernels: \n");
        printf("0 --> Mean Kernel \n");
        return 0;

    }
    
    get_MEAN_kernel(kernel,kernel_size);
    print_kernel(kernel,kernel_size);


    return 0;
}