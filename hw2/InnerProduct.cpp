#include <stdio.h>
#include <math.h>
#include "utils.h"


// not unrolling construction
void InnerProd0(long n, double *a, double *b, double *sum) {
    double s = 0;
    for (long i = 0; i < n; i++) {
        s += a[i] * b[i];
    }
    sum[0] = s;
}

// 2 loop unrolling construction
void InnerProd2(long n, double *a, double *b, double *sum) {
    double sum1 = 0; double sum2 = 0;
    double temp1 = 0; double temp2 = 0;
    for (long i = 0; i < n/2+1; i++) {
        sum1 += temp1;
        temp1 = *(a + 0) * *(b + 0);
        sum2 += temp2;
        temp2 = *(a + 1) * *(b + 1);
        a += 2; b += 2;
    }
    sum[0] = sum1 + sum2;
}

// another 2 loop unrolling construction
void InnerProd2_0(long n, double *a, double *b, double *sum) {
    double sum1 = 0; double sum2 = 0;
    double temp1 = 0; double temp2 = 0;
    for (long i = 0; i < n/2+1; i++) {
        sum1 += a[2*i] * b[2*i];
        sum2 += a[2*i+1] * b[2*i+1];
    }
    sum[0] = sum1 + sum2;
}


// 4 loop unrolling construction
void InnerProd4(long n, double *a, double *b, double *sum) {
    double sum1 = 0; double sum2 = 0; double sum3 = 0; double sum4 = 0;
    double temp1 = 0; double temp2 = 0; double temp3 = 0; double temp4 = 0;
    for (long i = 0; i < n/4+3; i++) {
        sum1 += temp1;
        temp1 = *(a + 0) * *(b + 0);
        sum2 += temp2;
        temp2 = *(a + 1) * *(b + 1);
        sum3 += temp3;
        temp3 = *(a + 2) * *(b + 2);
        sum4 += temp4;
        temp4 = *(a + 3) * *(b + 3);
        a += 4; b += 4;
    }
    sum[0] = sum1 + sum2 + sum3 + sum4;
}

int main(int argc, char** argv) {
    printf(" Dimension       Time       Gflop/s     Error\n");
    for (long p = 1e6; p < 1e7; p += 1e6) {
        long NREPEATS = 1e9 / p + 1;
        // use vectors with sizes that do not fit into cache
        double* a = (double*) aligned_malloc(p * sizeof(double));
        double* b = (double*) aligned_malloc(p * sizeof(double));
        double* s = (double*) aligned_malloc(sizeof(double));
        // double* s_ref = (double*) aligned_malloc(sizeof(double));

        for (long i = 0; i < p; i++) a[i] = p + 0.0;
        for (long i = 0; i < p; i++) b[i] = p + 0.0;

        // Error is tested to be 0
        // for (long rep = 0; rep < NREPEATS; rep++) {
        //     InnerProd0(p, a, b, s_ref); // use as true solution
        // }

        Timer t;
        t.tic();
        for (long rep = 0; rep < NREPEATS; rep++) {
            InnerProd0(p, a, b, s);
            // InnerProd2_0(p, a, b, s);
            // InnerProd2(p, a, b, s);
            // InnerProd4(p, a, b, s);
        }
        double time = t.toc();
        double flops_rate = p * NREPEATS * 1e-9 / time;
        printf("%10ld   %10f    %10f    \n", p, time, flops_rate);

        // printf("%10e \n", s[0]-s_ref[0]);

        aligned_free(a);
        aligned_free(b);
        aligned_free(s);
        // aligned_free(s_ref);
    }
    return 0;
}