#include "monitor/watchpoint.h"
#include "monitor/expr.h"
#include "nemu.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
WP *head = NULL;
WP *free_ = NULL;


WP* new_wp(){
	
	if(free_ == NULL){
		panic("NO FREE Watchpoints! ");
	}	
	
  WP *wp = free_;
  free_ = free_->next;
  wp->next = head;
  head = wp;
    
  return wp;
}

void free_wp(WP* wp)
{
    // 从head链表移除
    if (head == wp) {
        head = head->next;
    }
    else
    {
    	WP *prev = head;
      while (prev->next != wp) {
          prev = prev->next;
          if (prev == NULL) return; 
      }
      prev->next = wp->next;
    }
    
    wp->next = free_;
    free_ = wp;
}
//管理监视点
void check_watchpoints() {
    WP *wp = head;
    while (wp != NULL) {
        bool success;
        uint32_t new_val = expr(wp->expr, &success);
        
        if (success && new_val != wp->old_val) {
            nemu_state = NEMU_STOP;
            printf("Watchpoint %d: %s\n", wp->NO, wp->expr);
            printf("Old value = %u\n", wp->old_val);
            printf("New value = %u\n", new_val);
            wp->old_val = new_val;
        }
        wp = wp->next;
    }
}
//检查
void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */


