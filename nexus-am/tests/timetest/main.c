#include <am.h>
#include <klib.h>

int main(){
  _ioe_init();
  int sec = 1;
  
  // 初始输出当前时间
  printf("Initial uptime: %lu ms\n", _uptime());
  
  while (1) {
    // 等待到达下一秒
    while(_uptime() < 1000 * sec) {
      // 每隔一段时间输出当前时间值
      if (_uptime() % 100 == 0) {
        printf("Current uptime: %lu ms, target: %d ms\n", _uptime(), 1000 * sec);
      }
    }
    
    // 达到目标时间时输出
    printf("Reached %d ", sec);
    if (sec == 1) {
      printf("second. (uptime: %lu ms)\n", _uptime());
    } else {
      printf("seconds. (uptime: %lu ms)\n", _uptime());
    }
    sec++;
  }
  
  return 0;
}
