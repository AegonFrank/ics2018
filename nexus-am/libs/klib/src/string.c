#include "klib.h"

#ifndef __ISA_NATIVE__

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
  return NULL;
}

void* memcpy(void* out, const void* in, size_t n) {
  return NULL;
}

int memcmp(const void* s1, const void* s2, size_t n){
  assert(s1 != NULL && s2 != NULL);
  return -1;
}

#endif
