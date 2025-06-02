#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}
//为了防止其他形式，这些先保留
make_EHelper(push)  {
  // 检查操作码
  if (decoding.opcode >= 0x50 && decoding.opcode <= 0x57) {
    // 推送寄存器
    uint8_t reg = decoding.opcode & 0x7;
    rtl_push(&reg_l(reg));
    print_asm("push %%%s", reg_name(reg, 4));
  } 
  else if (decoding.opcode == 0x6A || decoding.opcode == 0x68) {
    // 推送立即数
    rtl_push(&id_dest->val);
    print_asm_template1(push);
  }
}

make_EHelper(push_rm) {
  // 处理内存操作数的push指令
  rtl_push(&id_dest->val);
  print_asm_template1(push);
}

make_EHelper(pop) {
  uint8_t reg = decoding.opcode & 0x7;
  rtl_pop(&reg_l(reg));
  print_asm_template1(pop);
}


make_EHelper(pusha) {
  t0 = cpu.esp;
  rtl_push(&cpu.eax);
  rtl_push(&cpu.ecx);
  rtl_push(&cpu.edx);
  rtl_push(&cpu.ebx);
  rtl_push(&t0);
  rtl_push(&cpu.ebp);
  rtl_push(&cpu.esi);
  rtl_push(&cpu.edi);

  print_asm("pusha");
}

make_EHelper(popa) {
  rtl_pop(&cpu.edi);
  rtl_pop(&cpu.esi);
  rtl_pop(&cpu.ebp);
  rtl_pop(&t0);
  rtl_pop(&cpu.ebx);
  rtl_pop(&cpu.edx);
  rtl_pop(&cpu.ecx);
  rtl_pop(&cpu.eax);

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
  
  //printf("MOVSX: src_width=%d, dest_width=%d, src_val=0x%x\n", id_src->width, id_dest->width, id_src->val);
    
  rtl_sext(&t2, &id_src->val, id_src->width);
  
  //printf("After sign extension: t2=0x%x\n", t2);
  
  operand_write(id_dest, &t2);
  print_asm_template2(movsx);
}

make_EHelper(movzx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  rtl_zext(&t2, &id_src->val, id_src->width);
  operand_write(id_dest, &t2);
  print_asm_template2(movzx);
}


