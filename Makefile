# MAKEFILE

objects = main.o utils.o cudaFunctions.cuo
target = mpiCudaOpenMP

gccFlags = -g -Wno-write-strings -Werror -lm
nvccFlags = -I/usr/local/cuda-9.1/include/ -I./include/ -g
buildIncludes = /usr/local/cuda-9.1/lib64/libcudart_static.a

MAX_K = 5
NP = 2

build: $(objects)
	mpicxx -fopenmp $(gccFlags) -o $(target) $(objects) $(buildIncludes) -ldl -lrt

clean:
	rm -f *.o *.cuo $(target)

run: runNetwork



# all `*.o` files are made from their corresponding `*.c` and `*.h`
%.o: %.c %.h
	mpicxx -fopenmp $(gccFlags) -c $*.c -o $*.o

# all `*.cuo` files are made from their corresponding `*.cu` and `*.h`
%.cuo: %.cu %.h
	nvcc $(nvccFlags) -c $*.cu -o $*.cuo



runNetwork: build
	mpiexec -np $(NP) -machinefile mf -map-by node $(target) $(MAX_K)

runLocal: build
	mpiexec -np $(NP) $(target) $(MAX_K)