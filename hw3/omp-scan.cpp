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

// Scan A array and write result into prefix_sum array;
// use long data type to avoid overflow
void scan_seq(long* prefix_sum, const long* A, long n) {
  if (n == 0) return;
  prefix_sum[0] = 0;
  for (long i = 1; i < n; i++) {
    prefix_sum[i] = prefix_sum[i-1] + A[i-1];
  }
}

const int NUM_OF_THREADS = 20;

void scan_omp(long* prefix_sum, const long* A, long n) {
  // Fill out parallel scan: One way to do this is array into p chunks
  // Do a scan in parallel on each chunk, then share/compute the offset
  // through a shared vector and update each chunk by adding the offset
  // in parallel
  if (n == 0) return;
  // init the shared offset vector s
  long* s = (long*) malloc(NUM_OF_THREADS * sizeof(long));
  // parallel
  #pragma omp parallel shared(s) num_threads(NUM_OF_THREADS)
  {
    int t = omp_get_thread_num();
    int p = omp_get_num_threads();
    // printf("calling with %d threads\n", p);

    long size = n / (long) p;
    long first_index = (long) t * size;
    long last_index  = first_index + size - 1;

    // partial scan
    #pragma omp parallel for schedule(static)
    for (long j = first_index+1; j < last_index+1; j++)
    {
      prefix_sum[j] = prefix_sum[j-1] + A[j-1];
    
    }
    #pragma omp barrier // make sure the prefix_sum[last_index] has been computed

    s[t] = prefix_sum[last_index] + A[last_index]; // store the offset of thread t that should be added to thread t+1

    #pragma omp barrier // make sure all s[t] have been computed here

    #pragma omp parallel for schedule(static)
    for (long j = first_index; j < last_index+1; j++)
    {
      long offset = 0;
      for (int i = 0; i < t; i++) offset = offset + s[i];

      if(t > 0) prefix_sum[j] = prefix_sum[j] + offset; 
    }
  }
  
  return;
}

int main() {
  long N = 100000000;
  long* A = (long*) malloc(N * sizeof(long));
  long* B0 = (long*) malloc(N * sizeof(long));
  long* B1 = (long*) malloc(N * sizeof(long));
  for (long i = 0; i < N; i++) A[i] = rand();
  for (long i = 0; i < N; i++) B1[i] = 0;
  
  double tt = omp_get_wtime();
  scan_seq(B0, A, N);
  printf("sequential-scan = %fs\n", omp_get_wtime() - tt);

  tt = omp_get_wtime();
  scan_omp(B1, A, N);
  printf("parallel-scan   = %fs\n", omp_get_wtime() - tt);

  // printf("first hi there %ld %ld\n", B0[0], B1[0]);
  // printf("last hi there %ld %ld\n", B0[N-1], B1[N-1]);

  long err = 0;
  for (long i = 0; i < N; i++) err = std::max(err, std::abs(B0[i] - B1[i]));
  printf("error = %ld\n", err);

  printf("calling with %d threads\n", NUM_OF_THREADS);

  free(A);
  free(B0);
  free(B1);
  return 0;
}