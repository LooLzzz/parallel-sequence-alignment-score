#include "utils.h"

// extern char AllLetters[];
// extern char ConservativeGroups[CONSERVATIVE_GROUPS_COUNT][GROUP_MAX];
// extern char SemiConservativeGroups[SEMI_CONSERVATIVE_GROUPS_COUNT][GROUP_MAX];

#define max(a,b) (a > b) ? a : b
#define min(a,b) (a < b) ? a : b

#define threadsPerBlock 256
#define cudaCheckErr()  err = cudaGetLastError();                                                            \
                            if (err != cudaSuccess)                                                          \
                            {                                                                                \
                                fprintf(stderr, "CUDA runtime error - %s\n", cudaGetErrorString(err)); \
                                exit(EXIT_FAILURE);                                                          \
                            }

typedef struct _GPU_TASK
{
    char *seq2;
    int offset;
    char *signs;
    float score;
} GPU_TASK;

int computeTasks(TASK *tasks, int tasks_count);
