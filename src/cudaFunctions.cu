// #include <math.h>
#include <cuda.h>
#include <cuda_runtime.h>
#include <helper_cuda.h>
#include <device_launch_parameters.h>

#include "cudaFunctions.h"

__global__ void heavy(float *A, int n, float *B, int K_MAX)
{
    int i = blockDim.x * blockIdx.x + threadIdx.x;
    int k;
    float val;

    for (i = 0; i < n; i++)
    {
        B[i] = __FLT_MIN__; // smallest float value
        for (k = 0; k < K_MAX; k++)
        {
            val = __sinf( A[i] * __cosf(k) );
            B[i] = max(val, B[i]);
        }
    }
}

int gpuCompute(float *A, int n, float *B, int K_MAX)
{
    int size = n * sizeof(float);
    float *device_A, *device_B;
    cudaMalloc((void**) &device_A, size);
    cudaMalloc((void**) &device_B, size);
    
    cudaMemcpy(device_A, A, size, cudaMemcpyHostToDevice);
    
    int threadsPerBlock = 256;
    int blocksPerGrid = (n + threadsPerBlock - 1) / threadsPerBlock;
    heavy<<<blocksPerGrid, threadsPerBlock>>>(device_A, n, device_B, K_MAX);
    
    cudaError_t err = cudaGetLastError();
    if (err != cudaSuccess)
    {
        fprintf(stderr, "Failed to launch vectorAdd kernel -  %s\n", cudaGetErrorString(err));
        exit(EXIT_FAILURE);
    }

    cudaMemcpy(B, device_B, size, cudaMemcpyDeviceToHost);
    cudaFree(device_A);
    cudaFree(device_B);
    return CUDA_SUCCESS;
}
