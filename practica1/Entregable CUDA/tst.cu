#include "file_system.h"

#include <stdio.h> 
#include <cuda_runtime.h>
#include <cuda.h>
#include <helper_cuda.h>

__global__ void blur(uint8_t * red, uint8_t * redNewDevice, int width, int height){
    printf("Here\n");
    for(int i = 0; i < width * height; i++) redNewDevice[i] = red[i];
}
int main(void) {

    char * imgname = "LittleRGB.jpg"; //argv[1]; 		// "GrandImg.jpg";//"img2.jpg";//"icon.png";//"GrandImg.jpg";

     // Image Lecture
    Image * img =  (Image *)malloc(sizeof(Image));
    if(!readImage(imgname, img)){
        printf("%s: ERROR LEYENDO IMAGEN\n", imgname);
        return 0;
    }

    int arrSize = img->width*img->height*sizeof(uint8_t);
    printf("size:%d\n", arrSize);

    uint8_t * redDevice, * redNewDevice, * redNewHost = (uint8_t * )malloc(arrSize);

    cudaMalloc(&redDevice, arrSize);
    cudaMalloc(&redNewDevice, arrSize);

    cudaMemcpy(redDevice, img->red, arrSize, cudaMemcpyHostToDevice);

    blur<<<1, 1>>>(redDevice, redNewDevice, img->width, img->height);	

    cudaMemcpy(redNewHost, redNewDevice, arrSize, cudaMemcpyDeviceToHost);

    for(int i = 0; i < img->width*img->height; i++) printf("%d  ", img->red[i]);
    printf("\n");
    for(int i = 0; i < img->width*img->height; i++) printf("%d  ", redNewHost[i]);
    printf("\n");

    cudaFree(redDevice);
    cudaFree(redNewDevice);
    freeImage(img);
    free(redNewHost);
    
	return 0;
}
