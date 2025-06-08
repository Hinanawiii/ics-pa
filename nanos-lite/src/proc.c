#include "proc.h"

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC];
static int nr_proc = 0;
PCB *current = NULL;

uintptr_t loader(_Protect *as, const char *filename);

void load_prog(const char *filename) {
  int i = nr_proc ++;
  _protect(&pcb[i].as);

  uintptr_t entry = loader(&pcb[i].as, filename);

  // TODO: remove the following three lines after you have implemented _umake()
  //_switch(&pcb[i].as);
  //current = &pcb[i];
  //((void (*)(void))entry)();

  _Area stack;
  stack.start = pcb[i].stack;
  stack.end = stack.start + sizeof(pcb[i].stack);

  pcb[i].tf = _umake(&pcb[i].as, stack, stack, (void *)entry, NULL, NULL);
}

_RegSet* schedule(_RegSet *prev) {
  if (current != NULL) {
    current->tf = prev;
  }else {
    current = &pcb[0];   // 第一次调度初始化为仙剑
  }

  // 当前系统只有一个用户进程，直接切换回它
  //current = &pcb[0];
  //_switch(&current->as);
  //return current -> tf;
  static int num = 0;         // 调度计数器
  static const int freq = 1000;
  
  if (current == &pcb[0]) {   // 当前是仙剑
    num++;
  if (num >= freq) {
      current = &pcb[1];      // 切换到 hello
      num = 0;
    }
  } else {                    // 当前是 hello
    current = &pcb[0];        // 调回仙剑
  }

  _switch(&current->as);      // 切换地址空间
  return current->tf;         // 返回新进程的上下文
}
