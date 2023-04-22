/* MPI-parallel Jacobi smoothing to solve -u''=f
 * Global vector has N x N unknowns, each processor works with its
 * part, which has lN = N x N / p unknowns. assuming the matrix in each processor has shape (sqrt_lN x sqrt_lN)
 * Author: Georg Stadler
 * 
 * Adapted to 2D by Kim
 */
#include <stdio.h>
#include <math.h>
#ifdef _OPENMP
#include <omp.h>
#endif
#include <mpi.h>
#include <string.h>

/* compuate global residual, assuming ghost values are updated */
double compute_residual(double **lu, int sqrt_lN, double invhsq) {
  int i, j;
  double tmp, gres = 0.0, lres = 0.0;

#pragma omp parallel for default(none) shared(lu,sqrt_lN,invhsq) private(i,j,tmp) reduction(+:lres)
  for (i = 1; i <= sqrt_lN; i++){
    for (j = 1; j < sqrt_lN; j++)
    {
      tmp = ((4.0*lu[i][j] - lu[i-1][j] - lu[i+1][j] - lu[i][j-1] - lu[i][j+1]) * invhsq - 1);
      lres += tmp * tmp;
    }
  }
  /* use allreduce for convenience; a reduce would also be sufficient */
  MPI_Allreduce(&lres, &gres, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
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
  int mpirank, i, j, p, N, lN, iter, max_iters;
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
  # pragma omp parallel
  {
  #ifdef _OPENMP
    int my_threadnum = omp_get_thread_num();
    int numthreads = omp_get_num_threads();
  #else
    int my_threadnum = 0;
    int numthreads = 1;
  #endif
    printf("Hello, I'm thread %d out of %d on mpirank %d\n", my_threadnum, numthreads, mpirank);
  }

  if (!isPowerOfFour(p) || !isPowerOfFour(N*N)) {
    printf("Exiting. Both p and NxN must be power of 4\n");
    MPI_Abort(MPI_COMM_WORLD, 0);
  }
  if ((N * N % p != 0) && mpirank == 0 ) {
    printf("Exiting. NxN must be a multiple of p;\n");
    MPI_Abort(MPI_COMM_WORLD, 0);
  }
  /* compute number of unknowns handled by each process */
  lN = N * N / p;
  int sqrt_p = sqrt(p);   // number of processes per edge
  int sqrt_lN = sqrt(lN);
  if (mpirank == 0) printf("NxN: %d, local grid: %d x %d = %d\n", N, sqrt_lN, sqrt_lN, lN);

  /* timing */
  MPI_Barrier(MPI_COMM_WORLD);
  double tt = MPI_Wtime();

  /* Allocation of vectors, including left/upper and right/lower ghost points */
  /* (sqrt_lN+2) x (sqrt_lN+2) */
  double ** lu    = (double **)malloc((sqrt_lN+2) * sizeof(double *));
  double ** lunew = (double **)malloc((sqrt_lN+2) * sizeof(double *));
  double ** lutemp;
  for (int i = 0; i < sqrt_lN+2; i++) {
    lu[i] = (double *)malloc((sqrt_lN+2) * sizeof(double));
    lunew[i] = (double *)malloc((sqrt_lN+2) * sizeof(double));
  }

  double h = 1.0 / (N + 1);
  double hsq = h * h;
  double invhsq = 1./hsq;
  double gres, gres0, tol = 1e-5;

  /* initial residual */
  gres0 = compute_residual(lu, sqrt_lN, invhsq);
  gres = gres0;

  for (iter = 0; iter < max_iters && gres/gres0 > tol; iter++) {

  #pragma omp parallel for default(none) private(i,j) shared(lN,sqrt_lN,lunew,lu,hsq)
    /* Jacobi step for local points */
    for (i = 1; i <= sqrt_lN; i++){
      for (j = 1; j <= sqrt_lN; j++){
        // Jacobi update, which only depend on values in the previous iteration, here we are choosing f(x,y)==1
        lunew[i][j] = 0.25 * (hsq + lu[i-1][j] + lu[i][j-1] + lu[i+1][j] + lu[i][j+1]);
      }
    }

    if (mpirank > sqrt_p - 1) {
      /* if not bottom row processes, send/recv bdry values to the bottom*/
      int brow_0[sqrt_lN]; int brow_1[sqrt_lN];
      for(int i = 0; i < sqrt_lN; i++) {
          brow_1[i] = lunew[1][i];
      }
      MPI_Send(&(brow_1), sqrt_lN, MPI_DOUBLE, mpirank-sqrt_p, 122, MPI_COMM_WORLD);
      MPI_Recv(&(brow_0), sqrt_lN, MPI_DOUBLE, mpirank-sqrt_p, 121, MPI_COMM_WORLD, &status1);
      for(int i = 0; i < sqrt_lN; i++) {
        lunew[0][i] = brow_0[i];
      }
    }
    if (mpirank < sqrt_p * (sqrt_p-1)) {
      /* if not top row processes, send/recv bdry values to the top*/
      int trow_0[sqrt_lN]; int trow_1[sqrt_lN];
      for(int i = 0; i < sqrt_lN; i++) {
          trow_1[i] = lunew[sqrt_lN][i];
      }
      MPI_Send(&(trow_1), sqrt_lN, MPI_DOUBLE, mpirank+sqrt_p, 121, MPI_COMM_WORLD);
      MPI_Recv(&(trow_0), sqrt_lN, MPI_DOUBLE, mpirank+sqrt_p, 122, MPI_COMM_WORLD, &status1);
      for(int i = 0; i < sqrt_lN; i++) {
        lunew[sqrt_lN+1][i] = trow_0[i];
      }
    }
    if (mpirank % sqrt_p != 0) {
      /* if not left-most col processes, send/recv bdry values to the left*/
      int lcol_0[sqrt_lN]; int lcol_1[sqrt_lN];
      for(int i = 0; i < sqrt_lN; i++) {
          lcol_1[i] = lunew[i][1];
      }
      MPI_Send(&(lcol_1), sqrt_lN, MPI_DOUBLE, mpirank-1, 123, MPI_COMM_WORLD);
      MPI_Recv(&(lcol_0), sqrt_lN, MPI_DOUBLE, mpirank-1, 124, MPI_COMM_WORLD, &status1);
      for(int i = 0; i < sqrt_lN; i++) {
          lunew[i][0] = lcol_0[i];
      }
    }
    if (mpirank % (sqrt_p-1) != 0) {
      /* if not right-most col processes, send/recv bdry values to the right*/
      int rcol_0[sqrt_lN]; int rcol_1[sqrt_lN];
      for(int i = 0; i < sqrt_lN; i++) {
          rcol_1[i] = lunew[i][sqrt_lN];
      }
      MPI_Send(&(rcol_1), sqrt_lN, MPI_DOUBLE, mpirank+1, 124, MPI_COMM_WORLD);
      MPI_Recv(&(rcol_0), sqrt_lN, MPI_DOUBLE, mpirank+1, 123, MPI_COMM_WORLD, &status1);
      for(int i = 0; i < sqrt_lN; i++) {
          lunew[i][sqrt_lN+1] = rcol_0[i];
      }
    }

    /* copy new u to u using pointer flipping */
    lutemp = lu; lu = lunew; lunew = lutemp;
    if (0 == (iter % 10)) {
      gres = compute_residual(lu, sqrt_lN, invhsq);
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
  }
  MPI_Finalize();
  return 0;
}
