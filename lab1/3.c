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
    ERROR_INPUT,
    INF,
    NO,
    ONE,
    TWO
} state;

state Flag(const char* f, char* res) {
    if (f == NULL || res == NULL) return ERROR_INPUT;
    if (f[2] != '\0' || (f[0] != '/' && f[0] != '-')) return ERROR_INPUT;
    if (f[1] != 'q' && f[1] != 'm' && f[1] != 't') return ERROR_INPUT;
    *res = f[1];
    return OK;
}

state NumberLL(const char* n, long long* res) {
    if (n == NULL || res == NULL) return ERROR_NULL;
    long long sign = 1;
    long long i = 0;
    if (n[0] == '-') {
        i = 1;
        sign = -1;
    } else if (n[0] == '+') {
        i = 1;
    }
    if (n[i] == '\0') return ERROR_INPUT;
    long long x = 0;
    while (n[i] != '\0') {
        if (n[i] < '0' || n[i] > '9') return ERROR_INPUT;
        if(x > (MAX - (n[i] - '0')) / 10) return ERROR_OVERFLOW;
        x = x * 10 + (n[i] - '0');
        ++i;
    }
    *res = x * sign;
    return OK;
}

state NumberD(const char *n, double *res) {
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

state Equal(double eps, double a, double b, double c, double* x1, double* x2) {
    if (x1 == NULL || x2 == NULL) return ERROR_NULL;
    if (fabs(a) < eps) {
        if (fabs(b) < eps) {
            if (fabs(c) < eps) {
                return INF;
            } else {
                return NO;
            }
        }
        *x1 = -c / b;
        return ONE;
    }

    double D = b * b - 4.0 * a * c;
    if (fabs(D) < eps) {
        *x1 = -b / (2.0 * a);
        return ONE;
    } else if (D > eps) {
        *x1 = (-b - sqrt(D)) / (2.0 * a);
        *x2 = (-b + sqrt(D)) / (2.0 * a);
        return TWO;
    } else {
        return NO;
    }
}

void Print(state s, double a, double b, double c, double *x1, double *x2) {
    printf("При a = %g, b = %g, c = %g: ", a, b, c);

    if (s == NO) {
        printf("Корней нет\n");
    } else if (s == INF) {
        printf("Бесконечно много решений\n");
    } else if (s == ONE) {
        printf("x = %g\n", *x1);
    } else if (s == TWO) {
        printf("x1 = %g, x2 = %g\n", *x1, *x2);
    } else {
        printf("ошибка\n");
    }
}

state Q(int argc, char *argv[]) {
    if (argv == NULL) return ERROR_NULL;
    if (argc != 6) {
        fprintf(stderr, "Аргументов строки должно быть 6\n");
        return ERROR_INPUT;
    }
    double a;
    double b;
    double c;
    double eps;

    state st = NumberD(argv[2], &eps);
    if (st != OK) return st;
    st = NumberD(argv[3], &a);
    if (st != OK) return st;
    st = NumberD(argv[4], &b);
    if (st != OK) return st;
    st = NumberD(argv[5], &c);
    if (st != OK) return st;
    
    double x1 = 0.0;
    double x2 = 0.0;
    st = Equal(eps, a, b, c, &x1, &x2);
    Print(st, a, b, c, &x1, &x2);

    if (fabs(b - c) >= eps) {
        st = Equal(eps, a, c, b, &x1, &x2);
        Print(st, a, c, b, &x1, &x2);
    }

    if (fabs(a - b) >= eps) {
        st = Equal(eps, b, a, c, &x1, &x2);
        Print(st, b, a, c, &x1, &x2);
    }

    if (fabs(a - b) >= eps && fabs(a - c) >= eps) {
        st = Equal(eps, b, c, a, &x1, &x2);
        Print(st, b, c, a, &x1, &x2);
    }

    if (fabs(a - c) >= eps && fabs(b - c) >= eps) {
        st = Equal(eps, c, a, b, &x1, &x2);
        Print(st, c, a, b, &x1, &x2);
    }

    if (fabs(a - b) >= eps && fabs(a - c) >= eps && fabs(b - c) >= eps) {
        st = Equal(eps, c, b, a, &x1, &x2);
        Print(st, c, b, a, &x1, &x2);
    }
    return OK;
}

state M(int argc, char *argv[], long long* res) {
    if (argc != 4) {
        fprintf(stderr, "Аргументов строки должно быть 4\n");
        return ERROR_INPUT;
    }
    long long x1;
    long long x2;
    *res = 0;
    state st = NumberLL(argv[2], &x1);
    if (st != OK) return st;
    st = NumberLL(argv[3], &x2);
    if (st != OK) return st;
    if (x2 == 0) return ERROR_RANGE;
    if (x1 % x2 == 0) *res = 1;
    return OK;
}

state T(int argc, char *argv[], long long *res) {
    if (argc != 6) {
        fprintf(stderr, "Аргументов строки должно быть 6\n");
        return ERROR_INPUT;
    }
    double a;
    double b;
    double c;
    double eps;
    *res = 0;
    state st = NumberD(argv[2], &eps);
    if (st != OK) return st;
    st = NumberD(argv[3], &a);
    if (st != OK) return st;
    st = NumberD(argv[4], &b);
    if (st != OK) return st;
    st = NumberD(argv[5], &c);
    if (st != OK) return st;
    if (a <= 0 || b <= 0 || c <= 0) return OK;
    if (fabs(a * a + b * b - c * c) < eps) *res = 1;
    if (fabs(c * c + b * b - a * a) < eps) *res = 1;
    if (fabs(a * a + c * c - b * b) < eps) *res = 1;
    return OK;
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Аргументов строки должно быть минимум 4\n");
        return ERROR_INPUT;
    }
    char flag = '\0';
    state st = Flag(argv[1], &flag);
    if (st != OK) {
        if (st == ERROR_INPUT) {
            fprintf(stderr, "Некорректный флаг\n");
            return st;
        }
    }

    long long res = 0;
    if (flag == 'q') {
        st = Q(argc, argv);
        if (st != OK) {
            if (st == ERROR_NULL) {
                fprintf(stderr, "NULL-указатель\n");
            } else if (st == ERROR_INPUT) {
                fprintf(stderr, "Ошибка ввода\n");
            } else if (st == ERROR_OVERFLOW) {
                fprintf(stderr, "Переполнение\n");
            } else if (st == ERROR_RANGE) {
                fprintf(stderr, "Число вне допустимого диапазона\n");
            }
            return st;
        }
    } else if (flag == 'm') {
        st = M(argc, argv, &res);
        if (st != OK) {
            if (st == ERROR_NULL) {
                fprintf(stderr, "NULL-указатель\n");
            } else if (st == ERROR_RANGE) {
                fprintf(stderr, "Значение вне допустимого диапозона\n");
            } else if (st == ERROR_OVERFLOW) {
                fprintf(stderr, "Переполнение\n");
            } else if (st == ERROR_INPUT) {
                fprintf(stderr, "Ошибка ввода\n");
            }
            return st;
        }
        if (res) {
            printf("Кратно\n");
        } else {
            printf("Не кратно\n");
        }
    } else if (flag == 't') {
        st = T(argc, argv, &res);
        if (st != OK) {
            if (st == ERROR_NULL) {
                fprintf(stderr, "NULL-указатель\n");
            } else if (st == ERROR_INPUT) {
                fprintf(stderr, "Ошибка ввода\n");
            } else if (st == ERROR_OVERFLOW) {
                fprintf(stderr, "Переполнение\n");
            } else if (st == ERROR_RANGE) {
                fprintf(stderr, "Число вне допустимого диапазона\n");
            }
            return st;
        }
        if (res) {
            printf("Могут быть сторонами прямоугольного треугольника\n");
        } else {
            printf("Не могут быть сторонами прямоугольного рреугольника\n");
        }
    } 
    return st;
}