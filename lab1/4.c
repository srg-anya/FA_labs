#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum state {
    OK,
    ERROR_NULL,
    ERROR_INPUT,
} state;

state Flag(const char* f, char* res, int* hasn) {
    if (f == NULL || res == NULL || hasn == NULL) return ERROR_INPUT;
    if (f[0] != '/' && f[0] != '-') return ERROR_INPUT;
    int i = 1;
    *hasn = 0;
    if (f[i] == 'n') {
        ++i;
        *hasn = 1;
    }
    if (f[i] == '\0' || f[i + 1] != '\0') return ERROR_INPUT;
    if (f[i] != 'd' && f[i] != 'i' && f[i] != 's' && f[i] != 'a' ) return ERROR_INPUT;
    *res = f[i];
    return OK;
}


state D(char* inFile, char* outFile) {
    if (inFile == NULL || outFile == NULL) return ERROR_NULL;
    FILE* in = fopen(inFile, "r");
    if (in == NULL) return ERROR_INPUT;

    FILE* out = fopen(outFile, "w");
    if (out == NULL) {
        fclose(in);
        return ERROR_INPUT;
    }
    int c;
    while ((c = fgetc(in)) != EOF) {
        if (c >= '0' && c <= '9') {
            continue;
        } else {
            fputc(c, out);
        }
    }

    fclose(in);
    fclose(out);
    return OK;
}

state I(char* inFile, char* outFile) {
    if (inFile == NULL || outFile == NULL) return ERROR_NULL;
    FILE* in = fopen(inFile, "r");
    if (in == NULL) return ERROR_INPUT;

    FILE* out = fopen(outFile, "w");
    if (out == NULL) {
        fclose(in);
        return ERROR_INPUT;
    }
    char str[2048];
    while (fgets(str, sizeof(str), in) != NULL) {
        int count = 0;
        for (int i = 0; str[i] != '\0'; ++i) {
            char c = str[i];
            if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
                ++count;
            }
        }
        fprintf(out, "%d\n", count);
    }

    fclose(in);
    fclose(out);
    return OK;
}

state S(char* inFile, char* outFile) {
    if (inFile == NULL || outFile == NULL) return ERROR_NULL;
    FILE* in = fopen(inFile, "r");
    if (in == NULL) return ERROR_INPUT;

    FILE* out = fopen(outFile, "w");
    if (out == NULL) {
        fclose(in);
        return ERROR_INPUT;
    }
    char str[2048];
    while (fgets(str, sizeof(str), in) != NULL) {
        int count = 0;
        for (int i = 0; str[i] != '\0'; ++i) {
            char c = str[i];
            if (c == '\n') continue;
            if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == ' ')) {
                ++count;
            }
        }
        fprintf(out, "%d\n", count);
    }

    fclose(in);
    fclose(out);
    return OK;
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
    int c;
    while ((c = fgetc(in)) != EOF) {
        if (c == '\n') {
            fputc(c, out);
        } else if (c >= '0' && c <= '9') {
            fputc(c, out);
        } else {
            fprintf(out, "%X", c);
        }
    }

    fclose(in);
    fclose(out);
    return OK;
}


int main(int argc, char *argv[]) {
    if (argc < 3 || argc > 4) {
        fprintf(stderr, "Аргументов строки должно быть от 3 до 4\n");
        return ERROR_INPUT;
    }
    char flag = '\0';
    int hasn;
    state st = Flag(argv[1], &flag, &hasn);
    if (st != OK) {
        if (st == ERROR_INPUT) {
            fprintf(stderr, "Некорректный флаг\n");
            return st;
        }
    }
    char outFile[1024];
    char* inFile = argv[2];
    if (hasn) {
        if (argc != 4) {
            fprintf(stderr, "Аргументов строки должно быть 4\n");
            return ERROR_INPUT;
        }
        snprintf(outFile, sizeof(outFile), "%s", argv[3]);
    } else {
        if (argc != 3) {
            fprintf(stderr, "Выходной файл только для флагов с n\n");
            return ERROR_INPUT;
        }
        int n = snprintf(outFile, sizeof(outFile), "out_%s", argv[2]);
        if (n < 0 || n >= (int)sizeof(outFile)) {
            fprintf(stderr, "Слишком длинное имя файла\n");
            return ERROR_INPUT;
        }
    }

    if (strcmp(inFile, outFile) == 0) {
        fprintf(stderr, "Входной и выходной файлы не должны совпадать\n");
        return ERROR_INPUT;
    }

    if (flag == 'd') {
        st = D(inFile, outFile);
        if (st != OK) {
            if (st == ERROR_NULL) {
                fprintf(stderr, "NULL-указатель\n");
            } else if (st == ERROR_INPUT) {
                fprintf(stderr, "Ошибка ввода\n");
            }
            return st;
        }
    } else if (flag == 'i') {
        st = I(inFile, outFile);
        if (st != OK) {
            if (st == ERROR_NULL) {
                fprintf(stderr, "NULL-указатель\n");
            } else if (st == ERROR_INPUT) {
                fprintf(stderr, "Ошибка ввода\n");
            }
            return st;
        }
    } else if (flag == 's') {
        st = S(inFile, outFile);
        if (st != OK) {
            if (st == ERROR_NULL) {
                fprintf(stderr, "NULL-указатель\n");
            } else if (st == ERROR_INPUT) {
                fprintf(stderr, "Ошибка ввода\n");
            }
            return st;
        }
    } else if (flag == 'a') {
        st = A(inFile, outFile);
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