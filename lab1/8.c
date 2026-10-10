#include <stdio.h>
#include <string.h>
#define MAX 100000000000

typedef enum state {
    OK,
    END,
    ERROR_NULL,
    ERROR_INPUT,
    ERROR_OVERFLOW
} state;


int IsSpace(char c) {
    return c == ' ' || c == '\t' || c == '\n';
}

state ReadLex(FILE* file, char* buffer, int size) {
    int c;
    while((c = fgetc(file)) != EOF) {
        if (!IsSpace(c)) break;
    }
    if (c == EOF) return END;
    int i = 0;
    int overflow = 0;
    while (c != EOF && !IsSpace(c)) {
        if (i + 1 < size) {
            buffer[i++] = c;
        } else {
            overflow = 1;
        }
        c = fgetc(file);
    }
    buffer[i] = '\0';
    if (overflow) return ERROR_OVERFLOW;
    return OK;
}

state Digit(char c, long long* val) {
    if (val == NULL) return ERROR_NULL;
    if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))) {
        return ERROR_INPUT;
    }
    if (c >= '0' && c <= '9') *val = c - '0';
    if (c >= 'A' && c <= 'Z') *val = c - 'A' + 10;
    if (c >= 'a' && c <= 'z') *val = c - 'a' + 10;
    return OK;
}

state Base(char* str, int* base) {
    int max = 0;
    long long c;
    for (int i = 0; str[i] != '\0'; ++i) {
        state st = Digit(str[i], &c);
        if (st != OK) return st;
        if (c > max) {
            max = c;
        }
    }
    int b = max + 1;
    if (b < 2) b = 2;
    if (b > 36) return ERROR_INPUT;
    *base = b;
    return OK;
}


state To10(char* str, int base, long long* res) {
    if (res == NULL) return ERROR_NULL;
    *res = 0;
    long long val = 0;
    for (int i = 0; str[i] != '\0'; ++i) {
        state st = Digit(str[i], &val);
        if (st != OK) {
            return st;
        }
        if (val >= base) return ERROR_INPUT;
        *res = *res * base + val;
        if (*res > MAX) return ERROR_OVERFLOW;
    }
    return OK;
}

char* DeleteZeros(char* str) {
    int i = 0;
    while (str[i] == '0' && str[i + 1] != '\0') ++i;
    return str + i;
}

state Result(const char* inFile, const char* outFile) {
    if (inFile == NULL || outFile == NULL) return ERROR_NULL;

    FILE* in = fopen(inFile, "r");
    if (in == NULL) return ERROR_INPUT;

    FILE* out = fopen(outFile, "w");
    if (out == NULL) {
        fclose(in);
        return ERROR_INPUT;
    }

    char buffer[2048];
    int base;
    state st;
    long long value;
    while (1) {
        st = ReadLex(in, buffer, 2048);
        if (st == END) break;
        if (st != OK) {
            fclose(in);
            fclose(out);
            return st;
        }
        st = Base(buffer, &base);
        if (st != OK) {
            fclose(in);
            fclose(out);
            return st;
        }
        char* str = DeleteZeros(buffer);
        st = To10(buffer, base, &value);
        if (st != OK) {
            fclose(in);
            fclose(out);
            return st;
        }
        fprintf(out, "%s %d %lld\n", str, base, value);
    }

    fclose(in);
    fclose(out);
    return OK;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Аргументов строки должно быть 3\n");
        return ERROR_INPUT;
    }

    char* inFile = argv[1];
    char* outFile = argv[2];

    if (strcmp(inFile, outFile) == 0) {
        fprintf(stderr, "Входной и выходной файлы не должны совпадать\n");
        return ERROR_INPUT;
    }

    state st = Result(inFile, outFile);
    if (st != OK) {
        if (st == ERROR_NULL) {
            fprintf(stderr, "NULL-указатель\n");
        } else if (st == ERROR_INPUT) {
            fprintf(stderr, "Ошибка ввода\n");
        } else if (st == ERROR_OVERFLOW) {
            fprintf(stderr, "Число слишком большое для буфера\n");
        }
        return st;
    }
    return OK;
}