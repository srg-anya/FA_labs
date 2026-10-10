#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define MAX 1000000000

typedef enum state {
    OK,
    ERROR_NULL,
    ERROR_RANGE,
    ERROR_OVERFLOW,
    ERROR_INPUT
} state;



state Number(const char* n, int* res) {
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

state FillArr(int a, int b, int* array, int size) {
    if (array == NULL) return ERROR_NULL;
    if (size <= 0) return ERROR_RANGE;
    for (int i = 0; i < size; ++i) {
        array[i] = a + rand() % (b - a + 1);
    }
    return OK;
}


state Swap(int* array, int size) {
    if (array == NULL) return ERROR_NULL;
    if (size <= 0) return ERROR_RANGE;
    int indMax = 0;
    int indMin = 0;
    for (int i = 0; i < size; ++i) {
        if(array[i] < array[indMin]) {
            indMin = i;
        }
        if (array[i] > array[indMax]) {
            indMax = i;
        }
    }

    if (indMax != indMin) {
        int temp = array[indMin];
        array[indMin] = array[indMax];
        array[indMax] = temp;
    }

    return OK;
}


int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Аргументов строки должно быть 3\n");
        return ERROR_INPUT;
    }
    
    int a;
    state st = Number(argv[1], &a);
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

    int b;
    st = Number(argv[2], &b);
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

    if (a > b) {
        fprintf(stderr, "Должно быть a <= b\n");
        return ERROR_RANGE;
    }

    srand((unsigned int)time(NULL));

    int size = 13;
    int array[size];
    st = FillArr(a, b, array, size);
    if (st != OK) {
        if (st == ERROR_NULL) {
            fprintf(stderr, "NULL-указатель\n");
        } else if (st == ERROR_INPUT) {
            fprintf(stderr, "Рамзер должен быть > 0\n");
        }
        return st;
    }

    printf("Изначальный массив: ");
    for (int i = 0; i < size; ++i) {
        printf("%d", array[i]);
        if (i < size - 1) {
            printf(" ");
        }
    }
    printf("\n");

    st = Swap(array, size);
    if (st != OK) {
        if (st == ERROR_NULL) {
            fprintf(stderr, "NULL-указатель\n");
        } else if (st == ERROR_INPUT) {
            fprintf(stderr, "Длина массива должна быть > 0\n");
        }
        return st;
    }

    printf("Массив после swap:  ");
    for (int i = 0; i < size; ++i) {
        printf("%d", array[i]);
        if (i < size - 1) {
            printf(" ");
        }
    }
    printf("\n");

    return OK;
}