#include "klib.h"

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  return 0;
}

char *strcpy(char* dst,const char* src) {
  assert(dst != NULL && src != NULL);
  char* d = dst;
  while ((*d++ = *src++) != '\0');
  return dst;
}

char* strncpy(char* dst, const char* src, size_t n) {
  return NULL;
}

char* strcat(char* dst, const char* src) {
  assert(dst != NULL && src != NULL);
  char* d = dst;
  while (*d++ != '\0');
  --d;
  while ((*d++ = *src++) != '\0');
  return dst;
}

int strcmp(const char* s1, const char* s2) {
  assert(s1 != NULL && s2 != NULL);
  while (*s1 == *s2) {
    if (!*s1) {
      return 0;
    }
    s1++, s2++;
  }
  return *s1 - *s2;
}

int strncmp(const char* s1, const char* s2, size_t n) {
  return 0;
}

void* memset(void* v,int c,size_t n) {
  unsigned char* p = (unsigned char*) v;
  while (n-- > 0) {
    *p++ = c;
  }
  return v;
}

void* memcpy(void* out, const void* in, size_t n) {
  return NULL;
}

int memcmp(const void* s1, const void* s2, size_t n){
  assert(s1 != NULL && s2 != NULL);
  
  unsigned char* str1 = (unsigned char*) s1;
  unsigned char* str2 = (unsigned char*) s2;
  while (n-- > 0) {
    if (*str1++ != *str2++) {
      return str1[-1] < str2[-1] ? -1 : 1;
    }
  }
  return 0;
}

#endif
