#define CL_TARGET_OPENCL_VERSION 300
#include <iostream>
#include <fstream>
#include <chrono>
#include <cstdlib>
#include <vector>
#include <CL/cl.h>

using namespace std;

const char* kernelSource =
"__kernel void matrixMultiplication(__global const int* A, __global const int* B, __global int* C, int size) {\n"
"    int row = get_global_id(0);\n"
"    int col = get_global_id(1);\n"
"    int sum = 0;\n"
"    for (int i = 0; i < size; ++i) {\n"
"        sum += A[row * size + i] * B[i * size + col];\n"
"    }\n"
"    C[row * size + col] = sum;\n"
"}\n";

int main()
{
    int size = 0; // Initial size of matrix
    cout << "Enter size of the matrix - ";
    cin >> size;

    cout << "Performing calculation of matrix sized - " << size << "X" << size << endl;

    vector<int> A(size * size);
    vector<int> B(size * size);
    vector<int> C(size * size);

    srand(time(NULL)); // Seed for random number generation

    // Generate random numbers for matrix A
    for (int i = 0; i < size * size; i++)
    {
        A[i] = rand() % 10; // Generate random numbers between 0 and 9
    }

    // Generate random numbers for matrix B
    for (int i = 0; i < size * size; i++)
    {
        B[i] = rand() % 10; // Generate random numbers between 0 and 9
    }

    cl_int err;

    // Get platform
    cl_platform_id platform;
    err = clGetPlatformIDs(1, &platform, nullptr);
    if (err != CL_SUCCESS) {
        cout << "Error getting platform IDs: " << err << endl;
        return 1;
    }

    // Get device
    cl_device_id device;
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, nullptr);
    if (err != CL_SUCCESS) {
        cout << "Error getting device IDs: " << err << endl;
        return 1;
    }

    // Create context
    cl_context context = clCreateContext(nullptr, 1, &device, nullptr, nullptr, &err);
    if (err != CL_SUCCESS) {
        cout << "Error creating context: " << err << endl;
        return 1;
    }

    // Create command queue
    cl_command_queue queue = clCreateCommandQueue(context, device, 0, &err);
    if (err != CL_SUCCESS) {
        cout << "Error creating command queue: " << err << endl;
        return 1;
    }

    // Create buffers for matrices A, B, and C
    cl_mem bufferA = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int) * A.size(), A.data(), &err);
    if (err != CL_SUCCESS) {
        cout << "Error creating buffer A: " << err << endl;
        return 1;
    }
    cl_mem bufferB = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int) * B.size(), B.data(), &err);
    if (err != CL_SUCCESS) {
        cout << "Error creating buffer B: " << err << endl;
        return 1;
    }
    cl_mem bufferC = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(int) * C.size(), nullptr, &err);
    if (err != CL_SUCCESS) {
        cout << "Error creating buffer C: " << err << endl;
        return 1;
    }

    // Create program with source
    cl_program program = clCreateProgramWithSource(context, 1, &kernelSource, nullptr, &err);
    if (err != CL_SUCCESS) {
        cout << "Error creating program: " << err << endl;
        return 1;
    }

    // Build program
    err = clBuildProgram(program, 1, &device, nullptr, nullptr, nullptr);
    if (err != CL_SUCCESS) {
        cout << "Error building program: " << err << endl;
        return 1;
    }

    // Create kernel
    cl_kernel kernel = clCreateKernel(program, "matrixMultiplication", &err);
    if (err != CL_SUCCESS) {
        cout << "Error creating kernel: " << err << endl;
        return 1;
    }

    // Set kernel arguments
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufferA);
    err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufferB);
    err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &bufferC);
    err |= clSetKernelArg(kernel, 3, sizeof(int), &size);
    if (err != CL_SUCCESS) {
        cout << "Error setting kernel arguments: " << err << endl;
        return 1;
    }

    auto start_time = std::chrono::high_resolution_clock::now(); // Timer to start

    // Enqueue kernel
    size_t globalSize[2] = { size, size };
    err = clEnqueueNDRangeKernel(queue, kernel, 2, nullptr, globalSize, nullptr, 0, nullptr, nullptr);
    if (err != CL_SUCCESS) {
        cout << "Error enqueueing kernel: " << err << endl;
        return 1;
    }

    // Read the result
    err = clEnqueueReadBuffer(queue, bufferC, CL_TRUE, 0, sizeof(int) * C.size(), C.data(), 0, nullptr, nullptr);
    if (err != CL_SUCCESS) {
        cout << "Error reading buffer: " << err << endl;
        return 1;
    }

    auto end_time = std::chrono::high_resolution_clock::now(); // Timer to stop

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time); // It calculates time duration of multiplication

    ofstream outputFile("output.txt");
    if (outputFile.is_open())
    {
        for (int i = 0; i < size; i++)
        {
            for (int j = 0; j < size; j++)
            {
                outputFile << C[i * size + j] << " ";
            }
            outputFile << endl;
        }
        outputFile.close();
    }
    else
    {
        cout << "Error opening output file" << endl;
        return 1;
    }

    cout << "The execution time of multiplication: " << duration.count() << " microseconds" << endl;

    // Release OpenCL resources
    clReleaseMemObject(bufferA);
    clReleaseMemObject(bufferB);
    clReleaseMemObject(bufferC);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);

    return 0;
}
