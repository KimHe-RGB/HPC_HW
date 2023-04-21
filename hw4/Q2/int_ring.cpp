#include <stdio.h>
#include <cstdlib>
#include <mpi.h>
#include <iostream>

/**
 * @brief 
 * @param start index of the first process, which receives the message from to process end
 * @param end index of the last process, which sends the message back to process start
 * @param Nrepeat perform the loop N times
 * @param comm MPI_COMM_WORLD
 * @return double time 
 */
double int_ring(int start, int end, long Nrepeat, MPI_Comm comm) {
  int rank;
  MPI_Comm_rank(comm, &rank);
  double tt = MPI_Wtime();

  for (long repeat = 0; repeat < Nrepeat; repeat++) {
    MPI_Status status;
    int msg_int = 0; 
    if (rank == start) {
        msg_int = rank;
        MPI_Send(&msg_int, 1, MPI_INT, rank+1, repeat, comm);
        MPI_Recv(&msg_int, 1, MPI_INT, end, repeat, comm, &status);
        if (repeat == 0) printf("The Result received by proc %d is %d\n", rank, msg_int);
    }
    else if (rank == end) {
        MPI_Recv(&msg_int, 1, MPI_INT, rank-1, repeat, comm, &status);
        msg_int += rank;
        MPI_Send(&msg_int, 1, MPI_INT, start, repeat, comm);
    } 
    else {
        MPI_Recv(&msg_int, 1, MPI_INT, rank-1, repeat, comm, &status);
        msg_int += rank;
        MPI_Send(&msg_int, 1, MPI_INT, rank+1, repeat, comm);
    }
  }
  tt = MPI_Wtime() - tt;

  return tt;
}

//communicate a large array of about 2MByte
double ints_ring(int start, int end, long Nrepeat, long Nsize, MPI_Comm comm) {
  int rank;
  MPI_Comm_rank(comm, &rank);
  double tt = MPI_Wtime();

  for (long repeat = 0; repeat < Nrepeat; repeat++) {
    MPI_Status status;
    int* msg_ints = (int*) malloc(Nsize);
    for (unsigned i = 0; i < Nsize; i++) msg_ints[i] = 0;
    
    if (rank == start) {
        MPI_Send(&msg_ints, Nsize, MPI_INT, rank+1, repeat, comm);
        MPI_Recv(&msg_ints, Nsize, MPI_INT, end, repeat, comm, &status);
        if (repeat == 0) printf("The Result index 0 and 1 received by proc %d is %d and %d\n", rank, msg_ints[0], msg_ints[1]);
    }
    else if (rank == end) {
        MPI_Recv(&msg_ints, Nsize, MPI_INT, rank-1, repeat, comm, &status);
        for (unsigned i = 0; i < Nsize; i++) msg_ints[i] += rank;
        MPI_Send(&msg_ints, Nsize, MPI_INT, start, repeat, comm);
    } 
    else {
        MPI_Recv(&msg_ints, Nsize, MPI_INT, rank-1, repeat, comm, &status);
        for (unsigned i = 0; i < Nsize; i++) msg_ints[i] += rank;
        MPI_Send(&msg_ints, Nsize, MPI_INT, rank+1, repeat, comm);
    }
  }
  tt = MPI_Wtime() - tt;

  return tt;
}

int main(int argc, char** argv) {
  MPI_Init(&argc, &argv);

  if (argc < 3) {
    printf("Usage: mpirun ./int_ring <process-rank-start> <process-rank-end>\n");
    abort();
  }
  int start = atoi(argv[1]);
  int end = atoi(argv[2]);

  int rank;
  MPI_Comm comm = MPI_COMM_WORLD;
  MPI_Comm_rank(comm, &rank);

  long Nrepeat = 1000;
  double tt = int_ring(start, end, Nrepeat, comm);
  if (!rank) printf("int ring latency: %e ms\n", tt/Nrepeat * 1000);

  long Nsize = 524288;
  tt = ints_ring(start, end, Nrepeat, Nsize, comm); // 2MB
  if (!rank) printf("pingpong bandwidth: %e GB/s\n", (Nsize*Nrepeat)/tt/1e9);

  MPI_Finalize();
}