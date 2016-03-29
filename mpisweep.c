#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char **argv)
{   
  int myproc, size, other_proc, nprocs, i, last;
  double t0, t1, t2, t3, time;
  double *a, *b;

  // Size of packet for bandwidth test
  int bw_size = 1024000; 
  // Number of iteraions for latency test
  int lat_iter=10000;
  // Number of iterations for bandwidth test
  int bw_iter=10;

  double max_rate = 0.0, min_latency = 10e6;
  MPI_Request request, request_a, request_b;
  MPI_Status status;
  // My hostname
  char hostname[256];
  // Hostname of the remote side
  char remote_hostname[256]; 
  // Rank of the sender
  int sender=0; 
 
  // Allocate the bandwidth test buffers
  a = (double *) malloc (bw_size * sizeof (double));
  b = (double *) malloc (bw_size * sizeof (double));

  // Initialize MPI and synchronize everything
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &myproc);
  
  gethostname(hostname,255);
  sleep(2);
  MPI_Barrier(MPI_COMM_WORLD);

  // If I'm the sending rank 
 
  if (myproc==sender) {
    // Iterate over the other ranks 
    for (other_proc=0;other_proc<nprocs;other_proc++) { 
     if (myproc!=other_proc) {	
 	     // The latency test
	     for (i=0;i<lat_iter+1;i++) 	{
		if (i==1) { t0 = MPI_Wtime(); }		
		MPI_Send(a, 1, MPI_DOUBLE, other_proc, 0, MPI_COMM_WORLD);
		MPI_Recv(b, 1, MPI_DOUBLE, other_proc, 0, MPI_COMM_WORLD, &status);
      		}
      	t1 = MPI_Wtime();
	// The bandwidth test
      	for (i=0;i<bw_iter+1;i++) {
		if (i==1) { t2 = MPI_Wtime(); }	
		MPI_Send(a, bw_size, MPI_DOUBLE, other_proc, 1, MPI_COMM_WORLD);
		MPI_Recv(b, bw_size, MPI_DOUBLE, other_proc, 1, MPI_COMM_WORLD, &status);
      	}
      	t3 = MPI_Wtime();
	
	// Receive the hostname of the remote system
	MPI_Recv(remote_hostname, 255, MPI_BYTE, other_proc, 2, MPI_COMM_WORLD, &status);

	// Print a line to of the system 
	printf("   %s \t %s \t %f \t %f\n", hostname, remote_hostname ,(t1-t0)/2/lat_iter*1000000, 8/((t3-t2)/bw_iter/2));
    		}
	}
  }

 // If I'm not the sending rank

  else {
    other_proc=sender;
    // The latency test
    for (i=0;i<lat_iter+1;i++) {
      MPI_Recv(a, 1, MPI_DOUBLE, other_proc, 0, MPI_COMM_WORLD, &status);
      MPI_Send(a, 1, MPI_DOUBLE, other_proc, 0, MPI_COMM_WORLD);
    }
  // The bandwidth test
  for (i=0;i<bw_iter+1;i++) {
      MPI_Recv(a, bw_size, MPI_DOUBLE, other_proc, 1, MPI_COMM_WORLD, &status);
      MPI_Send(b, bw_size, MPI_DOUBLE, other_proc, 1, MPI_COMM_WORLD);
    }
    MPI_Send(hostname, 255, MPI_BYTE, other_proc, 2, MPI_COMM_WORLD); 
  }
  MPI_Finalize();
  return 0; 
}
