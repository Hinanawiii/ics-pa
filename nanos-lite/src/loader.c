#include "common.h"
#include "fs.h"

extern void ramdisk_read(void *buf, off_t offset, size_t len);
extern size_t get_ramdisk_size(void);

#define DEFAULT_ENTRY ((void *)0x4000000)

extern uint8_t ramdisk_start;
extern uint8_t ramdisk_end;

#define RAMDISK_SIZE ((&ramdisk_end) - (&ramdisk_start))

void ramdisk_read(void *buf, off_t offset, size_t len);
void ramdisk_write(const void *buf, off_t offset, size_t len);
size_t get_ramdisk_size();

uintptr_t loader(_Protect *as, const char *filename) {
  // TODO();
  ramdisk_read(DEFAULT_ENTRY,0,get_ramdisk_size()); 
  //int fd = fs_open(filename, 0, 0);
  //Log("fd=%d\n",fd);
  //size_t f_size = fs_filesz(fd);
  //Log("filesize=%d",f_size);
  //fs_read(fd, DEFAULT_ENTRY, f_size);
  //fs_close(fd);
  
  return (uintptr_t)DEFAULT_ENTRY;
}
