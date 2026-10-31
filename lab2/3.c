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

state ReadStr(char** res) {
    if (res == NULL) {
        return ERROR_NULL;
    }
    size_t cap = 64;
    size_t len = 0;
    char* str = (char*)malloc(cap);
    if (str == NULL) {
        return ERROR_MEMORY;
    }
    str[0] = '\0';

    int c;
    while ((c = fgetc(stdin)) != EOF) {
        if (len + 1 >= cap) {
            size_t ncap = cap * 2;
            char* tmp = (char*)realloc(str, ncap);
            if (tmp == NULL) {
                free(str);
                return ERROR_MEMORY;
            }
            str = tmp;
            cap = ncap;
        }
        str[len++] = (char)c;
    }
    str[len] = '\0';
    *res = str;
    return OK;
}

void PrintMessage(int flag, const char *filename, int line, int index) {
    if (flag == 0) {
        printf("Не удалось открыть файл: %s\n", filename);
    } else if (flag == 1) {
        printf("Файл %s: Найдено в строке %d на индексе %d\n", filename, line, index);
    } else if (flag == 2) {
        printf("Файл %s: Вхождений нет\n", filename);
    }
}

void Prefix(const char *P, int *pi, int m) {
    pi[0] = 0;
    int j = 0;
    for (int i = 1; i < m; i++) {
        while (j > 0 && P[i] != P[j]) {
            j = pi[j - 1];
        }
        if (P[i] == P[j]) {
            ++j;
        }
        pi[i] = j;
    }
}

int GetCharIndex(const char *str, int byte) {
    int count = 1;
    for (int i = 0; i < byte; i++) {
        if ((str[i] & 0xC0) != 0x80) {
            count++;
        }
    }
    return count;
}

void KMP(const char *T, const char *P, int *pi, int m, int line, const char *file, int *count) {
    int n = strlen(T);
    int j = 0;
    for (int i = 0; i < n; i++) {
        while (j > 0 && T[i] != P[j]) {
            j = pi[j - 1];
        }
        if (T[i] == P[j]) {
            j++;
        }
        if (j == m) {
            int ind = GetCharIndex(T, i - m + 1);
            PrintMessage(1, file, line, ind);
            *count = 1;
            j = pi[j - 1];
        }
    }
}

state ReadFile(FILE *file, char **outBuf, size_t *outLen) {
    size_t cap = 4096;
    size_t len = 0;
    char *buf = (char *)malloc(cap);
    if (buf == NULL) {
        return ERROR_MEMORY;
    }

    size_t got;
    while ((got = fread(buf + len, 1, cap - len, file)) > 0) {
        len += got;
        if (len == cap) {
            cap *= 2;
            char *tmp = (char*)realloc(buf, cap);
            if (tmp == NULL) {
                free(buf);
                return ERROR_MEMORY;
            }
            buf = tmp;
        }
    }

    if (ferror(file)) {
        free(buf);
        return ERROR_INPUT;
    }

    *outBuf = buf;
    *outLen = len;
    return OK;
}

state Strstr(const char *str, int n, ...) {
    if (str == NULL) {
        return ERROR_NULL;
    }
    int m = strlen(str);
    if (m == 0) {
        return ERROR_INPUT;
    }

    int *pi = (int*)malloc(m * sizeof(int));
    if (pi == NULL) {
        return ERROR_MEMORY;
    }
    Prefix(str, pi, m);

    va_list param;
    va_start(param, n);

    state st = OK;
    for (int i = 0; i < n; ++i) {
        const char *f = va_arg(param, const char *);
        if (f == NULL) {
            free(pi);
            va_end(param);
            return ERROR_NULL;
        }

        FILE *file = fopen(f, "rb");
        if (file == NULL) {
            PrintMessage(0, f, 0, 0);
            continue;
        }

        char *buf = NULL;
        size_t len = 0;
        state cur = ReadFile(file, &buf, &len);
        fclose(file);

        if (cur != OK) {
            if (cur == ERROR_MEMORY) {
                free(pi);
                va_end(param);
                return ERROR_MEMORY;
            }
            if (st == OK) {
                st = cur;
            }
            continue;
        }

        int count = 0;
        size_t pos = 0;
        size_t line = 1;
        size_t lineStart = 0;
        int j = 0;
        for (size_t i2 = 0; i2 < len; i2++) {
            while (j > 0 && buf[i2] != str[j]) {
                j = pi[j - 1];
            }
            if (buf[i2] == str[j]) {
                j++;
            }
            if (j == m) {
                size_t fpos = i2 - m + 1;

                while (pos < fpos) {
                    if (buf[pos] == '\r' && (pos + 1 < len && buf[pos + 1] == '\n')) {
                        pos++;
                        continue;
                    }
                    if (buf[pos] == '\n') {
                        line++;
                        lineStart = pos + 1;
                    }
                    pos++;
                }
                int col = GetCharIndex(buf + lineStart, (int)(fpos - lineStart));
                PrintMessage(1, f, (int)line, col);
                count = 1;

                j = pi[j - 1];
            }
        }

        free(buf);
        if (count == 0) {
            PrintMessage(2, f, 0, 0);
        }
    }

    free(pi);
    va_end(param);
    return st;
}

int main() {
    int n;
    printf("Введите количество файлов: ");
    if (scanf("%d", &n) != 1 || n <= 0) {
        fprintf(stderr, "Некорректное количество файлов\n");
        return ERROR_INPUT;
    }
    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    char** files = (char**)malloc(n * sizeof(char *));
    if (!files) {
        fprintf(stderr, "Недостаточно памяти для массива путей к файлам.\n");
        return ERROR_MEMORY;
    }

    printf("Введите эти %d файлов(каждый с новой строки):\n", n);
    for (int i = 0; i < n; ++i) {
        char *path = NULL;
        size_t cap = 0;
        ssize_t len = getline(&path, &cap, stdin);
        if (len < 0) {
            fprintf(stderr, "Ошибка чтения пути к файлу\n");
            for (int j = 0; j < i; ++j) {
                free(files[j]);
            }
            free(files);
            return ERROR_INPUT;
        }
        path[strcspn(path, "\r\n")] = '\0';
        files[i] = path;
    }
    printf("\n");

    printf("Введите подстроку (конец ввода ctrl+d):\n");
    char* str;
    state st = ReadStr(&str);
    if (st != OK) {
        fprintf(stderr, "Не удалось прочитать подстроку\n");
        for (int i = 0; i < n; ++i){
            free(files[i]);
        }
        free(files);
        return st;
    }
    printf("\n");

    st = OK;
    for (int i = 0; i < n && st == OK; i += 4) {
        int count = n - i;
        if (count > 4) {
            count = 4;
        }

        if (count == 1) {
            st = Strstr(str, 1, files[i]);
        } else if (count == 2) {
            st = Strstr(str, 2, files[i], files[i + 1]);
        } else if (count == 3) {
            st = Strstr(str, 3, files[i], files[i + 1], files[i + 2]);
        } else if (count == 4) {
            st = Strstr(str, 4, files[i], files[i + 1], files[i + 2], files[i + 3]);
        }
        if (st != OK) {
            fprintf(stderr, "Ошибка при поиске\n");
        }
    }

    for (int i = 0; i < n; ++i) {
        free(files[i]);
    }
    free(files);
    free(str);
    return st;
}