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
        case 'd':
          break;
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
