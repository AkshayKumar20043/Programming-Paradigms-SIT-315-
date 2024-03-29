#include <iostream>
#include <CL/cl.h> 
#include <mpi.h>  
#include <time.h>

#define ARRAY_SIZE 100 // the size of the array is defined

using namespace std;

int main()
{
    MPI_Init(NULL, NULL); // Initialization MPI
    clock_t start_time = clock(); // It records start time

    int numProcesses, rank;// It get the number of processes and the rank of this process
    MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    cl_device_id deviceId; // It sets up OpenCL context and command queue
    cl_context context;
    cl_command_queue commandQueue;
    cl_int ret;

    cl_platform_id platformId;
    ret = clGetPlatformIDs(1, &platformId, NULL); // It gets platform ID

    ret = clGetDeviceIDs(platformId, CL_DEVICE_TYPE_GPU, 1, &deviceId, NULL); 
    context = clCreateContext(NULL, 1, &deviceId, NULL, NULL, &ret); // It creates OpenCL context
    commandQueue = clCreateCommandQueueWithProperties(context, deviceId, NULL, &ret); // It creates command queue

    // It creates OpenCL program from source code
    const char *source = "_kernel void sort(_global int *array, int start, int end) \
                            { \
                                int pivot = array[end]; \
                                int i = start - 1; \
                                for (int j = start; j <= end - 1; j++) \
                                { \
                                    if (array[j] < pivot) \
                                    { \
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

    cl_program program = clCreateProgramWithSource(context, 1, &source, NULL, &ret); // It creates program
    ret = clBuildProgram(program, 1, &deviceId, NULL, NULL, NULL); // build program

    // Create OpenCL kernel from program
    cl_kernel kernel = clCreateKernel(program, "sort", &ret); // Create kernel

    // Create OpenCL buffer for the input array and write data to it
    int inputArray[ARRAY_SIZE];
    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        inputArray[i] = rand(); // Generate random data for input array
    }

    cl_mem inputBuffer = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(int) * ARRAY_SIZE, NULL, &ret); // Create buffer
    ret = clEnqueueWriteBuffer(commandQueue, inputBuffer, CL_TRUE, 0, sizeof(int) * ARRAY_SIZE, inputArray, 0, NULL, NULL); // Write data to buffer

    // Set the kernel arguments
    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&inputBuffer); // Set kernel arguments
    int start = rank * (ARRAY_SIZE / numProcesses);
    int end = start + (ARRAY_SIZE / numProcesses) - 1;

    // Create a new input array
    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        inputArray[i] = rand() % 100;
    }

    // Copy the input array to the input buffer
    ret = clEnqueueWriteBuffer(commandQueue, inputBuffer, CL_TRUE, 0, sizeof(int) * ARRAY_SIZE, inputArray, 0, NULL, NULL); // Write data to buffer

    // Create the kernel and set its arguments
    cl_kernel kernel1 = clCreateKernel(program, "sort", &ret); // Create kernel
    ret = clSetKernelArg(kernel1, 0, sizeof(cl_mem), (void *)&inputBuffer); // Set kernel arguments
    ret = clSetKernelArg(kernel1, 1, sizeof(int), (void *)&start);
    ret = clSetKernelArg(kernel1, 2, sizeof(int), (void *)&end);

    // Define the global work size and enqueue the kernel
    size_t globalWorkSize[1] = {ARRAY_SIZE};
    ret = clEnqueueNDRangeKernel(commandQueue, kernel1, 1, NULL, globalWorkSize, NULL, 0, NULL, NULL); // Enqueue kernel

    // Create OpenCL buffer for the output array
    cl_mem outputBuffer = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(int) * ARRAY_SIZE, NULL, &ret); // Create buffer

    // Copy the result from the input buffer to the output buffer
    ret = clEnqueueCopyBuffer(commandQueue, inputBuffer, outputBuffer, 0, 0, sizeof(int) * ARRAY_SIZE, 0, NULL, NULL); // Copy data between buffers

    // Create a host-side output array and read the result from the output buffer
    int outputArray[ARRAY_SIZE];
    ret = clEnqueueReadBuffer(commandQueue, outputBuffer, CL_TRUE, 0, sizeof(int) * ARRAY_SIZE, outputArray, 0, NULL, NULL); // Read data from buffer
    clock_t end_time = clock(); // Record end time

    // Calculate duration
    double duration = (double)(end_time - start_time) / ((double)CLOCKS_PER_SEC / 1000000);

    // Print the sorted output array
    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        cout << outputArray[i] << " ";
    }
    cout << endl;

    printf("time taken: %f microseconds", duration); // Print duration

    // Release OpenCL resources
    ret = clFlush(commandQueue);
    ret = clFinish(commandQueue);
    ret = clReleaseKernel(kernel);
    ret = clReleaseKernel(kernel1);
    ret = clReleaseProgram(program);
    ret = clReleaseMemObject(inputBuffer);
    ret = clReleaseMemObject(outputBuffer);
    ret = clReleaseCommandQueue(commandQueue);
    ret = clReleaseContext(context);

    // Finalize MPI
    MPI_Finalize();

    return 0;
}
