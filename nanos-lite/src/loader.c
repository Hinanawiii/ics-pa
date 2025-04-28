#include "common.h"
#include "fs.h"

#define DEFAULT_ENTRY ((void *)0x4000000)

uintptr_t loader(_Protect *as, const char *filename) {
  Log("Loading file: %s", filename);
  int fd = fs_open(filename, 0, 0);
  Log("File opened, fd = %d", fd);
  size_t size = fs_filesz(fd);
  Log("File size: %d bytes", size);
  fs_read(fd, (void *)0x4000000, size);
  Log("File read to 0x4000000");
  fs_close(fd);
  Log("File closed, returning entry point 0x4000000");
  
  return (uintptr_t)DEFAULT_ENTRY;
}
