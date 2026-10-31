#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX 1000000

typedef enum state {
    OK,
    ERROR_NULL,
    ERROR_INPUT,
    ERROR_EOF
} state;


int FileSkipWs(FILE* f) {
    int c = fgetc(f);
    while (c != EOF && isspace(c)) {
        c = fgetc(f);
    }
    return c;
}

state FileInt(FILE* f, int* out) {
    int c = FileSkipWs(f);
    if (c == EOF) {
        return ERROR_EOF;
    }
    int sign = 1;
    if (c == '-') {
        sign = -1;
        c = fgetc(f);
    } else if (c == '+') {
        c = fgetc(f);
    }
    if (c < '0' || c > '9') {
        ungetc(c, f);
        return ERROR_INPUT;
    }
    int res = 0;
    while (c >= '0' && c <= '9') {
        if (res > (MAX - (c - '0')) / 10) {
            ungetc(c, f);
            return ERROR_INPUT;
        }
        res = res * 10 + (c - '0');
        c = fgetc(f);
    }
    if (c != EOF) {
        ungetc(c, f);
    }
    *out = sign * res;
    return OK;
}

state FileStr(FILE* f, char* out) {
    int c = FileSkipWs(f);
    if (c == EOF) {
        return ERROR_EOF;
    }
    int count = 0;
    while (c != EOF && !isspace(c)) {
        out[count++] = (char)c;
        c = fgetc(f);
    }
    if (c != EOF) {
        ungetc(c, f);
    }
    out[count] = '\0';
    if (count == 0) {
        return ERROR_INPUT;
    }
    return OK;
}

state FileRo(FILE* f, int* out) {
    int c = FileSkipWs(f);
    if (c == EOF) {
        return ERROR_EOF;
    }
    char buf[64];
    int len = 0;
    while (c != EOF && !isspace(c) && len < 63) {
        buf[len++] = (char)c;
        c = fgetc(f);
    }
    if (c != EOF) {
        ungetc(c, f);
    }
    buf[len] = '\0';
    if (len == 0) {
        return ERROR_INPUT;
    }
    int res = 0;
    int i = 0;
    while (i < len) {
        int val = 0;
        int step = 1;
        if (buf[i] == 'I') {
            if (i + 1 < len && buf[i + 1] == 'V') {
                val = 4;
                step = 2;
            } else if (i + 1 < len && buf[i + 1] == 'X') {
                val = 9;
                step = 2;
            } else {
                val = 1;
            }
        } else if (buf[i] == 'V') {
            val = 5;
        } else if (buf[i] == 'X') {
            if (i + 1 < len && buf[i + 1] == 'L') {
                val = 40;
                step = 2;
            } else if (i + 1 < len && buf[i + 1] == 'C') {
                val = 90;
                step = 2;
            } else {
                val = 10;
            }
        } else if (buf[i] == 'L') {
            val = 50;
        } else if (buf[i] == 'C') {
            if (i + 1 < len && buf[i + 1] == 'D') {
                val = 400;
                step = 2;
            } else if (i + 1 < len && buf[i + 1] == 'M') {
                val = 900;
                step = 2;
            } else {
                val = 100;
            }
        } else if (buf[i] == 'D') {
            val = 500;
        } else if (buf[i] == 'M') {
            val = 1000;
        } else {
            return ERROR_INPUT;
        }
        res += val;
        i += step;
    }
    *out = res;
    return OK;
}

state FileZeckendorf(FILE* f, unsigned int* out) {
    int c = FileSkipWs(f);
    if (c == EOF) {
        return ERROR_EOF;
    }
    char buf[64];
    int len = 0;
    while (c != EOF && !isspace(c) && len < 63) {
        buf[len++] = (char)c;
        c = fgetc(f);
    }
    if (c != EOF) {
        ungetc(c, f);
    }
    buf[len] = '\0';
    if (len < 2) {
        return ERROR_INPUT;
    }
    unsigned int fib[50];
    int n = 2;
    fib[0] = 1;
    fib[1] = 2;
    while (n < 50) {
        fib[n] = fib[n - 1] + fib[n - 2];
        n++;
    }
    unsigned int res = 0;
    for (int i = len - 2; i >= 0; i--) {
        if (buf[i] == '1') {
            if (i >= n) {
                return ERROR_INPUT;
            }
            res += fib[i];
        } else if (buf[i] != '0') {
            return ERROR_INPUT;
        }
    }
    *out = res;
    return OK;
}

state FileBase(FILE* f, int base, int* out) {
    if (base < 2 || base > 36) {
        return ERROR_INPUT;
    }
    int c = FileSkipWs(f);
    if (c == EOF) {
        return ERROR_EOF;
    }
    int sign = 1;
    if (c == '-') {
        sign = -1;
        c = fgetc(f);
    } else if (c == '+') {
        c = fgetc(f);
    }
    int res = 0;
    int hasDigits = 0;
    while (c != EOF) {
        int digit = -1;
        if (c >= '0' && c <= '9') {
            digit = c - '0';
        } else if (c >= 'a' && c <= 'z') {
            digit = c - 'a' + 10;
        } else if (c >= 'A' && c <= 'Z') {
            digit = c - 'A' + 10;
        } else {
            break;
        }
        if (digit < 0 || digit >= base) {
            break;
        }
        res = res * base + digit;
        hasDigits = 1;
        c = fgetc(f);
    }
    if (c != EOF) {
        ungetc(c, f);
    }
    if (!hasDigits) {
        return ERROR_INPUT;
    }
    *out = res * sign;
    return OK;
}


int StrSkipWs(char* s, size_t* ind) {
    int c = s[*ind];
    if (c == '\0') {
        return EOF;
    }
    while (c != EOF && isspace(c)) {
        (*ind)++;
        c = s[*ind];
        if (c == '\0') {
            return EOF;
        }
    }
    return c;
}

state StrInt(char* s, size_t* ind, int* out) {
    int c = StrSkipWs(s, ind);
    if (c == EOF) {
        return ERROR_EOF;
    }
    int sign = 1;
    if (c == '-') {
        sign = -1;
        (*ind)++;
        c = s[*ind];
        if (c == '\0') {
            c = EOF;
        }
    } else if (c == '+') {
        (*ind)++;
        c = s[*ind];
        if (c == '\0') {
            c = EOF;
        }
    }
    if (c < '0' || c > '9') {
        return ERROR_INPUT;
    }
    int res = 0;
    while (c >= '0' && c <= '9') {
        if (res > (MAX - (c - '0')) / 10) {
            return ERROR_INPUT;
        }
        res = res * 10 + (c - '0');
        (*ind)++;
        c = s[*ind];
        if (c == '\0') {
            c = EOF;
        }
    }
    *out = sign * res;
    return OK;
}

state StrStr(char* s, size_t* ind, char* out) {
    int c = StrSkipWs(s, ind);
    if (c == EOF) {
        return ERROR_EOF;
    }
    int count = 0;
    while (c != EOF && !isspace(c)) {
        out[count++] = (char)c;
        (*ind)++;
        c = s[*ind];
        if (c == '\0') {
            c = EOF;
        }
    }
    out[count] = '\0';
    if (count == 0) {
        return ERROR_INPUT;
    }
    return OK;
}

state StrRo(char* s, size_t* ind, int* out) {
    int c = StrSkipWs(s, ind);
    if (c == EOF) {
        return ERROR_EOF;
    }
    char buf[64];
    int len = 0;
    while (c != EOF && !isspace(c) && len < 63) {
        buf[len++] = (char)c;
        (*ind)++;
        c = s[*ind];
        if (c == '\0') {
            c = EOF;
        }
    }
    buf[len] = '\0';
    if (len == 0) {
        return ERROR_INPUT;
    }
    int res = 0;
    int i = 0;
    while (i < len) {
        int val = 0;
        int step = 1;
        if (buf[i] == 'I') {
            if (i + 1 < len && buf[i + 1] == 'V') {
                val = 4;
                step = 2;
            } else if (i + 1 < len && buf[i + 1] == 'X') {
                val = 9;
                step = 2;
            } else {
                val = 1;
            }
        } else if (buf[i] == 'V') {
            val = 5;
        } else if (buf[i] == 'X') {
            if (i + 1 < len && buf[i + 1] == 'L') {
                val = 40;
                step = 2;
            } else if (i + 1 < len && buf[i + 1] == 'C') {
                val = 90;
                step = 2;
            } else {
                val = 10;
            }
        } else if (buf[i] == 'L') {
            val = 50;
        } else if (buf[i] == 'C') {
            if (i + 1 < len && buf[i + 1] == 'D') {
                val = 400;
                step = 2;
            } else if (i + 1 < len && buf[i + 1] == 'M') {
                val = 900;
                step = 2;
            } else {
                val = 100;
            }
        } else if (buf[i] == 'D') {
            val = 500;
        } else if (buf[i] == 'M') {
            val = 1000;
        } else {
            return ERROR_INPUT;
        }
        res += val;
        i += step;
    }
    *out = res;
    return OK;
}

state StrZeckendorf(char* s, size_t* ind, unsigned int* out) {
    int c = StrSkipWs(s, ind);
    if (c == EOF) {
        return ERROR_EOF;
    }
    char buf[64];
    int len = 0;
    while (c != EOF && !isspace(c) && len < 63) {
        buf[len++] = (char)c;
        (*ind)++;
        c = s[*ind];
        if (c == '\0') {
            c = EOF;
        }
    }
    buf[len] = '\0';
    if (len < 2) {
        return ERROR_INPUT;
    }
    unsigned int fib[50];
    int n = 2;
    fib[0] = 1;
    fib[1] = 2;
    while (n < 50) {
        fib[n] = fib[n - 1] + fib[n - 2];
        n++;
    }
    unsigned int res = 0;
    for (int i = len - 2; i >= 0; i--) {
        if (buf[i] == '1') {
            if (i >= n) {
                return ERROR_INPUT;
            }
            res += fib[i];
        } else if (buf[i] != '0') {
            return ERROR_INPUT;
        }
    }
    *out = res;
    return OK;
}

state StrBase(char* s, size_t* ind, int base, int* out) {
    if (base < 2 || base > 36) {
        return ERROR_INPUT;
    }
    int c = StrSkipWs(s, ind);
    if (c == EOF) {
        return ERROR_EOF;
    }
    int sign = 1;
    if (c == '-') {
        sign = -1;
        (*ind)++;
        c = s[*ind];
        if (c == '\0') {
            c = EOF;
        }
    } else if (c == '+') {
        (*ind)++;
        c = s[*ind];
        if (c == '\0') {
            c = EOF;
        }
    }
    int res = 0;
    int hasDigits = 0;
    while (c != EOF) {
        int digit = -1;
        if (c >= '0' && c <= '9') {
            digit = c - '0';
        } else if (c >= 'a' && c <= 'z') {
            digit = c - 'a' + 10;
        } else if (c >= 'A' && c <= 'Z') {
            digit = c - 'A' + 10;
        } else {
            break;
        }
        if (digit < 0 || digit >= base) {
            break;
        }
        res = res * base + digit;
        hasDigits = 1;
        (*ind)++;
        c = s[*ind];
        if (c == '\0') {
            c = EOF;
        }
    }
    if (!hasDigits) {
        return ERROR_INPUT;
    }
    *out = res * sign;
    return OK;
}


int Overfscanf(FILE* file, char* flag, ...) {
    if (file == NULL || flag == NULL) {
        return -1;
    }
    va_list args;
    va_start(args, flag);
    int res = 0;
    int i = 0;
    while (flag[i] != '\0') {
        if (isspace((unsigned char)flag[i])) {
            int c = fgetc(file);
            while (c != EOF && isspace(c)) {
                c = fgetc(file);
            }
            if (c != EOF) {
                ungetc(c, file);
            }
            i++;
            continue;
        }
        if (flag[i] == '%') {
            i++;
            if (flag[i] == 'd') {
                int* out = va_arg(args, int*);
                if (FileInt(file, out) != OK) {
                    va_end(args);
                    if (res == 0) {
                        return -1;
                    }
                    return res;
                }
                res++;
                i++;
            } else if (flag[i] == 's') {
                char* out = va_arg(args, char*);
                if (FileStr(file, out) != OK) {
                    va_end(args);
                    if (res == 0) {
                        return -1;
                    }
                    return res;
                }
                res++;
                i++;
            } else if (flag[i] == 'R' && flag[i + 1] == 'o') {
                int* out = va_arg(args, int*);
                if (FileRo(file, out) != OK) {
                    va_end(args);
                    if (res == 0) {
                        return -1;
                    }
                    return res;
                }
                res++;
                i += 2;
            } else if (flag[i] == 'Z' && flag[i + 1] == 'r') {
                unsigned int* out = va_arg(args, unsigned int*);
                if (FileZeckendorf(file, out) != OK) {
                    va_end(args);
                    if (res == 0) {
                        return -1;
                    }
                    return res;
                }
                res++;
                i += 2;
            } else if (flag[i] == 'C' && flag[i + 1] == 'v') {
                int* out = va_arg(args, int*);
                int base = va_arg(args, int);
                if (FileBase(file, base, out) != OK) {
                    va_end(args);
                    if (res == 0) {
                        return -1;
                    }
                    return res;
                }
                res++;
                i += 2;
            } else if (flag[i] == 'C' && flag[i + 1] == 'V') {
                int* out = va_arg(args, int*);
                int base = va_arg(args, int);
                if (FileBase(file, base, out) != OK) {
                    va_end(args);
                    if (res == 0) {
                        return -1;
                    }
                    return res;
                }
                res++;
                i += 2;
            } else {
                va_end(args);
                if (res == 0) {
                    return -1;
                }
                return res;
            }
        } else {
            int c = fgetc(file);
            if (c != flag[i]) {
                if (c != EOF) {
                    ungetc(c, file);
                }
                va_end(args);
                if (res == 0) {
                    return -1;
                }
                return res;
            }
            i++;
        }
    }
    va_end(args);
    return res;
}

int Overscanf(char* str, char* flag, ...) {
    if (str == NULL || flag == NULL) {
        return -1;
    }
    va_list args;
    va_start(args, flag);
    int res = 0;
    size_t ind = 0;
    int i = 0;
    while (flag[i] != '\0') {
        if (isspace((unsigned char)flag[i])) {
            int c = str[ind];
            while (c != '\0' && isspace(c)) {
                ind++;
                c = str[ind];
            }
            i++;
            continue;
        }
        if (flag[i] == '%') {
            i++;
            if (flag[i] == 'd') {
                int* out = va_arg(args, int*);
                if (StrInt(str, &ind, out) != OK) {
                    va_end(args);
                    if (res == 0) {
                        return -1;
                    }
                    return res;
                }
                res++;
                i++;
            } else if (flag[i] == 's') {
                char* out = va_arg(args, char*);
                if (StrStr(str, &ind, out) != OK) {
                    va_end(args);
                    if (res == 0) {
                        return -1;
                    }
                    return res;
                }
                res++;
                i++;
            } else if (flag[i] == 'R' && flag[i + 1] == 'o') {
                int* out = va_arg(args, int*);
                if (StrRo(str, &ind, out) != OK) {
                    va_end(args);
                    if (res == 0) {
                        return -1;
                    }
                    return res;
                }
                res++;
                i += 2;
            } else if (flag[i] == 'Z' && flag[i + 1] == 'r') {
                unsigned int* out = va_arg(args, unsigned int*);
                if (StrZeckendorf(str, &ind, out) != OK) {
                    va_end(args);
                    if (res == 0) {
                        return -1;
                    }
                    return res;
                }
                res++;
                i += 2;
            } else if (flag[i] == 'C' && flag[i + 1] == 'v') {
                int* out = va_arg(args, int*);
                int base = va_arg(args, int);
                if (StrBase(str, &ind, base, out) != OK) {
                    va_end(args);
                    if (res == 0) {
                        return -1;
                    }
                    return res;
                }
                res++;
                i += 2;
            } else if (flag[i] == 'C' && flag[i + 1] == 'V') {
                int* out = va_arg(args, int*);
                int base = va_arg(args, int);
                if (StrBase(str, &ind, base, out) != OK) {
                    va_end(args);
                    if (res == 0) {
                        return -1;
                    }
                    return res;
                }
                res++;
                i += 2;
            } else {
                va_end(args);
                if (res == 0) {
                    return -1;
                }
                return res;
            }
        } else {
            if (str[ind] != flag[i]) {
                va_end(args);
                if (res == 0) {
                    return -1;
                }
                return res;
            }
            ind++;
            i++;
        }
    }
    va_end(args);
    return res;
}

int main() {
    printf("Overscanf:\n");
    int a = 0, b = 0, ro = 0, val = 0;
    unsigned int zr = 0;

    int res = Overscanf("42 -100 XIV 010011 ff", "%d %d %Ro %Zr %Cv", &a, &b, &ro, &zr, &val, 16);
    printf("res=%d, a=%d, b=%d, ro=%d, zr=%u, hex=%d\n", res, a, b, ro, zr, val);

    val = 0;
    res = Overscanf("DEADBEEF", "%CV", &val, 16);
    printf("res=%d, CV = %d\n", res, val);

    res = Overscanf("abc", "%d", &a);
    printf("res=%d\n", res);

    printf("Overfscanf:\n");
    FILE* f = fopen("6_test.txt", "w");
    if (f != NULL) {
        fprintf(f, "123 456 word 777");
        fclose(f);
    }

    f = fopen("6_test.txt", "r");
    if (f != NULL) {
        char w[50];
        res = Overfscanf(f, "%d %d %s %d", &a, &b, w, &val);
        printf("res=%d, x=%d, y=%d, w=%s, z=%d\n", res, a, b, w, val);
        fclose(f);
    }

    f = fopen("6_test.txt", "w");
    if (f != NULL) {
        fclose(f);
    }
    f = fopen("6_test.txt", "r");
    if (f != NULL) {
        res = Overfscanf(f, "%d", &a);
        printf("res=%d\n", res);
        fclose(f);
    }

    return OK;
}