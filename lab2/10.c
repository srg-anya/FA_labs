#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX 100000000000
#define MIN -1000000000000

typedef enum state {
    OK,
    ERROR_NULL,
    ERROR_RANGE,
    ERROR_OVERFLOW,
    ERROR_INPUT,
    ERROR_MEMORY
} state;

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

state BuildG(double a, double eps, double** g, int n, ...) {
    *g = (double*)malloc(n * sizeof(double));
    if (*g == NULL) return ERROR_MEMORY;

    double* f = (double*)malloc(n * sizeof(double));
    if (f == NULL) {
        free(*g);
        return ERROR_MEMORY;
    }

    va_list param;
    va_start(param, n);
    for (int i = 0; i < n; ++i) {
        f[n - i - 1] = va_arg(param, double);
    }
    va_end(param);

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n - i - 1; ++j) {
            f[j + 1] = f[j] * a + f[j + 1];
        }
        (*g)[i] = f[n - i - 1];
        if (fabs((*g)[i]) < eps) {
            (*g)[i] = 0.0;
        }
    }
    free(f);
    return OK;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Использование: %s <epsilon> <a>\n", argv[0]);
        return -1;
    }

    double eps;
    state st = NumberD(argv[1], &eps);
    if (st != OK || eps <= 0) {
        fprintf(stderr, "Неверная форма записи эпсилон\n");
        return st;
    }

    double a;
    st = NumberD(argv[2], &a);
    if (st != OK) {
        fprintf(stderr, "Неверная форма записи a\n");
        return st;
    }

    int n;
    printf("Введите старшую степень: ");
    if (scanf("%d", &n) != 1 || n < 0) {
        fprintf(stderr, "Некорректная старшая степень\n");
        return ERROR_INPUT;
    }
    ++n;
    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    double* coef = (double*)malloc(n * sizeof(double));
    if (!coef) {
        fprintf(stderr, "Недостаточно памяти для массива путей к файлам.\n");
        return ERROR_MEMORY;
    }
    char temp[20];
    printf("Введите %d коэффициентов:\n", n);
    for (int i = 0; i < n; ++i) {
        if (fgets(temp, sizeof(temp), stdin) == NULL) {
            fprintf(stderr, "Ошибка чтения ввода\n");
            free(coef);
            return ERROR_INPUT;
        }
        temp[strcspn(temp, "\r\n")] = '\0';
        st = NumberD(temp, &coef[i]);
        if (st != OK) {
            fprintf(stderr, "%s не является корректным коэффициентом\n", temp);
            free(coef);
            return st;
        }
    }

    double* g = NULL;
    if (n == 1) {
        st = BuildG(a, eps, &g, 1, coef[0]);
    } else if (n == 2) {
        st = BuildG(a, eps, &g, 2, coef[0], coef[1]);
    } else if (n == 3) {
        st = BuildG(a, eps, &g, 3, coef[0], coef[1], coef[2]);
    } else if (n == 4) {
        st = BuildG(a, eps, &g, 4, coef[0], coef[1], coef[2], coef[3]);
    } else if (n == 5) {
        st = BuildG(a, eps, &g, 5, coef[0], coef[1], coef[2], coef[3], coef[4]);
    } else if (n == 6) {
        st = BuildG(a, eps, &g, 6, coef[0], coef[1], coef[2], coef[3], coef[4], coef[5]);
    } else if (n == 7) {
        st = BuildG(a, eps, &g, 7, coef[0], coef[1], coef[2], coef[3], coef[4], coef[5], coef[6]);
    } else if (n == 8) {
        st = BuildG(a, eps, &g, 8, coef[0], coef[1], coef[2], coef[3], coef[4], coef[5], coef[6], coef[7]);
    } else {
        fprintf(stderr, "Не работаю с такой степенью\n");
        free(coef);
        return ERROR_RANGE;
    }

    if (st != OK) {
        fprintf(stderr, "Ошибка при выдлении памяти при подсчете коэффициентов\n");
        free(coef);
        return st;
    }

    if (g != NULL) {
        printf("Новые коэффициенты f:\n");
        for (int i = 0; i < n; ++i) {
            printf("g%d = %.4f\n", i, g[i]);
        }
        free(g);
    }

    free(coef);
    return OK;
}