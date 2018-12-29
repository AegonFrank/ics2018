#include "proc.h"

#define DEFAULT_ENTRY 0x8048000

int fs_open(const char *pathname, int flags, int mode);
size_t fs_read(int fd, void *buf, size_t len);
int fs_close(int fd);
size_t fs_filesz(int fd);
int mm_brk(uintptr_t new_brk);

static uintptr_t loader(PCB *pcb, const char *filename) {
  int fd = fs_open(filename, 0, 0);
  size_t size = fs_filesz(fd);
  
  for (int i = 0; i < size; i += 4096) {
    void *pa = new_page(1);
    _map(&pcb->as, (void *) DEFAULT_ENTRY + i, pa, 0);
    fs_read(fd, pa, 4096);
  }
  pcb->cur_brk = pcb->max_brk = DEFAULT_ENTRY + size;

  fs_close(fd);
  return DEFAULT_ENTRY;
}

void naive_uload(PCB *pcb, const char *filename) {
  int fd = fs_open(filename, 0, 0);
  size_t size = fs_filesz(fd);
  mm_brk(DEFAULT_ENTRY + size);
  fs_read(fd, (void *) DEFAULT_ENTRY, size);
  fs_close(fd);
  ((void(*)())DEFAULT_ENTRY) ();
}

void context_kload(PCB *pcb, void *entry) {
  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _kcontext(stack, entry, NULL);
}

void context_uload(PCB *pcb, const char *filename) {
  _protect(&pcb->as);

  uintptr_t entry = loader(pcb, filename);

  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _ucontext(&pcb->as, stack, stack, (void *)entry, NULL);
}
