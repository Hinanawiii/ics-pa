#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
    // 获取门描述符
    vaddr_t gate_addr = cpu.idtr.base + 8 * NO;

    // P 位校验
    if (cpu.idtr.limit < 0) {
        assert(0);
    }

    // 将 EFLAGS、CS、返回地址压栈
    rtlreg_t eflags_temp;
    memcpy(&eflags_temp, &cpu.eflags, sizeof(eflags_temp));
    rtl_push(&eflags_temp);
    cpu.eflags.IF = 0;  // 关闭IF位
    
    rtlreg_t cs_temp = cpu.cs;  // 转换为32位
    rtl_push(&cs_temp);
    
    rtlreg_t ret_addr_temp = ret_addr;
    rtl_push(&ret_addr_temp);

    // 组合中断处理程序入口点
    uint32_t high, low;
    low = vaddr_read(gate_addr, 4) & 0xffff;
    high = vaddr_read(gate_addr + 4, 4) & 0xffff0000;
    
    uint32_t target_addr = high | low;

    // 设置 eip 跳转
    decoding.jmp_eip = target_addr;
    decoding.is_jmp = true;
}

void dev_raise_intr() {
    cpu.INTR = true;
}
