#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536];

int cursor;

uint32_t choose(uint32_t n) {
  return rand() % n;
}

void gen_spaces() {
  int n = choose(3);
  for (int i = 0; i < n; ++i) {
    buf[cursor++] = ' ';
  }
}

void gen_num() {
  gen_spaces();

  int val = choose(1000);
  int len;
  if (choose(2) == 1) {
    len = sprintf(&buf[cursor], "%d", val);
  }
  else {
    buf[cursor++] = '0';
    buf[cursor++] = 'x';
    len = sprintf(&buf[cursor], "%x", val);
  }
  cursor += len;
  buf[cursor++] = 'u';

  gen_spaces();
}

void gen(char c) {
  gen_spaces();

  buf[cursor++] = c;
  
  gen_spaces();
}

void gen_op() {
  gen_spaces();

  char op;
  switch(choose(4)) {
    case 0: op = '+'; break;
    case 1: op = '-'; break;
    case 2: op = '*'; break;
    default: op = '/'; break;
  }
  buf[cursor++] = op;

  gen_spaces();
}

void gen_expr(int depth) {
  if (depth == 0) {
    switch(choose(3)) {
      case 0:
        gen('(');
        gen_expr(depth + 1);
        gen(')');
        break;
      default:
        gen_expr(depth + 1);
        gen_op();
        gen_expr(depth + 1);
        break;
    }
  }
  else if (depth < 10) {
    switch(choose(5)) {
      case 0:
        gen_expr(depth + 1);
        gen_op();
        gen_expr(depth + 1);
        break;
      case 1:
        gen('(');
        gen_expr(depth + 1);
        gen(')');
        break;
      default:
        gen_num();
    }
  }
  else {
    gen_num();
  }
}

static inline void gen_rand_expr() {
  cursor = 0;
  gen_expr(0);
  buf[cursor] = '\0';
}

static char code_buf[65536];
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    gen_rand_expr();

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen(".code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc .code.c -o .expr");
    if (ret != 0) continue;

    fp = popen("./.expr", "r");
    assert(fp != NULL);

    int result;
    fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}
