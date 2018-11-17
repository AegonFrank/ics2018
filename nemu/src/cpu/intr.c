#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */

  printf("%d\t%d\n", cpu.idtr.limit, cpu.idtr.base);
  TODO();
}

void dev_raise_intr() {
}
