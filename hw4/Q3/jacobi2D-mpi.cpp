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
    for (j = 1; j <= Nj; j++)
    {
      tmp = ((4.0*lu[i][j] - lu[i-1][j] - lu[i+1][j] - lu[i][j-1] - lu[i][j+1]) * invhsq - 1);
      lres += tmp * tmp;
    }
  }
  /* use allreduce for convenience; a reduce would also be sufficient */
  MPI_Allreduce(&lres, &gres, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

  printf("fin computing gres=%.2f; lres=%.2f\n", gres, lres);
  return sqrt(gres);
}

bool isPowerOfFour(int n) {
    if(n <= 0) return false;
    if((n & (n-1)) != 0) return false; // check if the integer is a power of 2
    return (n & 0x55555555) != 0; // check if the integer has a set bit in an odd position
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

  if ((!isPowerOfFour(p) || !isPowerOfFour(N*N))) {
    printf("Exiting. Both p and NxN must be power of 4\n");
    MPI_Abort(MPI_COMM_WORLD, 0);
  }
  if ((N * N % p != 0)) {
    printf("Exiting. NxN must be a multiple of p;\n");
    MPI_Abort(MPI_COMM_WORLD, 0);
  }
  /* compute number of unknowns handled by each process */
  int i_first = 1;
  int i_last  = N/p; 
  int size = i_last; // number of unknowns by each process
  if (mpirank == 0)        i_first++;
  if (mpirank == p - 1)    i_last--;

  /* timing */
  MPI_Barrier(MPI_COMM_WORLD);
  double tt = MPI_Wtime();

  /* Allocation of vectors, including left/upper and right/lower ghost points */
  /* (size+2) x (N) */
  double ** lu    = (double **)malloc((size+2) * sizeof(double *));
  double ** lunew = (double **)malloc((size+2) * sizeof(double *));
  double ** lutemp;
  for (int i = 0; i < size+2; i++) {
    lu[i] = (double *)malloc((N) * sizeof(double));
    lunew[i] = (double *)malloc((N) * sizeof(double));
  }
  // initialize the value
  for (i=1; i <= N/size; i++) 
    for (j=0; j < N; j++) 
        lu[i][j] = 1;
  // initialize boundary to be zero
  for (j = 0; j < N; j++)
  {
      lu[i_first-1][j] = 0;
      lu[i_last+1][j] = 0;
      lunew[i_first-1][j] = 0;
      lunew[i_last+1][j] = 0;
  }

  double h = 1.0 / (N + 1);
  double hsq = h * h;
  double invhsq = 1./hsq;
  double gres, gres0, tol = 1e-5;

  /* initial residual */
  printf("0 %d\n", mpirank);
  gres0 = compute_residual(lu, i_first, i_last, N, invhsq);
  gres = gres0;

  for (iter = 0; iter < max_iters && gres/gres0 > tol; iter++) {
    for (i=i_first; i<=i_last; i++) 
      for (j=1; j<N-1; j++) {
        lunew[i][j] = (lu[i][j+1] + lu[i][j-1] + lu[i+1][j] + lu[i-1][j]) * 0.25;
	  }
    /* copy new u to u using pointer flipping */
    lutemp = lu; lu = lunew; lunew = lutemp;
    if (0 == (iter % 10)) {
      gres = compute_residual(lu, i_first, i_last, N, invhsq);
      if (0 == mpirank) {
	      printf("Iter %d: Residual: %g\n", iter, gres);
      }
    }
  }
  printf("end! \n");

  /* Clean up */
  free(lu);
  free(lunew);

  /* timing */
  MPI_Barrier(MPI_COMM_WORLD);
  double elapsed = MPI_Wtime() - tt;
  if (0 == mpirank) {
    printf("Time elapsed is %f seconds.\n", elapsed);
  }
  MPI_Finalize();
  return 0;
}
