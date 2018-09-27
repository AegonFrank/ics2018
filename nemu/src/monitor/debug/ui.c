#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args) {
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    i = 1;
  }
  else {
    i = atoi(arg);
  }

  if (i <= 0) {
    printf("Invalid argument\n");
  }
  else {
    cpu_exec(i);
  }

  return 0;
}

static int cmd_info(char *args) {
  char *arg = strtok(NULL, " ");
  
  if (arg == NULL) {
    
  }
  else if (strcmp(arg, "r") == 0) {
    for (int i = 0; i < 8; ++i) {
      const char* name = reg_name(i, 4);
      rtlreg_t val = reg_l(i);
      printf("%s\t\t0x%x\t\t%d\n", name, val, val);
    }
    printf("eip\t\t0x%x\t\t%d\n", cpu.eip, cpu.eip);
  }
  else if (strcmp(arg, "w") == 0) {
    print_wp();
  }

  return 0;
}

static int cmd_x(char *args) {
  char *arg1 = strtok(NULL, " ");
  char *arg2 = strtok(NULL, " ");
  
  if (arg1 != NULL && arg2 != NULL) {
    int len = atoi(arg1);
    vaddr_t addr = strtol(arg2, NULL, 16);
    for (int i = 0; i < len; ++i) {
      if (i % 8 == 0) {
	printf("0x%x\t", addr + i);
      }
      printf("0x%02x\t", vaddr_read(addr + i, 1));
      if ((i + 1) % 8 == 0) {
	printf("\n");
      }
    }
    printf("\n");
  }
  return 0;
}

static int cmd_p(char *args) {
  if (args == NULL) {
    printf("Expression needed\n");
    return 0;
  }

  bool success;
  uint32_t ans = expr(args, &success);

  if (!success) {
    printf("Syntax error\n");
  }
  else {
    printf("Value = %u\n", ans);
  }

  return 0;
}

static int cmd_test(char *args) {
  FILE *fp = fopen(args, "r");
  if (fp == NULL) {
    printf("Tests not found\n");
    return 0;
  }
 
  char buf[1000]; 
  while (fgets(buf, 1000, fp) != NULL) {
    char *ans = strtok(buf, " ");
    char *arg = strtok(NULL, "\n");
    bool success;
    int lhs = expr(arg, &success), rhs = atoi(ans);
    assert(lhs == rhs);
    printf("%u == %u, OK\n", lhs, rhs);
  }
  printf("All tests passed\n");

  fclose(fp);
  return 0;
}

static int cmd_w(char *args) {
  if (args == NULL) {
    printf("Expression needed\n");
    return 0;
  }

  bool success;
  uint32_t ans = expr(args, &success);

  if (!success) {
    printf("Invalid expression\n");
  }
  else {
    WP* wp = new_wp();
    assert(strlen(args) < 100);
    strcpy(wp->what, args);
    wp->old_val = ans;
  }
  return 0;
}

static int cmd_d(char *args) {
  char *arg = strtok(NULL, " ");
  if (arg == NULL) {
    return 0;
  }
  int NO = atoi(arg);
  WP* wp = find_wp(NO);
  if (wp == NULL) {
    printf("No watchpoint number %d\n", NO);
  }
  else {
    free_wp(wp);
  }
  return 0;
}

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },

  /* TODO: Add more commands */
  { "si", "Execute a given number of instructions, default 1", cmd_si },
  { "info", "Print program status", cmd_info },
  { "x", "Scanning memory", cmd_x },
  { "p", "Evaluate expression", cmd_p },
  { "test", "Run expression evaluation tests", cmd_test },
  { "w", "Set a watchpoint for an expression", cmd_w },
  { "d", "delete a watchpoint", cmd_d },
};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  while (1) {
    char *str = rl_gets();
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
