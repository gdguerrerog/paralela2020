#include <stdio.h>
#include "omp.h"
#include <time.h>


#define iterations 1e09
#define THREADS 8
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
		
		//printf("\n%i pi: %2.10lf \n", i, (long double)(4.0/((i << 1) + 1)));		
	}while(i < end);

}  

int main(){
    printf("Begin run with %d threads\n", THREADS);
	double results[THREADS];
    for(int i = 0; i < THREADS; i++) results[i] = 0;
	#pragma omp parallel num_threads(THREADS)// Inicio de region paralela
	{
		int ID = omp_get_thread_num();
        printf("Running %d thread.\n", ID);
		calculatePi(ID, &results[ID]);
        printf("results[%d] = %.15f\n", ID, results[ID]);

	}

	double final = 0;
	for(int i = 0; i < THREADS; i++) final += results[i];

	printf("PI: %.15f\n", final);

    return 0;

}
