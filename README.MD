# Mutant Sequence Alignment Project

Written by Noam Levi - 205530611.

* The project is already compiled, you may run the project by using one the following commands
  1. `make run` or `make runLocal`  
     Will run locally.
  2. `make runNetwork`  
     Will try to run on the network by looking in the machinefile (`./machinefile`) for nodes.
     Make sure to add the correct ip addresses to your machinefile before running.  
     Both computer nodes should have `sequenceAlignmentExec` executable in the same path.

* In addition, you may change the passed arguments by adding the following to the above mentioned commands:
  1. `NP=2`  
     Number of proccess to run with mpi.  
     Defaults to `2`, Anything else will result in an error.
  2. `INPUT=input2.txt`  
     The specify an input text file.  
     Defaults to `input.txt`.
