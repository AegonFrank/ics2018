#include "klib.h"
#include <stdarg.h>

#ifndef __ISA_NATIVE__

static char * itoa(int num, char *str, int radix) {
  assert(num >= 0);
  static char ascii[] = "0123456789abcdef";
  int cnt = 0;
  while (num != 0) {
    str[cnt++] = ascii[num % radix];
    num /= radix;
  }
  str[cnt] = '\0';
  return str;
}

int printf(const char *fmt, ...) {
  return 0;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  int res = 0;
  while (*fmt != '\0') {
    if (*fmt == '%') {
      switch (*++fmt) {
        case 'd': {
          int i = va_arg(ap, int);
          itoa(i, out, 10);
          while (*out != '\0') {
            ++out;
            ++res;
          }
          break;
        }
        case 's': {
          char *s = va_arg(ap, char *);
          while (*s != '\0') {
            *out++ = *s++;
            ++res;
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
      ++res;
    }
  }
  *out = '\0';
  return res;
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
