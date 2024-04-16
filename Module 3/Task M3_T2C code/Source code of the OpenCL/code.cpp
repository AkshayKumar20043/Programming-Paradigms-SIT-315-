#include <iostream>
#include <CL/cl.h>
#include <mpi.h>
#include <time.h>

#define ARRAY_SIZE 1000000

using namespace std;

int main()
{
    // Initializing the MPI
    MPI_Init(NULL, NULL);

    clock_t start_time = clock();

    // It gets the number of processes and the rank of process
    int numProcesses, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    // It is used to set up OpenCL context 
    cl_device_id deviceId;
    cl_context context;
    cl_command_queue commandQueue;
    cl_int ret;

    cl_platform_id platformId;
    ret = clGetPlatformIDs(1, &platformId, NULL);

    ret = clGetDeviceIDs(platformId, CL_DEVICE_TYPE_GPU, 1, &deviceId, NULL);
    context = clCreateContext(NULL, 1, &deviceId, NULL, NULL, &ret);
    commandQueue = clCreateCommandQueueWithProperties(context, deviceId, NULL, &ret);
   
    // Create OpenCL program from source code
    const char *source = "_kernel void sort(_global int *array, int start, int end) { \
                            int pivot = array[end]; \
                            int i = start - 1; \
                            for (int j = start; j < end; j++) { \
                                if (array[j] < pivot) { \
                                    i++; \
                                    int temp = array[i]; \
                                    array[i] = array[j]; \
                                    array[j] = temp; \
                                } \
                            } \
                            int temp = array[i + 1]; \
                            array[i + 1] = array[end]; \
                            array[end] = temp; \
                        }";

    cl_program program = clCreateProgramWithSource(context, 1, &source, NULL, &ret);
    ret = clBuildProgram(program, 1, &deviceId, NULL, NULL, NULL);

    // It creates OpenCL kernel from program
    cl_kernel kernel = clCreateKernel(program, "sort", &ret);

    // It creates OpenCL buffer for the input array and write data to it
    int inputArray[ARRAY_SIZE];
    if (rank == 0) 
    {
        for (int i = 0; i < ARRAY_SIZE; i++) 
        {
            inputArray[i] = rand();
        }
    }

    cl_mem inputBuffer = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(int) * ARRAY_SIZE, NULL, &ret);
    ret = clEnqueueWriteBuffer(commandQueue, inputBuffer, CL_TRUE, 0, sizeof(int) * ARRAY_SIZE, inputArray, 0, NULL, NULL);

    // It sets the kernel arguments
    int start = rank * (ARRAY_SIZE / numProcesses);
    int end = start + (ARRAY_SIZE / numProcesses);
    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&inputBuffer);
    ret = clSetKernelArg(kernel, 1, sizeof(int), (void *)&start);
    ret = clSetKernelArg(kernel, 2, sizeof(int), (void *)&end);

    size_t localWorkSize[1] = {static_cast<size_t>(ARRAY_SIZE / numProcesses)};

    // It defines the global work size and enqueue the kernel
    size_t globalWorkSize[1] = {ARRAY_SIZE};
    ret = clEnqueueNDRangeKernel(commandQueue, kernel, 1, NULL, globalWorkSize, localWorkSize, 0, NULL, NULL);

    // It creates a host-side output array 
    int *outputArray = new int[ARRAY_SIZE / numProcesses];
    ret = clEnqueueReadBuffer(commandQueue, inputBuffer, CL_TRUE, 0, sizeof(int) * ARRAY_SIZE / numProcesses, outputArray, 0, NULL, NULL);

    // It gathers all sorted sub-arrays 
    int recvCounts[numProcesses];
    int displs[numProcesses];
    MPI_Gather(outputArray, ARRAY_SIZE / numProcesses, MPI_INT, NULL, 0, MPI_INT, 0, MPI_COMM_WORLD);

    int *gatheredArray = NULL;
    if (rank == 0) 
    {
        gatheredArray = new int[ARRAY_SIZE];
        displs[0] = 0;
        for (int i = 1; i < numProcesses; i++) 
        {
            displs[i] = displs[i - 1] + recvCounts[i - 1];
        }
    }
    
    MPI_Gatherv(outputArray, ARRAY_SIZE / numProcesses, MPI_INT, gatheredArray, recvCounts, displs, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) // It prints the sorted array in rank 0 process
    {
       // cout << "Sorted array:" << endl;
        for (int i = 0; i < ARRAY_SIZE; i++) 
        {
            if (gatheredArray[i] >= 0) 
            {
             //   cout << gatheredArray[i] << " ";
            }
        }
      //  cout << endl;
    }

    clock_t end_time = clock();

    double duration = (double) (end_time - start_time) / ((double)CLOCKS_PER_SEC);
    printf("Time taken is : %f seconds\n", duration);

    // Release OpenCL resources
    ret = clFlush(commandQueue);
    ret = clFinish(commandQueue);
    ret = clReleaseKernel(kernel);
    ret = clReleaseProgram(program);
    ret = clReleaseMemObject(inputBuffer);
    ret = clReleaseCommandQueue(commandQueue);
    ret = clReleaseContext(context);

    MPI_Finalize(); // Finalizing the MPI

    delete[] outputArray;
    if (gatheredArray != NULL) 
    {
        delete[] gatheredArray;
    }

    return 0;
}
