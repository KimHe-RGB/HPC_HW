# HW4

## Q1

```batch
#SBATCH --nodes=1
#SBATCH --cpus-per-task=4
```

pingpong latency: 1.863000e-06 ms
pingpong bandwidth: 7.574610e+05 GB/s

```batch
#SBATCH --nodes=4
#SBATCH --tasks-per-node=1
```

pingpong latency: 1.482000e-06 ms
pingpong bandwidth: 6.412312e+05 GB/s