# MAKEFILE

objects = main.o cFunctions.o cudaFunctions.o
target = mpiCudaOpemMP

gccFlags = -g
nvccFlags = -I../inc
buildIncludes = /usr/local/cuda-9.1/lib64/libcudart_static.a

build: $(objects)
	mpicxx -fopenmp $(gccFlags) -o $(target) $(objects) $(buildIncludes) -ldl -lrt

main.o:
	mpicxx -fopenmp $(gccFlags) -c main.c -o main.o

cFunctions.o:
	mpicxx -fopenmp $(gccFlags) -c cFunctions.c -o cFunctions.o

cudaFunctions.o:
	nvcc $(nvccFlags) $(gccFlags) -c cudaFunctions.cu -o cudaFunctions.o


clean:
	rm -f *.o $(target)

run:
	mpiexec -np 2 $(target)

runOn2:
	mpiexec -np 2 -machinefile mf -map-by node $(target)