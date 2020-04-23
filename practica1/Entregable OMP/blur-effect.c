#include "file_system.h"
#define M_PI 3.14159265358979323846
#include <omp.h>
#include <sys/time.h>

/**
 * bsize represents the block size of the blockwise sistem
 * threads represent the number of threads of the algorithm
 * ksize represents the kernel size
 * sigma represents the standard deviation of the kernel
 */ 

int bsize = 32, threads = 4, ksize = 12;
double sigma = 10;
Image *img, *newImg;
double * * kernel;

double * * gaussianKernel(int size){
    double mean = size/2;
    double * * kernel = malloc(size * sizeof(double *));
    for(int i = 0; i < size; i++){
        kernel[i] = malloc(size * sizeof(double));
        for(int j = 0; j < size; j++){
            kernel[i][j] = exp( -0.5 * (pow((i-mean)/sigma, 2.0) + pow((j-mean)/sigma,2.0)))
                         / (2 * M_PI * sigma * sigma);
        }
    }
    return kernel;
}

int main(int argc, char **argv) {
    // For time stamp
    struct timeval start, stop, diff;
    gettimeofday(&start, NULL);
    
    char * imgname = argv[1]; 		// "GrandImg.jpg";//"img2.jpg";//"icon.png";//"GrandImg.jpg";
    char * newImgName = argv[2];	//"GrandImgBlur.jpg";//"img2Blur.jpg";//"iconBlur.png";//"GrandImgBlur.jpg";
    ksize = atoi(argv[3]);
    threads = atoi(argv[4]);
    //printf("Ejecutando programa con:\n\t- %d threads\n\t- %d tamaño del kernel\n\t- %d tamaño de bloque\n\t- %.3f sigma\n", threads, ksize, bsize, sigma);
 
    // Even kernel size
    if((ksize & 1) == 0){
        printf("Detectado tamaño de kernel par (%d). Solo se pueden usar tamaños impares de kernel por lo que se usará %d\n", ksize, ksize + 1);
        ksize++;
    }

    // Image Lecture
    img =  malloc(sizeof(Image));
    if(!readImage(imgname, img)){
        printf("%s: ERROR LEYENDO IMAGEN\n", imgname);
        return 0;
    }

    //printf("Imagen %s cargada\n", imgname);

    // NewImage instance
    newImg =  malloc(sizeof(Image));
    newImg->height = img->height;
    newImg->width = img->width;

    Pixel * * pixels = (Pixel * *) malloc(newImg->height*sizeof(Pixel *));
    for(int i = 0; i < newImg->height; i++) pixels[i] = (Pixel *)malloc(newImg->width*sizeof(Pixel));

    newImg->pixels = pixels;


    // Kernel
    kernel = gaussianKernel(ksize);
    
    // Parallel setup

    omp_set_num_threads(threads);
    // RGB Values
    #pragma omp parallel 
    {
        double rvalue, gvalue, bvalue, sum;

        // Initial x, y and End x, y
        int ix, iy, ex, ey;
        int numThread = omp_get_thread_num();

        // Each thread will make complete rows
        ix = 0;
        ex = img->width;

        // Thre problme will be break on the columns of the img
        // Each thread will make width/threads columns of the image
        iy = img->height * numThread/threads;
        ey = img->height * (numThread + 1)/threads;

        //printf("Runing thread %u from (%d, %d) to (%d, %d)\n", * thread_id, ix, iy, ex, ey);

        for(int i = iy; i <= ey && i < newImg->height; i++) for(int j = ix; j <= ex && j < newImg->width; j++){
            rvalue = gvalue = bvalue = sum = 0;
            // For each location in kernel
            for(int k = -1 * ksize/2; k <= ksize/2; k++) for(int l = -1 * ksize/2; l <= ksize/2; l++) {
                if(i + k < 0 || i + k >= newImg->height || j + l < 0 || j + l >= newImg->width) continue;
                
                rvalue += img->pixels[i + k][j + l].R * kernel[k + ksize/2][l + ksize/2];
                gvalue += img->pixels[i + k][j + l].G * kernel[k + ksize/2][l + ksize/2];
                bvalue += img->pixels[i + k][j + l].B * kernel[k + ksize/2][l + ksize/2];

                // For normalization
                sum += kernel[k + ksize/2][l + ksize/2];
            }


            // Saving calculated values
            newImg->pixels[i][j].R = rvalue/sum;
            newImg->pixels[i][j].G = gvalue/sum;
            newImg->pixels[i][j].B = bvalue/sum;
            //printf("Saving in location (%d, %d) value [%d;%d;%d]\n", i, j, newImg->pixels[i][j].R, newImg->pixels[i][j].G, newImg->pixels[i][j].B);
        }
    }
    

    // Save new image
    writeImage(newImg, newImgName);

    // Free
    freeImage(img);
    freeImage(newImg);


    gettimeofday(&stop, NULL);
    timersub(&stop, &start, &diff);
    printf("Tiempo: %ld.%06ld\n", (long int) diff.tv_sec, (long int) diff.tv_usec);

    return 0;
}
