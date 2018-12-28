#include "memory.h"
#include "proc.h"

static void *pf = NULL;

void* new_page(size_t nr_page) {
  void *p = pf;
  pf += PGSIZE * nr_page;
  assert(pf < (void *)_heap.end);
  return p;
}

void free_page(void *p) {
  panic("not implement yet");
}

/* The brk() system call handler. */
int mm_brk(uintptr_t new_brk) {
  // Log("cur_brk is 0x%x, max_brk is 0x%x, new_brk is 0x%x", current->cur_brk, current->max_brk, new_brk);
  if (new_brk > current->max_brk) {
    uintptr_t page_begin = (current->max_brk & 0xfffff000) + 4096;
    uintptr_t page_end = (new_brk & 0xfffff000) + 4096;
    for (uintptr_t va = page_begin; va != page_end; va += 4096) {
      void *pa = new_page(1);
      _map(&current->as, (void *) va, pa, 0);
    }
    current->max_brk = new_brk;
  }
  current->cur_brk = new_brk;
  return 0;
}

void init_mm() {
  pf = (void *)PGROUNDUP((uintptr_t)_heap.start);
  Log("free physical pages starting from %p", pf);

  _vme_init(new_page, free_page);
}
