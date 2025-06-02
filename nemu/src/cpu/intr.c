#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
    // 计算中断门描述符地址
    vaddr_t gate_addr = cpu.idtr.base + NO * 8;
   	if(cpu.idtr.limit <0){
   		assert(0);
   	} //检查整个
    // 读取中断门描述符（64 位）
    uint32_t low = vaddr_read(gate_addr, 4) & 0xffff ;     
    uint32_t high = vaddr_read(gate_addr + 4, 4) & 0xffff0000;

    //if (!((high >> 8) & 1)) {
    //    panic("Interrupt gate not present! NO = %d", NO);
    //}

    //uint32_t offset = (high & 0xFFFF0000) | (low & 0x0000FFFF);

    // 保存上下文
    uint32_t t0 = cpu.cs;
    rtl_push(&cpu.eflags.val);
    rtl_push(&t0);
    rtl_push(&ret_addr);

		decoding.jmp_eip = high|low;
		decoding.is_jmp = true;  

    // 如果是中断门（type=0xE），清除 IF 位
    uint8_t type = (high >> 8) & 0xFF ;
    if (type == 0xE) {
        cpu.eflags.IF = 0;  
    }
}

void dev_raise_intr() {
	cpu.INTR = true;
}
