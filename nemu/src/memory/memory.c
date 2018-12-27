#include "nemu.h"

#define PMEM_SIZE (128 * 1024 * 1024)

#define pmem_rw(addr, type) *(type *)({\
    Assert(addr < PMEM_SIZE, "physical address(0x%08x) is out of bound", addr); \
    guest_to_host(addr); \
    })

uint8_t pmem[PMEM_SIZE];

int is_mmio(paddr_t addr);
uint32_t mmio_read(paddr_t addr, int len, int map_NO);
void mmio_write(paddr_t addr, int len, uint32_t data, int map_NO);

/* Memory accessing interfaces */

uint32_t paddr_read(paddr_t addr, int len) {
  int id = is_mmio(addr);
  if (id != -1) {
    return mmio_read(addr, len, id);
  }
  else {
    return pmem_rw(addr, uint32_t) & (~0u >> ((4 - len) << 3));
  }
}

void paddr_write(paddr_t addr, uint32_t data, int len) {
  int id = is_mmio(addr);
  if (id != -1) {
    mmio_write(addr, len, data, id);
  }
  else {
    memcpy(guest_to_host(addr), &data, len);
  }
}

paddr_t page_translate(vaddr_t vaddr) {
  if (cpu.PG == 1) {
    paddr_t pde_base = cpu.cr3 & 0xfffff000;
    uint32_t pde = paddr_read(pde_base + (vaddr >> 22) * 4, 4);
    if (!(pde & 1)) {
      panic("invalid pde, vaddr = 0x%x", vaddr);
    }
    paddr_t pte_base = pde & 0xfffff000;
    uint32_t pte = paddr_read(pte_base + (vaddr << 10 >> 22) * 4, 4);
    if (!(pte & 1)) {
      panic("invalid pte, vaddr = 0x%x", vaddr);
    }
    paddr_t paddr = (pte & 0xfffff000) + (vaddr & 0xfff);
    if (paddr != vaddr) {
      Log("vaddr = 0x%x, paddr = 0x%x", vaddr, paddr);
    }
    return paddr;
  }
  else {
    return vaddr;
  }
}

uint32_t vaddr_read(vaddr_t vaddr, int len) {
  if ((vaddr & 0xfff) + len > 4096) {
    TODO();
  }
  else {
    paddr_t paddr = page_translate(vaddr);
    return paddr_read(paddr, len);
  }
}

void vaddr_write(vaddr_t vaddr, uint32_t data, int len) {
  if ((vaddr & 0xfff) + len > 4096) {
    TODO();
  } 
  else {
    paddr_t paddr = page_translate(vaddr);
    paddr_write(paddr, data, len);
  }
}
