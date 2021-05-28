#include <stdio.h>
#include <math.h>

#define HEAVY 1000
#define SIZE 40
#define RADIUS 10

// This function performs heavy computations, 
// its run time depends on x and y values
// DO NOT change the function
double heavy(int x, int y)
{
	int i, loop;
	double sum = 0;

	if (sqrt((x - 0.75*SIZE)*(x - 0.75*SIZE) + (y - 0.25*SIZE)*(y - 0.25*SIZE)) < RADIUS)
		loop = 2*x*y;
	else
		loop = y + x;

	for (i = 0; i < loop*HEAVY; i++)
		sum += sin(exp(cos((double)i / HEAVY)));

	return sum;
}