#include "common.h"
#include "fs.h"

extern void ramdisk_read(void *buf, off_t offset, size_t len);
extern size_t get_ramdisk_size(void);

#define DEFAULT_ENTRY ((void *)0x4000000)

extern uint8_t ramdisk_start;
extern uint8_t ramdisk_end;

#define RAMDISK_SIZE ((&ramdisk_end) - (&ramdisk_start))


void hex_dump(void *buf, size_t size) {
  uint8_t *p = (uint8_t*)buf;
  for (size_t i = 0; i < size && i < 64; i++) {
    if (i % 16 == 0) Log("\n%08x: ", i);
    Log("%02x ", p[i]);
  }
  Log("\n");
}

uintptr_t loader(_Protect *as, const char *filename) {
  // TODO();
  ramdisk_read(DEFAULT_ENTRY, 0, RAMDISK_SIZE);
  //int fd = fs_open(filename, 0, 0);
  //Log("filename=%s,fd=%d",filename,fd);
  //fs_read(fd, DEFAULT_ENTRY, fs_filesz(fd));
  //fs_close(fd);
  return (uintptr_t)DEFAULT_ENTRY;
}
