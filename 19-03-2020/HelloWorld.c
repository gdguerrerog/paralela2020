#include <stdio.h>
#include "omp.h"
void main() {
    
	#pragma omp parallel // Inicio de region paralela
	{
		int ID = omp_get_thread_num();
		printf("Hello(%d)",  ID);
		printf(" World(%d)\n",  ID);
	} // Fin de region paralela
}
