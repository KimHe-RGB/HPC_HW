# HW3 Report

## Q1

The Code I am referring to is at `./Q1.cpp`

### (a) How long would each threads spend to execute the parallel region? How much of that time would be spent in waiting for the other thread?

Under the `schedule(static)` strategy, each thread is assigned to a contiguous block of iterations;
the number of iterations per block is determined at compile-time.

The total runtime of `#pragma omp for schedule(static)` is about `1070ms` on my computer.

### (b) How would the execution time of each thread change if we used schedule(static,1) for both loops?

`#pragma omp for schedule(static, 1)` clause specifies that each thread is assigned to a block of exactly 1 iteration.
The execution time is improved from `schedule(static)`.
The total runtime of `#pragma omp for schedule(static, 1)` is about `680ms` on my computer.

### (c) Would it improve if we used schedule(dynamic,1) instead?

`#pragma omp for schedule(dynamic, 1)` clause specifies that each thread will be assigned to a block of iterations at runtime when it becomes available,
and the block size is determined at runtime.

The execution time slightly improved from `schedule(static,1)`.
The total runtime of `#pragma omp for schedule(dynamic, 1)` is about `640ms` on my computer.

### (d) Is there an OpenMP directive that allows to eliminate the waiting time and how much would the threads take when using this clause

The `nowait` clause eliminate the waiting time.
The execution time with "nowait" should further improve, `especially when f(k) runtime varies with the input k`.
The total runtime of `#pragma omp for schedule(dynamic, 1) nowait` is about `580ms` on my computer.

## Q2

The Code for Q2 to is at `./omp-scan.cpp`

### Run it with different thread numbers and report the architecture you run it on, the number of cores of the processor and the time it takes

#### Architecture

CPU: 13th Gen Intel(R) Core(TM) i5-13600KF (14 cores, 20 threads)

System: Windows / Ubuntu WSL

#### Time Report

sequential-scan = 0.062000s

Threads = 2;
parallel-scan time = 0.052000s

Threads = 4;
parallel-scan time = 0.038000s

Threads = 8;
parallel-scan time = 0.036000s

Threads = 16;
parallel-scan time = 0.033000s

Threads = 20;
parallel-scan time = 0.029000s

Threads = 32;
parallel-scan time = 0.028000s

## Q3

The Code for Q3 to is at `./jacobi2D-omp.cpp` and `./gs2D-omp.cpp`

### report timings for different values of N and different numbers of threads, specifying the machine you run on

#### Jacobi & Gauss Seidel time

=====================================

N = 1000

=====================================

sequential Jacobi = 0.041000s

parallel Jacobi = 0.022000s, 2 threads, N = 1000

parallel Jacobi = 0.011000s, 4 threads, N = 1000

parallel Jacobi = 0.011000s, 8 threads, N = 1000

parallel Jacobi = 0.007000s, 16 threads, N = 1000

=====================================

sequential Gauss-Siedel = 0.010000s

parallel Gauss-Siedel = 0.006000s, 2 threads, N = 1000

parallel Gauss-Siedel = 0.004000s, 4 threads, N = 1000

parallel Gauss-Siedel = 0.005000s, 8 threads, N = 1000

parallel Gauss-Siedel = 0.004000s, 16 threads, N = 1000

=====================================

N = 5000

=====================================

sequential Jacobi = 1.070000s

parallel Jacobi = 0.547000s, 2 threads, N = 5000

parallel Jacobi = 0.280000s, 4 threads, N = 5000

parallel Jacobi = 0.184000s, 8 threads, N = 5000

parallel Jacobi = 0.132000s, 16 threads, N = 5000

=====================================

sequential Gauss-Siedel = 0.552000s

parallel Gauss-Siedel = 0.310000s, 2 threads, N = 5000

parallel Gauss-Siedel = 0.232000s, 4 threads, N = 5000

parallel Gauss-Siedel = 0.222000s, 8 threads, N = 5000

parallel Gauss-Siedel = 0.217000s, 16 threads, N = 5000

=====================================

N = 10000

=====================================

sequential Jacobi = 4.355000s

parallel Jacobi = 2.188000s, 2 threads, N = 10000

parallel Jacobi = 1.098000s, 4 threads, N = 10000

parallel Jacobi = 0.685000s, 8 threads, N = 10000

parallel Jacobi = 0.504000s, 16 threads, N = 10000

=====================================

sequential Gauss-Siedel = 2.130000s

parallel Gauss-Siedel = 1.225000s, 2 threads, N = 10000

parallel Gauss-Siedel = 0.963000s, 4 threads, N = 10000

parallel Gauss-Siedel = 0.876000s, 8 threads, N = 10000

parallel Gauss-Siedel = 0.837000s, 16 threads, N = 10000

=====================================

#### observation and analysis

With 16 threads the parallelized version is only twice faster than the sequential version.

The Reason may be that the performance is bounded by the squential part of the program, and in both jacobian and Gauss-Siedel iteration, each iteration depend on the previous step and is not parallizable.

Moreover the Gauss Seidal update requires to update red points first, and then update black points, which is also sequential. We should expect that as thread number increasesthe Jacobi method runtime converge faster than Gauss-Seidal. The Jacobi method is approximately exponentially decreasing, while GS is not.
