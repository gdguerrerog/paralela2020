// compile with mpicc blur-effect.c -o blur-effect -lm
// run with mpirun -np 6 --hostfile mpi-hosts ./blur-effect 720.jpg 720blur.png 11

#include "file_system.h"
#include <sys/time.h>
#include <mpi.h>


#define M_PI 3.14159265358979323846

/**
 * ksize represents the kernel size
 * sigma represents the standard deviation of the kernel
 */ 

int ksize = 15;
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

  
uint8_t * blur(uint8_t * color, int imgWidth, int imgHeight, int iterations, int index){
    

    uint8_t * newColor = (uint8_t *)malloc(iterations*sizeof(uint8_t));

    int i;
    
    int x, y, k, l;
    double sum, cColor;
    for (i = iterations*index; i < iterations*(index + 1) && i < imgWidth*imgHeight; i++){
        x = i % imgWidth;
        y = i / imgWidth;
        sum = cColor = 0;
        // For each location in kernel
        for(k = ksize/-2; k <= ksize/2; k++) for(l = ksize/-2; l <= ksize/2; l++) {
            if(y + k < 0 || y + k >= imgHeight || x + l < 0 || x + l >= imgWidth) continue;
            
            cColor += color[(y + k)*imgWidth + x + l] * kernel[k + ksize/2][l + ksize/2];
            // For normalization
            sum += kernel[k + ksize/2][l + ksize/2];
        }        

        newColor[i - iterations*index] = cColor/sum;
    }

    return newColor;
}

int main(int argc, char **argv) {

    
    // For time stamp
    struct timeval start, stop, diff, pstart, pend;
    gettimeofday(&start, NULL);
    
    char * imgname = argv[1]; 		// "GrandImg.jpg";//"img2.jpg";//"icon.png";//"GrandImg.jpg";
    char * newImgName = argv[2];	//"GrandImgBlur.jpg";//"img2Blur.jpg";//"iconBlur.png";//"GrandImgBlur.jpg";
    ksize = atoi(argv[3]);
 
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

    printf("Imagen %s cargada\n", imgname);

    // Kernel
    kernel = gaussianKernel(ksize);
    
    ///////////////////////////////////////Parallel setup/////////////////////////////////////

    int rank, size;
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int iterations = (img->width*img->height)/(size) + 1;

    printf("Running process %d with %d iterations (img: %d)\n", rank, iterations, img->width*img->height);

    uint8_t * currentPointer, * exit;

    for (int i = 0; i < 3; i++){
        printf("Running color %d in process %d\n", i, rank);
        // Alloc Image in memory;
        switch(i){
            case 0: currentPointer = img->red; break;
            case 1: currentPointer = img->blue; break;
            case 2: currentPointer = img->green; break;
        }


        exit = blur(currentPointer, img->width, img->height, iterations, rank);

        if(rank != 0) MPI_Send(exit, iterations, MPI_UNSIGNED_CHAR, 0, 0, MPI_COMM_WORLD);
        else{
            
            for(int i = 0; i < iterations && i < img->height*img->width; i++) currentPointer[i] = exit[i];

            for(int j = 1; j < size; j++) {
                printf("Ready to recive from process %d\n", j);
                MPI_Recv(exit, iterations, MPI_UNSIGNED_CHAR, j, 0, MPI_COMM_WORLD, &status);
                printf("Recived data from process %d\n", j);
                int k;
                for(k = 0; k < iterations && j*iterations + k < img->width*img->height; k++) currentPointer[j*iterations + k] = exit[k];
                printf("Added %d elements from process %d\n", k, j);
            }
        }

    }

    if(rank == 0){
        // Save new image
        printf("Guardando imagen\n");
        writeImage(img, newImgName);

        //Stadistics
        gettimeofday(&stop, NULL);
        timersub(&stop, &start, &diff);
        printf("Tiempo: %ld.%06ld\n", (long int) diff.tv_sec, (long int) diff.tv_usec);
    }

    // Free
    freeImage(img);

    MPI_Finalize();

    return 0;
}
