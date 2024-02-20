#include <iostream>
#include <fstream>
#include <chrono>
#include <pthread.h>
#include <cstdlib>
#include <vector>

using namespace std;
const int numb = 100;

vector<vector<int>> A(numb, vector<int>(numb)); // Matrix A
vector<vector<int>> B(numb, vector<int>(numb)); // Matrix B
vector<vector<int>> C(numb, vector<int>(numb)); // Matrix C

int chunkSize; // Declaration chunkSize 

void random_mat(vector<vector<int>> &matrix, int size)
{
  for (int i = 0; i < size; i++)
  {
    for (int j = 0; j < size; j++)
    {
      matrix[i][j] = rand() % 10; // here random numbers are generated b/w 0 to 9
    }
  }
}

void *multiplication_mat(void *args)
{
  long id = (long)args; // It is used to cast void pointer to long
  int init = id * chunkSize;
  int end = (id + 1) * chunkSize;

  for (int i = init; i < end; i++) // algorithm for matrix multiplication algorithm
  {
    for (int j = 0; j < numb; j++)
    {
      C[i][j] = 0;
      for (int k = 0; k < numb; k++)
      {
        C[i][j] += A[i][k] * B[k][j];
      }
    }
  }

  pthread_exit(NULL); 
}

int main()
{
  srand(time(NULL)); // It seeds random number
  random_mat(A, numb);
  random_mat(B, numb);

  int NUM_THREADS = 0;
  cout << "Enter number of threads: ";
  cin >> NUM_THREADS;
  pthread_t threads[NUM_THREADS];
  chunkSize = numb / NUM_THREADS;

  auto start_time = std::chrono::high_resolution_clock::now();

  for (long i = 0; i < NUM_THREADS; i++) // This loop is used for creating threads for parallel matrix multiplication
  {
    pthread_create(&threads[i], NULL, multiplication_mat, (void *)i);
  }

  for (long i = 0; i < NUM_THREADS; i++) // It is used for joining threads
  {
    pthread_join(threads[i], NULL);
  }

  auto lastT = std::chrono::high_resolution_clock::now();
  auto time_duration = std::chrono::duration_cast<std::chrono::microseconds>(lastT - start_time);

  ofstream outputFile("output.txt"); // It is used for writing result to output file
  if (outputFile.is_open())
  {
    for (int i = 0; i < numb; i++)
    {
      for (int j = 0; j < numb; j++)
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

  cout << "The time taken for multiplication of matrix is " << time_duration.count() << " microseconds" << endl;

  return 0;
}