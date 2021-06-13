# MAKEFILE

srcdir = ./src
bindir = ./bin

objects = main.o utils.o cudaFunctions.cuo
objectsWithPath = $(foreach obj, $(objects), $(bindir)/$(obj)) # prepend `bindir` to all objects

target = sequenceAlignmentExec

gccFlags = -g -Wno-write-strings -Werror -lm
nvccFlags = -I/usr/local/cuda-9.1/include/ -I./include/ -g
buildIncludes = /usr/local/cuda-9.1/lib64/libcudart_static.a

MAX_K = 5
NP = 2

build: $(objects)
	mpicxx -fopenmp $(gccFlags) -o $(target) $(objectsWithPath) $(buildIncludes) -ldl -lrt

clean:
	rm -f $(target)
	rm -f $(bindir)/*.o
	rm -f $(bindir)/*.cuo

run: runNetwork



# all `*.o` files are made from their corresponding `*.c` and `*.h`
%.o: $(srcdir)/%.c $(srcdir)/%.h
	mpicxx -fopenmp $(gccFlags) -c $(srcdir)/$*.c -o $(bindir)/$*.o

# all `*.cuo` files are made from their corresponding `*.cu` and `*.h`
%.cuo: $(srcdir)/%.cu $(srcdir)/%.h
	nvcc $(nvccFlags) -c $(srcdir)/$*.cu -o $(bindir)/$*.cuo



runNetwork: build
	mpiexec -np $(NP) -machinefile machinefile -map-by node $(target) $(MAX_K)

runLocal: build
	mpiexec -np $(NP) $(target) $(MAX_K)