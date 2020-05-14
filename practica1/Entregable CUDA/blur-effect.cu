// Compile with nvcc blur-effect.cu -o blur-effect -I /usr/local/cuda/samples/common/inc -lm to include helper_cuda.h

#include "file_system.h"
#define M_PI 3.14159265358979323846
#include <sys/time.h>


#include <cuda_runtime.h>
#include <cuda.h>
#include <helper_cuda.h>

/**
 * bsize represents the block size of the blockwise sistem
 * threads represent the number of threads of the algorithm
 * ksize represents the kernel size
 * sigma represents the standard deviation of the kernel
 */ 

int bsize = 32,  ksize = 15;
double threadsPerCore = 2;
double sigma = 10;
Image *img, *newImg;
double * * kernel;

double * * gaussianKernel(int size){
    double mean = size/2;
    double * * kernel = (double * *)malloc(size * sizeof(double *));
    for(int i = 0; i < size; i++){
        kernel[i] = (double *)malloc(size * sizeof(double));
        for(int j = 0; j < size; j++){
            kernel[i][j] = exp( -0.5 * (pow((i-mean)/sigma, 2.0) + pow((j-mean)/sigma,2.0)))
                         / (2 * M_PI * sigma * sigma);
        }
    }
    return kernel;
}


__global__  
void blur(uint8_t * color, uint8_t * newColor, double * kernel, int ksize, int imgWidth, int imgHeight, double iterations){
    
    int i;

    // Put on shared memory the kernel
    extern __shared__ double skernel[];

    for (i = threadIdx.x; i < ksize*ksize; i += blockDim.x) {
        skernel[i] = kernel[i];
    }
    __syncthreads();

    int index = blockIdx.x*blockDim.x+threadIdx.x;
    int x, y, k, l;
    double sum, cColor;
    for (i = iterations*index; i < iterations*(index + 1) && i < imgWidth*imgHeight; i++){
        x = i % imgWidth;
        y = i / imgWidth;
        sum = cColor = 0;
        // For each location in kernel
        for(k = ksize/-2; k <= ksize/2; k++) for(l = ksize/-2; l <= ksize/2; l++) {
            if(y + k < 0 || y + k >= imgHeight || x + l < 0 || x + l >= imgWidth) continue;
            
            cColor += color[(y + k)*imgWidth + x + l] * skernel[(k + ksize/2)*ksize + l + ksize/2];
            // For normalization
            sum += skernel[(k + ksize/2)*ksize + l + ksize/2];
        }        

        newColor[i] = cColor/sum;
    }
}

int main(int argc, char **argv) {

    
    // For time stamp
    struct timeval start, stop, diff;
    gettimeofday(&start, NULL);
    
    char * imgname = argv[1]; 		// "GrandImg.jpg";//"img2.jpg";//"icon.png";//"GrandImg.jpg";
    char * newImgName = argv[2];	//"GrandImgBlur.jpg";//"img2Blur.jpg";//"iconBlur.png";//"GrandImgBlur.jpg";
    ksize = atoi(argv[3]);
    threadsPerCore = atof(argv[4]);
    //printf("Ejecutando programa con:\n\t- %d threads\n\t- %d tamaño del kernel\n\t- %d tamaño de bloque\n\t- %.3f sigma\n", threads, ksize, bsize, sigma);
 
    // Even kernel size
    if((ksize & 1) == 0){
        printf("Detectado tamaño de kernel par (%d). Solo se pueden usar tamaños impares de kernel por lo que se usará %d\n", ksize, ksize + 1);
        ksize++;
    }

    // Image Lecture
    Image * img =  (Image *)malloc(sizeof(Image));
    if(!readImage(imgname, img)){
        printf("%s: ERROR LEYENDO IMAGEN\n", imgname);
        return 0;
    }

    //printf("Imagen %s cargada\n", imgname);

    // Kernel
    kernel = gaussianKernel(ksize);
    
    ///////////////////////////////////////Parallel setup/////////////////////////////////////

    //CARD INFO
    cudaSetDevice(0);
    cudaDeviceProp deviceProp;
    cudaGetDeviceProperties(&deviceProp, 0);

    int BLOCKS = 2 * deviceProp.multiProcessorCount, THREADS = threadsPerCore * _ConvertSMVer2Cores(deviceProp.major, deviceProp.minor);

    // Alloc Kernel in memory
    double * kDevice;
    cudaMalloc(&kDevice, ksize*ksize*sizeof(double));
    for(int i = 0; i < ksize; i++){
        cudaMemcpy(kDevice + i*ksize, kernel[i], ksize*sizeof(double), cudaMemcpyHostToDevice);
    }

    // Memory 
    int arrSize = img->width*img->height*sizeof(uint8_t);
    uint8_t * colorDevice, * colorNewDevice;
    
    cudaMalloc(&colorDevice, arrSize);
    cudaMalloc(&colorNewDevice, arrSize);

    double iterations = (1.0*img->width*img->height)/(BLOCKS * THREADS);
    
    if(BLOCKS * THREADS > img->width*img->height) iterations = 1;
    int sharedMemory = ksize*ksize*sizeof(double);

    printf("Size: %d, iterations: %f\n", img->width*img->height, iterations * BLOCKS * THREADS);

    printf("Running with %d threads, %d BLocks, %d ksize, %f iterations\n", THREADS, BLOCKS, ksize, iterations);

    uint8_t * currentPointer;

    for (int i = 0; i < 3; i++){
        printf("Running color %d\n", i);
        // Alloc Image in memory;
        switch(i){
            case 0: currentPointer = img->red; break;
            case 1: currentPointer = img->blue; break;
            case 2: currentPointer = img->green; break;
        }

        cudaMemcpy(colorDevice, currentPointer, arrSize, cudaMemcpyHostToDevice);

        // Execution
        blur<<<BLOCKS, THREADS, sharedMemory>>>(colorDevice, colorNewDevice, kDevice, ksize, img->width, img->height, iterations);	

        // Copy results
        cudaMemcpy(currentPointer, colorNewDevice, arrSize, cudaMemcpyDeviceToHost);
        
    }

    // Save new image
    //printf("Guardando imagen\n");
    writeImage(img, newImgName);

    // Free
    cudaFree(colorDevice);
    cudaFree(colorNewDevice);
    freeImage(img);

    //Stadistics
    gettimeofday(&stop, NULL);
    timersub(&stop, &start, &diff);
    printf("Tiempo: %ld.%06ld\n", (long int) diff.tv_sec, (long int) diff.tv_usec);

    return 0;
}
