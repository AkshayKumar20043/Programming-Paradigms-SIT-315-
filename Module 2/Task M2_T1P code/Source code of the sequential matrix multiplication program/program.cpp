#include <iostream>
#include <fstream>
#include <chrono>
#include <random>

using namespace std;

void multiplication_proc(double **matrixA, double **matrixB, double **Result, int length) // This function is used for matrix multiplication of two matrix
{
    for (int i = 0; i < length; i++) // Loop for iteration for rows for first matrix
    {
        for (int j = 0; j < length; j++) // Loop for iteration for columns for second matrix
        {
            double sum = 0; // Initialize sum for the result matrix element
            for (int k = 0; k < length; k++) // In this loop, theere is an interation for element first matrix and column of the second matrix 
            {
                sum += matrixA[i][k] * matrixB[k][j]; // Formula for multiplication
            }
            Result[i][j] = sum; // It is used to store the result 
        }
    }
}

void print_matrix(double **matrix, int length) // This function is used to print a matrix
{
    
    for (int i = 0; i < length; i++) // Loop for iteration for rows
    {
        for (int j = 0; j < length; j++) // Loop for iteration for columns
        {
            cout << matrix[i][j] << " ";
        }
        cout << endl; // for next row
    }
}

int main()
{
    int mat_size = 1200; // Initialization of matrix size

    double **matrixA = new double *[mat_size]; // These are used to allocate memory for matrices
    double **matrixB = new double *[mat_size];
    double **result = new double *[mat_size];
    
    for (int i = 0; i < mat_size; i++) // it allocate memory for each row in the matrices
    {
        matrixA[i] = new double[mat_size];
        matrixB[i] = new double[mat_size];
        result[i] = new double[mat_size];
    }

    random_device rd; // Here matrices are initialized with random values
    mt19937 gen(rd());
    uniform_real_distribution<> dis(-10, 10);
   
    for (int i = 0; i < mat_size; i++)  // This loop iterates over rows and columns of each matrix to assign random values
    {
        for (int j = 0; j < mat_size; j++)
        {
            matrixA[i][j] = dis(gen);
            matrixB[i][j] = dis(gen);
        }
    }

    auto start = chrono::high_resolution_clock::now(); // It is used to multipliy matrices and for measuring execution time
    multiplication_proc(matrixA, matrixB, result, mat_size);
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    cout << "It takes " << duration.count() << " m/s to execute the program for multiplication" << endl;

    ofstream output("result.txt"); 
    
    for (int i = 0; i < mat_size; i++) // This loop iterates over rows and columns to write to the file
    {
        for (int j = 0; j < mat_size; j++)
        {
            output << result[i][j] << " ";
        }
        output << endl;
    }
    output.close();

    for (int i = 0; i < mat_size; i++) // Used to clean the memory
    {
        delete[] matrixA[i];
        delete[] matrixB[i];
        delete[] result[i];
    }
    delete[] matrixA;
    delete[] matrixB;
    delete[] result;

    return 0;
}