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
      SYSCALL_ARG1(r) = 1;  // 返回1
      break;
    case SYS_exit:
      _halt(a[1]);  // 使用参数作为退出状态
      break;
    case SYS_write:
      // a[1]是fd，a[2]是buf的地址，a[3]是长度
      if (a[1] == 1 || a[1] == 2) {  // stdout或stderr
        // 逐个字符输出到串口
        for (int i = 0; i < a[3]; i++) {
          _putc(*(char *)(a[2] + i));
        }
        SYSCALL_ARG1(r) = a[3];  // 返回写入的字节数
      } else {
        SYSCALL_ARG1(r) = -1;  // 错误
      }
      break;
  	case SYS_brk:
  	// 在目前的Nanos-lite中，我们总是返回0表示成功
  		SYSCALL_ARG1(r) = 0;
  		break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
