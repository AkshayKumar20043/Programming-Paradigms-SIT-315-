#include <chrono>
#include <cstdlib>
#include <iostream>
#include <time.h>
#include <omp.h>

using namespace std::chrono;
using namespace std;

void randomVector(int vector[], int len)
{
  for (int i = 0; i < len; i++)
  {
    vector[i] = rand() % 100;
  }
}

int main()
{
  unsigned long len = 100000000;
  srand(time(0));
  int *v1, *v2, *v3;
  auto start = high_resolution_clock::now();
  v1 = (int *)malloc(len * sizeof(int));
  v2 = (int *)malloc(len * sizeof(int));
  v3 = (int *)malloc(len * sizeof(int));
  randomVector(v1, len);
  randomVector(v2, len);

#pragma omp parallel for
  for (int i = 0; i < len; i++)
  {
    v3[i] = v1[i] + v2[i];
  }

  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<microseconds>(stop - start);
  cout << "Time taken by function :" << duration.count() << " microseconds"
       << endl;

  free(v1);
  free(v2);
  free(v3);

  return 0;
}
