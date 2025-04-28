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
      Log("Program exit with code %d", a[1]);
      _halt(a[1]);  // 使用参数作为退出状态
      break;
		case SYS_write: {
			Log("Handling SYS_write: fd=%d, buf=%p, count=%d", a[1], (void*)a[2], a[3]);
			if (a[1] == 1 || a[1] == 2) {
				// 输出到串口
				for (int i = 0; i < a[3]; i++) {
				  char c = *(char*)(a[2] + i);
				  _putc(c);
				  Log("Output character: %c", c);
				}
				SYSCALL_ARG1(r) = a[3];
			} else {
				// ...
			}
			Log("SYS_write completed, returned %d", SYSCALL_ARG1(r));
			break;
		}
  	case SYS_brk:
  	// 在目前的Nanos-lite中，我们总是返回0表示成功
  		SYSCALL_ARG1(r) = 0;
  		break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
