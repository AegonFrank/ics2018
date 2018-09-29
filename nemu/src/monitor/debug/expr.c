#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <stdlib.h>

enum {
  TK_NOTYPE = 256, TK_EQ,

  /* TODO: Add more token types */
  TK_DEC, TK_HEX, TK_REG, TK_NE, TK_AND, TK_DEREF
};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},                // spaces
  {"\\+", '+'},                     // plus
  {"==", TK_EQ},                    // equal
  {"!=", TK_NE},                    // not equal
  {"&&", TK_AND},                   // and
  {"-", '-'},                       // minus
  {"\\*", '*'},                     // multiply
  {"\\/", '/'},                     // divide
  {"\\(", '('},                     // left brace
  {"\\)", ')'},                     // right brace
  {"0x[0-9a-f]+u?", TK_HEX},        // hexadecimal
  {"[0-9]+u?", TK_DEC},             // decimal
  {"\\$e?[abcdsi][hilpx]", TK_REG}, // register
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
	        case TK_NOTYPE: 
	          break;
          case TK_REG:
          case TK_HEX:
	        case TK_DEC: 
            assert(substr_len < 32);
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            tokens[nr_token].str[substr_len] = '\0';
          default: 
            assert(nr_token < 32);
            tokens[nr_token++].type = rules[i].token_type;
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

bool check_parentheses(int begin, int end) {
  if (!(tokens[begin].type == '(' && tokens[end].type == ')')) {
    return false;
  }
  int stack = 0;
  for (int i = begin + 1; i < end; ++i) {
    if (tokens[i].type == '(') {
      ++stack;
    }
    else if (tokens[i].type == ')') {
      if (--stack < 0) {
        return false;
      }
    }
  }
  if (stack > 0) {
    return false;
  }
  return true;
}

bool is_operator(int index) {
  switch(tokens[index].type) {
    case '+':
    case '-':
    case '*':
    case '/':
    case TK_EQ:
    case TK_NE:
    case TK_AND:
    case TK_DEREF:
      return true;
    default:
      return false;
  }
}

bool is_unary_operator(int index) {
  if (tokens[index].type == TK_DEREF) {
   return true;
  }
  else {
   return false;
  } 
}
   
int priority(int index) {
  switch(tokens[index].type) {
    case TK_AND:
      return 0;
    case TK_EQ:
    case TK_NE:
      return 1;
    case '+':
    case '-':
      return 2;
    case '*':
    case '/':
      return 3;
    case TK_DEREF:
      return 4;
    default:
      assert(0);
  }
}

int find_main_operator(int begin, int end) {
  int res = -1;
  int stack = 0;

  for (int i = begin; i <= end; ++i) {
    if (tokens[i].type == '(') {
      ++stack;
    }
    else if (tokens[i].type == ')') {
      if (--stack < 0) {
        return -1;
      }
    }
    else if (is_operator(i)) {
      if (stack != 0) {
        continue;
      }
      if (res == -1 || priority(res) >= priority(i)) {
        res = i;
      }
    }
  }

  if (stack > 0) {
    return -1;
  }
 
  return res;
}

uint32_t eval(int begin, int end, bool *success) {
  if (begin > end) {
    *success = false;
    return 0;
  }
  else if (begin == end) {
    if (tokens[begin].type == TK_DEC) {
      return atoi(tokens[begin].str);
    }
    else if (tokens[begin].type == TK_HEX) {
      return strtol(tokens[begin].str, NULL, 16);
    }
    else if (tokens[begin].type == TK_REG) {
      char *reg_name = &tokens[begin].str[1];
      if (strlen(reg_name) == 3) {
        int i;
        for (i = 0; i < 8; ++i) {
          if (strcmp(reg_name, regsl[i]) == 0) {
            break;
          }
        }
        if (i != 8) {
          return reg_l(i);
        }
        if (strcmp(reg_name, "eip") == 0) {
          return cpu.eip;
        }
      }
      else {
        int i;
        for (i = 0; i < 8; ++i) {
          if (strcmp(reg_name, regsw[i]) == 0) {
            break;
          }
        }
        if (i != 8) {
          return reg_w(i);
        }
        for (i = 0; i < 8; ++i) {
          if (strcmp(reg_name, regsb[i]) == 0) {
            break;
          }
        }
        if (i != 8) {
          return reg_b(i);
        }
      }
    }
   
    *success = false;
    return 0;
  }
  else if (check_parentheses(begin, end) == true) {
    return eval(begin + 1, end - 1, success);
  }
  else {
    int m_op = find_main_operator(begin, end);
    if (m_op == -1) {
      *success = false;
      return 0;
    }
    if (is_unary_operator(m_op)) {
      uint32_t operand = eval(m_op + 1, end, success);
      if (!success) return 0;
      switch (tokens[m_op].type) {
        case TK_DEREF:
          return vaddr_read(operand, 4);
        default:
          assert(0);
      }
    }
    else {
      uint32_t lhs = eval(begin, m_op - 1, success);
      if (!success) return 0;
      uint32_t rhs = eval(m_op + 1, end, success);
      if (!success) return 0;

      switch (tokens[m_op].type) {
        case '+':
          return lhs + rhs;
        case '-':
          return lhs - rhs;
        case '*':
          return lhs * rhs;
        case '/':
          return lhs / rhs;
        case TK_EQ:
          return lhs == rhs;
        case TK_NE:
          return lhs != rhs;
        case TK_AND:
          return lhs && rhs;
        default:
          assert(0);
      }
    }
  }
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */

   for (int i = 0; i < nr_token; ++i) {
     if (tokens[i].type == '*' && (i == 0 || is_operator(i - 1) || tokens[i - 1].type == '(')) {
       tokens[i].type = TK_DEREF;
     }
   }

  *success = true;
  return eval(0, nr_token - 1, success);
}
