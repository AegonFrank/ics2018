#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

static char * itoa(int num, char *str, int radix) {
  assert(num >= 0);
  static char ascii[] = "0123456789abcdef";
  int cnt = 0;
  do {
    str[cnt++] = ascii[num % radix];
    num /= radix;
  } while (num != 0);
  str[cnt] = '\0';
  for (int i = 0; i < cnt / 2; ++i) {
    char tmp = str[i];
    str[i] = str[cnt - i - 1];
    str[cnt - i - 1] = tmp;
  }
  return str;
}

int printf(const char *fmt, ...) {
  return 0;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  char *old = out;
  while (*fmt != '\0') {
    if (*fmt == '%') {
      switch (*++fmt) {
        case 'd': {
          int i = va_arg(ap, int);
          itoa(i, out, 10);
          while (*out != '\0') {
            ++out;
          }
          break;
        }
        case 's': {
          char *s = va_arg(ap, char *);
          while (*s != '\0') {
            *out++ = *s++;
          }
          break;
        }
        default: 
          assert(0);
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
