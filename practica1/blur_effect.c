#include <stdint.h>
#define STB_IMAGE_IMPLEMENTATION
#include "libs/stb/stb_image.h"

typedef struct Pixel{
    unsigned char R, G, B;
} Pixel;

int main() {
    int width, height, bpp;

    unsigned char *rgb_image = stbi_load("LittleRGB.jpg", &width, &height, &bpp, STBI_rgb);

    if(rgb_image == NULL){
        printf("ERROR LOADING IMAGE\n");
        return 0;
    }

    printf("width: %d, height: %d, bpp: %d\n", width, height, bpp);

    Pixel image[height][width];
    for(int i = 0; i < width*height; i++){
            image[i/height][i % height].R = rgb_image[3*i]; 
            image[i/height][i % height].G = rgb_image[3*i + 1]; 
            image[i/height][i % height].B = rgb_image[3*i + 2];
            
    }

    stbi_image_free(rgb_image);


    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            printf("[%03d;%03d;%03d] ", image[i][j].R, image[i][j].G, image[i][j].B);
        }

        printf("\n");
    }

    

    return 0;
}