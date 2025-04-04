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
  // the target address is calculated at the decode stage
  vaddr_t current_eip = *eip;
	uint32_t raw_rel32 = vaddr_read(current_eip+1 , 3);
	int32_t rel32 = (int32_t)raw_rel32;
  decoding.jmp_eip = current_eip + 5 + rel32;
  vaddr_t ret_addr = current_eip + 5;
  rtl_push(&ret_addr);
  
  *eip += 5;
  
  decoding.is_jmp = 1;
	print_asm("call 0x%x", decoding.jmp_eip);
}

make_EHelper(ret) {
  rtl_pop(&decoding.jmp_eip);    
  decoding.is_jmp = 1;           
  print_asm("ret");
}



make_EHelper(call_rm) {//暂时保留
  TODO();
  print_asm("call *%s", id_dest->str);
}
