#include "proc.h"

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC];
static int nr_proc = 0;
PCB *current = NULL;

extern int current_game;
uintptr_t loader(_Protect *as, const char *filename);

void load_prog(const char *filename) {
  int i = nr_proc ++;
  _protect(&pcb[i].as);

  uintptr_t entry = loader(&pcb[i].as, filename);

  // TODO: remove the following three lines after you have implemented _umake()
  // _switch(&pcb[i].as);
  // current = &pcb[i];
  // ((void (*)(void))entry)();

  _Area stack;
  stack.start = pcb[i].stack;
  stack.end = stack.start + sizeof(pcb[i].stack);

  pcb[i].tf = _umake(&pcb[i].as, stack, stack, (void *)entry, NULL, NULL);
}

_RegSet* schedule(_RegSet *prev) {
  if(current != NULL) {
    current->tf = prev;
  }
  else {
    current = &pcb[0];
  }

  static int num = 0;
  static const int freq = 1000;

  if(num == freq) {
    current = &pcb[1];
    num = 0;
  }
  else {
    if(current_game) {
      current = &pcb[0];
    }
    else {
      current = &pcb[2];
    }
    num++;
  }

  _switch(&current->as);
  return current->tf;
}
