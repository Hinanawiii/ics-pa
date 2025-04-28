#include "common.h"
#include "fs.h"

extern void ramdisk_read(void *buf, off_t offset, size_t len);
extern size_t get_ramdisk_size(void);

#define DEFAULT_ENTRY ((void *)0x4000000)

void hex_dump(void *buf, size_t size) {
  uint8_t *p = (uint8_t*)buf;
  for (size_t i = 0; i < size && i < 64; i++) {
    if (i % 16 == 0) Log("\n%08x: ", i);
    Log("%02x ", p[i]);
  }
  Log("\n");
}

uintptr_t loader(_Protect *as, const char *filename) {
  // 直接加载ramdisk内容到0x4000000
  ramdisk_read((void *)0x4000000, 0, get_ramdisk_size());
  
  
  Log("Program header:");
  hex_dump((void *)0x4000000, 64);
  return 0x4000000;
}
