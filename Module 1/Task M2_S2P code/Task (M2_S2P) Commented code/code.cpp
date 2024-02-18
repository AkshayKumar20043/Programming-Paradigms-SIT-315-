// These below are libraries used
#include <chrono>       // This library is used for measuring time
#include <cstdlib>      // This is a general library
#include <iostream>     // This library is used for Input and output stream handling
#include <time.h>       // This library is used for using time functions

using namespace std::chrono;
using namespace std;

void randomVector(int vector[], int size) { // This function is used to initialize a vector with random values
  for (int i = 0; i < size; i++) {
    vector[i] = rand() % 100;
  }
}

int main() {
  unsigned long size = 100000000; // This is the size of the vectors
  srand(time(0));
  int *v1, *v2, *v3; // Here vectors are declared
  auto start = high_resolution_clock::now(); // From here the starting time is recorded

  // The below lines allocate memory for vectors
  v1 = (int *)malloc(size * sizeof(int));  
  v2 = (int *)malloc(size * sizeof(int));
  v3 = (int *)malloc(size * sizeof(int));

  // Here vectors are initialized with random values
  randomVector(v1, size);
  randomVector(v2, size);

  for (int i = 0; i < size; i++) { // Here vector addition is performed
    v3[i] = v1[i] + v2[i];
  }
  auto stop = high_resolution_clock::now(); // Here the stop time is recorded
  auto duration = duration_cast<microseconds>(stop - start); // Calculate the duration
  cout << "Time taken by function :" << duration.count() << " microseconds" // Print the duration
       << endl; // Used to free the allocated memory
  return 0;
}