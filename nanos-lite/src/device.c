#include "common.h"
#include <amdev.h>

void switch_fg_pcb(int i);

size_t serial_write(const void *buf, size_t offset, size_t len) {
  for (int i = 0; i < len; ++i) {
    _putc(((char *) buf)[i]);
  }
  return len;
}

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

size_t events_read(void *buf, size_t offset, size_t len) {
  int key = read_key();
  if (key == _KEY_NONE) {
    return sprintf(buf, "t %d\n", uptime());
  }
  if (key & 0x8000) {
    switch (key % 256) {
      case _KEY_F1: switch_fg_pcb(1); break;
      case _KEY_F2: switch_fg_pcb(2); break;
      case _KEY_F3: switch_fg_pcb(3); break;
      default: break;
    }
  }
  return sprintf(buf, "%s %s\n", key & 0x8000 ? "kd" : "ku", keyname[key % 256]);
}

static char dispinfo[128] __attribute__((used));

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  memcpy(buf, dispinfo + offset, len);
  return len;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  offset /= 4, len /= 4;
  int w = screen_width();
  draw_rect((void *) buf, offset % w, offset / w, len, 1);
  return len;
}

void init_device() {
  Log("Initializing devices...");
  _ioe_init();

  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
  sprintf(dispinfo, "WIDTH:%d\nHEIGHT:%d", screen_width(), screen_height());
}
