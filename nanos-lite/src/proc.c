#include "proc.h"

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used));
static PCB pcb_boot;
PCB *current;
PCB *fg_pcb;

void context_kload(PCB *pcb, void *entry);
void context_uload(PCB *pcb, const char *filename);

void switch_boot_pcb() {
  current = &pcb_boot;
}

void switch_fg_pcb(int i) {
  fg_pcb = &pcb[i];
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    Log("Hello World from Nanos-lite for the %dth time!", j);
    j ++;
    _yield();
  }
}

void init_proc() {
  context_uload(&pcb[0], "/bin/hello");
  context_uload(&pcb[1], "/bin/init");
  context_uload(&pcb[2], "/bin/init");
  context_uload(&pcb[3], "/bin/init");
  switch_fg_pcb(1);
  switch_boot_pcb();
}

_Context* schedule(_Context *prev) {
  current->cp = prev;
  current = (current != fg_pcb) ? fg_pcb : &pcb[0];
  return current->cp;
}
