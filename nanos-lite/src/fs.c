#include "fs.h"
extern int screen_width();
extern int screen_height();

extern size_t events_read(void *buf, size_t len);
extern void dispinfo_read(void *buf, off_t offset, size_t len);
extern size_t fb_write(const void *buf, size_t offset, size_t len);
extern void init_dispinfo(void);

extern char dispinfo[];

void ramdisk_read(void *buf, off_t offset, size_t len);
void ramdisk_write(const void *buf, off_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  off_t disk_offset;
  off_t open_offset;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB, FD_EVENTS, FD_DISPINFO, FD_NORMAL};

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  {"stdin (note that this is not the actual stdin)", 0, 0},
  {"stdout (note that this is not the actual stdout)", 0, 0},
  {"stderr (note that this is not the actual stderr)", 0, 0},
  [FD_FB] = {"/dev/fb", 0, 0},
  [FD_EVENTS] = {"/dev/events", 0, 0},
  [FD_DISPINFO] = {"/proc/dispinfo", 128, 0},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

void init_fs() {
  // TODO: initialize the size of /dev/fb
  file_table[FD_FB].size = screen_width() * screen_height() * sizeof(uint32_t);
  for (int i = 0; i < NR_FILES; i++) {
    file_table[i].open_offset = 0;
  }
  
  // 初始化屏幕信息
  init_dispinfo();
}

int fs_open(const char *pathname, int flags, int mode) {
  for (int i = 0; i < NR_FILES; i++) {
    if (strcmp(file_table[i].name, pathname) == 0) {
      // 找到文件，重置偏移量
      file_table[i].open_offset = 0;
      return i;  // 返回文件描述符
    }
  }
  
  // 找不到文件
  assert(0);
  return -1;
}

size_t fs_read(int fd, void *buf, size_t len) {
  assert(fd >= 0 && fd < NR_FILES);
  
  // 特殊文件处理
  switch (fd) {
    case FD_STDIN:
    case FD_STDOUT:
    case FD_STDERR:
      return 0;
    case FD_EVENTS:
      return events_read(buf, len);
    case FD_DISPINFO:
      dispinfo_read(buf, file_table[fd].open_offset, len);
      return strlen(dispinfo) - file_table[fd].open_offset < len ? 
             strlen(dispinfo) - file_table[fd].open_offset : len;
  }
  
  // 普通文件读取
  size_t remain = file_table[fd].size - file_table[fd].open_offset;
  if (len > remain) len = remain;  // 调整读取长度
  
  if (len > 0) {
    ramdisk_read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
    file_table[fd].open_offset += len;  // 更新偏移量
  }
  
  return len;
}

size_t fs_write(int fd, const void *buf, size_t len) {
  assert(fd >= 0 && fd < NR_FILES);
  
  // 特殊文件处理
  switch (fd) {
    case FD_STDOUT:
    case FD_STDERR:
      // 输出到串口
      for (int i = 0; i < len; i++) {
        _putc(((char *)buf)[i]);
      }
      return len;
    case FD_FB:
      return fb_write(buf, file_table[fd].open_offset, len);
  }
  
  // 普通文件写入
  size_t remain = file_table[fd].size - file_table[fd].open_offset;
  if (len > remain) len = remain;  // 调整写入长度
  
  if (len > 0) {
    ramdisk_write(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
    file_table[fd].open_offset += len;  // 更新偏移量
  }
  
  return len;
}

off_t fs_lseek(int fd, off_t offset, int whence) {
  assert(fd >= 0 && fd < NR_FILES);
  
  // 根据whence计算
  switch (whence) {
    case SEEK_SET:
      file_table[fd].open_offset = offset;
      break;
    case SEEK_CUR:
      file_table[fd].open_offset += offset;
      break;
    case SEEK_END:
      file_table[fd].open_offset = file_table[fd].size + offset;
      break;
    default: assert(0);
  }
  
  if (file_table[fd].open_offset < 0) file_table[fd].open_offset = 0;
  if (file_table[fd].open_offset > file_table[fd].size) file_table[fd].open_offset = file_table[fd].size;
  
  return file_table[fd].open_offset;
}

int fs_close(int fd) {
  // 简单返回成功
  return 0;
}

// 辅助函数，获取文件大小
size_t fs_filesz(int fd) {
  assert(fd >= 0 && fd < NR_FILES);
  return file_table[fd].size;
}
