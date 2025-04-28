#include "common.h"
#include "fs.h"

#define DEFAULT_ENTRY ((void *)0x4000000)

uintptr_t loader(_Protect *as, const char *filename) {
  int fd = fs_open(filename, 0, 0);
  size_t size = fs_filesz(fd);
  fs_read(fd, (void *)0x4000000, size);
  fs_close(fd);
  // 返回程序入口地址
  return 0x4000000;
  return (uintptr_t)DEFAULT_ENTRY;//
}
