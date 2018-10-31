#include <am.h>
#include <x86.h>
#include <amdev.h>

#define KBD_DATA_PORT 0x60
#define KBD_STATUS_PORT 0x64

size_t input_read(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_INPUT_KBD: {
      _KbdReg *kbd = (_KbdReg *)buf;
      kbd->keydown = inl(KBD_STATUS_PORT);
      kbd->keycode = kbd->keydown ? inl(KBD_DATA_PORT) : _KEY_NONE;
      return sizeof(_KbdReg);
    }
  }
  return 0;
}
