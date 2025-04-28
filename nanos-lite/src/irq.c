#include "common.h"
#include "syscall.h"

extern _RegSet* do_syscall(_RegSet *r);

static _RegSet* do_event(_Event e, _RegSet* r) {
  switch (e.event) {
    case _EVENT_SYSCALL:
      return do_syscall(r);
    default: panic("Unhandled event ID = %d", e.event);
  }

  return NULL;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  _asye_init(do_event);
  // 打印一些 IDT 相关信息
}
