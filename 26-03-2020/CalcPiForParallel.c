#include <stdio.h>
#include "omp.h"
#include <sys/time.h>

#define iterations 1e09
#define THREADS 4
#define intentos 10 

int main(){
    struct timeval start, stop, diff;
    gettimeofday(&start, NULL);

    double pi = 0; int i;
    #pragma omp parallel num_threads(THREADS)// Inicio de region paralela
    {
        #pragma omp parallel for reduction(-:pi)
        for(i = 0; i < iterations; i += 2){
            pi -= (long double)(4.0/((i << 1) | 1)); // (long double)(4.0/((2 * i) + 1));
        }

        #pragma omp parallel for reduction(+:pi)
        for(i = 1; i < iterations; i += 2){
			pi +=  (long double)(4.0/((i << 1) | 1)); // (long double)(4.0/((2 * i) + 1));
        }

    }

    gettimeofday(&stop, NULL);

    timersub(&stop, &start, &diff);
    printf("Time: %ld.%06ld\n", (long int) diff.tv_sec, (long int) diff.tv_usec);
    printf("PI: %.15f. \n", final);

    return 0;

}