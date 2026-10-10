#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum state {
    OK,
    ERROR_RANGE,
    ERROR_NULL,
    ERROR_INPUT,
    ERROR_OVERFLOW
} state;

state NumberInt(int *n) {
    if (n == NULL)
        return ERROR_NULL;
    int x;
    if (scanf("%d", &x) != 1)
        return ERROR_INPUT;
    *n = x;
    return OK;
}

state NumberD(double *n) {
    if (n == NULL)
        return ERROR_NULL;
    double x;
    if (scanf("%lf", &x) != 1)
        return ERROR_INPUT;
    *n = x;
    return OK;
}


state IsConvex(int n, int *res, ...) {
    if (res == NULL)
        return ERROR_NULL;
    if (n < 3 || n > 5)
        return ERROR_RANGE;

    va_list param;
    va_start(param, res);

    double X[5], Y[5];
    for (int i = 0; i < n; ++i) {
        X[i] = va_arg(param, double);
        Y[i] = va_arg(param, double);
    }
    va_end(param);

    int sign = 0;
    for (int i = 0; i < n; ++i) {
        int i1 = (i + 1) % n;
        int i2 = (i + 2) % n;

        double ABx = X[i1] - X[i];
        double ABy = Y[i1] - Y[i];
        double BCx = X[i2] - X[i1];
        double BCy = Y[i2] - Y[i1];

        double cross = ABx * BCy - ABy * BCx;

        if (cross > 0) {
            if (sign == 0)
                sign = 1;
            else if (sign == -1) {
                *res = 0;
                return OK;
            }
        } else if (cross < 0) {
            if (sign == 0)
                sign = -1;
            else if (sign == 1) {
                *res = 0;
                return OK;
            }
        }
    }

    if (sign != 0) {
        *res = 1;
    } else {
        *res = 0;
    }
    return OK;
}

void DemoIsConvex() {
    int n;
    int res;
    state st;

    printf("Введите количество вершин (3-5): ");
    if (NumberInt(&n) != OK) {
        fprintf(stderr, "Некорректный ввод\n");
        return;
    }
    if (n < 3 || n > 5) {
        fprintf(stderr, "Количество вершин должно быть от 3 до 5\n");
        return;
    }

    double X[5], Y[5];
    printf("Введите координаты %d вершин x и y:\n", n);
    for (int i = 0; i < n; ++i) {
        if (NumberD(&X[i]) != OK || NumberD(&Y[i]) != OK) {
            fprintf(stderr, "Некорректный ввод\n");
            return;
        }
    }

    if (n == 3) {
        st = IsConvex(3, &res, X[0], Y[0], X[1], Y[1], X[2], Y[2]);
    } else if (n == 4) {
        st = IsConvex(4, &res, X[0], Y[0], X[1], Y[1], X[2], Y[2], X[3], Y[3]);
    } else {
        st = IsConvex(5, &res, X[0], Y[0], X[1], Y[1], X[2], Y[2], X[3], Y[3], X[4], Y[4]);
    }

    if (st == OK) {
        if (res) {
            printf("Многоугольник выпуклый\n");
        } else {
            printf("Многоугольник не выпуклый\n");
        }
    } else if (st == ERROR_NULL) {
        fprintf(stderr, "Нулевой указатель\n");
    } else if (st == ERROR_RANGE) {
        fprintf(stderr, "Нужно минимум 3 вершины\n");
    } else {
        fprintf(stderr, "Ошибка\n");
    }
}

state PolyValue(double x, int n, double *res, ...) {
    if (res == NULL)
        return ERROR_NULL;
    if (n < 0 || n > 5)
        return ERROR_RANGE;

    va_list param;
    va_start(param, res);

    *res = 0.0;
    for (int i = n; i >= 0; --i) {
        double coef = va_arg(param, double);
        *res = *res * x + coef;
    }
    va_end(param);
    return OK;
}

void DemoPolyValue() {
    int n;
    double x;
    state st;

    printf("Введите точку x: ");
    if (NumberD(&x) != OK) {
        fprintf(stderr, "Некорректный ввод\n");
        return;
    }
    printf("Введите степень (0-5): ");
    if (NumberInt(&n) != OK) {
        fprintf(stderr, "Некорректный ввод\n");
        return;
    }
    if (n < 0 || n > 5) {
        fprintf(stderr, "Степень должна быть от 0 до 5\n");
        return;
    }

    double polinom[6];
    printf("Введите %d коэффициентов от старшей степени к младшей:\n", n + 1);
    for (int i = 0; i < n + 1; ++i) {
        if (NumberD(&polinom[i]) != OK) {
            fprintf(stderr, "Некорректный ввод\n");
            return;
        }
    }

    double res;
    if (n == 0) {
        st = PolyValue(x, 0, &res, polinom[0]);
    } else if (n == 1) {
        st = PolyValue(x, 1, &res, polinom[0], polinom[1]);
    } else if (n == 2) {
        st = PolyValue(x, 2, &res, polinom[0], polinom[1], polinom[2]);
    } else if (n == 3) {
        st = PolyValue(x, 3, &res, polinom[0], polinom[1], polinom[2], polinom[3]);
    } else if (n == 4) {
        st = PolyValue(x, 4, &res, polinom[0], polinom[1], polinom[2], polinom[3], polinom[4]);
    } else {
        st = PolyValue(x, 5, &res, polinom[0], polinom[1], polinom[2], polinom[3], polinom[4], polinom[5]);
    }

    if (st == OK) {
        printf("Значение многочлена: %g\n", res);
    } else if (st == ERROR_NULL) {
        fprintf(stderr, "Нулевой указатель\n");
    } else if (st == ERROR_RANGE) {
        fprintf(stderr, "Степень вне диапазона\n");
    } else {
        fprintf(stderr, "Ошибка вычисления\n");
    }
}

int IsKaprekar(long long num, int base) {
    if (num < 0)
        return 0;
    if (num == 0)
        return 1;

    long long sq = num * num;

    int digits[128];
    int len = 0;
    long long v1 = sq;
    if (v1 == 0) {
        digits[len++] = 0;
    }
    while (v1 > 0) {
        digits[len++] = v1 % base;
        v1 /= base;
    }

    for (int k = 1; k < len; ++k) {
        long long right = 0, mult = 1;
        for (int i = 0; i < k; ++i) {
            right += digits[i] * mult;
            mult *= base;
        }
        long long left = 0;
        mult = 1;
        for (int i = k; i < len; ++i) {
            left += digits[i] * mult;
            mult *= base;
        }
        if (right > 0 && left + right == num)
            return 1;
    }
    return 0;
}

long long InBase(char *s, int base) {
    if (s == NULL || base < 2 || base > 36)
        return -1;
    long long res = 0;
    for (int i = 0; s[i] != '\0'; ++i) {
        int d;
        char c = s[i];
        if (c >= '0' && c <= '9') {
            d = c - '0';
        } else if (c >= 'A' && c <= 'Z') {
            d = c - 'A' + 10;
        } else if (c >= 'a' && c <= 'z') {
            d = c - 'a' + 10;
        } else {
            return -1;
        }
        if (d >= base) {
            return -1;
        }
        res = res * base + d;
    }
    return res;
}

state FindKaprekar(int base, int *res, int count, ...) {
    if (res == NULL) return ERROR_NULL;
    if (base < 2 || base > 36) return ERROR_RANGE;
    if (count < 1 || count > 5) return ERROR_RANGE;

    va_list param;
    va_start(param, count);

    *res = 0;
    for (int i = 0; i < count; ++i) {
        char *s = va_arg(param, char *);
        long long num = InBase(s, base);
        if (num < 0) {
            va_end(param);
            return ERROR_INPUT;
        }
        if (IsKaprekar(num, base)) {
            *res = 1;
            break;
        }
    }
    va_end(param);
    return OK;
}

void DemoFindKaprekar() {
    int base;
    int count;
    int res;
    state st;
    char buf[5][64];

    printf("Введите основание системы счисления (2-36): ");
    if (NumberInt(&base) != OK) {
        fprintf(stderr, "Некорректный ввод\n");
        return;
    }
    if (base < 2 || base > 36) {
        fprintf(stderr, "Основание от 2 до 36\n");
        return;
    }

    printf("Сколько строк (1-5): ");
    if (NumberInt(&count) != OK) {
        fprintf(stderr, "Некорректный ввод\n");
        return;
    }
    if (count < 1 || count > 5) {
        fprintf(stderr, "Количество строк от 1 до 5\n");
        return;
    }

    printf("Введите %d чисел в системе счисления %d (цифры 0-9, A-Z):\n", count, base);
    for (int i = 0; i < count; ++i) {
        if (scanf("%63s", buf[i]) != 1) {
            fprintf(stderr, "Некорректный ввод\n");
            return;
        }
    }

    if (count == 1) {
        st = FindKaprekar(base, &res, 1, buf[0]);
    } else if (count == 2) {
        st = FindKaprekar(base, &res, 2, buf[0], buf[1]);
    } else if (count == 3) {
        st = FindKaprekar(base, &res, 3, buf[0], buf[1], buf[2]);
    } else if (count == 4) {
        st = FindKaprekar(base, &res, 4, buf[0], buf[1], buf[2], buf[3]);
    } else {
        st = FindKaprekar(base, &res, 5, buf[0], buf[1], buf[2], buf[3], buf[4]);
    }

    if (st == OK) {
        if (res) {
            printf("Среди чисел есть число Капрекара\n");
        } else {
            printf("Среди чисел нет числа Капрекара\n");
        }
    } else if (st == ERROR_NULL) {
        fprintf(stderr, "Нулевой указатель\n");
    } else if (st == ERROR_RANGE) {
        fprintf(stderr, "Неверное основание или количество\n");
    } else {
        fprintf(stderr, "Некорректное число в строке\n");
    }
}

state GeoMean(int count, double* res, ...) {
    if (res == NULL)
        return ERROR_NULL;
    if (count < 1 || count > 5)
        return ERROR_RANGE;

    va_list param;
    va_start(param, res);

    double p = 1.0;
    for (int i = 0; i < count; ++i) {
        double v = va_arg(param, double);
        if (v < 0) {
            va_end(param);
            return ERROR_INPUT;
        }
        p *= v;
    }
    va_end(param);

    *res = pow(p, 1.0 / count);
    return OK;
}

void DemoGeoMean() {
    int n;
    double res;
    double vals[5];
    state st;

    printf("Сколько чисел (1-5): ");
    if (NumberInt(&n) != OK) {
        fprintf(stderr, "Некорректный ввод\n");
        return;
    }
    if (n < 1 || n > 5) {
        fprintf(stderr, "Количество чисел должно быть от 1 до 5\n");
        return;
    }

    printf("Введите %d чисел:\n", n);
    for (int i = 0; i < n; ++i) {
        if (NumberD(&vals[i]) != OK) {
            fprintf(stderr, "Некорректный ввод\n");
            return;
        }
    }

    if (n == 1) {
        st = GeoMean(1, &res, vals[0]);
    } else if (n == 2) {
        st = GeoMean(2, &res, vals[0], vals[1]);
    } else if (n == 3) {
        st = GeoMean(3, &res, vals[0], vals[1], vals[2]);
    } else if (n == 4) {
        st = GeoMean(4, &res, vals[0], vals[1], vals[2], vals[3]);
    } else {
        st = GeoMean(5, &res, vals[0], vals[1], vals[2], vals[3], vals[4]);
    }

    if (st == OK) {
        printf("Среднее геометрическое: %g\n", res);
    } else if (st == ERROR_NULL) {
        fprintf(stderr, "Нулевой указатель\n");
    } else if (st == ERROR_RANGE) {
        fprintf(stderr, "Количество вне диапазона\n");
    } else {
        fprintf(stderr, "Отрицательное число\n");
    }
}

state FastPow(double x, int n, double *res) {
    if (res == NULL) return ERROR_NULL;
    if (n == 0) {
        *res = 1.0;
        return OK;
    }
    if (n < 0) {
        if (x == 0.0) return ERROR_INPUT;
        double v1;
        state st = FastPow(x, -n, &v1);
        if (st != OK) return st;
        *res = 1.0 / v1;
        return OK;
    }
    if (n % 2 == 0) {
        double v2;
        state st = FastPow(x, n / 2, &v2);
        if (st != OK)
            return st;
        *res = v2 * v2;
    } else {
        double v3;
        state st = FastPow(x, n - 1, &v3);
        if (st != OK)
            return st;
        *res = x * v3;
    }
    return OK;
}

void DemoFastPow() {
    double x;
    double res;
    int n;
    state st;

    printf("Введите основание x: ");
    if (NumberD(&x) != OK) {
        fprintf(stderr, "Некорректный ввод\n");
        return;
    }
    printf("Введите целую степень n: ");
    if (NumberInt(&n) != OK) {
        fprintf(stderr, "Некорректный ввод\n");
        return;
    }

    st = FastPow(x, n, &res);
    if (st == OK) {
        printf("%g^%d = %g\n", x, n, res);
    } else if (st == ERROR_NULL) {
        fprintf(stderr, "Нулевой указатель\n");
    } else {
        fprintf(stderr, "0 нельзя возводить в отрицательную степень\n");
    }
}

state Dihotomiya(double a, double b, double eps, double (*f)(double), double *res) {
    if (f == NULL || res == NULL) return ERROR_NULL;
    if (eps <= 0) return ERROR_RANGE;

    double fa = f(a);
    double fb = f(b);
    if (fa * fb > 0) return ERROR_INPUT;

    while (b - a > eps) {
        double mid = (a + b) / 2.0;
        double fm = f(mid);
        if (fm == 0.0) {
            *res = mid;
            return OK;
        }
        if (fa * fm < 0) {
            b = mid;
            fb = fm;
        } else {
            a = mid;
            fa = fm;
        }
    }
    *res = (a + b) / 2.0;
    return OK;
}

double F1(double x) { 
    return x * x - 2.0; 
}
double F2(double x) {
    return cos(x) - x;
}
double F3(double x) {
    return x * x * x - x - 1.0;
}

void DemoDihotomiya() {
    double a;
    double b;
    double eps;
    double res;
    int eq;
    state st;

    printf("Выберите уравнение:\n");
    printf("1: x^2 - 2 = 0\n");
    printf("2: cos(x) - x = 0\n");
    printf("3: x^3 - x - 1 = 0\n");
    printf("Номер: ");
    if (NumberInt(&eq) != OK) {
        fprintf(stderr, "Некорректный ввод\n");
        return;
    }

    printf("Введите левую границу a: ");
    if (NumberD(&a) != OK) {
        fprintf(stderr, "Некорректный ввод\n");
        return;
    }
    printf("Введите правую границу b: ");
    if (NumberD(&b) != OK) {
        fprintf(stderr, "Некорректный ввод\n");
        return;
    }
    printf("Введите точность eps: ");
    if (NumberD(&eps) != OK) {
        fprintf(stderr, "Некорректный ввод\n");
        return;
    }

    double (*f)(double) = NULL;
    if (eq == 1) {
        f = F1;
    } else if (eq == 2) {
        f = F2;
    } else if (eq == 3) {
        f = F3;
    } else {
        fprintf(stderr, "Неверный номер уравнения\n");
        return;
    }

    st = Dihotomiya(a, b, eps, f, &res);
    if (st == OK) {
        printf("Корень: %.10f\n", res);
    } else if (st == ERROR_NULL) {
        fprintf(stderr, "Нулевой указатель\n");
    } else if (st == ERROR_RANGE) {
        fprintf(stderr, "Точность должна быть > 0\n");
    } else {
        fprintf(stderr, "Корень не отделён (f(a) * f(b) > 0)\n");
    }
}

int main() {
    int c = 0;

    printf("Выберите функцию (1-6): ");
    if (NumberInt(&c) != OK) {
        fprintf(stderr, "Некорректный ввод\n");
        return ERROR_INPUT;
    }

    if (c == 1) {
        DemoIsConvex();
    } else if (c == 2) {
        DemoPolyValue();
    } else if (c == 3) {
        DemoFindKaprekar();
    } else if (c == 4) {
        DemoGeoMean();
    } else if (c == 5) {
        DemoFastPow();
    } else if (c == 6) {
        DemoDihotomiya();
    } else {
        fprintf(stderr, "Неверный номер функции\n");
        return ERROR_RANGE;
    }

    return OK;
}