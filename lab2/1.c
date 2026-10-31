#include <stdio.h>
#include <stdlib.h>
#define MAX 1000000

typedef enum state {
    OK,
    ERROR_NULL,
    ERROR_RANGE,
    ERROR_OVERFLOW,
    ERROR_INPUT
} state;

state Flag(const char* f, char* res) {
    if (f == NULL || res == NULL) return ERROR_INPUT;
    if (f[2] != '\0' && f[0] != '-') return ERROR_INPUT;
    if (f[1] != 'l' && f[1] != 'r' && f[1] != 'u' && f[1] != 'n' && f[1] != 'c') return ERROR_INPUT;
    *res = f[1];
    return OK;
}

state Number(const char *n, unsigned int *res) {
    if (n == NULL || res == NULL) return ERROR_NULL;
    int sign = 1;
    int i = 0;
    if (n[0] == '-') {
        i = 1;
        sign = -1;
    } else if (n[0] == '+') {
        i = 1;
    }
    if (n[i] == '\0') return ERROR_INPUT;
    int x = 0;
    while (n[i] != '\0') {
        if (n[i] < '0' || n[i] > '9') return ERROR_INPUT;
        if(x > (MAX - (n[i] - '0')) / 10) return ERROR_OVERFLOW;
        x = x * 10 + (n[i] - '0');
        ++i;
    }
    *res = x * sign;
    return OK;
}

state L(const char* str, int* res) {
    if (str == NULL || res == NULL) return ERROR_NULL;
    int i = 0;
    while (str[i] != '\0') {
        i++;
        if (i == MAX) return ERROR_OVERFLOW;
    }
    *res = i;
    return OK;
}

state R(const char* str, char* res) {
    if (str == NULL || res == NULL) return ERROR_NULL;
    int size = 0;
    state st = L(str, &size);
    if (st != OK) return st;
    int i = 0;
    for (int i = 0; i < size; i++) {
        res[i] = str[size - 1 - i];
    }
    res[size] = '\0';
    return OK;
}

int IsLetter(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

int IsDigit(char c) {
    return (c >= '0' && c <= '9');
}

state N(const char* str, char* res) {
    if (str == NULL || res == NULL) return ERROR_NULL;
    int len = 0;
    state st = L(str, &len);
    if (st != OK) return st;

    char* digit = (char*)malloc(len + 1);
    char* letter = (char*)malloc(len + 1);
    char* other = (char*)malloc(len + 1);
    if (digit == NULL || letter == NULL || other == NULL) return ERROR_OVERFLOW;
    int i = 0, j = 0, k = 0;
    for (int n = 0; str[n] != '\0'; n++) {
        if (IsLetter(str[n])) {
            letter[i++] = str[n];
        } else if (IsDigit(str[n])) {
            digit[j++] = str[n];
        } else {
            other[k++] = str[n];
        }
    }
    int m = 0;
    for (int n = 0; n < j; n++) res[m++] = digit[n];
    for (int n = 0; n < i; n++) res[m++] = letter[n];
    for (int n = 0; n < k; n++) res[m++] = other[n];
    res[m] = '\0';
    
    free(digit);
    free(letter);
    free(other);
    return OK;
}


char ToUpper(char c) {
    if (IsLetter(c)) return c - 32;
}

state U(const char* str, char* res) {
    if (str == NULL || res == NULL) return ERROR_NULL;
    int i = 0;
    while (str[i] != '\0') {
        if (i & 1) {
            res[i] = ToUpper(str[i]);
        } else {
            res[i] = str[i];
        }
        ++i;
        if (i == MAX) return ERROR_OVERFLOW;
    }
    res[i] = '\0';
    return OK;
}


state C(int argc, char *argv[], int start, unsigned int seed, char *res) {
    if (res == NULL) return ERROR_NULL;
    if (start >= argc) return ERROR_INPUT;

    srand(seed);
    int k = 0;
    int count = argc - start;
    int* used = (int*)calloc(count, sizeof(int));
    for (int i = 0; i < count; ++i) {
        int ind = rand() % count;
        while (used[ind]) {
            ind = rand() % count;
        }
        used[ind] = 1;

        char* str = argv[start + ind];
        int len = 0;
        state st = L(str, &len);
        if (st != OK) {
            free(used);
            return st;
        }
        for (int j = 0; j < len; j++) {
            res[k++] = str[j];
            if (k == MAX) {
                free(used);
                return ERROR_OVERFLOW;
            }
        }
    }
    res[k] = '\0';
    free(used);
    return OK;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Недостаточно аргументов строки: <исполняемый файл> <флаг> <строка> ... \n");
        return ERROR_INPUT;
    }
    char flag = '\0';
    state st = Flag(argv[1], &flag);
    if (st != OK) {
        if (st == ERROR_INPUT) {
            fprintf(stderr, "Некорректный флаг %s\n", argv[1]);
            return st;
        }
    }

    if (argc > 3 && flag != 'c') {
        fprintf(stderr, "Избыток аргументов для флага -%c: <исполняемый файл> <флаг> <строка>\n", flag);
        return ERROR_INPUT;
    }
    
    char* res = (char*)malloc(MAX);
    if (flag == 'l') {
        int len = 0;
        st = L(argv[2], &len);
        if (st != OK) {
            if (st == ERROR_NULL) {
                fprintf(stderr, "NULL-указатель\n");
            } else if (st == ERROR_OVERFLOW) {
                fprintf(stderr, "Строка слишком длинная\n");
            }
            free(res);
            return st;
        } else {
            printf("Длина строки = %d\n", len);
        }
    } else if (flag == 'r') {
        st = R(argv[2], res);
        if (st != OK) {
            if (st == ERROR_NULL) {
                fprintf(stderr, "NULL-указатель\n");
            } else if (st == ERROR_OVERFLOW) {
                fprintf(stderr, "Строка слишком длинная\n");
            }
            free(res);
            return st;
        } else {
            printf("Reverse-строка: %s\n", res);
        }

    } else if (flag == 'u') {
        st = U(argv[2], res);
        if (st != OK) {
            if (st == ERROR_NULL) {
                fprintf(stderr, "NULL-указатель\n");
            } else if (st == ERROR_OVERFLOW) {
                fprintf(stderr, "Строка слишком длинная\n");
            }
            free(res);
            return st;
        } else {
            printf("Результат: %s\n", res);
        }

    } else if (flag == 'n') {
        st = N(argv[2], res);
        if (st != OK) {
            if (st == ERROR_NULL) {
                fprintf(stderr, "NULL-указатель\n");
            } else if (st == ERROR_OVERFLOW) {
                fprintf(stderr, "Строка слишком длинная\n");
            } 
            free(res);
            return st;
        } else {
            printf("Отсортированная строка: %s\n", res);
        }

    } else if (flag == 'c') {
        if (argc < 5) {
            fprintf(stderr, "Для флага -c нужно: <флаг> <число> <строки...>\n");
            free(res);
            return ERROR_INPUT;
        }
        unsigned int seed = 0;
        st = Number(argv[2], &seed);
        if (st != OK) {
            if (st == ERROR_NULL) {
                fprintf(stderr, "NULL-указатель\n");
            } else if (st == ERROR_OVERFLOW) {
                fprintf(stderr, "Число для <-c> слишком большое\n");
            } else if (st == ERROR_INPUT) {
                fprintf(stderr, "Некорректное число %s\n", argv[2]);
            }
            free(res);
            return st;
        }

        st = C(argc, argv, 3, seed, res);
        if (st != OK) {
            if (st == ERROR_NULL) {
                fprintf(stderr, "NULL-указатель\n");
            } else if (st == ERROR_INPUT) {
                fprintf(stderr, "Некорректный ввод\n");
            } else if (st == ERROR_OVERFLOW) {
                fprintf(stderr, "Результат слишком длинный\n");
            }
            free(res);
            return st;
        } else {
            printf("Результат конкатенации: %s\n", res);
        }
    }

    free(res);
    return OK;
}