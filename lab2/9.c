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


long long GCD(long long a, long long b) {
    while (b != 0) {
        long long temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

state Check(const char *str, int base, int* res) {
    if (str == NULL || res == NULL) return ERROR_NULL;
    if (base < 2 || base > 36) return ERROR_RANGE;
    char *dot = strchr(str, '.');
    if (!dot) return ERROR_INPUT;

    *res = 0;
    long long chis = 0;
    long long znam = 1;
    int k = 0;
    
    for (const char* c = str; *c != '\0'; ++c) {
        if (*c >= '0' && *c <= '9') {
            chis = chis * 10 + (*c - '0');
            if (c > dot) {
                znam *= 10;
                k++;
            }
        } else if (*c == '.') {
            continue;
        } else {
            return ERROR_INPUT;
        }
    }
    
    if (chis == 0) {
        *res = 1;
        return OK;
    }

    long long g = GCD(chis, znam);
    chis /= g;
    znam /= g;

    for (int c = 2; c <= base; ++c) {
        if (base % c == 0) {
            while (znam % c == 0) {
                znam /= c;
            }
        }
    }
    if (znam == 1) {
        *res = 1;
    }
    return OK;
}


state Find(int base, int n, int* res, ...) {
    if (res == NULL) return ERROR_NULL;
    if (base < 2 || base > 36 || n < 1 || n > 5) return ERROR_RANGE;

    va_list args;
    va_start(args, res);

    char buf[100];

    for (int i = 0; i < n; ++i) {
        double val = va_arg(args, double);

        if (val <= 0.0 || val >= 1.0) {
            res[i] = -1;
            continue;
        }

        snprintf(buf, sizeof(buf), "%.10f", val);

        int len = strlen(buf);
        while (len > 0 && buf[len - 1] == '0') {
            buf[len - 1] = '\0';
            len--;
        }
        if (len > 0 && buf[len - 1] == '.') {
            buf[len - 1] = '\0';
        }

        state st = Check(buf, base, &res[i]);
        if (st != OK) {
            res[i] = -2;
        }
    }

    va_end(args);
    return OK;
}


int main(int argc, char *argv[]) {
    int base;
    int n;
    
    printf("Введите основание системы счисления (2-36): ");
    if (scanf("%d", &base) != 1 || base < 2 || base > 36) {
        fprintf(stderr, "Некорректное основание\n");
        return ERROR_INPUT;
    }

    printf("Количество дробей (1-5): ");
    if (scanf("%d", &n) != 1 || n < 1 || n > 5) {
        fprintf(stderr, "Некорректное количество\n");
        return ERROR_INPUT;
    }

    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    double vals[5];
    printf("Введите %d дробей [0, 1] каждое с новой строки:\n", n);
    for (int i = 0; i < n; ++i) {
        char temp[100];
        char musor[100];
        
        if (fgets(temp, sizeof(temp), stdin) == NULL) {
            fprintf(stderr, "Ошибка чтения\n");
            return ERROR_INPUT;
        }

        if (sscanf(temp, "%lf %s", &vals[i], musor) != 1) {
            fprintf(stderr, "Некорректная форма записи числа\n");
            return ERROR_INPUT;
        }
    }

    int res[5];
    state st;
    if (n == 1) {
        st = Find(base, 1, res, vals[0]);
    } else if (n == 2) {
        st = Find(base, 2, res, vals[0], vals[1]);
    } else if (n == 3) {
        st = Find(base, 3, res, vals[0], vals[1], vals[2]);
    } else if (n == 4) {
        st = Find(base, 4, res, vals[0], vals[1], vals[2], vals[3]);
    } else {
        st = Find(base, 5, res, vals[0], vals[1], vals[2], vals[3], vals[4]);
    }

    if (st == OK) {
        printf("\n");
        printf("Результаты\n");
        for (int i = 0; i < n; ++i) {
            if (res[i] == -1) {
                printf("Дробь %f вне диапазона (0; 1)\n", vals[i]);
            } else if (res[i] == 1) {
                printf("Дробь %f имеет конечное представление\n", vals[i]);
            } else {
                printf("Дробь %f имеет бесконечное представление\n", vals[i]);
            }
        }
    } else {
        fprintf(stderr, "Ошибка выполнения функции\n");
    }
    return OK;
}