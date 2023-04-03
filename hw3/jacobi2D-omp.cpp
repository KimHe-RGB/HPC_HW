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
#include <iostream>

const int NUM_OF_THREADS = 2;

/**
 * @brief 
 * 
 * @param grid the grid to update
 * @param n dimension of the grid, in main() it is N+2; 
 * @param h grid interval size
 * @param iters number of Jacobian iterations
 */
void jacobi2D_seq(double** grid, int n, double h, int iters) {
    // f(x, y) ≡ 1
    double fij = 1.0;

    for (int k = 1; k < iters; k++)
    {
        for (int i = 1; i < n-1; i++)
        {
            for (int j = 1; j < n-1; j++)
            {
                // Jacobi update, which does not depend on values in previous iteration
                grid[i][j] = 0.25 * (h*h * fij + grid[i-1][j] + grid[i][j-1] + grid[i+1][j] + grid[i][j+1]);
            }
        }
    }    
}

/**
 * @brief 
 * 
 * @param grid the grid to update and store the laplacian value, the boundary points should be properly initialized 
 * @param n dimension of the grid, in main() it is N+2; 
 * @param h grid interval size
 * @param iters number of GS iterations
 */
void gs2D_seq(double** grid, int n, double h, int iters) {
    // f(x, y) ≡ 1; normally this should be read from a function value matrix
    double fij = 1.0; 

    // when we update red points at (m,n), we must make sure that all surrounding black points ARE NOT updated
    // when we update black points at (m,n), we must make sure that all surrounding red points ARE updated
    for (int k = 1; k < iters; k++)
    {
        // update the red points, 
        // alterantively we update the left black point, which is certainly updated
        for (int i = 1; i < n-1; i++)
        {   
            for (int j = 2; j < n-1; j+=2)
            {
                // gs update of the "red points", which indices (ih,jh) satisfies i+j=even
                grid[i][j] = 0.25 * (h*h * fij + grid[i-1][j] + grid[i][j-1] + grid[i+1][j] + grid[i][j+1]);
            }
        }
        // revisit the columns again, this time update black points; 
        for (int i = 1; i < n-1; i++)
        { 
            for (int j = 1; j < n-1; j+=2)
            {
                // gs update of the "black points", which indices (ih,jh) satisfies i+j=odd
                grid[i][j] = 0.25 * (h*h * fij + grid[i-1][j] + grid[i][j-1] + grid[i+1][j] + grid[i][j+1]);
            }
        }   
    }
}

void jacobi2D_omp(double** grid, int n, double h, int iters) {
    // f(x, y) ≡ 1
    double fij = 1.0;
    // iterations must be done in sequence
    for (int k = 1; k < iters; k++)
    {
        #pragma omp parallel for num_threads(NUM_OF_THREADS)
        for (int i = 1; i < n-1; i++)
        {
            for (int j = 1; j < n-1; j++)
            {
                // Jacobi update, which only depend on values in the previous iteration
                grid[i][j] = 0.25 * (h*h * fij + grid[i-1][j] + grid[i][j-1] + grid[i+1][j] + grid[i][j+1]);
            }
        }
        #pragma omp barrier 
    }
}


// (N+2) x (N+2) uniform sized points in [0,1]x[0,1]
int main() {
    const int N = 1000;
    const double h = 1.0 / (N+1);
    double **gridVal = (double **)malloc((N+2) * sizeof(double *));
    double **gridVal2 = (double **)malloc((N+2) * sizeof(double *));

    for (int i = 0; i < N+2; i++) {
        gridVal[i] = (double *)malloc((N+2) * sizeof(double));
        gridVal2[i] = (double *)malloc((N+2) * sizeof(double));
    }

    // initialize the boundary to be zero
    for (int j = 0; j < N+2; j++)
    {
        gridVal[0][j] = 0;
        gridVal[N+1][j] = 0;
        gridVal[j][0] = 0;
        gridVal[j][N+1] = 0;
        gridVal2[0][j] = 0;
        gridVal2[N+1][j] = 0;
        gridVal2[j][0] = 0;
        gridVal2[j][N+1] = 0;
    }
    
    int iters = 100;
    double tt = omp_get_wtime();
    jacobi2D_seq(gridVal, N+2, h, 20);
    printf("sequential Jacobi = %fs\n", omp_get_wtime() - tt);

    tt = omp_get_wtime();
    jacobi2D_omp(gridVal2, N+2, h, 20);
    printf("parallel Jacobi = %fs, %d threads, N = %d\n", omp_get_wtime() - tt, NUM_OF_THREADS, N);

    // visualize the matrix for small N
    // for(int a = 0; a < N+2; a++)
    // {
    //     for(int b = 0; b < N+2; b++)
    //     {
    //         printf("%.3f ", gridVal[a][b]);
    //     }
    //     printf("\n");
    // }
    // printf("\n");

    // for(int a = 0; a < N+2; a++)
    // {
    //     for(int b = 0; b < N+2; b++)
    //     {
    //         printf("%.3f ", gridVal2[a][b]);
    //     }
    //     printf("\n");
    // }
    // printf("\n");
    double max_err = 0;
    for(int a = 0; a < N+2; a++)
    {
        for(int b = 0; b < N+2; b++)
        {
            max_err = std::max(max_err, std::abs(gridVal[a][b] - gridVal2[a][b]));
        }
    }
    printf("error = %.6f\n", max_err);
    
    free(gridVal);
    return 0;
}