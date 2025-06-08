#include "common.h"
#include "syscall.h"
#include "fs.h"

extern int fs_open(const char *path,int,int);
extern ssize_t fs_read(int,void*,size_t);
extern ssize_t fs_write(int,void*,size_t);
extern off_t fs_lseek(int,off_t,int);
int fs_close(int fd);

static inline _RegSet* sys_none(_RegSet *r){
  SYSCALL_ARG1(r) = 1; //约定系统调用返回值存于此，即eax
  return NULL;
}
static inline _RegSet* sys_exit(_RegSet *r){
  //接受一个退出状态的参数，顺序ebx ecx edx
  _halt(SYSCALL_ARG2(r)); 
  return NULL;
}

static inline _RegSet* sys_write(_RegSet *r){
	
  int fd = (int)SYSCALL_ARG2(r);
  char *buf = (char *)SYSCALL_ARG3(r);
  int len = (int)SYSCALL_ARG4(r);
  SYSCALL_ARG1(r) = fs_write(fd,buf,len);

  return NULL;
}


static inline _RegSet* sys_open(_RegSet *r) {
  const char* pathname = (const char*)SYSCALL_ARG2(r);
  int flags = (int)SYSCALL_ARG3(r);
  int mode = (int)SYSCALL_ARG4(r);
  SYSCALL_ARG1(r) = fs_open(pathname,flags,mode);
  return NULL;
}

static inline _RegSet* sys_read(_RegSet *r) {
  int fd = (int)SYSCALL_ARG2(r);
  char *buf = (char *)SYSCALL_ARG3(r);
  int len = (int)SYSCALL_ARG4(r);
  //Log("Calling fs_read with fd=%d", fd);
  SYSCALL_ARG1(r) = fs_read(fd,buf,len);
  return NULL;
}

static inline _RegSet* sys_close(_RegSet *r) {
  int fd = (int)SYSCALL_ARG2(r);
  SYSCALL_ARG1(r) = fs_close(fd);
  return NULL;
}

static inline _RegSet* sys_lseek(_RegSet *r) {
  int fd = (int)SYSCALL_ARG2(r);
  off_t offset = (off_t)SYSCALL_ARG3(r);
  int whence = (int)SYSCALL_ARG4(r);
  SYSCALL_ARG1(r) = fs_lseek(fd,offset,whence);
  return NULL;
}
extern int mm_brk(uint32_t new_brk);
static inline _RegSet* sys_brk(_RegSet *r){
  SYSCALL_ARG1(r) = mm_brk(SYSCALL_ARG2(r));
  //r->eax=0;
  return NULL;
}

_RegSet* do_syscall(_RegSet *r) {
  uintptr_t a[4];
  a[0] = SYSCALL_ARG1(r);

  switch (a[0]) {
    case SYS_none:  
      sys_none(r); 
      break;
    case SYS_exit:  
      sys_exit(r); 
      break;
    case SYS_write: 
      sys_write(r); 
      break;
    case SYS_brk: 
      sys_brk(r); 
      break;
    case SYS_open: 
      sys_open(r); 
      break;
    case SYS_read: 
      sys_read(r); 
      break;
    case SYS_close: 
      sys_close(r); 
      break;
    case SYS_lseek: 
      sys_lseek(r); 
      break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
