#include <cuda.h>
#include <cuda_runtime.h>
#include <helper_cuda.h>
#include <device_launch_parameters.h>

#include "cudaFunctions.cuh"

void copyTasksToGPU(TASK *tasks, int tasks_count, GPU_TASK **device_tasks);
__global__ void _compute(GPU_TASK *tasks, int tasks_count);
__device__ void computeSigns(GPU_TASK *tasks);
__device__ void computeScore(GPU_TASK *tasks);

__device__ int _strlen(const char *str);
__device__ char *_strchr(const char *str, char ch);

__device__ char device_seq1[SEQ1_MAXLEN];
__device__ float device_weights[W_LEN];
cudaError_t err;

int computeTasks(TASK *tasks, int tasks_count)
{
    int i, blocksPerGrid, seq2_len;
    GPU_TASK *device_tasks, t;

    seq2_len = strlen(tasks->seq2);
    copyTasksToGPU(tasks, tasks_count, &device_tasks);

    blocksPerGrid = (tasks_count + threadsPerBlock - 1) / threadsPerBlock;
    _compute<<<blocksPerGrid, threadsPerBlock>>>(device_tasks, tasks_count);

    for (i = 0; i < tasks_count; i++)
    {        
        cudaMemcpy(&t, device_tasks+i, sizeof(GPU_TASK), cudaMemcpyDeviceToHost);
        cudaCheckErr();

        cudaMemcpy(tasks[i].signs, t.signs, seq2_len * sizeof(char), cudaMemcpyDeviceToHost);
        cudaCheckErr();
        
        tasks[i].score = t.score;
    }

    return CUDA_SUCCESS;
}

void copyTasksToGPU(TASK *tasks, int tasks_count, GPU_TASK **res)
{
    int seq2_len = strlen(tasks->seq2);
    GPU_TASK *device_tasks;

    device_tasks = (GPU_TASK *)malloc(tasks_count * sizeof(GPU_TASK));
    cudaMalloc(res, tasks_count * sizeof(GPU_TASK));
    cudaCheckErr();

    // seq1
    cudaMemcpyToSymbol(device_seq1, tasks[0].seq1, SEQ1_MAXLEN * sizeof(char), 0, cudaMemcpyHostToDevice);
    cudaCheckErr();

    // weights
    cudaMemcpyToSymbol(device_weights, tasks[0].weights, W_LEN * sizeof(float), 0, cudaMemcpyHostToDevice);
    cudaCheckErr();

    for (int i = 0; i < tasks_count; i++)
    {
        // offset+score
        device_tasks[i].offset = tasks[i].offset;
        device_tasks[i].score = 0;
        
        // seq2
        cudaMalloc(&(device_tasks[i].seq2), seq2_len * sizeof(char));
        cudaCheckErr();
        cudaMemcpy(device_tasks[i].seq2, tasks[i].seq2, seq2_len * sizeof(char), cudaMemcpyHostToDevice);
        cudaCheckErr();

        // signs
        cudaMalloc(&(device_tasks[i].signs), seq2_len * sizeof(char));
        cudaCheckErr();
    }

    cudaMemcpy(*res, device_tasks, tasks_count * sizeof(GPU_TASK), cudaMemcpyHostToDevice);
    free(device_tasks);
}

__global__ void _compute(GPU_TASK *tasks, int tasks_count)
{
    int idx = (blockDim.x * blockIdx.x) + threadIdx.x;
    if (idx > tasks_count-1)
        return; 
        
    computeSigns(tasks);
    computeScore(tasks);
}

__device__ void computeSigns(GPU_TASK *tasks)
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
    char *seq1 = device_seq1 + tasks[idx].offset;
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

__device__ void computeScore(GPU_TASK *tasks)
{
    int idx = (blockDim.x * blockIdx.x) + threadIdx.x;

    char *signs = tasks[idx].signs;
    float *weights = device_weights;
    
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
