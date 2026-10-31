#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum state { 
    OK,
    ERROR_NULL,
    ERROR_INPUT,
    ERROR_MEMORY
} state;

typedef struct File {
    char* name;
    int size;
    int importance;
} File;

typedef struct Result {
    File* files;
    int count;
    int size;
    int importance;
} Result;

state ReadInt(int *res) {
    if (res == NULL) {
        return ERROR_NULL;
    }
    if (scanf("%d", res) != 1 || *res < 0) {
        return ERROR_INPUT;
    }
    return OK;
}

state SelectFiles(File* files, int n, int cap, Result* result) {
    if (files == NULL || result == NULL) {
        return ERROR_NULL;
    }
    if (n < 0 || cap < 0) {
        return ERROR_INPUT;
    }

    result->files = NULL;
    result->count = 0;
    result->size = 0;
    result->importance = 0;

    int** dp = (int**)malloc((n + 1) * sizeof(int*));
    if (dp == NULL) {
        return ERROR_MEMORY;
    }

    for (int i = 0; i <= n; i++) {
        dp[i] = (int *)malloc((cap + 1) * sizeof(int));
        if (dp[i] == NULL) {
            for (int k = 0; k < i; k++) {
                free(dp[k]);
            }
            free(dp);
            return ERROR_MEMORY;
        }
        for (int w = 0; w <= cap; w++) {
            dp[i][w] = 0;
        }
    }

    for (int i = 1; i <= n; i++) {
        for (int w = 0; w <= cap; w++) {
            dp[i][w] = dp[i - 1][w];
            if (files[i - 1].size <= w) {
                int val = dp[i - 1][w - files[i - 1].size] + files[i - 1].importance;
                if (val > dp[i][w]) {
                    dp[i][w] = val;
                }
            }
        }
    }

    if (dp[n][cap] > 0) {
        result->files = (File*)malloc(n * sizeof(File));
        if (result->files == NULL) {
            for (int i = 0; i <= n; i++) {
                free(dp[i]);
            }
            free(dp);
            return ERROR_MEMORY;
        }

        int w = cap;
        for (int i = n; i > 0; i--) {
            if (dp[i][w] != dp[i - 1][w]) {
                result->files[result->count++] = files[i - 1];
                result->size += files[i - 1].size;
                result->importance += files[i - 1].importance;
                w = w - files[i - 1].size;
            }
        }
        
        for (int i = 0; i < result->count / 2; i++) {
            File temp = result->files[i];
            result->files[i] = result->files[result->count - 1 - i];
            result->files[result->count - 1 - i] = temp;
        }
    }

    for (int i = 0; i <= n; i++) {
        free(dp[i]);
    }
    free(dp);
    return OK;
}

int main() {
    int n;
    int cap;
    printf("Введите количество файлов и вместимость накопителя: ");
    if (ReadInt(&n) != OK || ReadInt(&cap) != OK) {
        fprintf(stderr, "Некорректные входные данные\n");
        return ERROR_INPUT;
    }

    File *files = (File *)malloc(n * sizeof(File));
    if (files == NULL) {
        fprintf(stderr, "Ошибка выделения памяти\n");
        return ERROR_MEMORY;
    }

    printf("Введите %d строк в формате <name size importance>:\n", n);
    for (int i = 0; i < n; i++) {
        size_t cap = 32;
        size_t len = 0;
        char *name = (char *)malloc(cap);
        if (name == NULL) {
            fprintf(stderr, "Ошибка выделения памяти\n");
            for (int k = 0; k < i; k++) {
                free(files[k].name);
            }
            free(files);
            return ERROR_MEMORY;
        }

        int c;
        while ((c = getchar()) != EOF && (c == ' ' || c == '\t' || c == '\n' || c == '\r'));

        if (c == EOF) {
            free(name);
            fprintf(stderr, "Неожиданный конец ввода\n");
            for (int k = 0; k < i; k++) {
                free(files[k].name);
            }
            free(files);
            return ERROR_INPUT;
        }

        while (c != EOF && c != ' ' && c != '\t' && c != '\n' && c != '\r') {
            if (len + 1 >= cap) {
                size_t ncap = cap * 2;
                char *tmp = (char*)realloc(name, ncap);
                if (tmp == NULL) {
                    free(name);
                    fprintf(stderr, "Ошибка выделения памяти\n");
                    for (int k = 0; k < i; k++) {
                        free(files[k].name);
                    }
                    free(files);
                    return ERROR_MEMORY;
                }
                name = tmp;
                cap = ncap;
            }
            name[len++] = (char)c;
            c = getchar();
        }
        name[len] = '\0';

        if (len == 0) {
            free(name);
            fprintf(stderr, "Пустое имя файла\n");
            for (int k = 0; k < i; k++) {
                free(files[k].name);
            }
            free(files);
            return ERROR_INPUT;
        }

        if (c != EOF) {
            ungetc(c, stdin);
        }

        if (scanf("%d %d", &files[i].size, &files[i].importance) != 2) {
            free(name);
            fprintf(stderr, "Некорректные числа для файла %d\n", i + 1);
            for (int k = 0; k < i; k++) {
                free(files[k].name);
            }
            free(files);
            return ERROR_INPUT;
        }
        c = getchar();
        while (c == ' ' || c == '\t') {
            c = getchar();
        }
        if (c != '\n' && c != EOF) {
            free(name);
            fprintf(stderr, "Лишние символы в строке файла %d\n", i + 1);
            for (int k = 0; k < i; k++) {
                free(files[k].name);
            }
            free(files);
            return ERROR_INPUT;
        }


        if (files[i].size <= 0) {
            free(name);
            fprintf(stderr, "Размер файла должен быть > 0\n");
            for (int k = 0; k < i; k++) {
                free(files[k].name);
            }
            free(files);
            return ERROR_INPUT;
        }

        if (files[i].importance < 0) {
            free(name);
            fprintf(stderr, "Важность должна быть >= 0\n");
            for (int k = 0; k < i; k++) {
                free(files[k].name);
            }
            free(files);
            return ERROR_INPUT;
        }

        for (int j = 0; j < i; j++) {
            if (strcmp(files[j].name, name) == 0) {
                fprintf(stderr, "Имя %s уже встречалось\n", name);
                free(name);
                for (int k = 0; k < i; k++) {
                    free(files[k].name);
                }
                free(files);
                return ERROR_INPUT;
            }
        }

        files[i].name = name;
    }
    printf("\n");

    Result result;
    state st = SelectFiles(files, n, cap, &result);
    if (st != OK) {
        fprintf(stderr, "Ошибка при выборе файлов\n");
        for (int i = 0; i < n; i++) {
            free(files[i].name);
        }
        free(files);
        return st;
    }

    printf("TOTAL_SIZE %d\n", result.size);
    printf("TOTAL_IMPORTANCE %d\n", result.importance);
    printf("FILES %d\n", result.count);
    for (int i = 0; i < result.count; i++) {
        printf("%s\n", result.files[i].name);
    }

    free(result.files);
    for (int i = 0; i < n; i++) {
        free(files[i].name);
    }
    free(files);
    return OK;
}