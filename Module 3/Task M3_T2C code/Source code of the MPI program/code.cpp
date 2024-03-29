#include <iostream>
#include <stdlib.h> 
#include <time.h>  
#include <mpi.h>  

#define MAX 1000000 // the size of the array is defined

int data[MAX]; 

int main(int argc, char *argv[])
{
    int rank, size;
    MPI_Init(&argc, &argv); // Initialization of MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Get the rank of the current process
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Get the total number of processes

    srand(time(NULL) + rank); // Seed the random number generator with different values for each process

    int local_size = MAX / size; // Calculate the local size of data for each process
    int local_data[local_size]; // Declare an array to hold local data for each process

    for(int i = 0; i < local_size; i++) {
        local_data[i] = rand() % 20; 
    }

    double start_time = MPI_Wtime(); // Record start time using MPI_Wtime() function
    long local_sum = 0;

    // Calculate the local sum of the local data array for each process
    for(int i = 0; i < local_size; i++) {
        local_sum += local_data[i];
    }

    long global_sum; // Declare a variable to hold the global sum

    // Reduce local sums from all processes to calculate the global sum
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    double end_time = MPI_Wtime(); // Record end time using MPI_Wtime() function

    if(rank == 0) {
        std::cout << "The final sum = " << global_sum << std::endl;
        std::cout << "Execution time = " << end_time - start_time << " seconds" << std::endl;
    }

    MPI_Finalize(); // Finalize MPI
    return 0;
}
