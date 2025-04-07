#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}
//为了防止其他形式，这些先保留
make_EHelper(push) {
printf("开始执行push指令: EIP=0x%x\n", cpu.eip);
  uint8_t reg = decoding.opcode & 0x7;
  rtl_push(&reg_l(reg));

  print_asm_template1(push);
  printf("执行完push指令: EIP=0x%x\n", cpu.eip);
}

make_EHelper(pop) {
  uint8_t reg = decoding.opcode & 0x7;
  rtl_pop(&reg_l(reg));
  print_asm_template1(pop);
}


make_EHelper(pusha) {
  TODO();

  print_asm("pusha");
}

make_EHelper(popa) {
  TODO();

  print_asm("popa");
}

make_EHelper(leave) {

  rtl_mv(&cpu.esp, &cpu.ebp);
  rtl_pop(&cpu.ebp);
  print_asm("leave");
}

make_EHelper(cltd) {
  if (decoding.is_operand_size_16) {
    // 将 AX 符号扩展到 DX
    rtl_sext(&t0, &cpu.eax, 2); // 读取 AX
    cpu.edx = (t0 >> 16) & 0xffff;
  }
  else {
    // 将 EAX 符号扩展到 EDX
    if (cpu.eax & 0x80000000) {
      cpu.edx = 0xffffffff; // 若 EAX 为负数，EDX 全 1
    } else {
      cpu.edx = 0;          // 若 EAX 为正数，EDX 全 0
    }
  }
  print_asm(decoding.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  if (decoding.is_operand_size_16) {
    TODO();
  }
  else {
    TODO();
  }

  print_asm(decoding.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  rtl_sext(&t2, &id_src->val, id_src->width);
  operand_write(id_dest, &t2);
  print_asm_template2(movsx);
}

make_EHelper(movzx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);// 直接赋值（高位自动清零）
  print_asm_template2(movzx);
}


