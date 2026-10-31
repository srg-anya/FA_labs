#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum state {
    OK,
    ERROR_NULL,
    ERROR_INPUT,
    ERROR_MEMORY
} state;

typedef double (*NormFunc)(double *vec, int n, void* param);

double NormInf(double *vec, int n, void *param) {
    double max = fabs(vec[0]);
    for (int i = 1; i < n; i++) {
        if (fabs(vec[i]) > max) {
            max = fabs(vec[i]);
        }
    }
    return max;
}

double NormP(double* vec, int n, void* param) {
    if (param == NULL) {
        return 0.0;
    }
    double p = *(double*)param;
    if (p < 1.0) {
        return 0.0;
    }

    double sum = 0.0;
    for (int i = 0; i < n; i++) {
        sum += pow(fabs(vec[i]), p);
    }
    return pow(sum, 1.0 / p);
}

double NormA(double *vec, int n, void* param) {
    if (param == NULL) {
        return 0.0;
    }
    double* A = (double*)param;
    double sum = 0.0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            sum += A[i * n + j] * vec[i] * vec[j];
        }
    }
    return sqrt(sum);
}

state MaxNorm(int n, NormFunc normFunc, void *param, double **res, int *resCount, int count, ...) {
    if (normFunc == NULL || res == NULL || resCount == NULL) {
        return ERROR_NULL;
    }
    if (n <= 0 || count <= 0) {
        return ERROR_INPUT;
    }
    *resCount = 0;
    double maxNorm = -1.0;

    va_list args;
    va_start(args, count);

    for (int i = 0; i < count; i++) {
        double *vec = va_arg(args, double *);
        if (vec == NULL) {
            va_end(args);
            return ERROR_NULL;
        }

        double cur = normFunc(vec, n, param);
        if (cur > maxNorm) {
            maxNorm = cur;
            res[0] = vec;
            *resCount = 1;
        } else if (fabs(cur - maxNorm) < 1e-9) {
            res[*resCount] = vec;
            (*resCount)++;
        }
    }

    va_end(args);
    return OK;
}

int main() {
    int n = 3;
    double v1[] = {3.0, -4.0, 1.0};
    double v2[] = {1.0, 2.0, 2.0};
    double v3[] = {1.0, 2.0, 1.0};
    double v4[] = {-3.0, 4.0, -1.0};

    double A[] = {2.0, 0.0, 0.0, 0.0, 2.0, 0.0, 0.0, 0.0, 2.0};
    double p = 2.0;

    double* res[4];
    int resCount = 0;
    state st;

    st = MaxNorm(n, NormInf, NULL, res, &resCount, 4, v1, v2, v3, v4);
    if (st == OK && resCount > 0) {
        printf("Максимальная Inf-норма у %d векторов:\n", resCount);
        for (int b = 0; b < resCount; b++) {
            for (int i = 0; i < n; i++) {
                printf("%.2f ", res[b][i]);
            }
            printf("\n");
        }
    }

    st = MaxNorm(n, NormP, &p, res, &resCount, 3, v1, v2, v3);
    if (st == OK && resCount > 0) {
        printf("\nМаксимальная P-норма у %d векторов:\n", resCount);
        for (int b = 0; b < resCount; b++) {
            for (int i = 0; i < n; i++) {
                printf("%.2f ", res[b][i]);
            }
            printf("\n");
        }
    }

    st = MaxNorm(n, NormA, A, res, &resCount, 3, v1, v2, v3);
    if (st == OK && resCount > 0) {
        printf("\nМаксимальная A-норма у %d векторов:\n", resCount);
        for (int b = 0; b < resCount; b++) {
            for (int i = 0; i < n; i++) {
                printf("%.2f ", res[b][i]);
            }
            printf("\n");
        }
    }

    return OK;
}