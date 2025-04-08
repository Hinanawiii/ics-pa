#include <am.h>
#include <klib.h>

int main(){
  _ioe_init();
  int sec = 1;
  
  // 初始输出当前时间
  unsigned long initial_time = _uptime();
  printf("Initial uptime: %d ms\n", (int)initial_time);
  
  while (1) {
    // 等待到达下一秒
    while(_uptime() < 1000 * sec);
    
    // 达到目标时间时输出
    unsigned long current_time = _uptime();
    printf("Reached %d ", sec);
    if (sec == 1) {
      printf("second. (uptime: %d ms)\n", (int)current_time);
    } else {
      printf("seconds. (uptime: %d ms)\n", (int)current_time);
    }
    sec++;
  }
  
  return 0;
}
