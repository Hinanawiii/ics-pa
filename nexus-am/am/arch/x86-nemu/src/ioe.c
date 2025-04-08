#include <am.h>
#include <x86.h>
#define RTC_PORT 0x48   // Note that this is not standard
#define I8042_DATA_PORT 0x60
#define I8042_STATUS_PORT 0x64
#define I8042_STATUS_HASKEY_MASK 0x1
#define KEYBOARD_IRQ 1
#define VMEM 0x40000

#define SCREEN_H 300
#define SCREEN_W 400
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

uint32_t* const fb = (uint32_t *)0x40000;

_Screen _screen = {
  .width  = 400,
  .height = 300,
};

extern void* memcpy(void *, const void *, int);

void _draw_rect(const uint32_t *pixels, int x, int y, int w, int h) {
  uint32_t *fb = (uint32_t *)(uintptr_t)VMEM;
  int screen_w = _screen.width;
  
  // 假设矩形完全在屏幕范围内，直接计算目标位置并复制
  uint32_t *dest = fb + y * screen_w + x;
  
  // 特殊情况：如果矩形宽度等于源宽度和屏幕宽度，可以一次性复制所有像素
  //if (w == screen_w) {
    memcpy(dest, pixels, w * h * sizeof(uint32_t));
    return;
  //}
  
  // 如果源和目标的宽度相同，可以逐行复制而不需要重新排列
  //for (int i = 0; i < h; i++) {
  //  memcpy(dest + i * screen_w, pixels + i * w, w * sizeof(uint32_t));
  //}
}

void _draw_sync() {
}

int _read_key() {
    if (inb(I8042_STATUS_PORT) & I8042_STATUS_HASKEY_MASK) {
    // 如果有按键事件，则读取数据端口获取键码
    return inl(I8042_DATA_PORT);
  } else {
    return _KEY_NONE;
  }
}
