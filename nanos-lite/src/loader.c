#include "common.h"
#include "fs.h"
#include "memory.h"
#define DEFAULT_ENTRY ((void *)0x8048000)


// 从ramdisk中`offset`偏移处的`len`字节读入到`buf`中
void ramdisk_read(void *buf, off_t offset, size_t len);

// 把`buf`中的`len`字节写入到ramdisk中`offset`偏移处
void ramdisk_write(const void *buf, off_t offset, size_t len);

// 返回ramdisk的大小, 单位为字节
size_t get_ramdisk_size();
uintptr_t loader(_Protect *as, const char *filename) {
  // TODO();
  int i;void *pa;
  int fd = fs_open(filename, 0, 0);
  int bytes = fs_filesz(fd);
  int n = bytes / PGSIZE;
  int m = bytes % PGSIZE;

  for (i = 0; i < n; i++) {
    pa = new_page();
    _map(as, DEFAULT_ENTRY + i * PGSIZE, pa);
    fs_read(fd, pa, PGSIZE);
  }
  
  pa = new_page();
  
  //Log("filesize=%d",f_size);
  //Log("Calling fs_read with fd=%d", fd);
  
  _map(as,DEFAULT_ENTRY+i*PGSIZE,pa);
  
  //fs_read(fd, DEFAULT_ENTRY, f_size);
  fs_read(fd,pa,m);
  fs_close(fd);
  
  return (uintptr_t)DEFAULT_ENTRY;

}
