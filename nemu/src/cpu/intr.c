#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
    // 打印调试信息
    printf("CPU IDTR: base=0x%x, limit=0x%x\n", cpu.idtr.base, cpu.idtr.limit);
    printf("Interrupt number: %d\n", NO);
    
    // 获取门描述符
    vaddr_t gate_addr = cpu.idtr.base + 8 * NO;
    printf("Gate address: 0x%x\n", gate_addr);
    printf("IDT end address: 0x%x\n", cpu.idtr.base + cpu.idtr.limit);
    
    // P 位校验
    assert(gate_addr <= cpu.idtr.base + cpu.idtr.limit);

    // 将 EFLAGS、CS、返回地址压栈
    rtlreg_t eflags_temp;
    memcpy(&eflags_temp, &cpu.eflags, sizeof(eflags_temp));
    rtl_push(&eflags_temp);
    
    rtlreg_t cs_temp = cpu.cs;  // cpu.cs 只有 16 位，需要转换成 32 位
    rtl_push(&cs_temp);
    
    rtlreg_t ret_addr_temp = ret_addr;
    rtl_push(&ret_addr_temp);

    // 组合中断处理程序入口点
    uint32_t high, low;
    low = vaddr_read(gate_addr, 4) & 0xffff;
    high = vaddr_read(gate_addr + 4, 4) & 0xffff0000;
    
    uint32_t target_addr = high | low;
    printf("[src/cpu/intr.c,37,raise_intr] target_addr=0x%x\n", target_addr);

    // 设置 eip 跳转
    decoding.jmp_eip = target_addr;
    decoding.is_jmp = true;
    // 注意：这里直接跳转到 eip，需要在调用 raise_intr 函数之后再执行 decode 和 execute
}
void dev_raise_intr() {
	cpu.INTR = true;
}
