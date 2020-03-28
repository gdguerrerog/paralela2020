#include "file_system.h"
#define M_PI 3.14159265358979323846

double * * gaussianKernel(int size){
    double sigma = 1, mean = size/2;
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



int main() {

    char * imgname = "img2.jpg";//"icon.png";//"GrandImg.jpg";
    char * newImgName = "img2Blur.jpg";//"iconBlur.png";//"GrandImgBlur.jpg";

    /*
    Image img;
    readImage(imgname, &img);
    writeImage(&img, newImgName);
    //*/
    //*
    int ksize = 3;
    int threads = 4;

    Image img;
    if(!readImage(imgname, &img)){
        printf("ERROR LEYENDO IMAGEN\n");
        return 0;
    }

    Image newImage;
    newImage.height = img.height;
    newImage.width = img.width;

    Pixel * * pixels = (Pixel * * )malloc(newImage.height*sizeof(Pixel *));
    for(int i = 0; i < newImage.height; i++) pixels[i] = (Pixel *)malloc(newImage.width*sizeof(Pixel));

    newImage.pixels = pixels;

    double * * kernel = gaussianKernel(ksize);

    double rvalue, gvalue, bvalue, sum;
    // For each pixel
    for(int i = 0; i < newImage.height; i++) for(int j = 0; j < newImage.width; j++){
        rvalue = gvalue = bvalue = sum = 0;
        // For each location in kernel
        for(int k = -1 * ksize/2; k <= ksize/2; k++) for(int l = -1 * ksize/2; l <= ksize/2; l++) {
            if(i + k < 0 || i + k >= newImage.height || j + l < 0 || j + l >= newImage.width) continue;
            
            rvalue += img.pixels[i + k][j + l].R * kernel[k + ksize/2][l + ksize/2];
            gvalue += img.pixels[i + k][j + l].G * kernel[k + ksize/2][l + ksize/2];
            bvalue += img.pixels[i + k][j + l].B * kernel[k + ksize/2][l + ksize/2];

            sum += kernel[k + ksize/2][l + ksize/2];
        }

        newImage.pixels[i][j].R = rvalue/sum;
        newImage.pixels[i][j].G = gvalue/sum;
        newImage.pixels[i][j].B = bvalue/sum;
    }

    writeImage(&newImage, newImgName);
    freeImage(&img);
    freeImage(&newImage);
    //*/

    return 0;
}