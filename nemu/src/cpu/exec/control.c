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
  // 使用解码阶段计算好的目标地址
  rtl_push(eip + 5);  // 保存下一条指令的地址（假设是32位偏移的call）
  decoding.is_jmp = 1;
  print_asm("call %x", decoding.jmp_eip);
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
