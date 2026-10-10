#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX 1000000000
#define MIN -1000000000

typedef enum state {
    OK,
    ERROR_RANGE,
    ERROR_NULL,
    ERROR_INPUT,
    ERROR_OVERFLOW
} state;

void StripNewline(char* s) {
    int len = (int)strlen(s);
    if (len > 0 && s[len - 1] == '\n') s[len - 1] = '\0';
}

state Base(int* base) {
    if(base == NULL) return ERROR_NULL;

    char buffer[1024];
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        return ERROR_INPUT;
    }

    int len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
        len--;
    }

    if (len == 0) {
        return ERROR_INPUT;
    }

    int b = 0;
    for (int i = 0; i < len; ++i) {
        if (buffer[i] < '0' || buffer[i] > '9') return ERROR_INPUT;
        b = b * 10 + (buffer[i] - '0');
        if (b > 36) return ERROR_RANGE;
    }

    if (b < 2) return ERROR_RANGE;
    *base = b;
    return OK;
}

int CharToDigit(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'Z') return c - 'A' + 10;
    return -1;
}

char DigitToChar(int c) {
    if (c >= 0 && c <= 9) return (char)('0' + c);
    if (c >= 10 && c <= 35) return (char)('A' + c - 10);
    return '\0';
}

state Number(const char* n, int base, int* res) {
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
    long long x = 0;
    while (n[i] != '\0') {
        int v = CharToDigit(n[i]);
        if (v < 0  || v >= base) return ERROR_INPUT;
        if(x > (MAX - v) / base) return ERROR_OVERFLOW;
        x = x * base + v;
        ++i;
    }
    *res = x * sign;
    return OK;
}

void PrintInBase(int number, int base) {
    if (number == 0) { 
        putchar('0'); 
        return;
    }

    if (number < 0) {
        putchar('-');
        number = -number;
    }

    char digits[64];
    int n = 0;
    while (number > 0) {
        digits[n++] = DigitToChar(number % base);
        number = number / base;
    }
    while (n > 0) {
        putchar(digits[--n]);
    }
}



int main() {
    int base = 0;
    state st = Base(&base);
    if (st != OK) {
        if (st == ERROR_NULL) {
            fprintf(stderr, "NULL-указатель\n");
        } else if (st == ERROR_INPUT) {
            fprintf(stderr, "Некорректный ввод основания\n");
        } else if (st == ERROR_RANGE) {
            fprintf(stderr, "Основание должно быть 2 <= base <= 36\n");
        }
        return st;
    }

    char buffer[1024];
    int sum = 0;
    int maxNumber = 0;
    char maxRes[1024];
    maxRes[0] = '\0';
    while (1) {
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            break;
        }
        StripNewline(buffer);

        if (buffer[0] == '\0') continue;
        if (strcmp(buffer, "Stop") == 0) break;

        int number = 0;
        st = Number(buffer, base, &number);
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

        if ((number > 0 && sum > MAX - number) ||
            (number < 0 && sum < MIN - number)) {
            fprintf(stderr, "Проблемы с переполнением суммы\n");
            return ERROR_OVERFLOW;
        }
        sum += number;

        if (abs(maxNumber) < abs(number)) {
            maxNumber = number;
            strcpy(maxRes, buffer);
        }

        printf("%d: ", number);
        PrintInBase(number, 9);
        printf(" ");
        PrintInBase(number, 18);
        printf(" ");
        PrintInBase(number, 27);
        printf(" ");
        PrintInBase(number, 36);
        printf("\n");
    }
    printf(" Сумма: %d\n", sum);
    printf("Максимально по модулю: %s\n", maxRes);

    return OK;
}