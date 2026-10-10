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


int GetSize() {
    return 10 + rand() % (10000 - 10 + 1);
}

state FillArr(int a, int b, int* array, int size) {
    if (array == NULL) return ERROR_NULL;
    if (size <= 0) return ERROR_RANGE;
    for (int i = 0; i < size; ++i) {
        array[i] = a + rand() % (b - a + 1);
    }
    return OK;
}

state BinSearch(int* a, int size, int x) {
    int left = 0;
    int right = size;
    while (left < right) {
        int mid = (left + right) / 2;
        if (x <= a[mid]) {
            right = mid;
        } else {
            left = mid + 1;
        }
    }
    return left;
}

void Merge(int* a, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    int* L = (int*)malloc(n1 * sizeof(int));
    int* R = (int*)malloc(n2 * sizeof(int));
    if (L == NULL || R == NULL) {
        free(L);
        free(R);
        return;
    }

    for (int i = 0; i < n1; ++i) {
        L[i] = a[left + i];
    }

    for (int j = 0; j < n2; ++j) {
        R[j] = a[mid + 1 + j];
    }

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            a[k++] = L[i++];
        } else {
            a[k++] = R[j++];
        }
    }
    while (i < n1) {
        a[k++] = L[i++];
    }
    while (j < n2) {
        a[k++] = R[j++];
    }

    free(L);
    free(R);
}

void MergeSort(int* a, int left, int right) {
    if (left >= right) return;

    int mid = (left + right) / 2;
    MergeSort(a, left, mid);
    MergeSort(a, mid + 1, right);
    Merge(a, left, mid, right);
}

state BuildC(int* A, int* B, int sizeA, int sizeB, int* C) {
    if (A == NULL || B == NULL || C == NULL) return ERROR_NULL;
    if (sizeA <= 0 || sizeB <= 0) return ERROR_RANGE;

    MergeSort(B, 0, sizeB - 1);
    int mindist;
    for (int i = 0; i < sizeA; ++i) {
        int ind = BinSearch(B, sizeB, A[i]);
        if (ind == 0) {
            mindist = B[0];
        } else if (ind == sizeB) {
            mindist = B[sizeB - 1];
        } else {
            if (A[i] - B[ind - 1] <= B[ind] - A[i]) {
                mindist = abs(A[i] - B[ind - 1]);
            } else {
                mindist = abs(B[ind] - A[i]);
            }
        }
        C[i] = A[i] + abs(A[i] - mindist);
    }
    return OK;
}

void PrintArr(char* name, int* array, int size) {
    printf("%s: ", name);
    for (int i = 0; i < size; ++i) {
        printf("%d", array[i]);
        if (i < size - 1) printf(" ");
    }
    printf("\n");
}


int main() {    
    state st;
    srand((unsigned int)time(NULL));

    int sizeA = GetSize();
    int* A = (int*)malloc(sizeA * sizeof(int));
    if (A == NULL) {
        fprintf(stderr, "Ошибка выделения памяти под A\n");
        return ERROR_NULL;
    }
    st = FillArr(-1000, 1000, A, sizeA);
    if (st != OK) {
        if (st == ERROR_NULL) {
            fprintf(stderr, "NULL-указатель\n");
        } else if (st == ERROR_INPUT) {
            fprintf(stderr, "Размер должен быть > 0\n");
        }
        return st;
    }


    int sizeB = GetSize();
    int* B = (int*)malloc(sizeB * sizeof(int));
    if (B == NULL) {
        fprintf(stderr, "Ошибка выделения памяти под B\n");
        free(A);
        return ERROR_NULL;
    }
    st = FillArr(-1000, 1000, B, sizeB);
    if (st != OK) {
        if (st == ERROR_NULL) {
            fprintf(stderr, "NULL-указатель\n");
        } else if (st == ERROR_INPUT) {
            fprintf(stderr, "Размер должен быть > 0\n");
        }
        return st;
    }


    int* C = (int*)malloc(sizeA * sizeof(int));
    if (C == NULL) {
        fprintf(stderr, "Ошибка выделения памяти под C\n");
        free(A);
        free(B);
        return ERROR_NULL;
    }
    st = BuildC(A, B, sizeA, sizeB, C);
    if (st != OK) {
        if (st == ERROR_NULL) {
            fprintf(stderr, "NULL-указатель\n");
        } else if (st == ERROR_INPUT) {
            fprintf(stderr, "Длина массива должна быть > 0\n");
        }
        return st;
    }

    printf("Размер A: %d\n", sizeA);
    PrintArr("A", A, sizeA);
    printf("Размер B: %d\n", sizeB);
    printf("\n");
    PrintArr("B", B, sizeB);
    PrintArr("C", C, sizeA);

    free(A);
    free(B);
    free(C);
    return OK;
}