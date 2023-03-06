# Report

## Processor I am Using

Intel(R) Core(TM) i9-9880H CPU @ 2.30GHz

## (a)

Compiling uniformly with:

```bash
g++-12 -fopenmp -std=c++11 -O3 -march=native InnerProduct.cpp && ./a.out
```

Using vector with size larger than cache size (L1: 32KB)

one double takes 8B, so vectors with 4096 entries fills up the cache.

But actually I don't observe a quick degeneration of performance for even larger dimensions. 

For loop unrolling, even though it is at-all-time faster than the naive build, we can observe decrease in performance as dimension grows, in 4-loop unrolling, this decrease is more obvious.

### Timing for `InnerProd0()`, the naive build

| Dimension | Time | GFLOP/s |
| :---      |   ---:  |   ---:|
|      1024  |   1.475682   |   0.677653    |
|      2048  |   1.510645   |   0.661970    |
|      3072  |   1.522737   |   0.656713    |
|      4096  |   1.528223   |   0.654356    |
|      5120  |   1.533163   |   0.652248    |
|      6144  |   1.535320   |   0.651332    |
|      7168  |   1.536103   |   0.650998    |
|      8192  |   1.537858   |   0.650259    |
|      9216  |   1.538152   |   0.650131    |
|     10240  |   1.538964   |   0.649793    |
|     11264  |   1.538546   |   0.649969    |
|     12288  |   1.540432   |   0.649175    |
|     13312  |   1.540234   |   0.649259    |
|     14336  |   1.540749   |   0.649040    |
|     15360  |   1.541176   |   0.648863    |
|     16384  |   1.552375   |   0.644183    |
|     17408  |   1.570622   |   0.636692    |
|     18432  |   1.561846   |   0.640274    |
|     19456  |   1.552886   |   0.643974    |
|     20480  |   1.546580   |   0.646600    |
|     21504  |   1.545652   |   0.646976    |
|     22528  |   1.549920   |   0.645206    |
|     23552  |   1.550565   |   0.644938    |
|     24576  |   1.547364   |   0.646275    |
|     25600  |   1.549621   |   0.645327    |
|     26624  |   1.551480   |   0.644561    |
|     27648  |   1.547616   |   0.646155    |
|     28672  |   1.544546   |   0.647454    |
|     29696  |   1.549066   |   0.645559    |
|     30720  |   1.547619   |   0.646172    |
|     31744  |   1.546928   |   0.646463    |

### Timing for `InnerProd2_0()`, the first 2-loop unrolling

The performance is about twice better than the naive build:

| Dimension | Time | GFLOP/s |
| :---      |   ---:  |   ---:|
|  1024 | 0.767567 | 1.302819 |
|  2048 | 0.771654 | 1.295920 |
|  3072 | 0.771776 | 1.295713 |
|  4096 | 0.772370 | 1.294718 |
|  5120 | 0.771585 | 1.296037 |
|  6144 | 0.771806 | 1.295667 |
|  7168 | 0.771916 | 1.295478 |
|  8192 | 0.772397 | 1.294678 |
|  9216 | 0.773999 | 1.291992 |
| 10240 | 0.772408 | 1.294663 |
| 11264 | 0.820657 | 1.218544 |
| 12288 | 0.824743 | 1.212511 |
| 13312 | 0.777067 | 1.286904 |
| 14336 | 0.781302 | 1.279925 |
| 15360 | 0.778970 | 1.283763 |
| 16384 | 0.793999 | 1.259465 |
| 17408 | 0.782417 | 1.278094 |
| 18432 | 0.786982 | 1.270689 |
| 19456 | 0.804095 | 1.243658 |
| 20480 | 0.808474 | 1.236920 |
| 21504 | 0.804989 | 1.242254 |
| 22528 | 0.815771 | 1.225856 |
| 23552 | 0.824723 | 1.212550 |
| 24576 | 0.823495 | 1.214363 |
| 25600 | 0.823471 | 1.214387 |
| 26624 | 0.826646 | 1.209737 |
| 27648 | 0.828387 | 1.207166 |
| 28672 | 0.825309 | 1.211694 |
| 29696 | 0.831319 | 1.202923 |
| 30720 | 0.831417 | 1.202800 |
| 31744 | 0.825617 | 1.211253 |

### Timing for `InnerProd2()`, the better 2-loop unrolling in the book

The improvement from `InnerProd2_0()` is quite small.
Probably the native optimization in g++ already counts this:

| Dimension | Time | GFLOP/s |
| :---      |   ---:  |   ---:|
|  1024 | 0.762042 | 1.312264 |
|  2048 | 0.769093 | 1.300235 |
|  3072 | 0.770173 | 1.298410 |
|  4096 | 0.776670 | 1.287550 |
|  5120 | 0.774959 | 1.290394 |
|  6144 | 0.781222 | 1.280050 |
|  7168 | 0.777073 | 1.286881 |
|  8192 | 0.781387 | 1.279783 |
|  9216 | 0.774907 | 1.290478 |
| 10240 | 0.773635 | 1.292609 |
| 11264 | 0.773356 | 1.293074 |
| 12288 | 0.773983 | 1.292031 |
| 13312 | 0.776217 | 1.288313 |
| 14336 | 0.773047 | 1.293592 |
| 15360 | 0.775827 | 1.288964 |
| 16384 | 0.779830 | 1.282348 |
| 17408 | 0.779243 | 1.283300 |
| 18432 | 0.782136 | 1.278562 |
| 19456 | 0.784103 | 1.275367 |
| 20480 | 0.787798 | 1.269384 |
| 21504 | 0.792136 | 1.262410 |
| 22528 | 0.795315 | 1.257386 |
| 23552 | 0.796402 | 1.255670 |
| 24576 | 0.810706 | 1.233520 |
| 25600 | 0.813458 | 1.229336 |
| 26624 | 0.810163 | 1.234349 |
| 27648 | 0.814584 | 1.227621 |
| 28672 | 0.826260 | 1.210299 |
| 29696 | 0.817326 | 1.223518 |
| 30720 | 0.828193 | 1.207482 |
| 31744 | 0.820948 | 1.218142 |

### Timing for `InnerProd4()`, the 4-loop unrolling

The performance is about twice better than the 2-loop unrolling build:

| Dimension | Time | GFLOP/s |
| :---      |   ---:  |   ---:|
|  1024 | 0.298058 | 3.355053 |
|  2048 | 0.322805 | 3.097850 |
|  3072 | 0.304387 | 3.285293 |
|  4096 | 0.322388 | 3.101857 |
|  5120 | 0.319362 | 3.131251 |
|  6144 | 0.316118 | 3.163387 |
|  7168 | 0.313671 | 3.188055 |
|  8192 | 0.369196 | 2.708604 |
|  9216 | 0.312647 | 3.198497 |
| 10240 | 0.313661 | 3.188180 |
| 11264 | 0.313732 | 3.187455 |
| 12288 | 0.314251 | 3.182201 |
| 13312 | 0.321250 | 3.112874 |
| 14336 | 0.410385 | 2.436755 |
| 15360 | 0.350700 | 2.851476 |
| 16384 | 0.380671 | 2.626977 |
| 17408 | 0.323995 | 3.086475 |
| 18432 | 0.323410 | 3.092080 |
| 19456 | 0.331493 | 3.016712 |
| 20480 | 0.335170 | 2.983614 |
| 21504 | 0.344829 | 2.899990 |
| 22528 | 0.347648 | 2.876524 |
| 23552 | 0.339133 | 2.948748 |
| 24576 | 0.384705 | 2.599452 |
| 25600 | 0.378652 | 2.640981 |
| 26624 | 0.365078 | 2.739207 |
| 27648 | 0.395164 | 2.530596 |
| 28672 | 0.386425 | 2.587881 |
| 29696 | 0.457515 | 2.185749 |
| 30720 | 0.422189 | 2.368674 |
| 31744 | 0.409521 | 2.441953 |

## (b)

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