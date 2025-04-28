#include "common.h"
#include "fs.h"

extern void ramdisk_read(void *buf, off_t offset, size_t len);
extern size_t get_ramdisk_size(void);

#define DEFAULT_ENTRY ((void *)0x4000000)

uintptr_t loader(_Protect *as, const char *filename) {
  // 直接加载ramdisk内容到0x4000000
  ramdisk_read((void *)0x4000000, 0, get_ramdisk_size());
  return 0x4000000;
}
