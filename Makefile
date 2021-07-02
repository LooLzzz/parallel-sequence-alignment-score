# MAKEFILE

srcdir = ./src
bindir = ./bin

objects = main.o utils.o cudaFunctions.cuo
# objects = main.o utils.o
objectsWithPath = $(foreach obj, $(objects), $(bindir)/$(obj)) # prepend `bindir` to all objects

target = sequenceAlignmentExec

gccFlags = -g -Wno-write-strings -Werror -lm
nvccFlags = -I/usr/local/cuda-9.1/include/ -I./include/ -g
buildIncludes = /usr/local/cuda-9.1/lib64/libcudart_static.a

NP = 2
INPUT = input.txt

build: $(objects)
	mpicxx -fopenmp $(gccFlags) -o $(target) $(objectsWithPath) $(buildIncludes) -ldl -lrt

clean:
	rm -f $(target)
	rm -f $(bindir)/*.o
	rm -f $(bindir)/*.cuo

run: runLocal



# each `*.o` file is made from its corresponding `*.c` and `*.h`
%.o: $(srcdir)/%.c $(srcdir)/%.h
	mpicxx -fopenmp $(gccFlags) -c $(srcdir)/$*.c -o $(bindir)/$*.o

# each `*.cuo` file is made from its corresponding `*.cu` and `*.cuh`
%.cuo: $(srcdir)/%.cu $(srcdir)/%.cuh
	nvcc $(nvccFlags) -c $(srcdir)/$*.cu -o $(bindir)/$*.cuo



runNetwork: build
	mpiexec -np $(NP) -machinefile machinefile -map-by node $(target) $(INPUT)

runLocal: build
	mpiexec -np $(NP) $(target) $(INPUT)
