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
#define swap(mem) (( (mem) & (short int)0xff00) >> 8) +	\
  ( ((mem) & (short int)0x00ff) << 8)
#else
#define I_M_LITTLE_ENDIAN 0
#define swap(mem) (mem)
#endif



// =============================================================
//  utilities for managinf pgm files
//
//  * write_pgm_image
//  * read_pgm_image
//  * swap_image
//
// =============================================================

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
      for ( int i = 0; i < size; i+= 1 )
  	((unsigned short int*)image)[i] = swap(((unsigned short int*)image)[i]);
    }
  return;
}

void get_MEAN_kernel(double* kernel, unsigned int kernel_size){
    double cc = 1./(double)(kernel_size*kernel_size);
    for(int i = 0; i<kernel_size; i++){
        for(int j = 0; j<kernel_size; j++){
            kernel[i*kernel_size+j] = cc;
        }
    }
}

double gauss(double x, double y, double sigma){
    return exp(-(x*x + y*y)/(2*sigma*sigma))/(sigma*pow(2*M_PI,0.5));
}


void get_WEIGHT_kernel(double* kernel, unsigned int kernel_size, double w){
    double other = (1.0 - w)/((kernel_size * kernel_size) - 1);
    int s = kernel_size/2;
    double sum = 0;
    for(int i = 0; i < kernel_size; i++){
        for(int j = 0; j < kernel_size; j++){
            
            kernel[i*kernel_size + j] = other;
            sum += kernel[i*kernel_size + j];


        }
    }
    kernel[s*kernel_size + s] = w;
    
    printf("normalization is %lf \n", sum - other + w);
}





void get_GAUSSIAN_kernel(double* kernel, unsigned int kernel_size){
    double sum = 0;
    for(int i = 0; i < kernel_size; i++){
        for(int j = 0; j < kernel_size; j++){
            int x = j - kernel_size/2;
            int y = i - kernel_size/2;
            kernel[i*kernel_size + j] = gauss(x,y,kernel_size/2.);
            sum += kernel[i*kernel_size+j];
        }
    }
    double cc = 0;
    for(int i = 0; i < kernel_size; i++){
        for(int j = 0; j < kernel_size; j++){
            
            kernel[i*kernel_size + j] = kernel[i*kernel_size+j]/sum;
            cc += kernel[i*kernel_size + j];
            
        }
    }
    printf("normalization is %f \n", cc);
}

void get_SHARPEN_kernel(double* kernel, unsigned int kernel_size){
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


void convolve_1B(unsigned char * source,int nrows,int ncols,double * kernel, int kernel_size, unsigned char *result){
    
    //printf("S %d \n", s);
    
    //interior convolution

    #pragma omp parallel 
    {
    int k_i_start, k_i_end, k_j_start, k_j_end, img_index, res_index, k_index;
    double tmp;
    int s = kernel_size/2;
    
    //printf("Processing Interior\n");
    //int ns = nrows/omp_get_num_threads();
    int ns = s;
    int remainder = nrows - s - ((nrows - 2*s) % ns);
    int remainder_cols = ncols - s - ((ncols - 2*s) % ns);
    //printf("remainder %d", (remainder - s)%ns);
    //printf("Processing Interior\n");
    #pragma omp for nowait
    for(int cc = s; cc < remainder; cc+=ns){   
        for(int dd = s; dd < remainder_cols; dd+=ns){
            for(int i = cc; i < cc+ns; i++){
            for(int j = dd; j < dd+ns; j++){
                res_index = i*ncols + j;
                result[res_index] = 0;
                double tmp = 0.;
                //single element
                for (int k_i = 0; k_i < kernel_size; k_i ++ ){
                for (int k_j = 0; k_j < kernel_size; k_j ++ ){  
                    k_index = k_i * kernel_size + k_j;
                    size_t img_index = (i + (k_i - s))*ncols + (j + (k_j - s));
                    tmp += (double)source[img_index]*kernel[k_index];
            
            
        }
        }
            result[res_index] = (unsigned char)tmp;
           // printf("%f ", tmp);


    }  
    }
    }
    
        for(int i = cc; i < cc+ns; i++){
            for(int j = remainder_cols; j < ncols - s; j++){
                res_index = i*ncols + j;
                result[res_index] = 0;
                double tmp = 0.;
                //single element
                for (int k_i = 0; k_i < kernel_size; k_i ++ ){
                for (int k_j = 0; k_j < kernel_size; k_j ++ ){  
                    k_index = k_i * kernel_size + k_j;
                    size_t img_index = (i + (k_i - s))*ncols + (j + (k_j - s));
                    tmp += (double)source[img_index]*kernel[k_index];
            
            
        }
        }
            result[res_index] = (unsigned char)tmp;
           // printf("%f ", tmp);


    } 
    }
    }
    

    
    
    //double tmp = 0.;
    
    //remainder
    #pragma omp for schedule(dynamic,ns) nowait
        for(int i = remainder; i < nrows - s; i++){
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
                
                tmp += kernel[k_index]*(double)source[img_index];
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
            k_j_end = s + ncols - j;
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
                tmp += kernel[k_index]*(double)source[img_index];
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
                tmp += kernel[k_index]*(double)source[img_index];
            }
            }
            result[res_index] = tmp;

        }
    }  
    

    
    
    }
    printf("Processing finished successfully!\n");
}


void convolve_2B(unsigned short * source,int nrows,int ncols,double * kernel, int kernel_size, unsigned short *result){
    
    //printf("S %d \n", s);
    
    //interior convolution

    #pragma omp parallel 
    {
    int k_i_start, k_i_end, k_j_start, k_j_end, img_index, res_index, k_index;
    double tmp;
    int s = kernel_size/2;
    
    //printf("Processing Interior\n");
    //int ns = nrows/omp_get_num_threads();
    int ns = s;
    int remainder = nrows - s - ((nrows - 2*s) % ns);
    int remainder_cols = ncols - s - ((ncols - 2*s) % ns);
    //printf("remainder %d", (remainder - s)%ns);
    //printf("Processing Interior\n");
    #pragma omp for nowait
    for(int cc = s; cc < remainder; cc+=ns){   
        for(int dd = s; dd < remainder_cols; dd+=ns){
            for(int i = cc; i < cc+ns; i++){
            for(int j = dd; j < dd+ns; j++){
                res_index = i*ncols + j;
                result[res_index] = 0;
                double tmp = 0.;
                //single element
                for (int k_i = 0; k_i < kernel_size; k_i ++ ){
                for (int k_j = 0; k_j < kernel_size; k_j ++ ){  
                    k_index = k_i * kernel_size + k_j;
                    size_t img_index = (i + (k_i - s))*ncols + (j + (k_j - s));
                    tmp += (double)source[img_index]*kernel[k_index];
            
            
        }
        }
            result[res_index] = tmp;
           // printf("%f ", tmp);


    }  
    }
    }
    
        for(int i = cc; i < cc+ns; i++){
            for(int j = remainder_cols; j < ncols - s; j++){
                res_index = i*ncols + j;
                result[res_index] = 0;
                double tmp = 0.;
                //single element
                for (int k_i = 0; k_i < kernel_size; k_i ++ ){
                for (int k_j = 0; k_j < kernel_size; k_j ++ ){  
                    k_index = k_i * kernel_size + k_j;
                    size_t img_index = (i + (k_i - s))*ncols + (j + (k_j - s));
                    tmp += (double)source[img_index]*kernel[k_index];
            
            
        }
        }
            result[res_index] = tmp;
           // printf("%f ", tmp);


    } 
    }
    }
    

    
    
    //double tmp = 0.;
    
    //remainder
    #pragma omp for schedule(dynamic,ns) nowait
        for(int i = remainder; i < nrows - s; i++){
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
                
                tmp += kernel[k_index]*(double)source[img_index];
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
            k_j_end = s + ncols - j;
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
                tmp += kernel[k_index]*(double)source[img_index];
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
                tmp += kernel[k_index]*(double)source[img_index];
            }
            }
            result[res_index] = tmp;

        }
    }  
    

    
    
    }
    printf("Processing finished successfully!\n");
}


int main(int argc, char**argv){
    unsigned short int kernel_type;
    unsigned int kernel_size;
    double* kernel;
    char* input_file;
    
    int out_len = 20;
    
    char of[120];

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
    double w = 0;
    
    //output file name = original_file_name.b_#TYPE_#XSIZEx#YSIZE<_#CENTRALVALUE>.pgm
    printf("argc %d\n", argc);
    if(kernel_type == 1){
        w = atof(argv[3]);
        input_file = argv[4];
      
        if(argc>4){sprintf(of,"%s",argv[5]);}
    }
    else
    {
        input_file = argv[3];
        
        if(argc > 3){sprintf(of,"%s",argv[4]);}
    }
    
    printf("if %s\n", input_file);
   // printf("of %s\n", ppp);
    
    
    
    kernel = (double*)malloc(kernel_size*kernel_size*sizeof(double));
    switch (kernel_type){
        case 0:
        //mean kernel
        get_MEAN_kernel(kernel,kernel_size);
        printf("Selected MEAN kernel \n");
        break;

        case 1:
        {
        
        get_WEIGHT_kernel(kernel,kernel_size,w);
        printf("Selected WEIGHT kernel %lf \n", w);
        break;
        }

        case 2:

        //gaussian kernel
        get_GAUSSIAN_kernel(kernel,kernel_size);
        printf("Selected GAUSSIAN kernel \n");
        break;

        case 3:
        //sharpen kernel
        kernel_size = 3;
        free(kernel);
        kernel = (double*)malloc(kernel_size*kernel_size*sizeof(double));
        get_SHARPEN_kernel(kernel,kernel_size);
        printf("Selected SHARPEN kernel \n ATTENTION FOR THIS KERNEL kernel_size BOUDNED TO 3 \n");
        break;


        default:
        
        printf("Unknown kernel\nThis is the list of Implemented kernels: \n");
        printf("0 --> Mean Kernel \n");
        printf("1 --> Weight Kernel \n");
        printf("2 --> Gaussian Kernel \n");
        printf("3 --> Sharpening Kernel \n");
        printf("+ others will be added developement\n");
        
    
        return 0;

    }

    //printf("---KERNEL---\n");
    //print_kernel(kernel,kernel_size);
    
    void* source;
    int maxval, nrows, ncols;
    read_pgm_image(&source, &maxval, &ncols, &nrows, input_file);
    printf("processing %d Bytes image\n", 1 + (maxval > 255));
    if ( I_M_LITTLE_ENDIAN ) swap_image( source, ncols, nrows, maxval);

    switch (1 + (maxval > 255))
    {
    case 1:
        {
        unsigned char* result = (unsigned char*)malloc(nrows*ncols*sizeof(unsigned char));
        convolve_1B((unsigned char* )source,nrows,ncols,kernel,kernel_size,result);
        if ( I_M_LITTLE_ENDIAN ) swap_image( result, ncols, nrows, maxval); 
        write_pgm_image(result, maxval, ncols, nrows, of);
        free(result);
        break;
        }

    case 2:
        {
        unsigned short* result = (unsigned short*)malloc(nrows*ncols*sizeof(unsigned short));
        convolve_2B((unsigned short int* )source,nrows,ncols,kernel,kernel_size,result);
        if ( I_M_LITTLE_ENDIAN ) swap_image( result, ncols, nrows, maxval); 
        write_pgm_image(result, maxval, ncols, nrows, of);
        free(result);
        break;
        }
    default:
        return 0;
        printf("Something went wrong aborting\n");
        break;
    }

    
    free(kernel);
    free(source);
    return 0;
    
}
