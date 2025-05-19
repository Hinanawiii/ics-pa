#include <am.h>
#include <x86.h>

#define RTC_PORT 0x48
#define I8042_DATA_PORT 0x60
#define I8042_STATUS_PORT 0x64
#define I8042_STATUS_HASKEY_MASK 0x1
#define VMEM 0x40000

static unsigned long boot_time;

void _ioe_init() {
  boot_time = inl(RTC_PORT);
}

unsigned long _uptime() {
  static unsigned long start_time = 0;
  unsigned long current_time = inl(RTC_PORT);
  
  if (start_time == 0) {
    start_time = current_time;
    return 0;
  }
  
  return current_time - start_time;
}

uint32_t* const fb = (uint32_t *)VMEM;

_Screen _screen = {
  .width  = 400,
  .height = 300,
};

extern void* memcpy(void *, const void *, int);

void _draw_rect(const uint32_t *pixels, int x, int y, int w, int h) {
    uint32_t *dest = fb + y * _screen.width + x;
    for (int i = 0; i < h; i++) {
        memcpy(dest, pixels + i * w, w * sizeof(uint32_t));
        dest += _screen.width;
    }
}

void _draw_sync() {
    // 保持空实现，与硬件同步逻辑由上层处理
}

int _read_key() {
  if (inb(I8042_STATUS_PORT) & I8042_STATUS_HASKEY_MASK) {
    return inl(I8042_DATA_PORT);
  }
  return _KEY_NONE;
}
