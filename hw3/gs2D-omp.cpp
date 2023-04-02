#if defined(_OPENMP)
#include <omp.h>
#else
typedef int omp_int_t;
inline omp_int_t omp_get_thread_num() { return 0;}
inline omp_int_t omp_get_num_threads() { return 1;}
#endif

#include <algorithm>
#include <stdio.h>
#include <math.h>

void jacobi2D_seq(double** grid, double** out, int n, double h) {
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            double fij = 1.0;
            out[i][j] = 0.25 * (h*h * fij + out[i-1][j] + out[i][j-1] + out[i+1][j] + out[i][j+1]);
        }
        
    }
    
}

void jacobi2D_omp(double** grid, double** out, long n) {

}

// (N+1) x (N+1) uniform sized points in [0,1]x[0,1]
int main() {
    const int N = 10;
    const double h = 1.0 / N;
    double **grid = (double **)malloc((N+1) * sizeof(double *));
    for (int i = 0; i <= N; i++) {
        grid[i] = (double *)malloc((N+1) * sizeof(double));
    }

    for (int i = 0; i <= N; i++) {
        for (int j = 0; j <= N; j++) {
            grid[i][j] = i * h;
        }
    }
  
    free(grid);
    return 0;
}