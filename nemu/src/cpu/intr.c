#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  // 需要临时变量来存储值，因为rtl_push需要rtlreg_t*类型参数
  rtlreg_t temp;
  
  // 保存EFLAGS
  temp = cpu.eflags.val;
  rtl_push(&temp);
  
  // 保存CS
  temp = cpu.cs;
  rtl_push(&temp);
  
  // 保存返回地址EIP
  rtl_push(&ret_addr);  // 这里ret_addr已经是rtlreg_t类型
  
  // 从IDTR中读取IDT的地址和目标门描述符位置
  vaddr_t gate_addr = cpu.idtr.base + NO * sizeof(GateDesc);
  
  // 读取门描述符
  uint32_t low = vaddr_read(gate_addr, 4);  // 读取低32位
  uint32_t high = vaddr_read(gate_addr + 4, 4);  // 读取高32位
  
  // 从门描述符提取目标地址
  uint32_t offset_15_0 = low & 0xFFFF;
  uint32_t offset_31_16 = high >> 16;
  vaddr_t target = (offset_31_16 << 16) | offset_15_0;
  
  // 跳转到目标地址
  cpu.eip = target;
}

void dev_raise_intr() {
}
