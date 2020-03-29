#include "file_system.h"
#define M_PI 3.14159265358979323846
#include "omp.h"

/**
 * bsize represents the block size of the blockwise sistem
 * threads represent the number of threads of the algorithm
 * ksize represents the kernel size
 * sigma represents the standard deviation of the kernel
 */ 
int bsize = 32, threads = 4, ksize = 12;
double sigma = 10;

Image *img, *newImg;

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

/**
 * @description:  Función que retorna el intervaloen el que debe actuar un hilo 
 * @param ix: initial x
 * @param iy: initial y
 * @param ex: end x
 * @param ey: end y
 * @return: False si no hay mas intervalos, True en otro caso
 */ 
bool getInterval(int *ix, int * iy, int *ex, int * ey){
    static int cx = 0, cy = 0;

    // End of img
    if(cy * bsize > img->height) return false;

    *ix = cx * bsize;  *iy = cy * bsize;
    *ex = (cx + 1) * bsize;  *ey = (cy + 1) * bsize;

    // Continue in line
    if(cx * bsize <= img->width) cx++;
    // End of line
    else {
        cx = 0;
        cy++;
    }


    return true;
}

int main() {

    char * imgname = "GrandImg.jpg";//"img2.jpg";//"icon.png";//"GrandImg.jpg";
    char * newImgName = "GrandImgBlur.jpg";//"img2Blur.jpg";//"iconBlur.png";//"GrandImgBlur.jpg";

    printf("Ejecutando programa con:\n\t- %d threads\n\t- %d tamaño del kernel\n\t- %d tamaño de bloque\n\t- %.3f sigma\n", threads, ksize, bsize, sigma);
 
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

    printf("Imagen %s cargada\n", imgname);

    // NewImage instance
    newImg =  malloc(sizeof(Image));
    newImg->height = img->height;
    newImg->width = img->width;

    Pixel * * pixels = (Pixel * * )malloc(newImg->height*sizeof(Pixel *));
    for(int i = 0; i < newImg->height; i++) pixels[i] = (Pixel *)malloc(newImg->width*sizeof(Pixel));

    newImg->pixels = pixels;


    // Kernel
    double * * kernel = gaussianKernel(ksize);

    // Paralel region
    omp_set_num_threads(threads);
    #pragma omp parallel
    {
        // Interval to proccess
        int ix, iy, ex, ey;
        // Detects if thread must end
        bool tmp;

        // RGB Values
        double rvalue, gvalue, bvalue, sum;
        while(true){
            // Get next interval to process
            #pragma omp critical
            tmp = getInterval(&ix, &iy, &ex, &ey);

            if(!tmp) break;

            // For each pixel in interval and in image
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
            }
        }
    }

    // Save new image
    writeImage(newImg, newImgName);

    // Free on images
    freeImage(img);
    freeImage(newImg);

    return 0;
}