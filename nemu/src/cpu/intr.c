#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */

  // 保存EFLAGS
  memcpy(&t1, &cpu.eflags, sizeof(cpu.eflags));
  rtl_li(&t0, t1);
  rtl_push(&t0);
  
  // 保存CS - 需要类型转换
  rtlreg_t cs_temp = cpu.cs;  // 将cs转换为rtlreg_t类型
  rtl_push(&cs_temp);
  
  // 保存返回地址
  rtl_li(&t0, ret_addr);
  rtl_push(&t0);
  
  // 计算IDT表项地址
  printf("CPU IDTR: base=0x%x, limit=0x%x\n", cpu.idtr.base, cpu.idtr.limit);
  printf("Interrupt number: %d\n", NO);
  
  vaddr_t gate_addr = cpu.idtr.base + NO * sizeof(GateDesc);
  printf("Gate address: 0x%x\n", gate_addr);
  printf("IDT end address: 0x%x\n", cpu.idtr.base + cpu.idtr.limit);
  //assert(gate_addr <= cpu.idtr.base + cpu.idtr.limit);
  
  // 获取目标地址
  uint32_t off_15_0 = vaddr_read(gate_addr, 2);
  uint32_t off_32_16 = vaddr_read(gate_addr + sizeof(GateDesc) - 2, 2);
  uint32_t target_addr = (off_32_16 << 16) + off_15_0;
  
#ifdef DEBUG
  Log("target_addr=0x%x", target_addr);
#endif
  
  // 设置跳转
  decoding.is_jmp = 1;
  decoding.jmp_eip = target_addr;
}
void dev_raise_intr() {
}
