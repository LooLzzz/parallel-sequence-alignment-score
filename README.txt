# HW3

Written by Noam Levi - 205530611.

* The project is already compiled, you may run the project by one the following commands
  1. `make run` or `make runNetwork`  
     Will try to run on the network by looking in the machinefile (`./mf`) for nodes.
     Make sure to add the correct ip addresses to your machinefile before running.  
     Both computers should have `mpiCudaOpenMP` executable in the same path.
  2. `make runLocal`  
     Will run locally.

* When changing the `input.dat` file, make sure to **NOT** include a first line with the number of elemets in the file. The program calculates by itself the number of elements.

* In addition, you may change the passed arguments by adding the following to the above mentioned commands:
  1. `NP=2`  
     Number of proccess to run with mpi.  
     Defaults to 2, Anything else will result in an error.
  2. `K_MAX=5`  
     Max value for `k` as requested.  
     Defaults to 5.
