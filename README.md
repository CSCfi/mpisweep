# mpisweep
MPI sweep diagnostic

## Building
To run the build

    mpicc mpisweep.c -o mpisweep

## Running
To execute
    mpirun ./mpisweep

The program will run MPI latency and bandwidth tests between the 1st MPI rank and the successive ranks. 

    0 -> 1 -> 0
    0 -> 2 -> 0
    0 -> 3 -> 0 
    ...

By default the tests have the following parameters
 * Latency: 1 double payload, 10000 iterations
 * Bandwidth: 1024000 double payload, 10 iterations

