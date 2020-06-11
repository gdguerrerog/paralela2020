#include <stdio.h>
#include <mpi.h>

double calcPi(long long iteraciones, int nhilos, int index){
	double tmppi = 0;
	long long i = (iteraciones/nhilos) * index, end = (iteraciones/nhilos) * (index + 1);
	
    printf("In process %d, running from %lli to %lli\n", index, i, end);

	do{
	
		if((i & 1) == 0) tmppi += (double)(4.0/((i << 1) | 1)); // (long double)(4.0/((2 * i) + 1));
		else tmppi -=  (double)(4.0/((i << 1) | 1)); // (long double)(4.0/((2 * i) + 1));
		i++;
		
	}while(i < end);
	return tmppi;
}

int main(int argc, char **argv){
    int rank, size;
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    long long iterations=2e9;

    double pi = calcPi(iterations, size, rank);
    printf("Pi in process %d is %.20f\n", rank, pi);

    if(rank == 0){
        double recivedPi;
        for(int i = 1; i < size; i++) {
            MPI_Recv(&recivedPi, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, &status);
            pi += recivedPi;
        }
        printf("Pi: %.20f\n", pi);
    }else{
        MPI_Send(&pi, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
}