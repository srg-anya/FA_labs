#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum state {
    OK,
    ERROR_NULL,
    ERROR_INPUT,
    ERROR_MEMORY
} state;

int CharToDigit(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    if (c >= 'a' && c <= 'z') {
        return c - 'a' + 10;
    }
    if (c >= 'A' && c <= 'Z') {
        return c - 'A' + 10;
    }
    return -1;
}

char DigitToChar(int d) {
    if (d >= 0 && d <= 9) {
        return (char)('0' + d);
    }
    if (d >= 10 && d <= 35) {
        return (char)('a' + d - 10);
    }
    return '?';
}

state Sum2Num(const char* a, const char* b, int base, char** res) {
    if (a == NULL || b == NULL || res == NULL) {
        return ERROR_NULL;
    }
    if (base < 2 || base > 36) {
        return ERROR_INPUT;
    }

    int lenA = strlen(a);
    int lenB = strlen(b);
    int max = lenA;
    if (lenB > lenA) {
        max = lenB;
    }

    char* buf = (char*)malloc(max + 2);
    if (buf == NULL) {
        return ERROR_MEMORY;
    }

    int carry = 0;
    int ind = max;
    buf[ind + 1] = '\0';
    int i = lenA - 1;
    int j = lenB - 1;

    while (i >= 0 || j >= 0 || carry > 0) {
        int A = 0;
        int B = 0;

        if (i >= 0) {
            A = CharToDigit(a[i]);
            if (A < 0 || A >= base) {
                free(buf);
                return ERROR_INPUT;
            }
        }
        if (j >= 0) {
            B = CharToDigit(b[j]);
            if (B < 0 || B >= base) {
                free(buf);
                return ERROR_INPUT;
            }
        }

        int total = A + B + carry;
        carry = total / base;
        int cur = total % base;

        buf[ind] = DigitToChar(cur);
        --ind;
        --i;
        --j;
    }

    int start = ind + 1;
    while (start < max && buf[start] == '0') {
        ++start;
    }

    int len = max - start + 1;
    char* sum = (char*)malloc(len + 1);
    if (sum == NULL) {
        free(buf);
        return ERROR_MEMORY;
    }
    for (int k = 0; k < len; k++) {
        sum[k] = buf[start + k];
    }
    sum[len] = '\0';

    free(buf);
    *res = sum;
    return OK;
}

state SumInBase(int base, int count, char** res, ...) {
    if (res == NULL) {
        return ERROR_NULL;
    }
    if (base < 2 || base > 36 || count <= 0) {
        return ERROR_INPUT;
    }

    va_list args;
    va_start(args, res);

    char* first = va_arg(args, char*);
    if (first == NULL) {
        va_end(args);
        return ERROR_NULL;
    }

    char* cur = (char*)malloc(strlen(first) + 1);
    if (cur == NULL) {
        va_end(args);
        return ERROR_MEMORY;
    }
    strcpy(cur, first);

    for (int i = 1; i < count; i++) {
        char* next = va_arg(args, char*);
        if (next == NULL) {
            free(cur);
            va_end(args);
            return ERROR_NULL;
        }

        char* temp = NULL;
        state st = Sum2Num(cur, next, base, &temp);
        free(cur);

        if (st != OK) {
            va_end(args);
            return st;
        }
        cur = temp;
    }

    va_end(args);
    *res = cur;
    return OK;
}

int main() {
    int base;
    int count;
    char** numbers = NULL;
    char* res = NULL;
    state st;

    printf("Введите основание системы счисления (от 2 до 36): ");
    if (scanf("%d", &base) != 1 || base < 2 || base > 36) {
        fprintf(stderr, "Некорректное основание\n");
        return ERROR_INPUT;
    }

    printf("Введите количество чисел для сложения: ");
    if (scanf("%d", &count) != 1 || count <= 0) {
        fprintf(stderr, "Некорректное количество чисел\n");
        return ERROR_INPUT;
    }

    numbers = (char**)malloc(count * sizeof(char*));
    if (numbers == NULL) {
        fprintf(stderr, "Ошибка выделения памяти\n");
        return ERROR_MEMORY;
    }

    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    printf("Введите %d чисел (каждое с новой строки):\n", count);
    for (int i = 0; i < count; i++) {
        size_t cap = 32;
        size_t len = 0;
        char* str = (char*)malloc(cap);
        if (str == NULL) {
            fprintf(stderr, "Ошибка выделения памяти\n");
            for (int k = 0; k < i; k++) {
                free(numbers[k]);
            }
            free(numbers);
            return ERROR_MEMORY;
        }

        while ((c = getchar()) != EOF && c != '\n' && c != '\r') {
            if (len + 1 >= cap) {
                cap *= 2;
                char* tmp = (char*)realloc(str, cap);
                if (tmp == NULL) {
                    fprintf(stderr, "Ошибка выделения памяти\n");
                    free(str);
                    for (int k = 0; k < i; k++) {
                        free(numbers[k]);
                    }
                    free(numbers);
                    return ERROR_MEMORY;
                }
                str = tmp;
            }
            str[len++] = (char)c;
        }
        str[len] = '\0';
        
        if (len > 0 && str[len - 1] == '\r') {
            str[--len] = '\0';
        }

        if (len == 0) {
            fprintf(stderr, "Пустое число недопустимо\n");
            free(str);
            for (int k = 0; k < i; k++) {
                free(numbers[k]);
            }
            free(numbers);
            return ERROR_INPUT;
        }
        numbers[i] = str;
    }

    if (count == 1) {
        st = SumInBase(base, 1, &res, numbers[0]);
    } else if (count == 2) {
        st = SumInBase(base, 2, &res, numbers[0], numbers[1]);
    } else if (count == 3) {
        st = SumInBase(base, 3, &res, numbers[0], numbers[1], numbers[2]);
    } else if (count == 4) {
        st = SumInBase(base, 4, &res, numbers[0], numbers[1], numbers[2], numbers[3]);
    } else if (count == 5) {
        st = SumInBase(base, 5, &res, numbers[0], numbers[1], numbers[2], numbers[3], numbers[4]);
    } else {
        printf("Для демонстрации введите от 1 до 5 чисел\n");
        st = ERROR_INPUT;
    }

    if (st == OK) {
        printf("Сумма: %s\n", res);
        free(res);
    } else if (st == ERROR_INPUT) {
        fprintf(stderr, "Некорректные данные\n");
    } else if (st == ERROR_MEMORY) {
        fprintf(stderr, "Ошибка выделения памяти\n");
    }

    for (int i = 0; i < count; i++) {
        free(numbers[i]);
    }
    free(numbers);

    return OK;
}