#include "common.h"
#include "fs.h"
#include "memory.h"

extern void ramdisk_read(void *buf, off_t offset, size_t len);
extern size_t get_ramdisk_size(void);

#define DEFAULT_ENTRY ((void *)0x8048000)

extern uint8_t ramdisk_start;
extern uint8_t ramdisk_end;

#define RAMDISK_SIZE ((&ramdisk_end) - (&ramdisk_start))

uintptr_t loader(_Protect *as, const char *filename) {
  int fd = fs_open(filename, 0, 0);
  int bytes = fs_filesz(fd);
  int n = bytes / PGSIZE;
  int m = bytes % PGSIZE;
  int i;
  void *pa;

  for (i = 0; i < n; i++) {
    pa = new_page();
    _map(as, DEFAULT_ENTRY + i * PGSIZE, pa);
    fs_read(fd, pa, PGSIZE);
  }

  pa = new_page();
  _map(as, DEFAULT_ENTRY + i * PGSIZE, pa);
  fs_read(fd, pa, m);

  fs_close(fd);
  return (uintptr_t)DEFAULT_ENTRY;
}
