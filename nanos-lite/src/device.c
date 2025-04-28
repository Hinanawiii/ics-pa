#include "common.h"
#include <am.h>

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

int screen_width() {
  return _screen.width;
}

int screen_height() {
  return _screen.height;
}

size_t events_read(void *buf, size_t len) {
   // 优先处理按键事件
  int key = _read_key();
  if (key != _KEY_NONE) {
    // 获取按键名
    const char *keyname_str = keyname[key & 0x7fff];
    
    // 判断是按下还是松开
    if (key & 0x8000) {
      sprintf(buf, "ku %s\n", keyname_str);
    } else {
      sprintf(buf, "kd %s\n", keyname_str);
    }
    
    return strlen(buf);
  }
  
  // 没有按键事件，返回时钟事件
  uint32_t time = _uptime();
  sprintf(buf, "t %d\n", time);
  
  return strlen(buf);
}

char dispinfo[128] __attribute__((used));

void dispinfo_read(void *buf, off_t offset, size_t len) {
  if (offset >= strlen(dispinfo)) {
    return;
  }
  
  int remain = strlen(dispinfo) - offset;
  int realLen = (len < remain) ? len : remain;
  
  memcpy(buf, dispinfo + offset, realLen);
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  int width = screen_width();
  int height = screen_height();
  
  // 计算屏幕坐标
  int x = (offset / 4) % width;
  int y = (offset / 4) / width;
  
  // 确保不越界
  if (y >= height) return 0;
  
  // 调用API
  _draw_rect(buf, x, y, len / 4, 1);
  
  return len;
}
void init_dispinfo() {

  //int width = screen_width();
  //int height = screen_height();
  sprintf(dispinfo, "WIDTH:%d\nHEIGHT:%d\n", screen_width(), screen_height());
}
void init_device() {
  _ioe_init();

  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
  
  init_dispinfo();
}
