#include "common.h"
#include "syscall.h"

_RegSet* do_syscall(_RegSet *r) {
  uintptr_t a[4];
  a[0] = SYSCALL_ARG1(r);  // 系统调用号
  a[1] = SYSCALL_ARG2(r);  // 参数
  a[2] = SYSCALL_ARG3(r);  
  a[3] = SYSCALL_ARG4(r);  

  switch (a[0]) {
    case SYS_none: 
      SYSCALL_ARG1(r) = 1;  // 返回1!
      break;
    case SYS_exit:
      _halt(a[1]);  // 使用参数作为退出状态
      break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
