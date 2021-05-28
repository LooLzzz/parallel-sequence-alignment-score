#include <stdio.h>
#include <time.h>

#include "heavy.c"

// Sequencial code to be parallelized
int main(int argc, char *argv[])
{
	int x, y;
	int size = SIZE;
	double answer = 0;
    double start, end;

	start = clock();

	for (x = 0; x < size; x++)
		for (y = 0; y < size; y++) 
			answer += heavy(x, y);

	end = clock();
	
	printf("answer = %.5f\n", answer);
	printf("exec time = %.2f\n", (double)(end-start)/CLOCKS_PER_SEC);
}