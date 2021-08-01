#include "main.h"

// global variables
int numOfProcesses;
int rank;

int main(int argc, char *argv[])
{
    // init values
    int i;
    MPI_Status status;

    char seq1[SEQ1_MAXLEN];
    char seq2[SEQ2_MAXLEN];
    float weights[W_LEN];
    DIR dir;

    int seq2_mutants_count = 0;
    char **seq2_mutants;
    
    TASK *tasks, best_task, worker_best_task;
    int tasks_count;

    if (argc != 2)
    {
        printf("> ERROR > Expected 1 command line argument, instead got %d\n", argc-1);
        return EXIT_FAILURE;
    }

    // init MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numOfProcesses);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (numOfProcesses != 2)
    {
        printf("> ERROR > Expected NP=2, instead got %d\n", numOfProcesses);
        MPI_Abort(MPI_COMM_WORLD, MPI_ERR_SPAWN);
    }

    // setup tasks to be executed
    if (rank == 0) // root
    {
        readInputsFromFile(argv[1], weights, seq1, seq2, &dir);

        MPI_Send(&dir, 1, MPI_INT, 1, 0, MPI_COMM_WORLD); // dir

        generateAllMutants(seq2, strlen(seq2), &seq2_mutants_count, &seq2_mutants);
        tasks_count = seq2_mutants_count * (strlen(seq1) - strlen(seq2) + 1);
        tasks = (TASK*) malloc(tasks_count * sizeof(TASK));
        TestAlloc(tasks);

        generateTasks(seq1, seq2_mutants, seq2_mutants_count, weights, dir, tasks, tasks_count);

        MPI_Send(seq1, SEQ1_MAXLEN, MPI_CHAR, 1, 0, MPI_COMM_WORLD); // seq1
        MPI_Send(&tasks_count, 1, MPI_INT, 1, 0, MPI_COMM_WORLD); // tasks_count
        MPI_Send(tasks+(tasks_count/2), (tasks_count-(tasks_count/2)) * sizeof(TASK), MPI_CHAR, 1, 0, MPI_COMM_WORLD); // second half of tasks
        
        tasks_count /= 2;
    }
    else // worker
    {
        MPI_Recv(&dir, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status); // dir

        MPI_Recv(seq1, SEQ1_MAXLEN, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status); // seq1
        MPI_Recv(&tasks_count, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status); // tasks_count
        tasks_count -= tasks_count/2;
        tasks = (TASK*) malloc(tasks_count * sizeof(TASK));
        TestAlloc(tasks);
        MPI_Recv(tasks, tasks_count * sizeof(TASK), MPI_CHAR, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status); // second half of tasks

        for (i = 0; i < tasks_count; i++)
            tasks[i].seq1 = seq1;
    }


    // execute tasks
    computeTasks(tasks, tasks_count);
    

    best_task = tasks[0];
    for (i = 1; i < tasks_count; i++)
        best_task = task_minmax(best_task, tasks[i], dir);

    // at this point, both MPI nodes only have one `best_task`
    // root should recv the selected task from worker and determine which task to keep, worker's or itself.
    if (rank == 0) // root
    {
        MPI_Recv(&worker_best_task, sizeof(TASK), MPI_CHAR, 1, MPI_ANY_TAG, MPI_COMM_WORLD, &status); // worker's `best_task`
        worker_best_task.seq1 = best_task.seq1;

        best_task = task_minmax(best_task, worker_best_task, dir);

        printTask(best_task, "best_task");
        printOutput(OUTPUT_FILE, best_task);
    }
    else // worker
        MPI_Send(&best_task, sizeof(TASK), MPI_CHAR, 0, 0, MPI_COMM_WORLD); // worker's `best_task'

    MPI_Finalize();
    freeMat((void**)seq2_mutants, seq2_mutants_count);
    free(tasks);
    return EXIT_SUCCESS;
}


void readInputsFromFile(const char *filepath, float weights[W_LEN], char seq1[SEQ1_MAXLEN], char seq2[SEQ2_MAXLEN], DIR *dir)
{
    char line[LINE_MAXLEN];
    FILE *fd = fopen(filepath, "r");

    fscanf(fd, "%f %f %f %f", weights, weights+1, weights+2, weights+3);
    fscanf(fd, "%s", seq1);
    fscanf(fd, "%s", seq2);
    fscanf(fd, "%s", line); //dir

    if (strcasecmp(line, "maximum") == 0)
        *dir = MAX;
    else
        *dir = MIN;

    fclose(fd);
}

/**
 * generate mutant letter groups.
 * 
 * returns array of strings of mutant letter groups.
 * MutantGroups[i] = letter_group
 * where `i` is enum `LETTERS` and `letter_group` are the characters `i` can mutate into.
 *
 * for each letter in `AllLetters`
 * start from a full set of letter, exculding the `pivot` letter.
 * look in the `ConservativeGroups` and remove the not allowed letters.
 */
void generateMutantGroups(char MutantGroups[LETTER_COUNT][LETTER_COUNT+1])
{
    int i, j;
    char ch, *loc, *loc2, str[LETTER_COUNT+1];

    for (i = 0; i < LETTER_COUNT; i++)
    {
        // start a new mutant group
        ch = AllLetters[i];
        strcpy(MutantGroups[i], AllLetters);
        
        // remove the letter itself from the group
        loc = strchr(MutantGroups[i], ch);
        strcpy(loc, loc+1);

        for (j = 0; j < CONSERVATIVE_GROUPS_COUNT; j++)
        {
            loc = strchr(ConservativeGroups[j], ch);
            if (loc)
            {
                // for `ch` in `MutantGroups[i]`:
                //     ConservativeGroups[j].removeChar(ch);
                
                strcpy(str, ConservativeGroups[j]);
                loc = str;
                while (*loc)
                {
                    loc2 = strchr(MutantGroups[i], *loc);
                    if (loc2)
                        strcpy(loc2, loc2+1);
                    loc++;
                }
            }
        }
    }
}

/**
 * seq = "PYEC"
 *
 * P -> GAVLIMCFYWHKRQNEDST -> 19 options
 * Y -> GPAVLIMCKRQNEDST    -> 16
 * E -> GPAVLIMCFYWHRST     -> 15
 * C -> GPAVLIMFYWHKRQNEDST -> 19
 * 
 * total 19+16+15+19 = 69 possible mutant sequences (nice)
 *
 * total += 1 (no mutation is also an option) =>
 * => total = 70 (less nice)
 */
void generateAllMutants(char seq[], int seq_n, int *res_n, char ***res)
{
    int i, j, k, count = 1;
    char *loc, **mutants; //mutant[SEQ2_MAXLEN];
    
    char MutantGroups[LETTER_COUNT][LETTER_COUNT+1] = {0};
    generateMutantGroups(MutantGroups);

    // count how many mutants are needed to be created
    for (i = 0; i < seq_n; i++)
    {
        j = LetterToId(seq[i]);
        count += strlen(MutantGroups[j]);
    }
    
    // first "mutant" is the original seq.
    mutants = (char**) malloc(count * sizeof(char*));
    TestAlloc(mutants);
    
    mutants[0] = (char*) calloc(seq_n, sizeof(char));
    TestAlloc(mutants[0]);
    memcpy(mutants[0], seq, seq_n*sizeof(char));

    // the rest of the mutants, the "real" ones
    k = 1;
    for (i = 0; i < seq_n; i++)
    {
        j = LetterToId(seq[i]);
        loc = MutantGroups[j]; // iterate over the mutant group and generate mutants
        while (*loc)
        {
            // start from a clean clone of `seq`
            mutants[k] = (char*) calloc(seq_n, sizeof(char));
            TestAlloc(mutants[k]);
            memcpy(mutants[k], seq, seq_n*sizeof(char));
            
            // mutate the `i` letter of the `k` mutant variant
            mutants[k][i] = *loc;

            k++;
            loc++;
        }
    }

    *res_n = count;
    *res = mutants;
}

void generateTasks(char seq1[SEQ1_MAXLEN], char **seq2_mutants, int seq2_mutants_count, float weights[W_LEN], DIR dir, TASK *tasks, int tasks_count)
{
    char str[LINE_MAXLEN];
    int i, j, offset;
    int max_offset = strlen(seq1) - strlen(seq2_mutants[0]) + 1;


    j = 0;
    #pragma omp parallel for shared(j)
    for (i = 0; i < seq2_mutants_count; i++)
    {
        for (offset = 0; offset < max_offset; offset++)
        {
            tasks[i].seq1 = seq1; //copy by reference, seq1 is the same in all tasks
            // strcpy(tasks[j].seq1, seq1);
            strcpy(tasks[j].seq2, seq2_mutants[i]);
            tasks[j].offset = offset;
            memcpy(tasks[j].weights, weights, 4*sizeof(float));
            tasks[j].dir = dir;

            j++;
        }
    }
}

void printOutput(const char *outfile, TASK task)
{
    FILE *file = fopen(outfile, "w");
    
    fprintf(file, "%s\n", task.seq2);
    fprintf(file, "score  = %.2f\n", task.score);
    fprintf(file, "offset = %d\n", task.offset);
    
    fclose(file);
}
