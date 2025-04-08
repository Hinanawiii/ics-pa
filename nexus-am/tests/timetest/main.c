#include <am.h>
#include <klib.h>

int main(){
  _ioe_init();
  int sec = 1;
  unsigned long last_time = 0;
  
  while (1) {
    unsigned long current_time = _uptime();
    
    // 打印时间差，看是否正常递增
    if (current_time > last_time) {
      _putc('.');  // 每当时间变化就输出一个点
      last_time = current_time;
    }
    
    while(_uptime() < 1000 * sec) ;
    
    if (sec == 1) {
      printf("%d second.\n", sec);
    } else {
      printf("%d seconds.\n", sec);
    }
    sec++;
  }
  return 0;
}
