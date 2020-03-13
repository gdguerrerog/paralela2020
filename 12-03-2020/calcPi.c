#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>

#include <time.h>

// Parámetros de la función
struct params{
	long double piTotal;
	int initIteration;
	int endIteration;
	pthread_t thread_id;
};

// Funcion a paralelizar
static void * calculatePi(void* params){
	struct params * info = params;
	int i = (*info).initIteration;
	
	do{
	
		if((i & 1) == 0){
			(*info).piTotal = (*info).piTotal + (long double)(4.0/((i << 1) + 1));
		}else {
			(*info).piTotal = (*info).piTotal - (long double)(4.0/((i << 1) + 1));
		}
		i++;
		
		//printf("\n%i pi: %2.10lf \n", i, (long double)(4.0/((i << 1) + 1)));		
	}while(i < (*info).endIteration);

	return 0;
}  

int main(){
	double pi, pi_hijo;
	pid_t pid;
	pi = 0;
	
	int total_threads = 1, totalIterations = 1000000000;
	
	int start, end;
	start = (int)time(NULL);
	
	pthread_t thread;
	pthread_attr_t attr;
	
	// Inicializar atributos del hilo
	int s = pthread_attr_init(&attr);
	
	// Información para cada hilo
	struct params *tinfo;
	
	tinfo = calloc(total_threads, sizeof(struct params));
	
	// Error instanciando el arreglo con los parametros de cada hilo
	if(tinfo == NULL){
		printf("Error En tinfo.\n");
		return -1;
	}

	
	for(int i = 0; i < total_threads; i++){
		tinfo[i].piTotal = 0;
		tinfo[i].initIteration = i * totalIterations/total_threads;
		tinfo[i].endIteration = (i + 1) * totalIterations/total_threads;
		
		s = pthread_create(&tinfo[i].thread_id, NULL, &calculatePi, &tinfo[i]);
		if(s != 0) printf("Error creando hilo %i\n", i);	
			
	}
	
	void *res;

	long double currentSum = 0;
	for(int i = 0; i < total_threads; i++){
		s = pthread_join(tinfo[i].thread_id, &res);
		if(s != 0) printf("Error esperando hilo %i\n", i);	
		currentSum += tinfo[i].piTotal;
	}
	
	
	end = (int)time(NULL) - start;
	
	
	printf("\nCon %d hilos, FInal Pi: %2.10Lf. Se demora %d segs\n", total_threads, currentSum, end);
	
	
}
