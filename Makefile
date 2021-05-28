# MAKEFILE

# objects = main.o cFunctions.o cudaFunctions.o
objects = main.o utils.o
target = mpiCudaOpenMP

gccFlags = -g
nvccFlags = -I../inc
buildIncludes = /usr/local/cuda-9.1/lib64/libcudart_static.a

MAX_K = 5
NP = 1

build: $(objects)
	mpicxx -fopenmp $(gccFlags) -o $(target) $(objects) $(buildIncludes) -ldl -lrt

# all `*.o` files are made from their corresponding `*.c` and `*.h`
%.o: %.c %.h
	mpicxx -fopenmp $(gccFlags) -c $*.c -o $*.o

# main.o: main.c main.h
# 	mpicxx -fopenmp $(gccFlags) -c main.c -o main.o

# cFunctions.o: cFunctions.c cFunctions.h
# 	mpicxx -fopenmp $(gccFlags) -c cFunctions.c -o cFunctions.o

# cudaFunctions.o: cudaFunctions.cu cudaFunctions.h
# 	nvcc $(nvccFlags) $(gccFlags) -c cudaFunctions.cu -o cudaFunctions.o


clean:
	rm -f *.o $(target)

run:
	mpiexec -np $(NP) $(target) $(MAX_K)

runOnNetwork:
	mpiexec -np $(NP) -machinefile mf -map-by node $(target) $(MAX_K)