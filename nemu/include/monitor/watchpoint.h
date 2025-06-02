#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"
#include "monitor/monitor.h"

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */
  
  char expr[128];
  //表达式字符串
	uint32_t old_val;   // 旧值

} WP;


void check_watchpoints(void);
WP *new_wp(void);
void free_wp(WP *wp);
void check_watchpoints(void);
extern WP *head;
#endif
