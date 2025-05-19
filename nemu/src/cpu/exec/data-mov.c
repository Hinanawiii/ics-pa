#include "cpu/exec.h"

/* 优化后的通用数据传输指令 */
make_EHelper(mov) {
  // 直接写入目标操作数，保留框架基础结构
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

/* 统一push指令处理逻辑 */
make_EHelper(push) {
  // 移除冗余操作码判断，直接处理操作数
  rtl_sext(&t1, &id_dest->val, id_dest->width); // 符号扩展保证数据完整性
  rtl_push(&t1);
  print_asm_template1(push);
}

/* 简化pop指令实现 */
make_EHelper(pop) {
  rtl_pop(&t1);
  operand_write(id_dest, &t1); // 统一使用临时寄存器保证原子性
  print_asm_template1(pop);
}

/* 精确实现pusha指令语义 */
make_EHelper(pusha) {
  const uint32_t original_esp = cpu.esp;
  // 严格按照x86手册寄存器顺序压栈
  rtl_push(&cpu.eax);
  rtl_push(&cpu.ecx);
  rtl_push(&cpu.edx);
  rtl_push(&cpu.ebx);
  rtl_push(&original_esp);  // 特殊处理原始ESP值
  rtl_push(&cpu.ebp);
  rtl_push(&cpu.esi);
  rtl_push(&cpu.edi);
  print_asm("pusha");
}

/* 精确实现popa指令语义 */
make_EHelper(popa) {
  // 倒序弹出寄存器，跳过ESP
  rtl_pop(&cpu.edi);
  rtl_pop(&cpu.esi);
  rtl_pop(&cpu.ebp);
  rtl_pop(&t0);            // 丢弃原始ESP
  rtl_pop(&cpu.ebx);
  rtl_pop(&cpu.edx);
  rtl_pop(&cpu.ecx);
  rtl_pop(&cpu.eax);
  print_asm("popa");
}

/* 优化leave指令实现 */
make_EHelper(leave) {
  // 分两步操作保证原子性
  rtl_mv(&cpu.esp, &cpu.ebp); // mov esp, ebp
  rtl_pop(&cpu.ebp);          // pop ebp
  print_asm("leave");
}

/* 符号扩展辅助函数增强 */
static inline void sign_extend(rtlreg_t* dest, int src_width) {
  const int dest_width = decoding.is_operand_size_16 ? 16 : 32;
  rtl_sext(dest, dest, src_width); // 重用RTL接口保证正确性
}

/* 改进的cltd指令实现 */
make_EHelper(cltd) {
  if (decoding.is_operand_size_16) {
    // 处理16位模式：DX:AX = sign_extend(AX)
    sign_extend(&cpu.eax, 2);
    cpu.edx = (cpu.eax >> 16) & 0xffff;
  } else {
    // 处理32位模式：EDX = sign_bit(EAX)
    const uint32_t sign_bit = (cpu.eax >> 31) & 0x1;
    cpu.edx = sign_bit ? 0xffffffff : 0;
  }
  print_asm(decoding.is_operand_size_16 ? "cwtl" : "cltd");
}

/* 增强的符号扩展指令 */
make_EHelper(cwtl) {
  if (decoding.is_operand_size_16) {
    // 8位->16位：AL符号扩展到AX
    sign_extend(&cpu.eax, 1);
  } else {
    // 16位->32位：AX符号扩展到EAX
    sign_extend(&cpu.eax, 2);
  }
  print_asm(decoding.is_operand_size_16 ? "cbtw" : "cwtl");
}

/* 精确的符号扩展移动 */
make_EHelper(movsx) {
  const int dest_width = decoding.is_operand_size_16 ? 2 : 4;
  rtl_sext(&t1, &id_src->val, id_src->width);
  id_dest->width = dest_width; // 显式设置目标宽度
  operand_write(id_dest, &t1);
  print_asm_template2(movsx);
}

/* 精确的零扩展移动 */
make_EHelper(movzx) {
  const int dest_width = decoding.is_operand_size_16 ? 2 : 4;
  id_dest->width = dest_width; // 显式设置目标宽度
  rtl_zext(&t1, &id_src->val, id_src->width);
  operand_write(id_dest, &t1);
  print_asm_template2(movzx);
}

/* 新增LEA指令实现 */
make_EHelper(lea) {
  // 直接传递地址值，不进行内存访问
  rtl_li(&t1, id_src->addr);
  operand_write(id_dest, &t1);
  print_asm_template2(lea);
}
