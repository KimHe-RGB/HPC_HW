# Report

## 1. compute.cpp

Report timings and study the latency of other functions such as sqrt,
sin, cos (as outlined at the bottom of the file). Try to run with different compiler
optimization flags and report timings.

## 1. Answer

Compiling with:

```bash
g++-12 -std=c++11 -O3 -march=native compute.cpp -ftree-vectorize -fopt-info-vec-optimized && ./a.out -n 1000000000
```

Timing:

Mult Plus (A = A * B + C):

```text
0.971381 seconds
3.205683 cycles/eval
2.058840 Gflop/s
```

Division (A = C / A):

```text
3.281350 seconds
10.828574 cycles/eval
0.609498 Gflop/s
```

Square Root (A = sqrt(A)):

```text
3.374693 seconds
11.136606 cycles/eval
0.592640 Gflop/s
```

Sine (A = sin(A)):

```text
12.833050 seconds
42.349200 cycles/eval
0.155847 Gflop/s
```

Compiling with:

```bash
g++-12 -std=c++11 -O3 -march=native compute.cpp && ./a.out -n 1000000000
```

The performance is very close to the other compiling flags.

Comparing to Intel Intrinsics, `_mm256_fmadd_pd` has `latency = 4` and `Throughput = 0.5`, which means

The latency of Division is much slower than Mult-Plus, and is close to sqrt function. The sin function is even slower.

## 2. compute-vec.cpp

Read the code and report timings for the vectorized code. Try to explain the different timings (it’s OK if these explanations aren’t fully correct).

## 2. Answer

Compile with (1):

```bash
g++-12 -std=c++11 -O3 -march=native compute-vec.cpp -ftree-vectorize -fopt-info-vec-optimized && ./a.out -n 1000000000
```

and with (2):

```bash
g++-12 -fopenmp -std=c++11 -O3 -march=native compute-vec.cpp && ./a.out -n 1000000000
```

does not produce very drastic change in flop-rate and running time in all three vectorizations.

Timing (1):

```text
compute-vec.cpp:16:17: optimized: loop vectorized using 32 byte vectors
compute-vec.cpp:52:21: optimized: loop vectorized using 16 byte vectors
compute-vec.cpp:46:5: optimized: basic block part vectorized using 32 byte vectors

time = 0.948463
flop-rate = 8.434548 Gflop/s

time = 0.928695
flop-rate = 8.614136 Gflop/s

time = 0.906875
flop-rate = 8.821405 Gflop/s
```

Explanation:

The difference between the three vectorizations are not significant.

## 3. compute-vec-pipe.cpp

Measure and report the performance of the code for different M as outlined in the comment at the bottom of the file. Summarize your observations.

## 3. Answer

As in Q2, compiling with the any of the two given flags does not affect the performance significantly, so we stick to:

```bash
g++-12 -std=c++11 -O3 -march=native compute-vec-pipe.cpp -ftree-vectorize -fopt-info-vec-optimized && ./a.out -n 1000000000
```

The theoretical peak performance of my computer CPU is `294.4 GFLOPS/s`

For `M = 1`, the performance is just identical to Q2, only `3%` of the peak performance.

For `M = 4`, the performance of all three vectorization raises to about `10%` of the peak performance.

```text
compute-vec-pipe.cpp:17:21: optimized: loop vectorized using 32 byte vectors
compute-vec-pipe.cpp:17:21: optimized:  loop versioned for vectorization because of possible aliasing
compute-vec-pipe.cpp:89:21: optimized: loop vectorized using 32 byte vectors
compute-vec-pipe.cpp:67:21: optimized: loop vectorized using 32 byte vectors
compute-vec-pipe.cpp:61:5: optimized: basic block part vectorized using 32 byte vectors
time = 1.052890
flop-rate = 30.391009 Gflop/s

time = 1.065510
flop-rate = 30.032228 Gflop/s

time = 1.066628
flop-rate = 30.000722 Gflop/s
```

For `M = 8`, all three vectorization methods has much higher performance, reaches about `17 ~ 19.4%` of the peak performance:

```text
...

time = 1.117773
flop-rate = 57.254862 Gflop/s

time = 1.194774
flop-rate = 53.566212 Gflop/s

time = 1.276603
flop-rate = 50.132577 Gflop/s
```

For `M = 12`, the performance for the Implicit vectorization is even higher and reaches `21%`, but for the other two vectorization, the performance is dropping to `12%`:

```text
...

time = 1.525174
flop-rate = 62.942152 Gflop/s

time = 2.547976
flop-rate = 37.676816 Gflop/s

time = 2.601651
flop-rate = 36.899518 Gflop/s
```

For `M = 32`, the performance has significantly dropped to `7 ~ 10%`:

```text
...

time = 9.630626
flop-rate = 26.581766 Gflop/s

time = 8.698313
flop-rate = 29.430903 Gflop/s

time = 11.187171
flop-rate = 22.883336 Gflop/s
```

The conclusion is that the optimal `M` value for the two explicit vectorizations is around `8`, while for implicit vectorization, the optimal `M` value is around `12`;