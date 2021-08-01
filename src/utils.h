#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#ifndef __UTILS__
    #define __UTILS__

    #define TRUE  1
    #define FALSE 0

    #define LINE_MAXLEN 255

    #define SEQ1_MAXLEN 10000
    #define SEQ2_MAXLEN 5000
    #define W_LEN 4

    #define LETTER_COUNT 20
    #define CONSERVATIVE_GROUPS_COUNT 9
    #define SEMI_CONSERVATIVE_GROUPS_COUNT 11
    #define GROUP_MAX 7

    #define LetterToId(ch) strchr(AllLetters, ch) ? (strchr(AllLetters, ch) - AllLetters) : -1

    #define TestAlloc(var) if (var == NULL) {printf("error in line %d: cannot allocate\n", __LINE__-1); exit(EXIT_FAILURE);}

    #define task_max(a,b) (a.score > b.score) ? a : b
    #define task_min(a,b) (a.score < b.score) ? a : b
    #define task_minmax(a,b,dir) dir ? task_max(a,b) : task_min(a,b)

    typedef enum _DIR
    {
        MIN = 0,
        MAX
    } DIR;

    typedef struct _TASK
    {
        char seq1[SEQ1_MAXLEN];
        char seq2[SEQ2_MAXLEN];
        int offset;
        float weights[W_LEN];
        char signs[SEQ2_MAXLEN];
        float score;
        DIR dir;
    } TASK;

    // typedef enum _LETTERS
    // {
    //     G=0, P, A, V, L,
    //     I, M, C, F, Y,
    //     W, H, K, R, Q,
    //     N, E, D, S, T
    // } LETTERS;


    void printFloatArr(float *arr, int n, const char *prefix, const char *arrName);
    void printTask(TASK task, const char *taskName);
    void freeAll(void *a, ...);
    void freeMat(void **mat, int n);
#endif