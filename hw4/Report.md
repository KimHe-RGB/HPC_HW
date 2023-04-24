# HW4

## Q1

At first I wrote the sbatch file as:

```batch
#SBATCH --nodes=1
#SBATCH --cpus-per-task=4
```

pingpong latency: 1.863000e-06 ms
pingpong bandwidth: 7.574610e+05 GB/s

I tried another setting that is posted on Edstem, **For the rest of the problems I used this second setting**

```batch
#SBATCH --nodes=4
#SBATCH --tasks-per-node=1
```

pingpong latency: 1.482000e-06 ms
pingpong bandwidth: 6.412312e+05 GB/s

## Q2

Running the code with 4 processes from 0 to 4:

```batch
mpiexec ./int_ring 0 3
```

We expect the int / int list have value 0+1+2+3=6

```text
The Result received by proc 0 is 6
int ring latency: 6.405122e-02 ms
The Result index 0 and 1 received by proc 0 is 6 and 6
ints ring bandwidth: 5.283442e-01 GB/s
```

# Q3 2D-Jacobi

Running the code with 4 processes from 0 to 4, on a 16 x 16 2D grid with max_iter 100, with initial data:

0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
0 3 3 3 3 3 3 3 3 3 3 3 3 3 3 0
        ...
0 3 3 3 3 3 3 3 3 3 3 3 3 3 3 0
0 3 3 3 3 3 3 3 3 3 3 3 3 3 3 0
0 3 3 3 3 3 3 3 3 3 3 3 3 3 3 0
0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0

```batch
mpiexec ./jacobi2D-mpi 16 100
```

Outcome:

Iter 0: Residual: 3003.35
Iter 10: Residual: 221.622
Iter 20: Residual: 143.149
Iter 30: Residual: 109.397
Iter 40: Residual: 86.9342
Iter 50: Residual: 69.3281
Iter 60: Residual: 55.0114
Iter 70: Residual: 43.3118
Iter 80: Residual: 33.7805
Iter 90: Residual: 26.074
Time elapsed is 0.003367 seconds.
end! The Residue is 26.07 at iteration 100
