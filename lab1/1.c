#include <stdio.h>
#include <stdlib.h>
#define MAX 1000000000000
#define MIN -1000000000000

typedef enum state {
    OK,
    ERROR_NULL,
    ERROR_RANGE,
    ERROR_OVERFLOW,
    ERROR_INPUT
} state;

state Flag(const char* f, char* res) {
    if (f == NULL || res == NULL) return ERROR_INPUT;
    if (f[2] != '\0' || f[0] != '/' && f[0] != '-') return ERROR_INPUT;
    if (f[1] != 'h' && f[1] != 'p' && f[1] != 's' && f[1] != 'e' && f[1] != 'a' && f[1] != 'f') return ERROR_INPUT;
    *res = f[1];
    return OK;
}

state Number(const char* n, long long* res) {
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

state H(long long x, long long* have, long long** arr) {
    if (have == NULL || arr == NULL) return ERROR_NULL;
    if (x <= 0) return ERROR_RANGE;
    long long *a = malloc(100 * sizeof(long long));
    if (a == NULL) return ERROR_NULL;
    *arr = a;
    *have = 0;
    if (x <= 100) {
        for (long long i = 1; i <= 100; ++i) {
            if (i % x == 0) {
                (*arr)[*have] = i;
                *have += 1;
            }
        }
    }
    return OK;
}

state P(long long x, long long *prime) {
    if (prime == NULL) return ERROR_NULL;
    if (x == 1 || x <= 0) {
        *prime = 0;
        return OK;
    }
    for (long long i = 2; i * i <= x; ++i) {
        if (x % i == 0) {
            *prime = 1;
            return OK;
        }
    }
    *prime = 2;
    return OK;
}

state S(long long x, char* r, long long* i) {
    if (r == NULL || i == NULL) return ERROR_NULL;
    if (x == MIN) return ERROR_RANGE;
    *i = 0;
    if (x < 0) {
        x *= -1;
    }
    if (x == 0) {
        r[*i] = '0';
        *i = *i + 1;
        r[*i] = '\0';
        return OK;
    }
    long long c = 0;
    while (x > 0) {
        c = x % 16;
        x = x / 16;
        if (0 <= c && c <= 9) {
            r[*i] = '0' + c;
        } else if (c >= 10 && c <= 15) {
            r[*i] = 'A' + (c - 10);
        }
        *i = *i + 1;
    }
    r[*i] = '\0';
    return OK;
}

void PrintTable(long long i, long long k, long long res) {
    printf("   %lld      |    %lld       |     %lld   \n", i, k, res);
}

state E(long long x) {
    for (long long i = 1; i <= 10; ++i) {
        long long res = 1;
        for (long long k = 1; k <= x; ++k) {
            res *= i;
            PrintTable(i, k, res);
        }
    }
    return OK;
}

state A(long long x, long long* res) {
    if (res == NULL) return ERROR_NULL;
    if (x < 0) return ERROR_RANGE;
    if (x == 0 || x == 1) {
        *res = x;
        return OK;
    } 
    *res = x * (x - 1) / 2 + x;
    return OK;
}

state F(long long x, long long* res) {
    if (res == NULL) return ERROR_NULL;
    if (x < 0) return ERROR_RANGE;
    if (x > 20) return ERROR_OVERFLOW;
    if (x == 1 || x == 0) {
        *res = 1;
        return OK;
    }
    long long prev = 0;
    state st = F(x - 1, &prev);
    if (st != OK) return st;
    *res = prev * x;
    return OK;
}





int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Аргументов строки должно быть 3\n");
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

    long long number = 0;
    st = Number(argv[2], &number);
    if (st != OK) {
        if (st == ERROR_NULL) {
            fprintf(stderr, "NULL-указатель\n");
        } else if (st == ERROR_INPUT) {
            fprintf(stderr, "Некорректное число\n");
        } else if (st == ERROR_OVERFLOW) {
            fprintf(stderr, "Проблемы с переполнением\n");
        }
        return st;
    }
    
    long long res = 0;
    if (flag == 'h') {
        long long have = 0;
        long long* arr;
        st = H(number, &have, &arr);
        if (st != OK) {
            if (st == ERROR_NULL) {
                fprintf(stderr, "NULL-указатель\n");
            } else if (st == ERROR_RANGE) {
                fprintf(stderr, "Число должно быть натуральным\n");
            }
            return st;
        }
        if (have == 0) {
            printf("Нет кратных чисел\n");
        } else {
            for (long long i = 0; i < have; ++i) {
                printf("%lld", arr[i]);
                printf(" ");
            }
            printf("\n");
        }
        free(arr);
    } else if (flag == 'p') {
        long long prime = 0;
        st = P(number, &prime);
        if (st != OK) {
            if (st == ERROR_NULL) {
                fprintf(stderr, "NULL-указатель\n");
            }
            return st;
        }
        if (prime == 0) {
            printf("Не простое, не составное\n");
        } else if (prime == 2) {
            printf("Простое\n");
        } else if (prime == 1) {
            printf("Составное\n");
        }
        return OK;
    } else if (flag == 's') {
        char r[32];
        long long len = 0;
        st = S(number, r, &len);
        if (st == OK) {
            if (number < 0) printf("- ");
            for (long long j = len - 1; j >= 0; --j) {
                printf("%c", r[j]);
                printf(" ");
            }
            printf("\n");
            return OK;
        } else if (st == ERROR_NULL) {
            fprintf(stderr, "NULL-указатель\n");
        } else if (st == ERROR_RANGE) {
            fprintf(stderr, "Число вне допустимого диапазона\n");
        }
    } else if (flag == 'e') {
        if (number >= 1 && number <= 10) {
            printf("Основание | Показатель | Результат\n");
            st = E(number);
        } else {
            fprintf(stderr, "Число должно быть 1<=x<=10\n");
            return ERROR_RANGE;
        }
    } else if (flag == 'a') {
        st = A(number, &res);
        if (st != OK) {
            if (st == ERROR_NULL) {
                fprintf(stderr, "NULL-указатель\n");
            } else if (st == ERROR_RANGE) {
                fprintf(stderr, "Значение вне допустимого диапазона\n");
            }
        } else {
            printf("%lld\n", res);
        }
    } else if (flag == 'f') {
        st = F(number, &res);
        if (st != OK) {
            if (st == ERROR_OVERFLOW) {
                fprintf(stderr, "Проблема с переполнением\n");
            } else if (st == ERROR_NULL) {
                fprintf(stderr, "NULL-указатель\n");
            } else if (st == ERROR_RANGE) {
                fprintf(stderr, "Значение вне допустимого диапазона\n");
            }
        } else {
            printf("%lld\n", res);
        }
    }
    return st;
}