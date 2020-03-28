#include "file_system.h"

int main() {
    Image img;
    if(!readImage("LittleBlue.jpg", &img)){
        printf("ERROR LOADING IMAGE\n");
        return 0;
    }

    printImage(&img);
    img.pixels[2][2].R = 0xFF;
    img.pixels[2][2].G = 0xFF;
    img.pixels[2][2].B = 0xFF;
    printImage(&img);
    writeImage(&img, "image_created.png");
    
    freeImage(&img);
    return 0;
}