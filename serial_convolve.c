#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>

void write_to_pgm(short int *mat,int nrows,int ncols, int max){
    FILE* pgmimg;
    pgmimg = fopen("mbb_convolved.PGM", "wb"); //write the file in binary mode
    fprintf(pgmimg, "P2\n"); // Writing Magic Number to the File
    fprintf(pgmimg, "%d %d\n", ncols, nrows); // Writing Width and Height into the file
    fprintf(pgmimg, "%d\n", max); // Writing the maximum gray value
    int count = 0;
    for (int i = 0; i < nrows; i++) {
        for (int j = 0; j < ncols; j++) {
            fprintf(pgmimg, "%hd ", mat[i * ncols +j]); //Copy gray value from array to file
        }
        fprintf(pgmimg, "\n");
    }
    fclose(pgmimg);

}
void write_to_pgm3(unsigned char *mat,int nrows,int ncols, int max){
    FILE* pgmimg;
    pgmimg = fopen("mbb_convolved.PGM", "wb"); //write the file in binary mode
    fprintf(pgmimg, "P2\n"); // Writing Magic Number to the File
    fprintf(pgmimg, "%d %d\n", ncols, nrows); // Writing Width and Height into the file
    fprintf(pgmimg, "%d\n", max); // Writing the maximum gray value
    int count = 0;
    for (int i = 0; i < nrows; i++) {
        for (int j = 0; j < ncols; j++) {
            fprintf(pgmimg, "%hhu ", mat[i * ncols +j]); //Copy gray value from array to file
        }
        fprintf(pgmimg, "\n");
    }
    fclose(pgmimg);

}

void write_to_pgm_1B(unsigned char *mat,int nrows,int ncols, int max){
    FILE* pgmimg;
    pgmimg = fopen("out.PGM", "wb"); //write the file in binary mode
    fprintf(pgmimg, "P5\n"); // Writing Magic Number to the File
    fprintf(pgmimg, "%d %d\n", ncols, nrows); // Writing Width and Height into the file
    fprintf(pgmimg, "255\n"); // Writing the maximum gray value
    fwrite(mat,sizeof(unsigned char), nrows*ncols,pgmimg);
    fclose(pgmimg);

}

void write_to_pgm_2B(short int *mat,int nrows,int ncols, int max){
    FILE* pgmimg;
    pgmimg = fopen("out.PGM", "wb"); //write the file in binary mode
    fprintf(pgmimg, "P5\n"); // Writing Magic Number to the File
    fprintf(pgmimg, "%d %d\n", ncols, nrows); // Writing Width and Height into the file
    fprintf(pgmimg, "65535\n"); // Writing the maximum gray value
    fwrite(mat,sizeof(short int), nrows*ncols,pgmimg);
    fclose(pgmimg);

}

void read_pgm(short int* image, int* nr, int* nc, int* m, char* filename){
    FILE* pgm_file;
    pgm_file = fopen(filename,"rb");
    int nrows,ncols,max;
    char MAGIC[2];
    fscanf(pgm_file,"%s", MAGIC);
    fscanf(pgm_file,"%d", &ncols);
    fscanf(pgm_file,"%d", &nrows);
    fscanf(pgm_file,"%d", &max);

    image = (short int*)malloc(nrows*ncols*sizeof(short int));
    printf("file opened %s\n%d %d\n%d \n", MAGIC, nrows, ncols, max);
    for(int i = 0; i < nrows; i++){
        for(int j = 0; j < ncols; j++){
            fscanf(pgm_file,"%hd ",image+(i*ncols+j));          
        }
        //fscanf(pgm_file,"%hd\n",image+(i*ncols + (ncols - 1)));
    }
    printf("Imported\n");
    *nr = nrows;
    *nc=ncols;
    *m = max;

}
void read_pgm2(short int* image, int* nr, int* nc, int* m, char* filename){
    FILE* pgm_file;
    pgm_file = fopen(filename,"rb");
    int nrows,ncols,max;
    char MAGIC[2];
    fscanf(pgm_file,"%s", MAGIC);
    fscanf(pgm_file,"%d", &nrows);
    fscanf(pgm_file,"%d", &ncols);
    fscanf(pgm_file,"%d", &max);

    image = (short int*)malloc(nrows*ncols*sizeof(short int));
    printf("file opened %s\n%d %d\n%d \n", MAGIC, nrows, ncols, max);
    fread(image, sizeof(short int), nrows*ncols, pgm_file);

    *nr = nrows;
    *nc=ncols;
    *m = max;
}

unsigned char* read_pgm_1B(int* nr, int* nc, int* m, char* filename){
    FILE* pgm_file;
    pgm_file = fopen(filename,"rb");
    int nrows,ncols,max;
    char MAGIC[2];
    fscanf(pgm_file,"%s", MAGIC);
    fscanf(pgm_file,"%d", &ncols);
    fscanf(pgm_file,"%d", &nrows);
    fscanf(pgm_file,"%d", &max);

    unsigned char* image = (unsigned char*)malloc(nrows*ncols*sizeof(unsigned char));
    printf("file opened %s\n%d %d\n%d \n", MAGIC, nrows, ncols, max);
    fread(image, sizeof(unsigned char), nrows*ncols, pgm_file);
    *nr = nrows;
    *nc=ncols;
    *m = max;
    //write_to_pgm3(image, ncols, nrows, max);
    fclose(pgm_file);
    return image;
}

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

float gauss(float x, float y, float sigma){
    return exp(-(x*x + y*y)/(2*sigma*sigma))/(sigma*pow(2*M_PI,0.5));
}

void get_GAUSSIAN_kernel(float* kernel, unsigned int kernel_size){
    float sum = 0;
    for(int i = 0; i < kernel_size; i++){
        for(int j = 0; j < kernel_size; j++){
            int x = j - kernel_size/2;
            int y = i - kernel_size/2;
            kernel[i*kernel_size + j] = gauss(x,y,kernel_size/2.);
            sum+= kernel[i*kernel_size+j];
        }
    }
    for(int i = 0; i < kernel_size; i++){
        for(int j = 0; j < kernel_size; j++){
            
            kernel[i*kernel_size + j] = kernel[i*kernel_size+j]/sum;
            
        }
    }
    //printf("normalization is %f \n", sum);
}

void get_SHARPEN_kernel(float* kernel, unsigned int kernel_size){
    kernel[0] = 0.;
    kernel[1] = -1.;
    kernel[2] = 0.;
    kernel[3] = -1.;
    kernel[4] = 5.;
    kernel[5] = -1.;
    kernel[6] = 0.;
    kernel[7] = -1.;
    kernel[8] = 0;
    
}



void convolve(short int * source,int nrows,int ncols,float * kernel, int kernel_size, short int *result){
    int s = kernel_size/2;
    int img_index, k_index, res_index;
    //interior convolution
    for(int i = s; i < nrows - s; i++){
        for(int j = s; j < ncols - s; j++){
            res_index = i*ncols + j;
            result[res_index] = 0;
            //single element
            for (int k_i = 0; k_i < kernel_size; k_i ++ ){
              for (int k_j = 0; k_j < kernel_size; k_j ++ ){  
                k_index = k_i * kernel_size + k_j;
                img_index = (i + (k_i - s))*ncols + (j + (k_j - s));
                result[res_index]+= kernel[k_index]*source[img_index];
            }
            }


    }  
    }    
}

void convolve_1B(unsigned char * source,int nrows,int ncols,float * kernel, int kernel_size, unsigned char *result){
    int s = kernel_size/2;
    printf("S %d \n", s);
    int img_index, k_index, res_index;
    //interior convolution
    printf("Processing Interior\n");
    for(int i = s; i < nrows - s; i++){
        for(int j = s; j < ncols - s; j++){
            res_index = i*ncols + j;
            result[res_index] = 0;
            //single element
            for (int k_i = 0; k_i < kernel_size; k_i ++ ){
              for (int k_j = 0; k_j < kernel_size; k_j ++ ){  
                k_index = k_i * kernel_size + k_j;
                img_index = (i + (k_i - s))*ncols + (j + (k_j - s));
                result[res_index]+= kernel[k_index]*source[img_index];
            }
            }


    }  
    }
    //halo up
    printf("Processing HALO UP\n");
    int k_i_start, k_i_end, k_j_start, k_j_end;
    for(int i = 0; i <s; i++){
        for(int j = s; j < ncols - s; j++){
            res_index = i*ncols + j;
            result[res_index] = 0;
            //single element
            k_i_start = s - i;
            k_i_end = kernel_size;
            k_j_start = 0;
            k_j_end = kernel_size;
            for (int k_i = k_i_start; k_i < k_i_end; k_i ++ ){
              for (int k_j = k_j_start; k_j < k_j_end; k_j ++ ){  
                k_index = k_i * kernel_size + k_j;
                img_index = (i + (k_i - s))*ncols + (j + (k_j - s));
                result[res_index]+= kernel[k_index]*source[img_index];
            }
            }

        }
    }  
    //halo down
    printf("Processing HALO DOWN\n");
    for(int i = nrows - s; i < nrows; i++){
        for(int j = s; j < ncols - s; j++){
            res_index = i*ncols + j;
            result[res_index] = 0;
            //single element
            k_i_start = 0;
            k_i_end = s + (nrows - i);
            k_j_start = 0;
            k_j_end = kernel_size;
            for (int k_i = k_i_start; k_i < k_i_end; k_i ++ ){
              for (int k_j = k_j_start; k_j < k_j_end; k_j ++ ){  
                k_index = k_i * kernel_size + k_j;
                img_index = (i + (k_i - s))*ncols + (j + (k_j - s));
                result[res_index]+= kernel[k_index]*source[img_index];
            }
            }

        }
    }  

    //halo left
    printf("Processing HALO LEFT\n");
    for(int i = s; i < nrows - s; i++){
        for(int j = 0; j < s; j++){
            res_index = i*ncols + j;
            result[res_index] = 0;
            //single element
            k_i_start = 0;
            k_i_end = kernel_size;
            k_j_start = s - j;
            k_j_end = kernel_size;
            for (int k_i = k_i_start; k_i < k_i_end; k_i ++ ){
              for (int k_j = k_j_start; k_j < k_j_end; k_j ++ ){  
                k_index = k_i * kernel_size + k_j;
                img_index = (i + (k_i - s))*ncols + (j + (k_j - s));
                result[res_index]+= kernel[k_index]*source[img_index];
            }
            }

        }
    }  
    //halo right
    printf("Processing HALO RIGHT\n");
    for(int i = s; i < nrows - s; i++){
        for(int j = ncols - s; j < ncols; j++){
            res_index = i*ncols + j;
            result[res_index] = 0;
            //single element
            k_i_start = 0;
            k_i_end = kernel_size;
            k_j_start = 0;
            k_j_end = s + nrows - j;
            for (int k_i = k_i_start; k_i < k_i_end; k_i ++ ){
              for (int k_j = k_j_start; k_j < k_j_end; k_j ++ ){  
                k_index = k_i * kernel_size + k_j;
                img_index = (i + (k_i - s))*ncols + (j + (k_j - s));
                result[res_index]+= kernel[k_index]*source[img_index];
            }
            }

        }
    }  

    printf("Processing finished successfully!\n");
}

int spy(char* filename){
    FILE *file = fopen(filename, "r");
    int nrows,ncols,max;
    char MAGIC[2];
    fscanf(file,"%s", MAGIC);
    fscanf(file,"%d", &ncols);
    fscanf(file,"%d", &nrows);
    fscanf(file,"%d", &max);
    fclose(file);
    if(max <= 255){ return 1;}
    else {return 2;}
}

void prc_1B(char* filename, float *kernel, unsigned int kernel_size){
    unsigned char* image;
    int nrows, ncols, max;
    image = read_pgm_1B(&nrows,&ncols,&max, filename);
    unsigned char* convolved_image = (unsigned char*)malloc(nrows*ncols*sizeof(unsigned char));
    convolve_1B(image,nrows,ncols,kernel,kernel_size,convolved_image);
    write_to_pgm_1B(convolved_image,nrows,ncols,max);
    free(image);
    free(convolved_image);
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
    if(kernel_size%2 == 0){
        printf("Please insert an odd kernel size!\n");
        return 0;
    }
    input_file = argv[3];
    kernel = (float*)malloc(kernel_size*kernel_size*sizeof(float));
    switch (kernel_type){
        case 0:
        //mean kernel
        get_MEAN_kernel(kernel,kernel_size);
        printf("Selected MEAN kernel \n");
        break;

        case 1:
        //gaussian kernel
        get_GAUSSIAN_kernel(kernel,kernel_size);
        printf("Selected GAUSSIAN kernel \n");
        break;

        case 2:
        //sharpen kernel
        kernel_size = 3;
        free(kernel);
        kernel = (float*)malloc(kernel_size*kernel_size*sizeof(float));
        get_SHARPEN_kernel(kernel,kernel_size);
        printf("Selected SHARPEN kernel \n ATTENTION FOR THIS KERNEL kernel_size BOUDNED TO 3 \n");
        break;


        default:
        printf("Unknown kernel\nThis is the list of Implemented kernels: \n");
        printf("0 --> Mean Kernel \n");
        return 0;

    }

    printf("---KERNEL---\n");
    print_kernel(kernel,kernel_size);
    
    int N_BYTES = spy(input_file);

    switch (N_BYTES)
    {
    case 1:
        prc_1B(input_file, kernel, kernel_size);
        break;
    case 2:
        //2_BYTE
        break;
    default:
        return 0;
        break;
    }

    //system("eog out.PGM");
    free(kernel);
    return 0;
    
}