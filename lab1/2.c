#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#define MAX 100000000000
#define MIN -1000000000000
#define PI 3.14159265358979323846

typedef enum state {
    OK,
    ERROR_NULL,
    ERROR_RANGE,
    ERROR_OVERFLOW,
    ERROR_INPUT
} state;

state NumberD(const char *n, double *res) {
    if (n == NULL || res == NULL) return ERROR_NULL;
    long long sign = 1;
    long long i = 0;
    if (n[0] == '-') {
        i = 1;
        sign = -1;
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
            t = 1;
        } else {
            return ERROR_INPUT;
        }
        ++i;
    }
    if (!digit) return ERROR_INPUT;
    *res = (x + f) * sign;
    return OK;
}

double ELimit(const double eps) {
    double cur;
    double prev = -5.0;
    for (long long n = 1;; n *= 2) {
        cur = pow(1.0 + 1.0 / n, (double)n);
        if (fabs(cur - prev) < eps) {
            break;
        }
        prev = cur;
    }
    return cur;
}

double ESum(const double eps) {
    double next;
    double sum = 1.0;
    double cur = 1.0;
    for (long long n = 1;; ++n) {
        cur /= n;
        next = sum + cur;
        if (fabs(sum - next) < eps) {
            break;
        }
        sum = next;
    }
    return sum;
}

double EEqua(const double eps) {
    double x = 2.0;
    double prev = 0.0;
    while (fabs(x - prev) >= eps) {
        prev = x;
        x = x + (1.0 - log(x));
    }
    return x;
}


double PiLimit(const double eps) {
    double cur = 1.0;
    double prev = -5.0;
    double eps1 = eps * eps;
    for (long long n = 1;; ++n) {
        cur *= 4.0 * n * n / ((2.0 * n - 1) * (2.0 * n + 1));
        if (fabs(cur - prev) < eps1) {
            break;
        }
        prev = cur;
    }
    return 2 * cur;
}

double PiSum(const double eps) {
    double next;
    double sum = 0.0;
    double sign = 1.0;
    for (long long n = 1;; ++n) {
        next = sum + sign / (2.0 * n - 1);
        if (fabs(sum - next) < eps / 4.0) {
            sum = next;
            break;
        }
        sum = next;
        sign *= -1.0;
    }
    return 4.0 * sum;
}

double PiEqua(const double eps) {
    double x = 3.0;
    double prev = 0.0;
    while (fabs(x - prev) >= eps) {
        prev = x;
        x = x + 1.0 + cos(x);
    }
    return x;
}

double Ln2Limit(const double eps) {
    double cur = 1.0;
    double prev = 0.0;
    for (long long n = 1;; n *= 2) {
        cur = n * (pow(2.0, 1.0 / n) - 1);
        if (fabs(cur - prev) < eps) {
            break;
        }
        prev = cur;
    }
    return cur;
}

double Ln2Sum(const double eps) {
    double next;
    double sum = 0.0;
    double sign = 1.0;
    for (long long n = 1;; ++n) {
        next = sum + sign / n;
        if (fabs(sum - next) < eps) {
            sum = next;
            break;
        }
        sum = next;
        sign *= -1.0;
    }
    return sum;
}

double Ln2Equa(const double eps) {
    double x = 0.5;
    double prev = 0.0;
    while (fabs(x - prev) >= eps) {
        prev = x;
        x = x + 1.0 - exp(x) / 2.0;
    }
    return x;
}

double Sqrt2Limit(const double eps) {
    double cur;
    double prev = -0.5;
    for (long long n = 1;; ++n) {
        cur = prev - pow(prev, 2) / 2 + 1;
        if (fabs(cur - prev) < eps) {
            break;
        }
        prev = cur;
    }
    return cur;
}

double Sqrt2Mult(const double eps) {
    double next;
    double p = 1;
    for (long long n = 2;; ++n) {
        next = p * pow(2.0, 1.0 / (1ULL << n));
        if (fabs(p - next) < eps) {
            p = next;
            break;
        }
        p = next;
    }
    return p;
}

double Sqrt2Equa(const double eps) {
    double x = 0.5;
    double prev = 0.0;
    while (fabs(x - prev) >= eps) {
        prev = x;
        x = x + 1.0 - x * x / 2.0;
    }
    return x;
}

double YLimit(const double eps) {
    double cur = 0.0;
    double prev = MIN;
    for (long long n = 1; n <= 200; ++n) {
        double sum = 0.0;
        double C = 1.0;
        double ln = 0.0;
        int sign = 1;
        for (long long k = 1; k <= n; ++k) {
            C *= (n - k + 1) / (double)k;
            ln += log((double)k);
            sign = -sign;
            sum += C * sign / (double)k * ln;
        }
        cur = sum;
        if (fabs(cur - prev) < eps) {
            break;
        }
        prev = cur;
    }
    return cur;
}

double YSum(const double eps, double pi) {
    double next;
    double sum = -PI * PI / 6.0;
    for (long long n = 2;; ++n) {
        long long r = (long long)sqrt((double)n);
        next = sum + 1.0 / (r * r) - 1.0 / n;
        if (fabs(sum - next) < eps) {
            sum = next;
            break;
        }
        sum = next;
    }
    return sum;
}

int Prime(long long x) {
    for (long long i = 2; i * i <= x; ++i) {
        if (x % i == 0) {
            return 0;
        }
    }
    return 1;
}

double YEqua(const double eps) {
    int t = 10.0;
    double x = 0.0;
    double prev = MIN;
    while (fabs(x - prev) >= eps) {
        prev = x;
        t *= 2;
        double P = 1.0;
        for (int p = 2; p <= t; ++p) {
            if (Prime(p)) P *= (p - 1.0) / p;
        }
        x = log(t) * P;
    }
    return -log(x);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Аргументов строки должно быть 2\n");
        return -1;
    }

    double eps;
    state st = NumberD(argv[1], &eps);
    if (st != OK) {
        fprintf(stderr, "Неверная форма записи эпсилон\n");
        return st;
    }

    printf("e\n");
    double eLim = ELimit(eps);
    double eSum = ESum(eps);
    double eEqua = EEqua(eps);
    printf("%.10f | %.10f | %.10f\n", eLim, eSum, eEqua);

    printf("pi\n");
    double piLim = PiLimit(eps);
    double piSum = PiSum(eps);
    double piEqua = PiEqua(eps);
    printf("%.10f | %.10f | %.10f\n", piLim, piSum, piEqua);

    printf("ln2\n");
    double ln2Lim = Ln2Limit(eps);
    double ln2Sum = Ln2Sum(eps);
    double ln2Equa = Ln2Equa(eps);
    printf("%.10f | %.10f | %.10f\n", ln2Lim, ln2Sum, ln2Equa);

    printf("sqrt(2)\n");
    double sqrt2Lim = Sqrt2Limit(eps);
    double sqrt2Mult = Sqrt2Mult(eps);
    double sqrt2Equa = Sqrt2Equa(eps);
    printf("%.10f | %.10f | %.10f\n", sqrt2Lim, sqrt2Mult, sqrt2Equa);

    printf("y\n");
    double yLim = YLimit(eps);
    double ySum = YSum(eps, piLim);
    double yEqua = YEqua(eps);
    printf("%.10f | %.10f | %.10f\n", yLim, ySum, yEqua);
}