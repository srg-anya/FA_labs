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


void IntToRoman(int num, char* out) {
    int vals[] = {1000, 900, 500, 400, 100, 90, 50, 40, 10, 9, 5, 4, 1};
    char* syms[] = {"M", "CM", "D", "CD", "C", "XC", "L", "XL", "X", "IX", "V", "IV", "I"};
    out[0] = '\0';
    for (int i = 0; i < 13; i++) {
        while (num >= vals[i]) {
            strcat(out, syms[i]);
            num -= vals[i];
        }
    }
}

void IntToZeckendorf(unsigned int num, char* out) {
    unsigned int fib[50];
    int n = 2;
    fib[0] = 1;
    fib[1] = 2;
    while (n < 50) {
        fib[n] = fib[n - 1] + fib[n - 2];
        n++;
    }
    int last = -1;
    for (int i = n - 1; i >= 0; i--) {
        if (fib[i] <= num) {
            last = i;
            break;
        }
    }
    if (last < 0) {
        strcpy(out, "0");
        return;
    }
    char temp[64];
    int len = 0;
    for (int i = last; i >= 0; i--) {
        if (fib[i] <= num) {
            temp[len++] = '1';
            num -= fib[i];
        } else {
            temp[len++] = '0';
        }
    }
    temp[len] = '\0';
    for (int i = 0; i < len; i++) {
        out[i] = temp[len - 1 - i];
    }
    out[len] = '\0';
}

void IntToBase(int num, int base, int isUpper, char* out) {
    char* digits;
    if (isUpper) {
        digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    } else {
        digits = "0123456789abcdefghijklmnopqrstuvwxyz";
    }

    if (base < 2 || base > 36) {
        base = 10;
    }
    if (num == 0) {
        strcpy(out, "0");
        return;
    }
    int sign = 1;
    if (num < 0) {
        sign = -1;
        num = -num;
    }
    char temp[64];
    int len = 0;
    while (num > 0) {
        temp[len++] = digits[num % base];
        num /= base;
    }
    if (sign == -1) {
        out[0] = '-';
        for (int i = 0; i < len; i++) {
            out[i + 1] = temp[len - 1 - i];
        }
        out[len + 1] = '\0';
    } else {
        for (int i = 0; i < len; i++) {
            out[i] = temp[len - 1 - i];
        }
        out[len] = '\0';
    }
}

int StrToBase(char* s, int base, int isUpper) {
    if (base < 2 || base > 36) {
        base = 10;
    }
    int result = 0;
    int i = 0;
    while (s[i] != '\0') {
        int digit = -1;
        if (s[i] >= '0' && s[i] <= '9') {
            digit = s[i] - '0';
        } else if (isUpper && s[i] >= 'A' && s[i] <= 'Z') {
            digit = s[i] - 'A' + 10;
        } else if (!isUpper && s[i] >= 'a' && s[i] <= 'z') {
            digit = s[i] - 'a' + 10;
        } else {
            break;
        }
        if (digit < 0 || digit >= base) {
            break;
        }
        result = result * base + digit;
        i++;
    }
    return result;
}


void DumpMemory(void* ptr, int size, char* out) {
    unsigned char* p = (unsigned char*)ptr;
    out[0] = '\0';
    for (int i = 0; i < size; i++) {
        for (int bit = 7; bit >= 0; bit--) {
            char ch[2] = { (p[i] & (1 << bit)) ? '1' : '0', '\0' };
            strcat(out, ch);
        }
        if (i < size - 1) {
            strcat(out, " ");
        }
    }
}


state Overfprintf(FILE* file, char* flag, ...) {
    if (file == NULL || flag == NULL) {
        return ERROR_NULL;
    }
    va_list args;
    va_start(args, flag);
    int i = 0;
    while (flag[i] != '\0') {
        if (flag[i] != '%') {
            fputc(flag[i], file);
            i++;
            continue;
        }
        i++;
        if (flag[i] == '%') {
            fputc('%', file);
            i++;
            continue;
        }

        if (flag[i] == 'd') {
            int val = va_arg(args, int);
            fprintf(file, "%d", val);
            i++;
        } else if (flag[i] == 's') {
            char* s = va_arg(args, char*);
            fprintf(file, "%s", s);
            i++;
        } else if (flag[i] == 'c') {
            int c = va_arg(args, int);
            fputc(c, file);
            i++;
        } else if (flag[i] == 'f') {
            double val = va_arg(args, double);
            fprintf(file, "%f", val);
            i++;
        } else if (flag[i] == 'R' && flag[i + 1] == 'o') {
            int val = va_arg(args, int);
            char buf[64];
            IntToRoman(val, buf);
            fprintf(file, "%s", buf);
            i += 2;
        } else if (flag[i] == 'Z' && flag[i + 1] == 'r') {
            unsigned int val = va_arg(args, unsigned int);
            char buf[64];
            IntToZeckendorf(val, buf);
            fprintf(file, "%s", buf);
            i += 2;
        } else if (flag[i] == 'C' && flag[i + 1] == 'v') {
            int val = va_arg(args, int);
            int base = va_arg(args, int);
            char buf[64];
            IntToBase(val, base, 0, buf);
            fprintf(file, "%s", buf);
            i += 2;
        } else if (flag[i] == 'C' && flag[i + 1] == 'V') {
            int val = va_arg(args, int);
            int base = va_arg(args, int);
            char buf[64];
            IntToBase(val, base, 1, buf);
            fprintf(file, "%s", buf);
            i += 2;
        } else if (flag[i] == 't' && flag[i + 1] == 'o') {
            char* s = va_arg(args, char*);
            int base = va_arg(args, int);
            int val = StrToBase(s, base, 0);
            fprintf(file, "%d", val);
            i += 2;
        } else if (flag[i] == 'T' && flag[i + 1] == 'O') {
            char* s = va_arg(args, char*);
            int base = va_arg(args, int);
            int val = StrToBase(s, base, 1);
            fprintf(file, "%d", val);
            i += 2;
        } else if (flag[i] == 'm' && flag[i + 1] == 'i') {
            int val = va_arg(args, int);
            char buf[128];
            DumpMemory(&val, sizeof(int), buf);
            fprintf(file, "%s", buf);
            i += 2;
        } else if (flag[i] == 'm' && flag[i + 1] == 'u') {
            unsigned int val = va_arg(args, unsigned int);
            char buf[128];
            DumpMemory(&val, sizeof(unsigned int), buf);
            fprintf(file, "%s", buf);
            i += 2;
        } else if (flag[i] == 'm' && flag[i + 1] == 'd') {
            double val = va_arg(args, double);
            char buf[128];
            DumpMemory(&val, sizeof(double), buf);
            fprintf(file, "%s", buf);
            i += 2;
        } else if (flag[i] == 'm' && flag[i + 1] == 'f') {
            double val = va_arg(args, double);
            float fval = (float)val;
            char buf[128];
            DumpMemory(&fval, sizeof(float), buf);
            fprintf(file, "%s", buf);
            i += 2;
        } else {
            fputc(flag[i], file);
            i++;
        }
    }
    va_end(args);
    return OK;
}


state Oversprintf(char* buf, char* flag, ...) {
    if (buf == NULL || flag == NULL) {
        return ERROR_NULL;
    }
    va_list args;
    va_start(args, flag);
    int i = 0;
    int out = 0;
    while (flag[i] != '\0') {
        if (flag[i] != '%') {
            buf[out++] = flag[i];
            i++;
            continue;
        }
        i++;
        if (flag[i] == '%') {
            buf[out++] = '%';
            i++;
            continue;
        }

        if (flag[i] == 'd') {
            int val = va_arg(args, int);
            char temp[64];
            snprintf(temp, sizeof(temp), "%d", val);
            strcpy(buf + out, temp);
            out += strlen(temp);
            i++;
        } else if (flag[i] == 's') {
            char* s = va_arg(args, char*);
            strcpy(buf + out, s);
            out += strlen(s);
            i++;
        } else if (flag[i] == 'c') {
            int c = va_arg(args, int);
            buf[out++] = (char)c;
            i++;
        } else if (flag[i] == 'f') {
            double val = va_arg(args, double);
            char temp[64];
            snprintf(temp, sizeof(temp), "%f", val);
            strcpy(buf + out, temp);
            out += strlen(temp);
            i++;
        } else if (flag[i] == 'R' && flag[i + 1] == 'o') {
            int val = va_arg(args, int);
            char temp[64];
            IntToRoman(val, temp);
            strcpy(buf + out, temp);
            out += strlen(temp);
            i += 2;
        } else if (flag[i] == 'Z' && flag[i + 1] == 'r') {
            unsigned int val = va_arg(args, unsigned int);
            char temp[64];
            IntToZeckendorf(val, temp);
            strcpy(buf + out, temp);
            out += strlen(temp);
            i += 2;
        } else if (flag[i] == 'C' && flag[i + 1] == 'v') {
            int val = va_arg(args, int);
            int base = va_arg(args, int);
            char temp[64];
            IntToBase(val, base, 0, temp);
            strcpy(buf + out, temp);
            out += strlen(temp);
            i += 2;
        } else if (flag[i] == 'C' && flag[i + 1] == 'V') {
            int val = va_arg(args, int);
            int base = va_arg(args, int);
            char temp[64];
            IntToBase(val, base, 1, temp);
            strcpy(buf + out, temp);
            out += strlen(temp);
            i += 2;
        } else if (flag[i] == 't' && flag[i + 1] == 'o') {
            char* s = va_arg(args, char*);
            int base = va_arg(args, int);
            int val = StrToBase(s, base, 0);
            char temp[64];
            snprintf(temp, sizeof(temp), "%d", val);
            strcpy(buf + out, temp);
            out += strlen(temp);
            i += 2;
        } else if (flag[i] == 'T' && flag[i + 1] == 'O') {
            char* s = va_arg(args, char*);
            int base = va_arg(args, int);
            int val = StrToBase(s, base, 1);
            char temp[64];
            snprintf(temp, sizeof(temp), "%d", val);
            strcpy(buf + out, temp);
            out += strlen(temp);
            i += 2;
        } else if (flag[i] == 'm' && flag[i + 1] == 'i') {
            int val = va_arg(args, int);
            char temp[128];
            DumpMemory(&val, sizeof(int), temp);
            strcpy(buf + out, temp);
            out += strlen(temp);
            i += 2;
        } else if (flag[i] == 'm' && flag[i + 1] == 'u') {
            unsigned int val = va_arg(args, unsigned int);
            char temp[128];
            DumpMemory(&val, sizeof(unsigned int), temp);
            strcpy(buf + out, temp);
            out += strlen(temp);
            i += 2;
        } else if (flag[i] == 'm' && flag[i + 1] == 'd') {
            double val = va_arg(args, double);
            char temp[128];
            DumpMemory(&val, sizeof(double), temp);
            strcpy(buf + out, temp);
            out += strlen(temp);
            i += 2;
        } else if (flag[i] == 'm' && flag[i + 1] == 'f') {
            double val = va_arg(args, double);
            float fval = (float)val;
            char temp[128];
            DumpMemory(&fval, sizeof(float), temp);
            strcpy(buf + out, temp);
            out += strlen(temp);
            i += 2;
        } else {
            buf[out++] = flag[i];
            i++;
        }
    }
    buf[out] = '\0';
    va_end(args);
    return OK;
}


int main() {
    char buf[512];

    printf("Oversprintf:\n");
    Oversprintf(buf, "d=%d, s=%s, c=%c, f=%.2f", 42, "hello", 'X', 3.14);
    printf("%s\n", buf);

    Oversprintf(buf, "Ro=%Ro, Zr=%Zr, Cv=%Cv, CV=%CV", 1994, 10, 255, 255, 16, 16);
    printf("%s\n", buf);

    Oversprintf(buf, "to=%to, TO=%TO", "ff", 16, "FF", 16);
    printf("%s\n", buf);

    int x = 5;
    Oversprintf(buf, "mi=%mi", x);
    printf("%s\n", buf);

    printf("\nOverfprintf:\n");
    Overfprintf(stdout, "d=%d, s=%s, c=%c, f=%.2f\n", 42, "hello", 'X', 3.14);
    Overfprintf(stdout, "Ro=%Ro, Zr=%Zr, Cv=%Cv, CV=%CV\n", 2024, 10, 255, 255, 16, 16);
    Overfprintf(stdout, "to=%to, TO=%TO\n", "ff", 16, "FF", 16);
    Overfprintf(stdout, "mi=%mi\n", x);

    return OK;
}