#include <stdint.h>
#include <stdbool.h>

#define STB_IMAGE_IMPLEMENTATION
#include "libs/stb/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "libs/stb/stb_image_write.h"


typedef struct Pixel{
    uint8_t R, G, B;
} Pixel;

typedef struct Image{
    int width, height;
    Pixel * * pixels;
} Image;


bool readImage(char * name, Image * img){
    int width, height, bpp;

    uint8_t *rgb_image = stbi_load(name, &width, &height, &bpp, STBI_rgb);
    if(rgb_image == NULL) return false;
    

    Pixel * * image = (Pixel * * )malloc(height* sizeof(Pixel *));
    for(int i = 0; i < height; i++) image[i] = (Pixel *)malloc(width*sizeof(Pixel));

    int r = 0, c = 0;
    for(int i = 0; i < width*height; i++){

        image[r][c].R = rgb_image[3*i]; 
        image[r][c].G = rgb_image[3*i + 1]; 
        image[r][c].B = rgb_image[3*i + 2];    

        if(i % width != width - 1) c++;
        else {
            c = 0;
            r++;
        }
    }

    /*for(int i = 0; i < width*height; i++){
        image[i%height][i/height].R = rgb_image[3*i]; 
        image[i%height][i/height].G = rgb_image[3*i + 1]; 
        image[i%height][i/height].B = rgb_image[3*i + 2];    
    }*/

    stbi_image_free(rgb_image);

    img->pixels = image;
    img->width = width;
    img->height = height;

    return true;
}

void writeImage(Image * img, char * name){

    int width = img->width, height = img->height;
    uint8_t* rgb_image = (uint8_t*)malloc(width*height*3* sizeof(uint8_t));

    int r = 0, c = 0;

    for(int i = 0; i < width*height; i++){
        rgb_image[3 * i] = img->pixels[r][c].R;
        rgb_image[(3 * i) + 1] = img->pixels[r][c].G;
        rgb_image[(3 * i) + 2] = img->pixels[r][c].B;

        if(i % width != width - 1) c++;
        else {
            c = 0;
            r++;
        }
    }

    stbi_write_png(name, width, height, 3, rgb_image, width*3);
}

void printImage(Image * img){
    for(int i = 0; i < img->height; i++){
        for(int j = 0; j < img -> width; j++){
            printf("[%03d;%03d;%03d] ", img->pixels[i][j].R, img->pixels[i][j].G, img->pixels[i][j].B);
        }

        printf("\n");
    }
}

void freeImage(Image * img){
    for(int i = 0 ; i < img->height; i++) free(img->pixels[i]);
    free(img->pixels);
}