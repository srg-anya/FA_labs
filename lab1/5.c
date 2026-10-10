#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#define MAX 100000000000
#define MIN -1000000000000

typedef enum state {
    OK,
    ERROR_NULL,
    ERROR_RANGE,
    ERROR_OVERFLOW,
    ERROR_INPUT
} state;

state NumberD(char *n, double *res) {
    if (n == NULL || res == NULL) return ERROR_NULL;
    long long sign = 1;
    long long i = 0;
    if (n[0] == '-') {
        i = 1;
        sign = -1;
    } else if (n[0] == '+') {
        i = 1;
    }
    if (n[i] == '\0') {
        return ERROR_INPUT;
    }
    double x = 0.0;
    double f = 0.0;
    double c = 1.0;
    int digit = 0;
    int t = 0;
    while (n[i] != '\0') {
        if (n[i] >= '0' && n[i] <= '9') {
            if (!t) {
                x = x * 10.0 + (n[i] - '0');
            } else {
                c *= 0.1;
                f = f + (n[i] - '0') * c;
            }
            
            digit = 1;
        } else if (n[i] == '.') {
            if (t) {
                return ERROR_INPUT;
            }
            t = 1;
        } else {
            return ERROR_INPUT;
        }
        ++i;
    }
    if (!digit) return ERROR_INPUT;
    *res = (x + f) * sign;
    return OK;
}

state A1(double eps, double x, double *res) {
    if (res == NULL) return ERROR_NULL;
    double sum = 1.0;
    double cur = 1.0;
    for (long long n = 1;; ++n) {
        cur = cur * x / n;
        sum += cur;
        if (fabs(cur) < eps) {
            break;
        }
    }
    *res = sum;
    return OK;
}

state B1(double eps, double x, double *res) {
    if (res == NULL) return ERROR_NULL;
    double sum = 1.0;
    double cur = 1.0;
    for (long long n = 1;; ++n) {
        cur = cur * (-1.0) * x * x / ((2 * n - 1) * (2 * n));
        sum += cur;
        if (fabs(cur) < eps) {
            break;
        }
    }
    *res = sum;
    return OK;
}

state C1(double eps, double x, double *res) {
    if (res == NULL) return ERROR_NULL;
    if (fabs(x) >= 1.0) {
        return ERROR_RANGE;
    }
    double sum = 1.0;
    double cur = 1.0;
    for (long long n = 1;; ++n) {
        cur = cur * 9.0 * n * n * x * x / ((3 * n - 2) * (3 * n - 1));
        sum += cur;
        if (fabs(cur) < eps) {
            break;
        }
    }
    *res = sum;
    return OK;
}

state D1(double eps, double x, double *res) {
    if (res == NULL) return ERROR_NULL;
    if (fabs(x) >= 1.0) return ERROR_RANGE;
    double sum = 0.0;
    double cur = 1.0;
    for (long long n = 1;; ++n) {
        cur = cur * (-1.0) * x * x * (2 * n - 1) / (2 * n);
        sum += cur;
        if (fabs(cur) < eps) {
            break;
        }
    }
    *res = sum;
    return OK;
}

double A2(double eps) {
    double sum = 0.0;
    double cur = 1.0;
    for (long long n = 1;; ++n) {
        sum += cur;
        if (fabs(cur) < eps) {
            break;
        }
        cur = cur * (-1.0) * ((double)n * n) / ((double)(n + 1) * (n + 1));
    }
    return sum;
}

double B2( double eps) {
    double sum = 0.0;
    double cur = 1.0;
    for (long long n = 1;; ++n) {
        sum += cur;
        if (fabs(cur) < eps) {
            break;
        }
        cur = cur * (-1.0) * (2.0 * n + 1.0) / (2.0 * (n + 1.0) * (2.0 * n + 3.0));
    }
    return sum;
}

double C2(double eps) {
    double sum = 0.0;
    double cur = 0.5;
    for (long long n = 1;; ++n) {
        sum += cur;
        if (fabs(cur) < eps) {
            break;
        }
        cur = cur * (double)n / (double)(n + 2);
    }
    return sum;
}

double D2(double eps) {
    double sum = 0.0;
    double cur = -1.0;
    for (long long n = 1;; ++n) {
        cur *= (-1.0) / pow((double)(n), (double)(n));
        sum += cur;
        if (fabs(cur) < eps) {
            break;
        }
    }
    return sum;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Аргументов строки должно быть 2\n");
        return -1;
    }
    double eps;
    state st = NumberD(argv[1], &eps);
    if (st != OK) {
        fprintf(stderr, "Неверная форма записи эпсилон\n");
        return st;
    }

    double x;
    st = NumberD(argv[2], &x);
    if (st != OK) {
        fprintf(stderr, "Неверная форма записи x\n");
        return st;
    }

    double a1 = 0.0;
    double b1 = 0.0;
    double c1 = 0.0;
    double d1 = 0.0;
    printf("1.\n");
    st = A1(eps, x, &a1);
    if (st == OK) { 
        printf("a1 = %.10f\n", a1); 
    } else {
        fprintf(stderr, "NULL-указатель");
    }
    st = B1(eps, x, &b1);
    if (st == OK) {
        printf("b1 = %.10f\n", b1);
    } else {
        fprintf(stderr, "NULL-указатель\n");
    }

    st = C1(eps, x, &c1);
    if (st == OK) {
        printf("c1 = %.10f\n", c1);
    } else if (st == ERROR_RANGE) {
        fprintf(stderr, "c1. x вне диапозона |x| >= 1\n");
    } else {
        fprintf(stderr, "c1. NULL-указатель\n");
    }

    st = D1(eps, x, &d1);
    if (st == OK) {
        printf("d1 = %.10f\n", d1);
    } else if (st == ERROR_RANGE) {
        fprintf(stderr, "d1. x вне диапозона |x| >= 1\n");
    } else {
        fprintf(stderr, "d1. NULL-указатель\n");
    }

    printf("2.\n");
    double a2 = A2(eps);
    printf("a2 = %.10f\n", a2);
    double b2 = B2(eps);
    printf("b2 = %.10f\n", b2);
    double c2 = C2(eps);
    printf("c2 = %.10f\n", c2);
    double d2 = D2(eps);
    printf("d2 = %.10f\n", d2);
}