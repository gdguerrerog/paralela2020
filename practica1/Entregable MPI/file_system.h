#include <stdint.h>
#include <stdbool.h>

#define STB_IMAGE_IMPLEMENTATION
#include "libs/stb/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "libs/stb/stb_image_write.h"


typedef struct Image{
    int width, height;
    uint8_t * red, * green, * blue;
} Image;


bool readImage(char * name, Image * img){
    int width, height, bpp;

    uint8_t *rgb_image = stbi_load(name, &width, &height, &bpp, STBI_rgb);
    if(rgb_image == NULL) return false;

    uint8_t * red = (uint8_t *) malloc(height*width*sizeof(uint8_t));
    uint8_t * green = (uint8_t *) malloc(height*width*sizeof(uint8_t));
    uint8_t * blue = (uint8_t *) malloc(height*width*sizeof(uint8_t));

    for(int i = 0; i < width*height; i++){
        red[i] =  rgb_image[3*i]; 
        green[i] =  rgb_image[3*i + 1]; 
        blue[i] =  rgb_image[3*i + 2]; 
    }

    stbi_image_free(rgb_image);
    
    (*img).red = red;
    (*img).green = green;
    (*img).blue = blue;
    (*img).width = width;
    (*img).height = height;

    return true;
}

void writeImage(Image * img, char * name){

    int width = img->width, height = img->height;
    uint8_t* rgb_image = (uint8_t*)malloc(width*height*3* sizeof(uint8_t));

    for(int i = 0; i < width*height; i++){
        rgb_image[3 * i] = img->red[i];
        rgb_image[(3 * i) + 1] = img->green[i];
        rgb_image[(3 * i) + 2] = img->blue[i];
    }

    stbi_write_png(name, width, height, 3, rgb_image, width*3);
}

void printImage(Image * img){
    for(int i = 0; i < img->height; i++){
        for(int j = 0; j < img -> width; j++){
            printf("[%03d;%03d;%03d] ", img->red[img->width*i + j], img->green[img->width*i + j], img->blue[img->width*i + j]);
        }

        printf("\n");
    }
}

void freeImage(Image * img){
    free(img->red);
    free(img->green);
    free(img->blue);
    free(img);
}