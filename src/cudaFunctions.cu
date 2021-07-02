#include <cuda.h>
#include <cuda_runtime.h>
#include <helper_cuda.h>
#include <device_launch_parameters.h>

#include "cudaFunctions.cuh"

__global__ void _compute(TASK *tasks, int tasks_count);
__device__ void computeSigns(TASK *tasks);
__device__ void computeScore(TASK *tasks);

__device__ int _strlen(const char *str);
__device__ char *_strchr(const char *str, char ch);


int computeTasks(TASK *tasks, int tasks_count, DIR dir)
{
    cudaError_t err;
    int size, blocksPerGrid;
    TASK *device_tasks;
          
    size = tasks_count * sizeof(TASK);
    cudaMalloc((void**) &device_tasks, size);
    cudaCheckErr();
    cudaMemcpy(device_tasks, tasks, size, cudaMemcpyHostToDevice);
    cudaCheckErr();

    blocksPerGrid  = (tasks_count + threadsPerBlock - 1) / threadsPerBlock;
    _compute<<<blocksPerGrid, threadsPerBlock>>>(device_tasks, tasks_count);
    cudaCheckErr();
    
    cudaMemcpy(tasks, device_tasks, size, cudaMemcpyDeviceToHost);
    cudaCheckErr();
    
    cudaFree(device_tasks);
    cudaCheckErr();

    return CUDA_SUCCESS;
}

__global__ void _compute(TASK *tasks, int tasks_count)
{
    int idx = (blockDim.x * blockIdx.x) + threadIdx.x;
    if (idx > tasks_count-1)
        return; 
        
    computeSigns(tasks);
    computeScore(tasks);
}

__device__ void computeSigns(TASK *tasks)
{
    int idx = (blockDim.x * blockIdx.x) + threadIdx.x;
    // TASK *task = tasks + i;

    char ConservativeGroups[CONSERVATIVE_GROUPS_COUNT][GROUP_MAX] = {
        "NDEQ", "NEQK", "STA",
        "MILV", "QHRK", "NHQK",
        "FYW", "HY", "MILF"
    };
    char SemiConservativeGroups[SEMI_CONSERVATIVE_GROUPS_COUNT][GROUP_MAX] = {
        "SAG", "ATV", "CSA",
        "SGND", "STPA", "STNK",
        "NEQHRK", "NDEQHK", "SNDEQK",
        "HFY", "FVLIM"
    };

    char *signs = tasks[idx].signs;
    char *seq1 = tasks[idx].seq1 + tasks[idx].offset;
    char *seq2 = tasks[idx].seq2;
    int seq2_len = _strlen(seq2);    
    int i, j, flag;
    char *a, *b;

    for (i = 0; i < seq2_len; i++)
    {
        // equal
        if (seq1[i] == seq2[i])
        {
            *(signs++) = '*';
            // printf("> %d > got '*' > (%ch, %ch)\n", idx, seq1[i], seq2[i]);
            continue;
        }
        
        // conservative
        flag = FALSE;
        for (j = 0; j < CONSERVATIVE_GROUPS_COUNT; j++)
        {
            a = _strchr(ConservativeGroups[j], seq1[i]);
            b = _strchr(ConservativeGroups[j], seq2[i]);
            flag = a && b;
            if (flag)
                break;
        }
        if (flag)
        {
            *(signs++) = ':';
            // printf("> %d > got ':' > (%ch, %ch)\n", idx, seq1[i], seq2[i]);
            continue;
        }

        // semi-conservative
        flag = FALSE;
        for (j = 0; j < SEMI_CONSERVATIVE_GROUPS_COUNT; j++)
        {
            a = _strchr(SemiConservativeGroups[j], seq1[i]);
            b = _strchr(SemiConservativeGroups[j], seq2[i]);
            flag = a && b;
            if (flag)
                break;
        }
        if (flag)
        {
            *(signs++) = '.';
            // printf("> %d > got '.' > (%ch, %ch)\n", idx, seq1[i], seq2[i]);
            continue;
        }
        
        // none
        *(signs++) = ' ';
    }
}

__device__ void computeScore(TASK *tasks)
{
    int idx = (blockDim.x * blockIdx.x) + threadIdx.x;

    char *signs = tasks[idx].signs;
    float *weights = tasks[idx].weights;
    
    tasks[idx].score = 0;
    while (*signs)
    {
        switch (*(signs++))
        {
            case '*':
                tasks[idx].score += weights[0];
                break;
            
            case ':':
                tasks[idx].score -= weights[1];
                break;
            
            case '.':
                tasks[idx].score -= weights[2];
                break;
            
            case ' ':
            default:
                tasks[idx].score -= weights[3];
                break;
        }
    }
}

__device__ int _strlen(const char *str)
{
    int len = 0;
    while (*str++)
        len++;
    return len;
}

__device__ char *_strchr(const char *str, char ch)
{
    while (*str != ch)
        if (!*str++)
            return 0;
    return (char *)str;
}
