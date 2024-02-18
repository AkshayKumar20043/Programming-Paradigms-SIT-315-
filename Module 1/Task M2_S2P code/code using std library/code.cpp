// These below are libraries used
#include <chrono>       // This library is used for measuring time
#include <cstdlib>      // This is a general library
#include <iostream>     // This library is used for Input and output stream handling
#include <time.h>       // This library is used for using time functions
#include <thread>   // This library is used for managing threads
#include <vector>   // Library for using dynamic arrays


using namespace std::chrono;
using namespace std;

void randomVector(int vector[], int size) {
  for (int i = 0; i < size; i++) {
    vector[i] = rand() % 100;
  }
}

// Function to perform vector addition in parallel
void parallelVectorAddition(int *v1, int *v2, int *v3, int size, int start, int end) {
  for (int i = start; i < end; i++) {
    v3[i] = v1[i] + v2[i];
  }
}

int main() {
  unsigned long size = 100000000;
  srand(time(0));
  
  int *v1, *v2, *v3;
  v1 = (int *)malloc(size * sizeof(int));
  v2 = (int *)malloc(size * sizeof(int));
  v3 = (int *)malloc(size * sizeof(int));

  randomVector(v1, size);
  randomVector(v2, size);

  auto start = high_resolution_clock::now();
  
  const int num_threads = 4;  // Adjust based on your system
  std::vector<std::thread> threads;

  // Divide the work among threads
  int chunk_size = size / num_threads;
  for (int i = 0; i < num_threads; i++) {
    int start_index = i * chunk_size;
    int end_index = (i == num_threads - 1) ? size : (i + 1) * chunk_size;
    threads.emplace_back(parallelVectorAddition, v1, v2, v3, size, start_index, end_index);
  }

  // Join threads
  for (auto &thread : threads) {
    thread.join();
  }

  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<microseconds>(stop - start);
  cout << "Time taken by function: " << duration.count() << " microseconds" << endl;

  free(v1);
  free(v2);
  free(v3);

  return 0;
}
