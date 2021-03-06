#include "common.h"
#include "syscall.h"
#include "proc.h"

int fs_open(const char *pathname, int flags, int mode);
size_t fs_lseek(int fd, size_t offset, int whence);
size_t fs_read(int fd, void *buf, size_t len);
size_t fs_write(int fd, const void *buf, size_t len);
int fs_close(int fd);

_Context* naive_uload(PCB *pcb, const char *filename);

int mm_brk(uintptr_t new_brk);

_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;

  switch (a[0]) {
    case SYS_exit:
      return naive_uload(current, "/bin/init");
    case SYS_yield: 
      _yield();
      c->GPRx = 0;
      break;
    case SYS_open:
      c->GPRx = fs_open((void *) a[1], a[2], a[3]);
      break;
    case SYS_read:
      c->GPRx = fs_read(a[1], (void *) a[2], a[3]);
      break;
    case SYS_write:
      c->GPRx = fs_write(a[1], (void *) a[2], a[3]);
      break;
    case SYS_close:
      c->GPRx = fs_close(a[1]);
      break;
    case SYS_lseek:
      c->GPRx = fs_lseek(a[1], a[2], a[3]);
      break;
    case SYS_brk:
      c->GPRx = mm_brk(a[1]);
      break;
    case SYS_execve:
      return naive_uload(current, (void *) a[1]);
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
