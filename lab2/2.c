#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#define MAX 1000000

typedef enum state {
    OK,
    ERROR_NULL,
    ERROR_OVERFLOW,
    ERROR_INPUT,
    ERROR_MEMORY
} state;

typedef struct Employee {
    int id;
    char *name;
    char *surname;
    double wages;
} Employee;

state Flag(const char *f, char *res) {
    if (f == NULL || res == NULL) {
        return ERROR_INPUT;
    }
    if (f[2] != '\0' && f[0] != '-' && f[0] != '/') {
        return ERROR_INPUT;
    }
    if (f[1] != 'a' && f[1] != 'd') {
        return ERROR_INPUT;
    }
    *res = f[1];
    return OK;
}

state CheckFiles(const char *in, const char *out, int *flag) {
    if (in == NULL || out == NULL || flag == NULL) {
        return ERROR_NULL;
    }

    int inFd = open(in, O_RDONLY);
    if (inFd < 0) {
        *flag = 1;
        return ERROR_INPUT;
    }

    int outFd = open(out, O_WRONLY | O_CREAT, 0600);
    if (outFd < 0) {
        *flag = 2;
        close(inFd);
        return ERROR_INPUT;
    }

    struct stat stIn;
    struct stat stOut;
    if (fstat(inFd, &stIn) != 0 || fstat(outFd, &stOut) != 0) {
        *flag = 3;
        close(inFd);
        close(outFd);
        return ERROR_INPUT;
    }

    state state = OK;
    if (stIn.st_dev == stOut.st_dev && stIn.st_ino == stOut.st_ino) {
        *flag = 4;
        state = ERROR_INPUT;
    }

    close(inFd);
    close(outFd);
    return state;
}

state ID(const char *n, int *res) {
    if (n == NULL || res == NULL) {
        return ERROR_NULL;
    }
    int i = 0;
    if (n[0] == '-') {
        return ERROR_INPUT;
    } else if (n[0] == '+') {
        i = 1;
    }
    if (n[i] == '\0') {
        return ERROR_INPUT;
    }
    int x = 0;
    while (n[i] != '\0') {
        if (n[i] < '0' || n[i] > '9') {
            return ERROR_INPUT;
        }
        if (x > (MAX - (n[i] - '0')) / 10) {
            return ERROR_OVERFLOW;
        }
        x = x * 10 + (n[i] - '0');
        ++i;
    }
    *res = x;
    return OK;
}

state Wages(const char *n, double *res) {
    if (n == NULL || res == NULL) {
        return ERROR_NULL;
    }
    int i = 0;
    if (n[0] == '-') {
        return ERROR_INPUT;
    } else if (n[0] == '+') {
        i = 1;
    }
    if (n[i] == '\0') {
        return ERROR_INPUT;
    }
    double x = 0.0;
    double f = 0.0;
    double c = 1.0;
    int digit = 0;
    int t = 0;
    while (n[i] != '\0') {
        if (n[i] >= '0' && n[i] <= '9') {
            if (!t) {
                x = x * 10.0 + (n[i] - '0');
            } else {
                c *= 0.1;
                f = f + (n[i] - '0') * c;
            }

            digit = 1;
        } else if (n[i] == '.') {
            if (t) {
                return ERROR_INPUT;
            }
            if (!digit) {
                return ERROR_INPUT;
            }
            t = 1;
        } else {
            return ERROR_INPUT;
        }
        ++i;
    }
    if (!digit) {
        return ERROR_INPUT;
    }
    *res = (x + f);
    return OK;
}

state Name(const char *str) {
    if (str == NULL) {
        return ERROR_NULL;
    }
    if (str[0] == '\0') {
        return ERROR_INPUT;
    }
    for (const char *p = str; *p; ++p) {
        if (!isalpha((unsigned char)*p)) {
            return ERROR_INPUT;
        }
    }
    return OK;
}

state UpdateCap(Employee **arr, int *cap) {
    if (arr == NULL || cap == NULL || *arr == NULL) {
        return ERROR_NULL;
    }

    int ncap = (*cap) * 2;
    Employee *tmp = (Employee *)realloc(*arr, ncap * sizeof(Employee));
    if (tmp == NULL) {
        return ERROR_MEMORY;
    }

    *arr = tmp;
    *cap = ncap;
    return OK;
}

void Free(Employee *e, int n) {
    if (e == NULL) {
        return;
    }
    for (int i = 0; i < n; ++i) {
        free(e[i].name);
        free(e[i].surname);
    }
    free(e);
}

state ReadFile(char *inFile, Employee **employees, int *n) {
    if (inFile == NULL || employees == NULL || n == NULL) {
        return ERROR_NULL;
    }
    FILE *in = fopen(inFile, "r");
    if (in == NULL) {
        return ERROR_INPUT;
    }

    int cap = 24;
    Employee *res = (Employee *)malloc(cap * sizeof(Employee));
    if (res == NULL) {
        fclose(in);
        return ERROR_MEMORY;
    }

    char line[1024];
    int i = 0;
    state st = OK;

    while (fgets(line, sizeof(line), in) != NULL) {
        line[strcspn(line, "\r\n")] = '\0';
        if (line[0] == '\0') {
            continue;
        }
        int count = 0;
        int id = 0;
        double wages = 0.0;
        char *name = NULL;
        char *surname = NULL;
        char *c = line;
        while (*c && st == OK) {
            while (*c == ' ' || *c == '\t') {
                ++c;
            }
            char *str = c;
            while (*c && *c != ' ' && *c != '\t') {
                ++c;
            }
            if (*c) {
                *c++ = '\0';
            }

            ++count;
            if (count == 1) {
                st = ID(str, &id);
            } else if (count == 2) {
                st = Name(str);
                if (st == OK) {
                    name = (char *)malloc(strlen(str) + 1);
                    if (name == NULL) {
                        st = ERROR_MEMORY;
                    } else {
                        strcpy(name, str);
                    }
                }
            } else if (count == 3) {
                st = Name(str);
                if (st == OK) {
                    surname = (char *)malloc(strlen(str) + 1);
                    if (surname == NULL) {
                        st = ERROR_MEMORY;
                    } else {
                        strcpy(surname, str);
                    }
                }
            } else if (count == 4) {
                st = Wages(str, &wages);
            } else {
                st = ERROR_INPUT;
            }
        }

        if (st != OK || count != 4) {
            if (st == OK) {
                st = ERROR_INPUT;
            }
            free(name);
            free(surname);
            for (int k = 0; k < i; ++k) {
                free(res[k].name);
                free(res[k].surname);
            }
            free(res);
            fclose(in);
            return st;
        }

        if (i == cap) {
            state g = UpdateCap(&res, &cap);
            if (g != OK) {
                for (int k = 0; k < i; ++k) {
                    free(res[k].name);
                    free(res[k].surname);
                }
                free(res);
                fclose(in);
                return g;
            }
        }

        res[i].id = id;
        res[i].wages = wages;
        res[i].name = name;
        res[i].surname = surname;
        ++i;
    }
    fclose(in);
    *employees = res;
    *n = i;
    return OK;
}

int SortId(const void *a, const void *b) {
    const Employee *e1 = (const Employee *)a;
    const Employee *e2 = (const Employee *)b;

    if (e1->id < e2->id) {
        return -1;
    }
    if (e1->id > e2->id) {
        return 1;
    }

    return 0;
}

int A(const void *a, const void *b) {
    const Employee *e1 = (const Employee *)a;
    const Employee *e2 = (const Employee *)b;

    if (e1->wages < e2->wages) {
        return -1;
    }
    if (e1->wages > e2->wages) {
        return 1;
    }

    int f = strcmp(e1->surname, e2->surname);
    if (f != 0) {
        return f;
    }

    f = strcmp(e1->name, e2->name);
    if (f != 0) {
        return f;
    }

    if (e1->id < e2->id) {
        return -1;
    }
    if (e1->id > e2->id) {
        return 1;
    }

    return 0;
}

int D(const void *a, const void *b) {
    const Employee *e1 = (const Employee *)a;
    const Employee *e2 = (const Employee *)b;

    if (e1->wages > e2->wages) {
        return -1;
    }
    if (e1->wages < e2->wages) {
        return 1;
    }

    int f = -strcmp(e1->surname, e2->surname);
    if (f != 0) {
        return f;
    }

    f = -strcmp(e1->name, e2->name);
    if (f != 0) {
        return f;
    }

    if (e1->id > e2->id) {
        return -1;
    }
    if (e1->id < e2->id) {
        return 1;
    }
    return 0;
}

state WriteFile(char *outFile, Employee *employees, int n) {
    if (outFile == NULL) {
        return ERROR_NULL;
    }
    FILE *out = fopen(outFile, "w");
    if (out == NULL) {
        return ERROR_INPUT;
    }

    for (size_t i = 0; i < n; ++i) {
        if (fprintf(out, "%d %s %s %.10g\n", employees[i].id, employees[i].name,
                    employees[i].surname, employees[i].wages) < 0) {
            fclose(out);
            return ERROR_INPUT;
        }
    }

    fclose(out);
    return OK;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Введите строго по примеру: <исполняемый файл> <входной файл> <флаг> <выходной файл>\n");
        return ERROR_INPUT;
    }
    char flag = '\0';
    state st = Flag(argv[2], &flag);
    if (st != OK) {
        if (st == ERROR_INPUT) {
            fprintf(stderr, "Некорректный флаг %s\n", argv[1]);
            return st;
        }
    }
    int res = 0;
    st = CheckFiles(argv[1], argv[3], &res);
    if (st != OK) {
        if (res == 1) {
            fprintf(stderr, "Не удалось открыть входной файл: %s\n", argv[1]);
        } else if (res == 2) {
            fprintf(stderr, "Не удалось открыть/создать входной файл: %s\n",argv[3]);
        } else if (res == 3) {
            fprintf(stderr, "Не удалось получить информацию о файлах\n");
        } else if (res == 4) {
            fprintf(stderr, "Входной и выходной файлы не должны совпадать\n");
        }
        return ERROR_INPUT;
    }

    Employee *employees = NULL;
    int n = 0;
    st = ReadFile(argv[1], &employees, &n);
    if (st != OK) {
        if (st == ERROR_NULL) {
            fprintf(stderr, "NULL-указатель\n");
        } else if (st == ERROR_OVERFLOW) {
            fprintf(stderr, "Переполнение числа\n");
        } else if (st == ERROR_INPUT) {
            fprintf(stderr, "Некорректные данные во входном файле\n");
        } else if (st == ERROR_MEMORY) {
            fprintf(stderr, "Не хватило памяти\n");
        }
        return st;
    }

    qsort(employees, n, sizeof(Employee), SortId);
    for (int i = 0; i < n - 1; ++i) {
        if (employees[i].id == employees[i + 1].id) {
            fprintf(stderr, "ID сторудников должны различаться!\n");
            Free(employees, n);
            return ERROR_INPUT;
        }
    }

    if (flag == 'a') {
        qsort(employees, n, sizeof(Employee), A);
    } else if (flag == 'd') {
        qsort(employees, n, sizeof(Employee), D);
    }

    st = WriteFile(argv[3], employees, n);
    if (st != OK) {
        if (st == ERROR_NULL) {
            fprintf(stderr, "NULL-указатель\n");
        } else if (st == ERROR_INPUT) {
            fprintf(stderr, "Некорректные данные во входном файле\n");
        }
        Free(employees, n);
        return st;
    }

    Free(employees, n);
    return OK;
}