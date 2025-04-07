#include "cpu/exec.h"

make_EHelper(jmp) {
  // the target address is calculated at the decode stage
  decoding.is_jmp = 1;
  print_asm("jmp %x", decoding.jmp_eip);
}

make_EHelper(jcc) {
  // the target address is calculated at the decode stage
  uint8_t subcode = decoding.opcode & 0xf;
  rtl_setcc(&t2, subcode);
  decoding.is_jmp = t2;
  print_asm("j%s %x", get_cc_name(subcode), decoding.jmp_eip);
}

make_EHelper(jmp_rm) {
  decoding.jmp_eip = id_dest->val;
  decoding.is_jmp = 1;
  print_asm("jmp *%s", id_dest->str);
}

make_EHelper(call) {
  vaddr_t current_eip = *eip;
  // 正确读取4字节的偏移量
  uint32_t raw_rel32 = vaddr_read(current_eip+1, 4);
  int32_t rel32 = (int32_t)raw_rel32;
  decoding.jmp_eip = current_eip + 5 + rel32;  // 5 = 操作码1字节 + 偏移量4字节
  
  // 保存返回地址
  vaddr_t ret_addr = current_eip + 5;
  rtl_push(&ret_addr);
  
  // 设置跳转标志，不要修改eip
  decoding.is_jmp = 1;
  
  print_asm("call 0x%x", decoding.jmp_eip);
}

make_EHelper(ret) {
  rtl_pop(&decoding.jmp_eip);    
  decoding.is_jmp = 1;           
  print_asm("ret");
}

make_EHelper(call_rm) {
  rtl_push(eip + 2);  // 保存返回地址（当前eip + ModR/M字节长度）
  decoding.jmp_eip = id_dest->val;  
  decoding.is_jmp = 1;
  print_asm("call *%s", id_dest->str);
}
