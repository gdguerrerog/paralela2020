#include <stdio.h> 
 
__global__ void calcpi(long long iteraciones, int nhilos, double *pi){
	
	int index = blockIdx.x*blockDim.x+threadIdx.x;
	double tmppi = 0;
	//printf("Hilo: %d \n", index);
	long long i = (iteraciones/nhilos) * index, end = (iteraciones/nhilos) * (index + 1);
	
	do{
	
		if((i & 1) == 0) tmppi += (double)(4.0/((i << 1) | 1)); // (long double)(4.0/((2 * i) + 1));
		else tmppi -=  (double)(4.0/((i << 1) | 1)); // (long double)(4.0/((2 * i) + 1));
		i++;
		
	}while(i < end);
	pi[index] = tmppi;
}
int main(void) {
	long long iterations=2e10;
	int nhilos=1280;
	
	double *pi, *pi_device;
	
	cudaMalloc(&pi_device, 1280*sizeof(double));
	
	pi=(double *)malloc(1280*sizeof(double));


	//cudaMemcpy(ite_device,iterations, sizeof(int), cudaMemcpyHostToDevice);
	//cudaMemcpy(nhilos_device,nhilos, sizeof(int), cudaMemcpyHostToDevice);
	//cudaMemcpy(pi_device,pi, 1280*sizeof(double), cudaMemcpyHostToDevice);

	calcpi<<<5,256>>>(iterations,nhilos,pi_device);	

	cudaMemcpy(pi,pi_device, 1280*sizeof(double), cudaMemcpyDeviceToHost);
	
	double out;
	for(int i=0; i<1280; i++){	
		out+=pi[i];
	}
	printf("PI: %.20f. \n", out);
	
	free(pi);
    cudaFree(pi_device);
    
	return 0;
}
