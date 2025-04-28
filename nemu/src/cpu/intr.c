#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  Log("开始处理中断: NO = %d, ret_addr = 0x%x", NO, ret_addr);
  
  // 保存当前状态
  rtlreg_t temp;
  
  // 保存 EFLAGS
  temp = cpu.eflags.val;
  Log("保存 EFLAGS: 0x%x", temp);
  rtl_push(&temp);
  
  // 保存 CS
  temp = cpu.cs;
  Log("保存 CS: 0x%x", temp);
  rtl_push(&temp);
  
  // 保存返回地址
  Log("保存返回地址: 0x%x", ret_addr);
  rtl_push(&ret_addr);
  
  // 从 IDT 获取中断处理程序的地址
  vaddr_t gate_addr = cpu.idtr.base + NO * sizeof(GateDesc);
  Log("IDT基地址: 0x%x, 中断描述符地址: 0x%x", cpu.idtr.base, gate_addr);
  
  uint32_t low = vaddr_read(gate_addr, 4);
  uint32_t high = vaddr_read(gate_addr + 4, 4);
  Log("中断描述符内容: low = 0x%x, high = 0x%x", low, high);
  
  uint32_t offset_15_0 = low & 0xFFFF;
  uint32_t offset_31_16 = high >> 16;
  vaddr_t target = (offset_31_16 << 16) | offset_15_0;
  Log("计算得到的目标地址: 0x%x", target);
  
  // 跳转到中断处理程序
  cpu.eip = target;
  Log("设置 EIP = 0x%x, 完成中断处理", target);
}
void dev_raise_intr() {
}
