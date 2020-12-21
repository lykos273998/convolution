#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <omp.h>

#define XWIDTH 256
#define YWIDTH 256
#define MAXVAL 65535


#if ((0x100 & 0xf) == 0x0)
#define I_M_LITTLE_ENDIAN 1
#define swap(mem) (( (mem) & (short int)0xff00) >> 8) + ( ((mem) & (short int)0x00ff) << 8)
#else
#define I_M_LITTLE_ENDIAN 0
#define swap(mem) (mem)
#endif



void write_pgm_image( void *image, int maxval, int xsize, int ysize, const char *image_name)
/*
 * image        : a pointer to the memory region that contains the image
 * maxval       : either 255 or 65536
 * xsize, ysize : x and y dimensions of the image
 * image_name   : the name of the file to be written
 *
 */
{
  FILE* image_file; 
  image_file = fopen(image_name, "w"); 
  
  // Writing header
  // The header's format is as follows, all in ASCII.
  // "whitespace" is either a blank or a TAB or a CF or a LF
  // - The Magic Number (see below the magic numbers)
  // - the image's width
  // - the height
  // - a white space
  // - the image's height
  // - a whitespace
  // - the maximum color value, which must be between 0 and 65535
  //
  // if he maximum color value is in the range [0-255], then
  // a pixel will be expressed by a single byte; if the maximum is
  // larger than 255, then 2 bytes will be needed for each pixel
  //

  int color_depth = 1 + ( maxval > 255 );

  fprintf(image_file, "P5\n# generated by\n# put here your name\n%d %d\n%d\n", xsize, ysize, maxval);
  
  // Writing file
  fwrite( image, 1, xsize*ysize*color_depth, image_file);  

  fclose(image_file); 
  return ;

  /* ---------------------------------------------------------------
     TYPE    MAGIC NUM     EXTENSION   COLOR RANGE
           ASCII  BINARY
     PBM   P1     P4       .pbm        [0-1]
     PGM   P2     P5       .pgm        [0-255]
     PPM   P3     P6       .ppm        [0-2^16[
  
  ------------------------------------------------------------------ */
}


void read_pgm_image( void **image, int *maxval, int *xsize, int *ysize, const char *image_name)
/*
 * image        : a pointer to the pointer that will contain the image
 * maxval       : a pointer to the int that will store the maximum intensity in the image
 * xsize, ysize : pointers to the x and y sizes
 * image_name   : the name of the file to be read
 *
 */
{
  FILE* image_file; 
  image_file = fopen(image_name, "r"); 

  *image = NULL;
  *xsize = *ysize = *maxval = 0;
  
  char    MagicN[2];
  char   *line = NULL;
  size_t  k, n = 0;
  
  // get the Magic Number
  k = fscanf(image_file, "%2s%*c", MagicN );

  // skip all the comments
  k = getline( &line, &n, image_file);
  while ( (k > 0) && (line[0]=='#') )
    k = getline( &line, &n, image_file);

  if (k > 0)
    {
      k = sscanf(line, "%d%*c%d%*c%d%*c", xsize, ysize, maxval);
      if ( k < 3 )
	fscanf(image_file, "%d%*c", maxval);
    }
  else
    {
      *maxval = -1;         // this is the signal that there was an I/O error
			    // while reading the image header
      free( line );
      return;
    }
  free( line );
  
  int color_depth = 1 + ( *maxval > 255 );
  unsigned int size = *xsize * *ysize * color_depth;
  
  if ( (*image = (char*)malloc( size )) == NULL )
    {
      fclose(image_file);
      *maxval = -2;         // this is the signal that memory was insufficient
      *xsize  = 0;
      *ysize  = 0;
      return;
    }
  
  if ( fread( *image, 1, size, image_file) != size )
    {
      free( image );
      image   = NULL;
      *maxval = -3;         // this is the signal that there was an i/o error
      *xsize  = 0;
      *ysize  = 0;
    }  

  fclose(image_file);
  return;
}


void swap_image( void *image, int xsize, int ysize, int maxval )
/*
 * This routine swaps the endianism of the memory area pointed
 * to by ptr, by blocks of 2 bytes
 *
 */
{
  if ( maxval > 255 )
    {
      // pgm files has the short int written in
      // big endian;
      // here we swap the content of the image from
      // one to another
      //
      unsigned int size = xsize * ysize;
      for ( int i = 0; i < size; i+= 2 )
  	((unsigned short int*)image)[i] = swap(((unsigned short int*)image)[i]);
    }
  return;
}



void write_to_pgm_1B(unsigned char *mat,int nrows,int ncols, int max){
    FILE* pgmimg;
    pgmimg = fopen("out.PGM", "w"); //write the file in binary mode
    fprintf(pgmimg, "P5\n"); // Writing Magic Number to the File
    fprintf(pgmimg, "%d %d\n", ncols, nrows); // Writing Width and Height into the file
    fprintf(pgmimg, "255\n"); // Writing the maximum gray value
    fwrite(mat,sizeof(unsigned char), nrows*ncols,pgmimg);
    fclose(pgmimg);

}

void write_to_pgm_2B(void *mat,int nrows,int ncols, int max){
    FILE* pgmimg;
    pgmimg = fopen("out.PGM", "w"); //write the file in binary mode
    fprintf(pgmimg, "P5\n"); // Writing Magic Number to the File
    fprintf(pgmimg, "%d %d\n", ncols, nrows); // Writing Width and Height into the file
    fprintf(pgmimg, "65535\n"); // Writing the maximum gray value
    fwrite(mat,1, nrows*ncols*2,pgmimg);
    fclose(pgmimg);

}

void write_to_pgm_2B_ASCII(unsigned short int*mat,int nrows,int ncols, int max){
    FILE* pgmimg;
    pgmimg = fopen("out.PGM", "w"); //write the file in binary mode
    fprintf(pgmimg, "P2\n"); // Writing Magic Number to the File
    fprintf(pgmimg, "%d %d\n", ncols, nrows); // Writing Width and Height into the file
    fprintf(pgmimg, "65535\n"); // Writing the maximum gray value
    for(int i = 0; i< nrows; i++){
        for(int j = 0; j<ncols; j++){
            //printf("%d ", mat[i*ncols+j]);
            fprintf(pgmimg, "%d ", (int)mat[i*ncols+j]);
        }
        fprintf(pgmimg, "\n");
    }
    fclose(pgmimg);

}

void print_matrix(unsigned short int *matrix, unsigned int nrows, unsigned int ncols){
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
            printf("%f ", matrix[i*ncols+j]);
        }
        printf("\n");
    }
}

void get_MEAN_kernel(float* kernel, unsigned int kernel_size){
    float cc = 1./(kernel_size*kernel_size);
    for(int i = 0; i<kernel_size; i++){
        for(int j = 0; j<kernel_size; j++){
            kernel[i*kernel_size+j] = cc;
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
    float cc = 0;
    for(int i = 0; i < kernel_size; i++){
        for(int j = 0; j < kernel_size; j++){
            
            kernel[i*kernel_size + j] = kernel[i*kernel_size+j]/sum;
            cc += kernel[i*kernel_size + j];
            
        }
    }
    printf("normalization is %f \n", cc);
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

void convolve_1B(unsigned char * source,int nrows,int ncols,float * kernel, int kernel_size, unsigned char *result){
    
    //printf("S %d \n", s);
    
    //interior convolution

    #pragma omp parallel 
    {
    int k_i_start, k_i_end, k_j_start, k_j_end, img_index, res_index, k_index;
    float tmp;
    int s = kernel_size/2;
    
    //printf("Processing Interior\n");
    //int ns = nrows/omp_get_num_threads();
    int ns = s + 1;

    //printf("Processing Interior\n");
    #pragma omp for nowait
    for(int cc = s; cc < nrows - ns - s; cc+=ns){    
        for(int i = cc; i < cc+ns; i++){
            for(int j = s; j < ncols - s; j++){
            res_index = i*ncols + j;
            result[res_index] = 0;
            tmp = 0.;
        //single element
        for (int k_i = 0; k_i < kernel_size; k_i ++ ){
        for (int k_j = 0; k_j < kernel_size; k_j ++ ){  
            k_index = k_i * kernel_size + k_j;
            img_index = (i + (k_i - s))*ncols + (j + (k_j - s));
            tmp += kernel[k_index]*source[img_index];
        }
        }
        result[res_index] = tmp;


    }  
    }
    }
    
    
    
    //remainder
    #pragma omp for schedule(dynamic,ns) nowait
        for(int i = ((nrows - s)/ns)*ns; i < nrows - s; i++){
                for(int j = s; j < ncols - s; j++){
                    res_index = i*ncols + j;
                    result[res_index] = 0;
                    tmp = 0.;
                    //single element
                    for (int k_i = 0; k_i < kernel_size; k_i ++ ){
                    for (int k_j = 0; k_j < kernel_size; k_j ++ ){  
                        k_index = k_i * kernel_size + k_j;
                        img_index = (i + (k_i - s))*ncols + (j + (k_j - s));
                        tmp+= kernel[k_index]*source[img_index];
                    }
                    }
                    result[res_index] = tmp;
    }
    }
    
    
    //img_index, k_index, res_index;
    //halo up
    //printf("Processing HALO UP\n");
    
    
    
    //printf("Processing HALO UP\n");
    #pragma omp for schedule(dynamic,ns) nowait
    for(int i = 0; i <s; i++){
        for(int j = s; j < ncols - s; j++){
            res_index = i*ncols + j;
            result[res_index] = 0;
            //single element
            k_i_start = s - i;
            k_i_end = kernel_size;
            k_j_start = 0;
            k_j_end = kernel_size;
            tmp = 0.;
            //single element
            for (int k_i = k_i_start; k_i < k_i_end; k_i ++ ){
            for (int k_j = k_j_start; k_j < k_j_end; k_j ++ ){ 
                k_index = k_i * kernel_size + k_j;
                img_index = (i + (k_i - s))*ncols + (j + (k_j - s));
                tmp += kernel[k_index]*source[img_index];
            }
            }
            result[res_index] = tmp;

        }
    }
    //halo down
    
    
    
    //printf("Processing HALO DOWN\n");
    #pragma omp for schedule(dynamic,ns) nowait
    for(int i = nrows - s; i < nrows; i++){
        for(int j = s; j < ncols - s; j++){
            res_index = i*ncols + j;
            result[res_index] = 0;
            //single element
            k_i_start = 0;
            k_i_end = s + (nrows - i);
            k_j_start = 0;
            k_j_end = kernel_size;
            tmp = 0.;
            //single element
            for (int k_i = k_i_start; k_i < k_i_end; k_i ++ ){
            for (int k_j = k_j_start; k_j < k_j_end; k_j ++ ){  
                k_index = k_i * kernel_size + k_j;
                img_index = (i + (k_i - s))*ncols + (j + (k_j - s));
                float c1 = kernel[k_index];
                float c2 = source[img_index];
                tmp += c1*c2;
            }
            }
            result[res_index] = tmp;

        }
    }  
    

    //halo left
    
    #pragma omp for schedule(dynamic,ns) nowait
    //printf("Processing HALO LEFT\n");
    for(int i = s; i < nrows - s; i++){
        for(int j = 0; j < s; j++){
            res_index = i*ncols + j;
            result[res_index] = 0;
            //single element
            k_i_start = 0;
            k_i_end = kernel_size;
            k_j_start = s - j;
            k_j_end = kernel_size;
            tmp = 0.;
            //single element
            for (int k_i = k_i_start; k_i < k_i_end; k_i ++ ){
            for (int k_j = k_j_start; k_j < k_j_end; k_j ++ ){  
                k_index = k_i * kernel_size + k_j;
                img_index = (i + (k_i - s))*ncols + (j + (k_j - s));
                tmp+= kernel[k_index]*source[img_index];
            }
            }
            result[res_index] = tmp;

        }
    }  
    
    //halo right
    
    
    //printf("Processing HALO RIGHT\n");
    #pragma omp for schedule(dynamic,ns) nowait
    for(int i = s; i < nrows - s; i++){
        for(int j = ncols - s; j < ncols; j++){
            res_index = i*ncols + j;
            result[res_index] = 0;
            //single element
            k_i_start = 0;
            k_i_end = kernel_size;
            k_j_start = 0;
            k_j_end = s + nrows - j;
            tmp = 0.;
            //single element
            for (int k_i = k_i_start; k_i < k_i_end; k_i ++ ){
            for (int k_j = k_j_start; k_j < k_j_end; k_j ++ ){   
                k_index = k_i * kernel_size + k_j;
                img_index = (i + (k_i - s))*ncols + (j + (k_j - s));
                tmp+= kernel[k_index]*source[img_index];
            }
            }
            result[res_index] = tmp;

        }
    }  
    
    
    
    
    //printf("Processing Q UP LEFT\n");
    #pragma omp for schedule(dynamic,ns) nowait
    for(int i = 0; i < s; i++){
        for(int j = 0; j < s; j++){
            res_index = i*ncols + j;
            result[res_index] = 0;
            //single element
            k_i_start = s - i;
            k_i_end = kernel_size;
            k_j_start = s - j;
            k_j_end = kernel_size;
            tmp = 0.;
            //single element
            for (int k_i = k_i_start; k_i < k_i_end; k_i ++ ){
            for (int k_j = k_j_start; k_j < k_j_end; k_j ++ ){ 
                k_index = k_i * kernel_size + k_j;
                img_index = (i + (k_i - s))*ncols + (j + (k_j - s));
                tmp+= kernel[k_index]*source[img_index];
            }
            }
            result[res_index] = tmp;

        }
    }  
    

   
    //printf("Processing Q UP RIGHT\n");
    #pragma omp for schedule(dynamic,ns) nowait
    for(int i = 0; i < s; i++){
        for(int j = ncols - s; j < ncols; j++){
            res_index = i*ncols + j;
            result[res_index] = 0;
            //single element
            k_i_start = s - i;
            k_i_end = kernel_size;
            k_j_start = 0;
            k_j_end = s + (ncols - j);
            tmp = 0.;
            //single element
            for (int k_i = k_i_start; k_i < k_i_end; k_i ++ ){
            for (int k_j = k_j_start; k_j < k_j_end; k_j ++ ){  
                k_index = k_i * kernel_size + k_j;
                img_index = (i + (k_i - s))*ncols + (j + (k_j - s));
                tmp+= kernel[k_index]*source[img_index];
            }
            }
            result[res_index] = tmp;

        }
    }  
    

    
    //printf("Processing Q DOWN LEFT\n");
    #pragma omp for schedule(dynamic,ns) nowait
    for(int i = nrows - s; i < nrows; i++){
        for(int j = 0; j < s; j++){
            res_index = i*ncols + j;
            result[res_index] = 0;
            //single element
            k_i_start = 0;
            k_i_end = s + (nrows -i);
            k_j_start = s - j;
            k_j_end = kernel_size;
            tmp = 0.;
            //single element
            for (int k_i = k_i_start; k_i < k_i_end; k_i ++ ){
            for (int k_j = k_j_start; k_j < k_j_end; k_j ++ ){ 
                k_index = k_i * kernel_size + k_j;
                img_index = (i + (k_i - s))*ncols + (j + (k_j - s));
                float c1 = kernel[k_index];
                float c2 = source[img_index];
                tmp += c1*c2;
            }
            }
            result[res_index] = tmp;

        }
    }  
    

    

    //printf("Processing Q DOWN RIGHT\n");
    #pragma omp for schedule(dynamic,ns) nowait
    for(int i = nrows - s; i < nrows; i++){
        for(int j = ncols - s; j < ncols; j++){
            res_index = i*ncols + j;
            result[res_index] = 0;
            //single element
            k_i_start = 0;
            k_i_end = s + (nrows - i);
            k_j_start = 0;
            k_j_end = s + (ncols - j);
            tmp = 0.;
            //single element
            for (int k_i = k_i_start; k_i < k_i_end; k_i ++ ){
            for (int k_j = k_j_start; k_j < k_j_end; k_j ++ ){  
                k_index = k_i * kernel_size + k_j;
                img_index = (i + (k_i - s))*ncols + (j + (k_j - s));
                float c1 = kernel[k_index];
                float c2 = source[img_index];
                tmp += c1*c2;
            }
            }
            result[res_index] = tmp;

        }
    }  
    

    
    
    }
    printf("Processing finished successfully!\n");
}

void convolve_2B(unsigned short int* source,int nrows,int ncols,float * kernel, int kernel_size, unsigned short int* result){
    
    //printf("S %d \n", s);
    
    //interior convolution

    #pragma omp parallel 
    {
    int k_i_start, k_i_end, k_j_start, k_j_end, img_index, res_index, k_index;
    //float tmp;
    int s = kernel_size/2;
    
    //printf("Processing Interior\n");
    //int ns = nrows/omp_get_num_threads();
    int ns = s + 1;

    //printf("Processing Interior\n");
    #pragma omp for nowait
    for(int cc = s; cc < nrows - ns; cc+=ns){    
        for(int i = cc; i < cc+ns; i++){
            for(int j = s; j < ncols - s; j++){
            res_index = i*ncols + j;
            result[res_index] = 0;
            float tmp = 0.;
        //single element
        for (int k_i = 0; k_i < kernel_size; k_i ++ ){
        for (int k_j = 0; k_j < kernel_size; k_j ++ ){  
            k_index = k_i * kernel_size + k_j;
            size_t img_index = (i + (k_i - s))*ncols + (j + (k_j - s));
            tmp += kernel[k_index]*(float)source[img_index];
            
        }
        }
            result[res_index] = tmp;
           // printf("%f ", tmp);


    }  
    }
    }
    
    float tmp = 0.;
    
    //remainder
    #pragma omp for schedule(dynamic,ns) nowait
        for(int i = ((nrows - s)/ns)*ns; i < nrows - s; i++){
                for(int j = s; j < ncols - s; j++){
                    res_index = i*ncols + j;
                    result[res_index] = 0;
                    tmp = 0.;
                    //single element
                    for (int k_i = 0; k_i < kernel_size; k_i ++ ){
                    for (int k_j = 0; k_j < kernel_size; k_j ++ ){  
                        k_index = k_i * kernel_size + k_j;
                        img_index = (i + (k_i - s))*ncols + (j + (k_j - s));
                        tmp+= kernel[k_index]*source[img_index];
                    }
                    }
                    result[res_index] = tmp;
    }
    }
    
    
    //img_index, k_index, res_index;
    //halo up
    //printf("Processing HALO UP\n");
    
    
    
    //printf("Processing HALO UP\n");
    #pragma omp for schedule(dynamic,ns) nowait
    for(int i = 0; i <s; i++){
        for(int j = s; j < ncols - s; j++){
            res_index = i*ncols + j;
            result[res_index] = 0;
            //single element
            k_i_start = s - i;
            k_i_end = kernel_size;
            k_j_start = 0;
            k_j_end = kernel_size;
            tmp = 0.;
            //single element
            for (int k_i = k_i_start; k_i < k_i_end; k_i ++ ){
            for (int k_j = k_j_start; k_j < k_j_end; k_j ++ ){ 
                k_index = k_i * kernel_size + k_j;
                img_index = (i + (k_i - s))*ncols + (j + (k_j - s));
                tmp += kernel[k_index]*source[img_index];
            }
            }
            result[res_index] = tmp;
        }
    }
    //halo down
    
    
    
    //printf("Processing HALO DOWN\n");
    #pragma omp for schedule(dynamic,ns) nowait
    for(int i = nrows - s; i < nrows; i++){
        for(int j = s; j < ncols - s; j++){
            res_index = i*ncols + j;
            result[res_index] = 0;
            //single element
            k_i_start = 0;
            k_i_end = s + (nrows - i);
            k_j_start = 0;
            k_j_end = kernel_size;
            tmp = 0.;
            //single element
            for (int k_i = k_i_start; k_i < k_i_end; k_i ++ ){
            for (int k_j = k_j_start; k_j < k_j_end; k_j ++ ){  
                k_index = k_i * kernel_size + k_j;
                img_index = (i + (k_i - s))*ncols + (j + (k_j - s));
                float c1 = kernel[k_index];
                float c2 = source[img_index];
                tmp += c1*c2;
            }
            }
            result[res_index] = tmp;
        }
    }  
    
    //halo left
    
    #pragma omp for schedule(dynamic,ns) nowait
    //printf("Processing HALO LEFT\n");
    for(int i = s; i < nrows - s; i++){
        for(int j = 0; j < s; j++){
            res_index = i*ncols + j;
            result[res_index] = 0;
            //single element
            k_i_start = 0;
            k_i_end = kernel_size;
            k_j_start = s - j;
            k_j_end = kernel_size;
            tmp = 0.;
            //single element
            for (int k_i = k_i_start; k_i < k_i_end; k_i ++ ){
            for (int k_j = k_j_start; k_j < k_j_end; k_j ++ ){  
                k_index = k_i * kernel_size + k_j;
                img_index = (i + (k_i - s))*ncols + (j + (k_j - s));
                tmp+= kernel[k_index]*source[img_index];
            }
            }
            result[res_index] = tmp;
        }
    }  
    
    //halo right
    
    
    //printf("Processing HALO RIGHT\n");
    #pragma omp for schedule(dynamic,ns) nowait
    for(int i = s; i < nrows - s; i++){
        for(int j = ncols - s; j < ncols; j++){
            res_index = i*ncols + j;
            result[res_index] = 0;
            //single element
            k_i_start = 0;
            k_i_end = kernel_size;
            k_j_start = 0;
            k_j_end = s + nrows - j;
            tmp = 0.;
            //single element
            for (int k_i = k_i_start; k_i < k_i_end; k_i ++ ){
            for (int k_j = k_j_start; k_j < k_j_end; k_j ++ ){   
                k_index = k_i * kernel_size + k_j;
                img_index = (i + (k_i - s))*ncols + (j + (k_j - s));
                tmp+= kernel[k_index]*source[img_index];
            }
            }
            result[res_index] = tmp;
        }
    }  
    
    
    
    
    //printf("Processing Q UP LEFT\n");
    #pragma omp for schedule(dynamic,ns) nowait
    for(int i = 0; i < s; i++){
        for(int j = 0; j < s; j++){
            res_index = i*ncols + j;
            result[res_index] = 0;
            //single element
            k_i_start = s - i;
            k_i_end = kernel_size;
            k_j_start = s - j;
            k_j_end = kernel_size;
            tmp = 0.;
            //single element
            for (int k_i = k_i_start; k_i < k_i_end; k_i ++ ){
            for (int k_j = k_j_start; k_j < k_j_end; k_j ++ ){ 
                k_index = k_i * kernel_size + k_j;
                img_index = (i + (k_i - s))*ncols + (j + (k_j - s));
                tmp+= kernel[k_index]*source[img_index];
            }
            }
            result[res_index] = tmp;
        }
    }  
    
   
    //printf("Processing Q UP RIGHT\n");
    #pragma omp for schedule(dynamic,ns) nowait
    for(int i = 0; i < s; i++){
        for(int j = ncols - s; j < ncols; j++){
            res_index = i*ncols + j;
            result[res_index] = 0;
            //single element
            k_i_start = s - i;
            k_i_end = kernel_size;
            k_j_start = 0;
            k_j_end = s + (ncols - j);
            tmp = 0.;
            //single element
            for (int k_i = k_i_start; k_i < k_i_end; k_i ++ ){
            for (int k_j = k_j_start; k_j < k_j_end; k_j ++ ){  
                k_index = k_i * kernel_size + k_j;
                img_index = (i + (k_i - s))*ncols + (j + (k_j - s));
                tmp+= kernel[k_index]*source[img_index];
            }
            }
            result[res_index] = tmp;
        }
    }  
    
    
    //printf("Processing Q DOWN LEFT\n");
    #pragma omp for schedule(dynamic,ns) nowait
    for(int i = nrows - s; i < nrows; i++){
        for(int j = 0; j < s; j++){
            res_index = i*ncols + j;
            result[res_index] = 0;
            //single element
            k_i_start = 0;
            k_i_end = s + (nrows -i);
            k_j_start = s - j;
            k_j_end = kernel_size;
            tmp = 0.;
            //single element
            for (int k_i = k_i_start; k_i < k_i_end; k_i ++ ){
            for (int k_j = k_j_start; k_j < k_j_end; k_j ++ ){ 
                k_index = k_i * kernel_size + k_j;
                img_index = (i + (k_i - s))*ncols + (j + (k_j - s));
                float c1 = kernel[k_index];
                float c2 = source[img_index];
                tmp += c1*c2;
            }
            }
            result[res_index] = tmp;
        }
    }  
    
    
    //printf("Processing Q DOWN RIGHT\n");
    #pragma omp for schedule(dynamic,ns) nowait
    for(int i = nrows - s; i < nrows; i++){
        for(int j = ncols - s; j < ncols; j++){
            res_index = i*ncols + j;
            result[res_index] = 0;
            //single element
            k_i_start = 0;
            k_i_end = s + (nrows - i);
            k_j_start = 0;
            k_j_end = s + (ncols - j);
            tmp = 0.;
            //single element
            for (int k_i = k_i_start; k_i < k_i_end; k_i ++ ){
            for (int k_j = k_j_start; k_j < k_j_end; k_j ++ ){  
                k_index = k_i * kernel_size + k_j;
                img_index = (i + (k_i - s))*ncols + (j + (k_j - s));
                float c1 = kernel[k_index];
                float c2 = source[img_index];
                tmp += c1*c2;
            }
            }
            result[res_index] = tmp;
        }
    }  
    
    
    }
    printf("Processing finished successfully!\n");
}

void convolve_2B_float(float* source,int nrows,int ncols,float * kernel, int kernel_size, float* result){
    
    //printf("S %d \n", s);
    
    //interior convolution

    
    int k_i_start, k_i_end, k_j_start, k_j_end, img_index, res_index, k_index;
    //float tmp;
    int s = kernel_size/2;
    
    //printf("Processing Interior\n");
    //int ns = nrows/omp_get_num_threads();
    int ns = s + 1;

    //printf("Processing Interior\n");
     
        for(int i = s; i < nrows - s; i++){
            for(int j = s; j < ncols - s; j++){
            res_index = i*ncols + j;
            result[res_index] = 0;
            float tmp = 0.;
           // if(abs(source[res_index] - 17.) < 0.0001 ) printf("Hi I am the hjhkjh %f %d\n", source[res_index], res_index);
        //single element
            for (int k_i = 0; k_i < kernel_size; k_i ++ ){
            for (int k_j = 0; k_j < kernel_size; k_j ++ ){  
                k_index = k_i * kernel_size + k_j;
                img_index = (i + (k_i - s))*ncols + (j + (k_j - s));
                tmp += kernel[k_index] * source[img_index];
                
                
            }
            }
            //printf("%f %d %d %f %f\n", tmp ,i ,j, source[res_index]*kernel[s*kernel_size + s], source[res_index]);
            result[res_index] = tmp;
            


    }  
    }
    
    float tmp = 0.;
    
    //remainder
    #pragma omp for schedule(dynamic,ns) nowait
        for(int i = ((nrows - s)/ns)*ns; i < nrows - s; i++){
                for(int j = s; j < ncols - s; j++){
                    res_index = i*ncols + j;
                    result[res_index] = 0;
                    tmp = 0.;
                    //single element
                    for (int k_i = 0; k_i < kernel_size; k_i ++ ){
                    for (int k_j = 0; k_j < kernel_size; k_j ++ ){  
                        k_index = k_i * kernel_size + k_j;
                        img_index = (i + (k_i - s))*ncols + (j + (k_j - s));
                        tmp+= kernel[k_index]*source[img_index];
                    }
                    }
                    result[res_index] = tmp;
    }
    }
    
    
    //img_index, k_index, res_index;
    //halo up
    //printf("Processing HALO UP\n");
    
    
    
    //printf("Processing HALO UP\n");
    #pragma omp for schedule(dynamic,ns) nowait
    for(int i = 0; i <s; i++){
        for(int j = s; j < ncols - s; j++){
            res_index = i*ncols + j;
            result[res_index] = 0;
            //single element
            k_i_start = s - i;
            k_i_end = kernel_size;
            k_j_start = 0;
            k_j_end = kernel_size;
            tmp = 0.;
            //single element
            for (int k_i = k_i_start; k_i < k_i_end; k_i ++ ){
            for (int k_j = k_j_start; k_j < k_j_end; k_j ++ ){ 
                k_index = k_i * kernel_size + k_j;
                img_index = (i + (k_i - s))*ncols + (j + (k_j - s));
                tmp += kernel[k_index]*source[img_index];
            }
            }
            result[res_index] = tmp;
        }
    }
    //halo down
    
    
    
    //printf("Processing HALO DOWN\n");
    #pragma omp for schedule(dynamic,ns) nowait
    for(int i = nrows - s; i < nrows; i++){
        for(int j = s; j < ncols - s; j++){
            res_index = i*ncols + j;
            result[res_index] = 0;
            //single element
            k_i_start = 0;
            k_i_end = s + (nrows - i);
            k_j_start = 0;
            k_j_end = kernel_size;
            tmp = 0.;
            //single element
            for (int k_i = k_i_start; k_i < k_i_end; k_i ++ ){
            for (int k_j = k_j_start; k_j < k_j_end; k_j ++ ){  
                k_index = k_i * kernel_size + k_j;
                img_index = (i + (k_i - s))*ncols + (j + (k_j - s));
                float c1 = kernel[k_index];
                float c2 = source[img_index];
                tmp += c1*c2;
            }
            }
            result[res_index] = tmp;
        }
    }  
    
    //halo left
    
    #pragma omp for schedule(dynamic,ns) nowait
    //printf("Processing HALO LEFT\n");
    for(int i = s; i < nrows - s; i++){
        for(int j = 0; j < s; j++){
            res_index = i*ncols + j;
            result[res_index] = 0;
            //single element
            k_i_start = 0;
            k_i_end = kernel_size;
            k_j_start = s - j;
            k_j_end = kernel_size;
            tmp = 0.;
            //single element
            for (int k_i = k_i_start; k_i < k_i_end; k_i ++ ){
            for (int k_j = k_j_start; k_j < k_j_end; k_j ++ ){  
                k_index = k_i * kernel_size + k_j;
                img_index = (i + (k_i - s))*ncols + (j + (k_j - s));
                tmp+= kernel[k_index]*source[img_index];
            }
            }
            result[res_index] = tmp;
        }
    }  
    
    //halo right
    
    
    //printf("Processing HALO RIGHT\n");
    #pragma omp for schedule(dynamic,ns) nowait
    for(int i = s; i < nrows - s; i++){
        for(int j = ncols - s; j < ncols; j++){
            res_index = i*ncols + j;
            result[res_index] = 0;
            //single element
            k_i_start = 0;
            k_i_end = kernel_size;
            k_j_start = 0;
            k_j_end = s + nrows - j;
            tmp = 0.;
            //single element
            for (int k_i = k_i_start; k_i < k_i_end; k_i ++ ){
            for (int k_j = k_j_start; k_j < k_j_end; k_j ++ ){   
                k_index = k_i * kernel_size + k_j;
                img_index = (i + (k_i - s))*ncols + (j + (k_j - s));
                tmp+= kernel[k_index]*source[img_index];
            }
            }
            result[res_index] = tmp;
        }
    }  
    
    
    
    
    //printf("Processing Q UP LEFT\n");
    #pragma omp for schedule(dynamic,ns) nowait
    for(int i = 0; i < s; i++){
        for(int j = 0; j < s; j++){
            res_index = i*ncols + j;
            result[res_index] = 0;
            //single element
            k_i_start = s - i;
            k_i_end = kernel_size;
            k_j_start = s - j;
            k_j_end = kernel_size;
            tmp = 0.;
            //single element
            for (int k_i = k_i_start; k_i < k_i_end; k_i ++ ){
            for (int k_j = k_j_start; k_j < k_j_end; k_j ++ ){ 
                k_index = k_i * kernel_size + k_j;
                img_index = (i + (k_i - s))*ncols + (j + (k_j - s));
                tmp+= kernel[k_index]*source[img_index];
            }
            }
            result[res_index] = tmp;
        }
    }  
    
   
    //printf("Processing Q UP RIGHT\n");
    #pragma omp for schedule(dynamic,ns) nowait
    for(int i = 0; i < s; i++){
        for(int j = ncols - s; j < ncols; j++){
            res_index = i*ncols + j;
            result[res_index] = 0;
            //single element
            k_i_start = s - i;
            k_i_end = kernel_size;
            k_j_start = 0;
            k_j_end = s + (ncols - j);
            tmp = 0.;
            //single element
            for (int k_i = k_i_start; k_i < k_i_end; k_i ++ ){
            for (int k_j = k_j_start; k_j < k_j_end; k_j ++ ){  
                k_index = k_i * kernel_size + k_j;
                img_index = (i + (k_i - s))*ncols + (j + (k_j - s));
                tmp+= kernel[k_index]*source[img_index];
            }
            }
            result[res_index] = tmp;
        }
    }  
    
    
    //printf("Processing Q DOWN LEFT\n");
    #pragma omp for schedule(dynamic,ns) nowait
    for(int i = nrows - s; i < nrows; i++){
        for(int j = 0; j < s; j++){
            res_index = i*ncols + j;
            result[res_index] = 0;
            //single element
            k_i_start = 0;
            k_i_end = s + (nrows -i);
            k_j_start = s - j;
            k_j_end = kernel_size;
            tmp = 0.;
            //single element
            for (int k_i = k_i_start; k_i < k_i_end; k_i ++ ){
            for (int k_j = k_j_start; k_j < k_j_end; k_j ++ ){ 
                k_index = k_i * kernel_size + k_j;
                img_index = (i + (k_i - s))*ncols + (j + (k_j - s));
                float c1 = kernel[k_index];
                float c2 = source[img_index];
                tmp += c1*c2;
            }
            }
            result[res_index] = tmp;
        }
    }  
    
    
    //printf("Processing Q DOWN RIGHT\n");
    #pragma omp for schedule(dynamic,ns) nowait
    for(int i = nrows - s; i < nrows; i++){
        for(int j = ncols - s; j < ncols; j++){
            res_index = i*ncols + j;
            result[res_index] = 0;
            //single element
            k_i_start = 0;
            k_i_end = s + (nrows - i);
            k_j_start = 0;
            k_j_end = s + (ncols - j);
            tmp = 0.;
            //single element
            for (int k_i = k_i_start; k_i < k_i_end; k_i ++ ){
            for (int k_j = k_j_start; k_j < k_j_end; k_j ++ ){  
                k_index = k_i * kernel_size + k_j;
                img_index = (i + (k_i - s))*ncols + (j + (k_j - s));
                float c1 = kernel[k_index];
                float c2 = source[img_index];
                tmp += c1*c2;
            }
            }
            result[res_index] = tmp;
        }
    }  
    
    
    
    
    printf("Processing finished successfully!\n");
}

int spy(char* filename){
    FILE *file = fopen(filename, "r");
    int nrows,ncols,max;
    char MAGIC[2];
    size_t  k, n = 0;
    char * line = NULL;
    // get the Magic Number
    k = fscanf(file, "%2s%*c", MAGIC);

    // skip all the comments
    k = getline( &line, &n, file);
    while ( (k > 0) && (line[0]=='#') ){
        k = getline( &line, &n, file);
        }

    if (k > 0)
        {
        k = sscanf(line, "%d %d", &ncols, &nrows);
        }

    fscanf(file, "%d", &max);
    printf("MAX is %d \n", max);
    fclose(file);
    if(max <= 255){ return 1;}
    else {return 2;}
}

void prc_1B(char* filename, char* out_file, float *kernel, unsigned int kernel_size){
    printf("this machine is %s\n", (I_M_LITTLE_ENDIAN)?"little endian":"big endian");
    void* image;
    int nrows, ncols, max;
    read_pgm_image(&image, &max, &ncols, &nrows, filename);
    if ( I_M_LITTLE_ENDIAN ) swap_image( image, ncols, nrows, max);

    unsigned char* convolved_image = (unsigned char*)malloc(nrows*ncols*sizeof(unsigned char));
    convolve_1B((unsigned char* )image,nrows,ncols,kernel,kernel_size,convolved_image);

    if ( I_M_LITTLE_ENDIAN ) swap_image( convolved_image, ncols, nrows, max); 
    write_pgm_image(convolved_image, max, ncols, nrows, out_file);
    free(image);
    free(convolved_image);
}

void prc_2B(char* filename, char* out_file, float *kernel, unsigned int kernel_size){
    printf("this machine is %s\n", (I_M_LITTLE_ENDIAN)?"little endian":"big endian");
    int maxval, ncols, nrows;
    void* ptr;
    
    read_pgm_image( &ptr, &maxval, &ncols, &nrows, filename);
    printf("The imaget has been read\n");
    
    printf("%d %d\n", ncols, nrows);
    
    if ( I_M_LITTLE_ENDIAN ) swap_image( ptr, ncols, nrows, maxval);

    unsigned short* res = (unsigned short*)malloc(nrows*ncols*sizeof(unsigned short));
    /*
    convolve_2B((unsigned short*)ptr, nrows,ncols,kernel, kernel_size, res);
    */
    unsigned char* rr = (unsigned char*)malloc(nrows*ncols*sizeof(unsigned char));
    unsigned short* pp = (unsigned short*)ptr;
    for(int i = 0; i < ncols*nrows; i++){
        float vv = pp[i]/255.;
        //if(vv ) printf("ccccc\n");
        rr[i] = vv;
        res[i] = vv;
        //if((int)(vv*256) - pp[i] != 0) printf("ccccc\n");
        
    }
    
    //if ( I_M_LITTLE_ENDIAN ) swap_image( res, ncols, nrows, maxval);   
    write_pgm_image(res, maxval, ncols, nrows, out_file);
    write_pgm_image(rr, 255, ncols, nrows, "t.pgm");

}

int main(int argc, char**argv){
    unsigned short int kernel_type;
    unsigned int kernel_size;
    float* kernel;
    char* input_file, *out_file; 
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
    out_file = "out.pgm";
    if (argc > 4){out_file = argv[4];}
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

    //printf("---KERNEL---\n");
    //print_kernel(kernel,kernel_size);
    
    int N_BYTES = spy(input_file);
    printf("Processing %d bytes pixels\n", N_BYTES);
    switch (N_BYTES)
    {
    case 1:
        prc_1B(input_file,out_file, kernel, kernel_size);
        break;
    case 2:
        prc_2B(input_file,out_file, kernel, kernel_size);
        break;
    default:
        return 0;
        break;
    }

    
    free(kernel);

    return 0;
    
}