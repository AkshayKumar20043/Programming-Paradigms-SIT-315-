#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

void quick_sort(int *array, int left, int right) // This is the function of quick sort
{
    int i, j, pivot, temp;

    if (left < right) {
        pivot = left;
        i = left;
        j = right;

        while (i < j) // Loop for doing partition
        {
            while (array[i] <= array[pivot] && i <= right)
                i++;
            while (array[j] > array[pivot])
                j--;
            if (i < j) 
            {
                temp = array[i];
                array[i] = array[j];
                array[j] = temp;
            }
        }

        temp = array[pivot]; // Used for placing pivot 
        array[pivot] = array[j];
        array[j] = temp;

        quick_sort(array, left, j - 1); // Recursive calls to sort sub-arrays
        quick_sort(array, j + 1, right);
    }
}

int main(int argc, char **argv) 
{
    int rank, size, *array, *chunk, a, x, order;
    MPI_Status status;

    MPI_Init(&argc, &argv); // Initializing the MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) 
    {
        printf("no of elements in the array: "); // Used to input array size and its elements``
        fflush(stdout);
        scanf("%d", &a);

        array = (int *) malloc(a * sizeof(int));
        printf("And what are the elements of the array ?: ");
        fflush(stdout);
        for (x = 0; x < a; x++)
            scanf("%d", &array[x]);

        quick_sort(array, 0, a - 1); // This is used for sorting the entire array 
    }

    MPI_Bcast(&a, 1, MPI_INT, 0, MPI_COMM_WORLD); // It broadcast array size to all processes

    chunk = (int *) malloc(a / size * sizeof(int));  // It allocates memory for chunk on each process

    MPI_Scatter(array, a / size, MPI_INT, chunk, a / size, MPI_INT, 0, MPI_COMM_WORLD);

    clock_t start_time = clock(); // Here the timing the sorting process will be started
    quick_sort(chunk, 0, a / size - 1);
    clock_t end_time = clock();

    double duration = (double)(end_time - start_time) / ((double)CLOCKS_PER_SEC); // Used to calculate the duration of sorting

    if (rank == 0) // It prints the execution time 
    {
        printf("Time taken in execution is %f seconds\n", duration);
    }

    for (order = 1; order < size; order *= 2) 
    {
        if (rank % (2 * order) != 0) 
        {
            MPI_Send(chunk, a / size, MPI_INT, rank - order, 0, MPI_COMM_WORLD);
            break;
        }
        int recv_size;
        if ((rank + order) < size)
            recv_size = a / size;
        else
            recv_size = a - (rank + order) * (a / size);

        int *other = (int *) malloc(recv_size * sizeof(int)); // It receives the chunk from higher ranked process
        MPI_Recv(other, recv_size, MPI_INT, rank + order, 0, MPI_COMM_WORLD, &status);

        // Merge chunks
        int *temp = (int *) malloc((a / size + recv_size) * sizeof(int));
        int i = 0, j = 0, k = 0;
        while (i < a / size && j < recv_size) 
        {
            if (chunk[i] < other[j])
                temp[k++] = chunk[i++];
            else
                temp[k++] = other[j++];
        }
        while (i < a / size)
            temp[k++] = chunk[i++];
        while (j < recv_size)
            temp[k++] = other[j++];

        free(other);// Used to free memory and update chunk
        free(chunk);
        chunk = temp;
    }

    if (rank == 0) // Gathers the sorted chunks 
    {
        array = (int *) malloc(a * sizeof(int));
    }
    MPI_Gather(chunk, a / size, MPI_INT, array, a / size, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0)  // Prints the sorted array on rank 0
    {
        for (x = 0; x < a; x++) 
        {
            printf("%d ", array[x]);
        }
        printf("\n");
        fflush(stdout);
    }

    // Finalize MPI
    MPI_Finalize();
    return 0;
}
