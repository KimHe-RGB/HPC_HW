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

    printf("hello world from thread %d, size %ld, [%ld - %ld]\n", t, size, first_index, last_index);

    // the scan has to be sequential
    // #pragma omp for schedule(static)
    for (long j = first_index+1; j < last_index+1; j++)
    {
      prefix_sum[j] = prefix_sum[j-1] + A[j-1];
    }
    #pragma omp barrier

    s[t] = prefix_sum[last_index] + A[last_index]; // store the offset of thread t that should be added to thread t+1
    #pragma omp barrier // make sure all s[t] have been computed here


    printf("hello world 2 from thread %d, s[t] = %ld = %ld + %ld \n", t, s[t], prefix_sum[last_index], A[last_index]);
    #pragma omp barrier

    // The update, where
    // the partial sums are all corrected by the correction should then be done in parallel again.
    // the first entry of each thread need access to the last entry of previous thread
    // #pragma omp for schedule(static)
    for (long j = first_index; j < last_index+1; j++)
    {
      long offset = 0;
      for (int i = 0; i < t; i++) offset = offset + s[i];

      if(t > 0) prefix_sum[j] = prefix_sum[j] + offset; 
    }

    #pragma omp barrier
    printf("hello world 3 from thread %d:  [%ld = %ld]\n", t, prefix_sum[first_index], prefix_sum[last_index]);

  }
  
  return;
}

int main() {
  long N = 10000;
  // long* A = (long*) malloc(N * sizeof(long));
  // long* B0 = (long*) malloc(N * sizeof(long));
  // long* B1 = (long*) malloc(N * sizeof(long));
  long* A = new long[N];
  long* B0 = new long[N];
  long* B1 = new long[N];
  for (long i = 0; i < N; i++) A[i] = rand();
  for (long i = 0; i < N; i++) B1[i] = 0;
  
  double tt = omp_get_wtime();
  scan_seq(B0, A, N);
  printf("sequential-scan = %fs\n", omp_get_wtime() - tt);

  tt = omp_get_wtime();
  scan_omp(B1, A, N);
  printf("parallel-scan   = %fs\n", omp_get_wtime() - tt);

  printf("first hi there %ld %ld\n", B0[0], B1[0]);
  printf("last hi there %ld %ld\n", B0[N-1], B1[N-1]);

  long err = 0;
  for (long i = 0; i < N; i++) err = std::max(err, std::abs(B0[i] - B1[i]));
  printf("error = %ld\n", err);

  // free(A);
  // free(B0);
  // free(B1);
  delete[] A;
  delete[] B0;
  delete[] B1;
  return 0;
}

  // printf("second hi there %ld %ld\n", B0[1], B1[1]);
  // printf("fourth hi there %ld %ld\n", B0[3], B1[3]);
  // printf("fifth hi there %ld %ld\n", B0[4], B1[4]);
  // printf("sixth hi there %ld %ld\n", B0[5], B1[5]);
  // printf("seventh hi there %ld %ld\n", B0[6], B1[6]);
  // printf("tenth hi there %ld %ld\n", B0[9], B1[9]);
  // printf("eleventh hi there %ld %ld\n", B0[10], B1[10]);