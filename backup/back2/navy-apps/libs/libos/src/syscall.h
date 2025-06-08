#ifndef __SYSCALL_H__
#define __SYSCALL_H__

enum {
  SYS_none,
  SYS_exit,
  SYS_write,
  SYS_brk,
  SYS_open,
  SYS_read,
  SYS_close,
  SYS_lseek,
  SYS_kill,
  SYS_getpid
};

#endif
