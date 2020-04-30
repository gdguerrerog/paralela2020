// Compile with nvcc calcPiCUDA.cu -o calcPi -I /usr/local/cuda/samples/common/inc to include helper_cuda.h

#include <stdio.h>

// For the CUDA runtime routines (prefixed with "cuda_")
#include <cuda_runtime.h>
#include <cuda.h>
#include <helper_cuda.h>

#define ITERATIONS 8e9

__global__ void
caclPi(double * vals, long long size)
{
    int indx = blockDim.x * blockIdx.x + threadIdx.x;

    double pi = 0;
    for(long long i = indx * size; i < (indx + 1)*size; i++){
        if((i & 1) != 0) pi -= 4.0/((i << 1) | 1); // (double)(4.0/((2 * i) + 1));
        else pi += 4.0/((i << 1) | 1);
    }

    vals[indx] = pi;
}

/**
 * Host main routine
 */
int
main(void)
{

    cudaSetDevice(0);
    cudaDeviceProp deviceProp;
    cudaGetDeviceProperties(&deviceProp, 0);

    int BLOCKS = deviceProp.multiProcessorCount, THREADS = 2 * _ConvertSMVer2Cores(deviceProp.major, deviceProp.minor);

    int size = BLOCKS * THREADS;

    double * values = (double *)malloc(size * sizeof(double));

    double * d_values = NULL;

    cudaMalloc(&d_values, size * sizeof(double));

    long long iterations_per_thread = ITERATIONS/size;

    printf("Running with %d multiprocessor, and %d threads for MP.\nIterations: %.0f, iterations per thread: %lli\n", 
        BLOCKS, THREADS, ITERATIONS, iterations_per_thread);

    caclPi<<<BLOCKS, THREADS>>>(d_values, iterations_per_thread);

    cudaMemcpy(values, d_values, size * sizeof(double), cudaMemcpyDeviceToHost);

    double pi = 0;

    for(int i = 0; i < size; i++){
        pi += values[i];
    }

    printf("pi: %.20f\n", pi);


    cudaFree(d_values);
    free(values);

    return 0;
}

