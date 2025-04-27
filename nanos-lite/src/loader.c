#include "common.h"

#define DEFAULT_ENTRY ((void *)0x4000000)

uintptr_t loader(_Protect *as, const char *filename) {
  size_t size = get_ramdisk_size();
  ramdisk_read((void *)0x4000000, 0, size);
  
  // 返回程序入口地址
  return 0x4000000;
  return (uintptr_t)DEFAULT_ENTRY;
}
