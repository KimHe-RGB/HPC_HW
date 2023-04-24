/* MPI-parallel Jacobi smoothing to solve -u''=f
 * Global vector has N x N unknowns, each processor works with its
 * part, which has lN = N x N / p unknowns. assuming the matrix in each processor has shape (sqrt_lN x sqrt_lN)
 * Author: Georg Stadler
 * 
 * Adapted to 2D by Kim
 */
#include <stdio.h>
#include <math.h>
#include <mpi.h>
#include <string.h>

/* compuate global residual, assuming ghost values are updated */
double compute_residual(double **lu, int Ni_first, int Ni_last, int Nj, double invhsq) {
  int i, j;
  double tmp, gres = 0.0, lres = 0.0;
  for (i = Ni_first; i <= Ni_last; i++){
    for (j = 1; j < Nj; j++)
    {
      tmp = ((4.0*lu[i][j] - lu[i-1][j] - lu[i+1][j] - lu[i][j-1] - lu[i][j+1]) * invhsq - 1);
      lres += tmp * tmp;
    }
  }
  /* use allreduce for convenience; a reduce would also be sufficient */
  MPI_Allreduce(&lres, &gres, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

  return sqrt(gres);
}


int sqrt(int x) {
    if(x == 0 || x == 1) return x;

    int start = 1, end = x, ans;
    while(start <= end) {
      int mid = (start + end) / 2;
      if(mid * mid == x) {
          return mid;
      }
      if(mid * mid < x) {
          start = mid + 1;
          ans = mid;
      }
      else {
          end = mid - 1;
      }
    }
    return ans;
}

int main(int argc, char * argv[]) {
  if (argc < 3) {
    printf("Usage: mpirun ./jacobi2D-mpi <NxN-grid-size> <jacobian-iterations>\n");
    abort();
  }
  int mpirank, i, j, p, N, iter, max_iters;
  int i_first, i_last;
  MPI_Status status, status1;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &mpirank);                               // mpirank: current process rank
  MPI_Comm_size(MPI_COMM_WORLD, &p);                                     // p: total number of processes

  /* get name of host running MPI process */
  char processor_name[MPI_MAX_PROCESSOR_NAME];
  int name_len;
  MPI_Get_processor_name(processor_name, &name_len);
  printf("Rank %d/%d running on %s.\n", mpirank, p, processor_name);

  sscanf(argv[1], "%d", &N);                                             // N: discretization number of each dimension
  sscanf(argv[2], "%d", &max_iters);                                     // max_iters: number of jacobian iterations that we want to perform

  if ((N * N % p != 0)) {
    printf("Exiting. NxN must be a multiple of p;\n");
    MPI_Abort(MPI_COMM_WORLD, 0);
  }
  /* compute number of unknowns handled by each process */
  i_first = 1;
  i_last  = N/p; 
  int size = N/p; // number of unknowns by each process

  /* Allocation of vectors, including left/upper and right/lower ghost points */
  double ** lu    = (double **)malloc((size+2) * sizeof(double *));
  double ** lunew = (double **)malloc((size+2) * sizeof(double *));
  /* (size+2) x (N) */
  double ** lutemp;
  for (i = 0; i < size+2; i++) {
    lu[i] = (double *)malloc((N) * sizeof(double));
    lunew[i] = (double *)malloc((N) * sizeof(double));
  }
  // initialize the value that is not on the bdry
  for (i=i_first; i<=i_last; i++) { 
    for (j = 1; j < N-1; j++) {
      lu[i][j] = 3;
    }
  }
  // initialize bdry value
  if (mpirank == 0) { // bdry at bottom row
    for (j=0; j<N; j++) {
      lu[i_first][j] = 0;
    }
  }
  if (mpirank == p-1) { // bdry at top row
    for (j=0; j<N; j++) {
      lu[i_last][j] = 0;
    }
  }
  // bdry at two ends
  for (j=i_first; i<=i_last; i++) { 
    lu[i][0] = 0;
    lu[i][N-1] = 0;
  }
  // initialize ghost values
  for (j=0; j < N; j++) {
    lu[i_first-1][j] = 0; // ghost at bottom row
    lu[i_last+1][j] = 0;  // ghost at top row
  }
  /* timing */
  MPI_Barrier(MPI_COMM_WORLD);
  double tt = MPI_Wtime();
  printf("proc %d, lu:\n", mpirank);
  for (i=0; i < size+2; i++) {
    for (j=0; j < N; j++){
      printf("%.1f ", lu[i][j]);
    }
    printf("\n"); 
  }
  MPI_Barrier(MPI_COMM_WORLD);
  double h = 1.0 / (N + 1);
  double hsq = h * h;
  double invhsq = 1./hsq;
  double gres, gres0, tol = 1e-5;

  /* initial residual */
  gres0 = compute_residual(lu, i_first, i_last, N-1, invhsq);
  gres = gres0;
  printf("proc %d, starting with residue %.2f\n", mpirank, gres);

  for (iter = 0; iter < max_iters && gres/gres0 > tol; iter++) {
    /* communicate */
    /* Send up unless the top proc */
    if (mpirank < p - 1) 
        MPI_Send( lu[size], N, MPI_DOUBLE, mpirank + 1, 0, MPI_COMM_WORLD );
    /* Recv down unless the bottom proc */
    if (mpirank > 0)
        MPI_Recv( lu[0], N, MPI_DOUBLE, mpirank - 1, 0, MPI_COMM_WORLD, &status );
    /* Send down unless the bottom proc */
    if (mpirank > 0) 
        MPI_Send( lu[1], N, MPI_DOUBLE, mpirank - 1, 1, MPI_COMM_WORLD );
    /* Recv up unless the top proc */
    if (mpirank < p - 1) 
        MPI_Recv( lu[size+1], N, MPI_DOUBLE, mpirank + 1, 1, MPI_COMM_WORLD, &status );
    
    /* copy new u to u using pointer flipping */
    lutemp = lu; lu = lunew; lunew = lutemp;
    
    /* compute new values */
    for (i=i_first; i<=i_last; i++) 
      for (j=1; j<N-1; j++)
        lunew[i][j] = (lu[i][j+1] + lu[i][j-1] + lu[i+1][j] + lu[i-1][j]) * 0.25;

    if (0 == (iter % 10)) {
      gres = compute_residual(lu, i_first, i_last, N-1, invhsq);
      if (0 == mpirank) {
	      printf("Iter %d: Residual: %g\n", iter, gres);
      }
    }
  }

  /* Clean up */
  free(lu);
  free(lunew);

  /* timing */
  MPI_Barrier(MPI_COMM_WORLD);
  double elapsed = MPI_Wtime() - tt;
  if (0 == mpirank) {
    printf("Time elapsed is %f seconds.\n", elapsed);
    printf("end! The Residue is %.2f at iteration %d\n", gres, iter);
  }
  MPI_Finalize();
  return 0;
}
