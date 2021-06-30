#include "main.h"

// global variables
int numOfProcesses;
int rank;

#define INPUT_FILE "input.txt"

int main(int argc, char *argv[])
{
    // init values
    MPI_Status status;
    char workerPrefix[20];

    float W[W_LEN];
    char seq1[SEQ1_MAXLEN];
    char seq2[SEQ2_MAXLEN];
    DIR dir;

    int num_of_mutants = 0;
    char **seq2_mutants;

    int i, len;
    char *seq2_offset;

    // init MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numOfProcesses);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // char MutantGroups[LETTER_COUNT][LETTER_COUNT+1] = {0};
    // generateMutantGroups(MutantGroups);

    if (rank == 0) // root
    {
        sprintf(workerPrefix, ">  root   > ");
        readInputsFromFile(INPUT_FILE, &W, &seq1, &seq2, &dir);

        // root & worker will each work on half of `seq2`
        // root   -> seq2[:len/2]
        // worker -> seq2[len/2:]
        len = strlen(seq2);
        seq2_offset = seq2+(len/2);

        MPI_Send(W, W_LEN, MPI_FLOAT, 1, 0, MPI_COMM_WORLD); // W
        MPI_Send(seq1, SEQ1_MAXLEN, MPI_CHAR, 1, 0, MPI_COMM_WORLD); // seq1
        MPI_Send(&dir, 1, MPI_INT, 1, 0, MPI_COMM_WORLD); // dir
        MPI_Send(&len, 1, MPI_INT, 1, 0, MPI_COMM_WORLD); // strlen(seq2)
        MPI_Send(seq2_offset, len-(len/2), MPI_CHAR, 1, 0, MPI_COMM_WORLD); // second half of seq2
        
        generateAllMutants(seq2, len/2, &num_of_mutants, seq2_mutants);

        // TODO: recv seq2 mutant results from worker & join both results
    }
    else // worker
    {
        sprintf(workerPrefix, "> worker%d > ", rank);

        MPI_Recv(W, W_LEN, MPI_FLOAT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status); // W
        MPI_Recv(seq1, SEQ1_MAXLEN, MPI_CHAR, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status); // seq1
        MPI_Recv(&dir, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status); // dir
        MPI_Recv(&len, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status); // strlen(seq2)
        MPI_Recv(&seq2, len-(len/2), MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status); // second half of seq2
        
        generateAllMutants(seq2, len-(len/2), &num_of_mutants, seq2_mutants);

        // TODO: send seq2 mutant results back to root
    }

    MPI_Finalize();
    freeMat((void**)seq2_mutants, num_of_mutants);
    return EXIT_SUCCESS;
}

void readInputsFromFile(const char *filepath, float (*W)[W_LEN], char (*seq1)[SEQ1_MAXLEN], char (*seq2)[SEQ2_MAXLEN], DIR *dir)
{
    char line[LINE_MAX];
    FILE *fd = fopen(filepath, "r");

    fscanf(fd, "%f %f %f %f", *W, (*W)+1, (*W)+2, (*W)+3);
    fscanf(fd, "%s", *seq1);
    fscanf(fd, "%s", *seq2);
    fscanf(fd, "%s", line); //dir

    if (strcasecmp(line, "maximum"))
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
    char ch, *loc, *loc2, str[LINE_MAX];
    // char **MutantGroups = *_MutantGroups;

    #pragma omp parallel for
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
        // printf("> MutantGroups[%c] = %s\n", ch, MutantGroups[i]); //DEBUG
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
 */
void generateAllMutants(char seq[], int seq_n, int *res_n, char **res)
{
    int i, count = 0;
    char ch, mutant[SEQ2_MAXLEN];
    char MutantGroups[LETTER_COUNT][LETTER_COUNT+1] = {0};
    generateMutantGroups(MutantGroups);

    //TODO: enum_id to string
    //TODO: all this
    
    for (i = 0; i < seq_n; i++)
    {
        ch = seq[i];
    }
}

void cpuCompute()
{
    
}
