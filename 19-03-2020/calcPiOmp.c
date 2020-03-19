#include <stdio.h>
#include "omp.h"
#include <sys/time.h>

#define iterations 1e09
#define THREADS 32
#define intentos 10 
// Funcion a paralelizar
static void calculatePi(int ID, double * result){
	int i = (iterations/THREADS) * ID, end = (iterations/THREADS) * (ID + 1);
	
	do{
	
		if((i & 1) == 0){
			*result = *result + (long double)(4.0/((i << 1) | 1)); // (long double)(4.0/((2 * i) + 1));
		}else {
			*result = *result - (long double)(4.0/((i << 1) | 1)); // (long double)(4.0/((2 * i) + 1));
		}
		i++;
		
	}while(i < end);

}  

int main(){
    struct timeval start, stop;

    double secs[intentos];

    for(int i = 0; i < intentos; i++){
        printf("Begin run with %d threads\n", THREADS);
        gettimeofday(&start, NULL);
        double results[THREADS];
        for(int i = 0; i < THREADS; i++) results[i] = 0;
        #pragma omp parallel num_threads(THREADS)// Inicio de region paralela
        {
            int ID = omp_get_thread_num();
            //printf("Running %d thread.\n", ID);
            calculatePi(ID, &results[ID]);
            //printf("results[%d] = %.15f\n", ID, results[ID]);

        }

        double final = 0;
        for(int i = 0; i < THREADS; i++) final += results[i];

        gettimeofday(&stop, NULL);

        double sec = (double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec);
        secs[i] = sec;
        printf("PI: %.15f. Time: %.15f\n", final, sec);
    }

    double sum = 0;

    for(int i = 0; i < intentos; i++) sum += secs[i];

    printf("%d: media: %.15f\n", THREADS,  sum/intentos);
    for(int i = 0; i < intentos; i++){
        printf("    - %.15f\n", secs[i]);
    }
    

    return 0;

}
