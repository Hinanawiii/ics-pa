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
  printf("Indirect jump/call to address 0x%x\n", id_dest->val);
}

make_EHelper(call) {
  // 手动读取偏移量并计算目标地址
  vaddr_t current_eip = *eip;
  
  // 读取4字节偏移量
  uint32_t raw_rel32 = vaddr_read(current_eip, 4);
  int32_t rel32 = (int32_t)raw_rel32;
  
  // 计算目标地址和返回地址
  vaddr_t target_addr = current_eip + 4 + rel32;
  vaddr_t ret_addr = current_eip + 4;
  
  /*
  // 打印调试信息
  printf("CALL调试信息:\n");
  printf("  当前EIP: 0x%x\n", current_eip);
  printf("  读取到的raw偏移量: 0x%x\n", raw_rel32);
  printf("  转换为有符号的偏移量: 0x%x (%d)\n", rel32, rel32);
  printf("  计算后的目标地址: 0x%x\n", target_addr);
  printf("  返回地址: 0x%x\n", ret_addr);
  */
  
  // 设置跳转目标和保存返回地址
  decoding.jmp_eip = target_addr;
  rtl_push(&ret_addr);
  
  // 设置跳转标志
  decoding.is_jmp = 1;
  
  print_asm("call %x", decoding.jmp_eip);
}

make_EHelper(ret) {
  rtl_pop(&decoding.jmp_eip);    
  decoding.is_jmp = 1;           
  print_asm("ret");
}

make_EHelper(call_rm) {
  rtl_push(eip + decoding.seq_eip - *eip);   // 保存返回地址（当前eip + ModR/M字节长度）
  decoding.jmp_eip = id_dest->val;  
  decoding.is_jmp = 1;
  print_asm("call *%s", id_dest->str);
  printf("Indirect jump/call to address 0x%x\n", id_dest->val);
}
