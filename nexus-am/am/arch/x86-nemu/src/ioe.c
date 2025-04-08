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
  int screen_h = _screen.height;
  
  // 裁剪矩形，确保它在屏幕范围内
  int x1 = x < 0 ? 0 : x;
  int y1 = y < 0 ? 0 : y;
  int x2 = x + w > screen_w ? screen_w : x + w;
  int y2 = y + h > screen_h ? screen_h : y + h;
  
  // 计算裁剪后的宽度和高度
  int copy_w = x2 - x1;
  int copy_h = y2 - y1;
  
  // 如果矩形完全在屏幕外，直接返回
  if (copy_w <= 0 || copy_h <= 0) return;
  
  // 特殊情况：如果没有裁剪，并且矩形大小等于屏幕大小，可以一次性复制所有像素
  if (x == 0 && y == 0 && w == screen_w && h == screen_h) {
    memcpy(fb, pixels, screen_w * screen_h * sizeof(uint32_t));
    return;
  }
  
  // 特殊情况：如果矩形宽度与源宽度和屏幕宽度都相同，可以连续复制
  if (x1 == 0 && copy_w == screen_w && w == screen_w) {
    uint32_t *dest = fb + y1 * screen_w;
    const uint32_t *src = pixels + (y1 - y) * w;
    memcpy(dest, src, copy_w * copy_h * sizeof(uint32_t));
    return;
  }
  
  // 标准情况：使用定长缓冲区优化复制
  // 定义一个足够大的静态缓冲区，根据你的屏幕最大宽度调整
  static uint32_t line_buffer[400]; // 假设最大宽度不超过400
  
  // 如果矩形太宽，回退到逐行复制
  if (copy_w > 400) {
    int src_offset_y = y1 - y;
    int src_offset_x = x1 - x;
    for (int i = 0; i < copy_h; i++) {
      for (int j = 0; j < copy_w; j++) {
        fb[(y1 + i) * screen_w + x1 + j] = pixels[(src_offset_y + i) * w + src_offset_x + j];
      }
    }
    return;
  }
  
  // 使用临时缓冲区进行优化复制
  for (int i = 0; i < copy_h; i++) {
    // 填充缓冲区
    for (int j = 0; j < copy_w; j++) {
      line_buffer[j] = pixels[(y1 - y + i) * w + (x1 - x + j)];
    }
    
    // 一次性复制一行到帧缓冲区
    memcpy(fb + (y1 + i) * screen_w + x1, line_buffer, copy_w * sizeof(uint32_t));
  }
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
