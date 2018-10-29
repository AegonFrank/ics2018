#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

#define PRINT_BUF_SIZE 1000

static char print_buf[PRINT_BUF_SIZE];

static char * itoa(int num, char *str, int radix, int width, char fill) {
  assert(num >= 0);
  static char ascii[] = "0123456789abcdef";
  int cnt = 0;
  do {
    str[cnt++] = ascii[num % radix];
    num /= radix;
  } while (num != 0);
  while (cnt < width) {
    str[cnt++] = fill;
  }
  str[cnt] = '\0';
  for (int i = 0; i < cnt / 2; ++i) {
    char tmp = str[i];
    str[i] = str[cnt - i - 1];
    str[cnt - i - 1] = tmp;
  }
  return str;
}

int printf(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int res = vsprintf(print_buf, fmt, ap);
  va_end(ap);
  assert(res < PRINT_BUF_SIZE);
  for (char *p = print_buf; *p != '\0'; ++p) {
    _putc(*p);
  }
  return res;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  char *old = out;
  while (*fmt != '\0') {
    if (*fmt == '%') {
      _Bool exit = false;
      char fill = ' ';
      int width = 0;
      while (true) {
        switch (*++fmt) {
          case 'd': {
            int i = va_arg(ap, int);
            itoa(i, out, 10, width, fill);
            while (*out != '\0') {
              ++out;
            }
            exit = true;
            break;
          }
          case 's': {
            char *s = va_arg(ap, char *);
            while (*s != '\0') {
              *out++ = *s++;
            }
            exit = true;
            break;
          }
          default: 
            if (*(fmt + 1) == 'd') {
              width = *fmt - '0';
              assert(0 <= width && width <= 9);
            }
            else {
              fill = *fmt;
            }
        }
        if (exit) {
          break;
        }
      }
      ++fmt;
    }
    else {
      *out++ = *fmt++;
    }
  }
  *out = '\0';
  return out - old;
}

int sprintf(char *out, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int res = vsprintf(out, fmt, ap);
  va_end(ap);
  return res;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  return 0;
}

#endif
