// #pragma omp parallel
// {
//     #pragma omp for schedule(static)
//     for (i = 1; i < n; i++)
//         f(i)
//     #pragma omp for schedule(static)
//     for (i = 1; i < n; i++)
//         f(n-i)
// }

/**
 * 
(a) How long would each threads spend to execute the parallel region? How much of that time
would be spent in waiting for the other thread?

Under the "schedule(static)" strategy, each thread is assigned to a contiguous block of iterations;
the number of iterations per block is determined at compile-time.

The total runtime of "#pragma omp for schedule(static)" is about 1070ms on my computer. 

(b) How would the execution time of each thread change if we used schedule(static,1) for
both loops?

"static, 1" clause specifies that each thread is assigned to a block of exactly 1 iteration. 
The execution time is improved from schedule(static). 
The total runtime of "#pragma omp for schedule(static, 1)" is about 680ms on my computer. 

(c) Would it improve if we used schedule(dynamic,1) instead?

"dynamic, 1" clause specifies that each thread will be assigned to a block of iterations at runtime when it becomes available, 
and the block size is determined at runtime.
The execution time slightly improved from schedule(static,1). 
The total runtime of "#pragma omp for schedule(dynamic, 1)" is about 640ms on my computer. 

(d) Is there an OpenMP directive that allows to eliminate the waiting time and how much would
the threads take when using this clause

The "nowait" clause eliminate the waiting time.
The execution time with "nowait" should further improve, especially when f(k) runtime varies with the input k.
The total runtime of "#pragma omp for schedule(dynamic, 1) nowait" is about 580ms on my computer. 
*/


#include <iostream>
#include <chrono>

#include <windows.h>

void myFunction() {
    int n = 100;
    #pragma omp parallel
    {
        // #pragma omp for schedule(static)            // (a)
        // #pragma omp for schedule(static,1)          // (b)
        // #pragma omp for schedule(dynamic, 1)        // (c)
        #pragma omp for schedule(dynamic, 1) nowait    // (d)
        for (int i = 1; i < n; i++)
            Sleep(i);
        // #pragma omp for schedule(static)            // (a)
        // #pragma omp for schedule(static,1)          // (b)
        // #pragma omp for schedule(dynamic, 1)      // (c)
        #pragma omp for schedule(dynamic, 1) nowait    // (d)
        for (int i = 1; i < n; i++)
            Sleep(n-i);
    }
}

int main() {
  // Start the timer
  auto start_time = std::chrono::high_resolution_clock::now();

  // Call the function to be timed
  myFunction();

  // Stop the timer and calculate the elapsed time
  auto end_time = std::chrono::high_resolution_clock::now();
  auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

  // Print the elapsed time in milliseconds
  std::cout << "Elapsed time: " << elapsed_time.count() << " milliseconds" << std::endl;

  return 0;
}