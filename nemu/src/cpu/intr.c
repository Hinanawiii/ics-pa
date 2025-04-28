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
    if (NO == 0x80) {
    // 系统调用特殊处理
    // 不跳转到IDT中的地址，而是模拟处理系统调用
    // 设置eax为1（SYS_none的返回值）
    cpu.eax = 1;
    
    // 然后直接返回到原来的位置
    rtl_pop(&temp);  // 弹出返回地址
    cpu.eip = temp;
    rtl_pop(&temp);  // 弹出CS
    cpu.cs = temp;
    rtl_pop(&temp);  // 弹出EFLAGS
    cpu.eflags.val = temp;
    
    // 设置跳转标志
    decoding.is_jmp = true;
    
    return;  // 不执行后面的代码
  }
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
  Log("Content at target address 0x%x:", target);
  for(int i = 0; i < 16; i += 4) {
    uint32_t *p = (uint32_t*)(target + i);
    Log("0x%08x: 0x%08x", target + i, *p);
  }
  
  // 跳转到中断处理程序
  cpu.eip = target;
  Log("设置 EIP = 0x%x, 完成中断处理", target);
}
void dev_raise_intr() {
}
