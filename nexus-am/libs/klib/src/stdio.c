#include "klib.h"
#include <stdarg.h>

#ifndef __ISA_NATIVE__

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
          if (i < 0) {
            *out++ = '-';
            ++res;
          }
          int cnt = 0;
          while (i != 0) {
            int r = i % 10;
            r = r < 0 ? -r : r;
            *(out + cnt++) = r + '0';
            i /= 10;
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
