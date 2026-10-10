#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum state {
    OK,
    END,
    ERROR_NULL,
    ERROR_INPUT,
} state;

state Flag(const char* f, char* res) {
    if (f == NULL || res == NULL) return ERROR_INPUT;
    if (f[0] != '/' && f[0] != '-') return ERROR_INPUT;
    int i = 1;
    if (f[i] == '\0' || f[i + 1] != '\0') return ERROR_INPUT;
    if (f[i] != 'r' && f[i] != 'a') return ERROR_INPUT;
    *res = f[i];
    return OK;
}

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
    while (c != EOF && !IsSpace(c)) {
        if (i + 1 < size) {
            buffer[i++] = c;
        }
        c = fgetc(file);
    }
    buffer[i] = '\0';
    return OK;
}

state R(const char* file1, const char* file2, const char* outFile) {
    if (file1 == NULL || file2 == NULL || outFile == NULL) return ERROR_NULL;

    FILE* f1 = fopen(file1, "r");
    if (f1 == NULL) return ERROR_INPUT;

    FILE* f2 = fopen(file2, "r");
    if (f2 == NULL) { 
        fclose(f1); 
        return ERROR_INPUT; 
    }

    FILE *out = fopen(outFile, "w");
    if (out == NULL) {
        fclose(f1);
        fclose(f2);
        return ERROR_INPUT;
    }
    
    char buffer[2048];
    int endf1 = 0;
    int endf2 = 0;
    int empty = 1;
    while(!(endf1 && endf2)) {
        if (!endf1) {
            if (ReadLex(f1, buffer, 2048) == OK) {
                if (!empty) fputc(' ', out);
                fputs(buffer, out);
                empty = 0;
            } else {
                endf1 = 1;
            }
        }
        if (!endf2) {
            if (ReadLex(f2, buffer, 2048) == OK) {
                if (!empty) fputc(' ', out);
                fputs(buffer, out);
                empty = 0;
            } else {
                endf2 = 1;
            }
        }
    }

    fclose(f1);
    fclose(f2);
    fclose(out);
    return OK;
}

void ToLower(char* s) {
    for (int i = 0; s[i] != '\0'; ++i) {
        if (s[i] >= 'A' && s[i] <= 'Z') {
            s[i] = s[i] - 'A' + 'a';
        }
    }
}

void PrintASCII(FILE* out, unsigned char c, int base) {
    if (c >= (unsigned)base) {
        PrintASCII(out, c / base, base);
    }
    fputc('0' + (c % base), out);
}

state A(char* inFile, char* outFile) {
    if (inFile == NULL || outFile == NULL) return ERROR_NULL;
    FILE* in = fopen(inFile, "r");
    if (in == NULL) return ERROR_INPUT;

    FILE* out = fopen(outFile, "w");
    if (out == NULL) {
        fclose(in);
        return ERROR_INPUT;
    }

    char buffer[2048];
    int empty = 1;
    int n = 0;
    while (ReadLex(in, buffer, 2048) == OK) {
        ++n;
        if (!empty) fputc(' ', out);
        empty = 0;

        if (n % 10 == 0) {
            ToLower(buffer);
            for (int i = 0; buffer[i] != '\0'; ++i) {
                PrintASCII(out, buffer[i], 4);
                fputc(' ', out);
            }
        } else if (n % 2 == 0) {
            ToLower(buffer);
            fputs(buffer, out);
        } else if (n % 5 == 0) {
            for (int i = 0; buffer[i] != '\0'; ++i) {
                PrintASCII(out, buffer[i], 8);
            }
            fputc(' ', out);
        } else {
            fputs(buffer, out);
        }
    }

    fclose(in);
    fclose(out);
    return OK;
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Аргументов строки должно быть больше 4\n");
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
    char* file1 = argv[2];
    char* outFile = argv[argc - 1];

    if (strcmp(file1, outFile) == 0) {
        fprintf(stderr, "Входной и выходной файлы не должны совпадать\n");
        return ERROR_INPUT;
    }

    if (flag == 'r') {
        if (argc != 5) {
            fprintf(stderr, "Аргументов строки для флага 'r' должно быть >= 5 \n");
            return ERROR_INPUT;
        }
        char *file2 = argv[3];
        if (strcmp(file2, outFile) == 0) {
            fprintf(stderr, "Входной и выходной файлы не должны совпадать\n");
            return ERROR_INPUT;
        }
        st = R(file1, file2, outFile);
        if (st != OK) {
            if (st == ERROR_NULL) {
                fprintf(stderr, "NULL-указатель\n");
            } else if (st == ERROR_INPUT) {
                fprintf(stderr, "Ошибка ввода\n");
            }
            return st;
        }
    } else if (flag == 'a') {
        if (argc != 4) {
            fprintf(stderr, "Аргументов строки для флага 'a' должно быть >= 4 \n");
            return ERROR_INPUT;
        }
        st = A(file1, outFile);
        if (st != OK) {
            if (st == ERROR_NULL) {
                fprintf(stderr, "NULL-указатель\n");
            } else if (st == ERROR_INPUT) {
                fprintf(stderr, "Ошибка ввода\n");
            }
            return st;
        }
    }
    return st;
}