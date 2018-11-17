#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  rtl_push((rtlreg_t *) &cpu.eflags);
  rtl_push((rtlreg_t *) &cpu.cs);
  rtl_push((rtlreg_t *) &ret_addr);
  vaddr_t offset = 0;
  offset += vaddr_read(cpu.idtr.base + NO * 8, 2);
  offset += vaddr_read(cpu.idtr.base + NO * 8 + 6, 2) << 16;
  rtl_j(offset);
}

void dev_raise_intr() {
}
