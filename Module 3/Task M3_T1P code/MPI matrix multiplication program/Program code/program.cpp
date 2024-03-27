#include <iostream>
#include <fstream>
#include <chrono>
#include <cstdlib>
#include <vector>
#include <mpi.h>

using namespace std;

void random_matrix(vector<vector<int>> &matrix, int size)
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            matrix[i][j] = rand() % 10; // It generates random numbers between 0 and 9
        }
    }
}

void multiplication_mat(vector<vector<int>> &A, vector<vector<int>> &B, vector<vector<int>> &C, int size, int startRow, int endRow)
{
    for (int i = startRow; i < endRow; i++)
    {
        for (int j = 0; j < size; j++)
        {
            C[i][j] = 0;
            for (int k = 0; k < size; k++)
            {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

int main(int argc, char **argv)
{
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int matrixSize = 0; // Initial size of matrix
    if (rank == 0)
    {
        cout << "Enter size of the matrix - ";
        cin >> matrixSize;
    }
    MPI_Bcast(&matrixSize, 1, MPI_INT, 0, MPI_COMM_WORLD);

    cout << "Performing calculation of matrix sized - " << matrixSize << "X" << matrixSize << " on process " << rank << endl;

    int rowsPerProcess = matrixSize / size;
    int startRow = rank * rowsPerProcess;
    int endRow = (rank == size - 1) ? matrixSize : startRow + rowsPerProcess;

    vector<vector<int>> A(matrixSize, vector<int>(matrixSize));
    vector<vector<int>> B(matrixSize, vector<int>(matrixSize));
    vector<vector<int>> C(matrixSize, vector<int>(matrixSize));

    srand(time(NULL)); // It is used for seeding random number

    random_matrix(A, matrixSize);
    random_matrix(B, matrixSize);

    MPI_Barrier(MPI_COMM_WORLD); // Wait for all processes to reach this point before starting the timer
    auto start_time = std::chrono::high_resolution_clock::now(); // Timer to start

    multiplication_mat(A, B, C, matrixSize, startRow, endRow); // It performs multiplication

    auto end_time = std::chrono::high_resolution_clock::now(); // Timer to stop

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time); // It calculates time duration of multiplication

    ofstream outputFile("output.txt");
    if (outputFile.is_open())
    {
        if (rank == 0)
        {
            for (int i = 0; i < matrixSize; i++)
            {
                for (int j = 0; j < matrixSize; j++)
                {
                    outputFile << C[i][j] << " ";
                }
                outputFile << endl;
            }
        }
        outputFile.close();
    }
    else
    {
        cout << "Error ";
    }

    MPI_Finalize();

    if (rank == 0)
    {
        cout << "The execution time of multiplication: " << duration.count() << " microseconds" << endl;
    }

    return 0;
}
