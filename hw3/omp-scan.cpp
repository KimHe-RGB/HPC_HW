#include <algorithm>
#include <stdio.h>
#include <math.h>
#include <omp.h>

#include <iostream>

// Scan A array and write result into prefix_sum array;
// use long data type to avoid overflow
void scan_seq(long* prefix_sum, const long* A, long n) {
  if (n == 0) return;
  prefix_sum[0] = 0;
  for (long i = 1; i < n; i++) {
    prefix_sum[i] = prefix_sum[i-1] + A[i-1];
  }
}

void scan_omp(long* prefix_sum, const long* A, long n) {
  // int t = omp_get_thread_num();
  int p = omp_get_num_threads();
  // Fill out parallel scan: One way to do this is array into p chunks
  // Do a scan in parallel on each chunk, then share/compute the offset
  // through a shared vector and update each chunk by adding the offset
  // in parallel
  if (n == 0) return;
  // init the shared offset vector s
  long s[p];
  // parallel
  #pragma omp parallel shared(s)
  {
    int t = omp_get_thread_num();
    int p = omp_get_num_threads();

    long size = n / (long) p;
    long first_index = (long) t * size;
    long last_index  = first_index + size - 1;

    // printf("hello world from thread %d of %ld, index %d - %ld\n", t, p, first_index, last_index);
    // s[t] = A[last_index];
    // printf("%ld %ld %d\n", s[t], A[last_index], t);

    #pragma omp for schedule(static)
    for (long j = first_index+1; j < last_index; j++)
    {
      printf("hi there %ld, %ld, %ld - %ld\n", j, size, first_index+1, last_index);
      prefix_sum[j] = prefix_sum[j-1] + A[j-1];
    }

    // #pragma omp barrier
    s[t] = prefix_sum[last_index] + A[last_index]; // store the offset of thread t

    // The update, where
    // the partial sums are all corrected by the correction should then be done in parallel again.
    // the first entry of each thread need access to the last entry of previous thread
    #pragma omp for schedule(static)
    for (long j = first_index; j < last_index; j++)
    {
      prefix_sum[j] = prefix_sum[j] + s[t]; 
    }
  }
  return;
}

int main() {
  long N = 10;
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

  long x = 0;
  printf("first hi there %ld %ld %d\n", N, x, x<N);
  long N = 10;
  for (long x = 0; x < N; x++)  printf("hi there %ld %ld, %d\n", N, x, x < N);


  long err = 0;
  for (long i = 0; i < N; i++) err = std::max(err, std::abs(B0[i] - B1[i]));
  printf("error = %ld\n", err);

  free(A);
  free(B0);
  free(B1);
  return 0;
}