#include <iostream>
#include <fstream>
#include <chrono>
#include <cstdlib>
#include <vector>
#include <omp.h>

using namespace std;

void random_matrix(vector<vector<int>> &matrix, int size)
{
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            matrix[i][j] = rand() % 10; // It generates random numbers between 0 and 9
        }
    }
}

void multiplication_mat(vector<vector<int>> &A, vector<vector<int>> &B, vector<vector<int>> &C, int size)
{
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < size; i++)
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

int main()
{
    int size = 0; // Initial size of matrix
    cout << "Enter size of the matrix - ";

    cin >> size;
    
    cout << "Performing calculation of matrix sized - " << size << "X" << size << endl;
    vector<vector<int>> A(size, vector<int>(size));
    vector<vector<int>> B(size, vector<int>(size));
    vector<vector<int>> C(size, vector<int>(size));

    srand(time(NULL)); // It is used for seeding random number

    random_matrix(A, size);
    random_matrix(B, size);

    auto start_time = std::chrono::high_resolution_clock::now(); // Timer to start

    multiplication_mat(A, B, C, size); // It performs multiplication

    auto end_time = std::chrono::high_resolution_clock::now(); // Timer to stop

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time); // It calculates time duration of multiplication

    ofstream outputFile("output.txt");
    if (outputFile.is_open())
    {
        for (int i = 0; i < size; i++)
        {
            for (int j = 0; j < size; j++)
            {
                outputFile << C[i][j] << " ";
            }
            outputFile << endl;
        }
        outputFile.close();
    }
    else
    {
        cout << "Error ";
    }

    cout << "The execution time of multiplication: " << duration.count() << " microseconds" << endl;

    return 0;
}
