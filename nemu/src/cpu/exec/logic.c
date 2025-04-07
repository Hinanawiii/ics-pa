#include "cpu/exec.h"

make_EHelper(test) {
  rtl_and(&t0, &id_dest->val, &id_src->val);
  
  rtl_update_ZFSF(&t0, id_dest->width);
  
  rtl_li(&t1, 0);
  rtl_set_CF(&t1);
  rtl_set_OF(&t1);
  
  print_asm_template2(test);
}

make_EHelper(and) {
/*
  printf("AND调试信息:\n");
  printf("  操作码: 0x%02x\n", decoding.opcode);
  printf("  目标操作数宽度: %d\n", id_dest->width);
  printf("  目标操作数值: 0x%x\n", id_dest->val);
  printf("  源操作数值: 0x%x\n", id_src->val);
  */
  
  rtl_and(&t0, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t0); 
//  printf("  执行结果: 0x%x\n", t0);
  rtl_update_ZFSF(&t0, id_dest->width);
  
  rtl_li(&t1, 0);
  rtl_set_CF(&t1);
  rtl_set_OF(&t1);
  
  print_asm_template2(and);
}

make_EHelper(xor) {
  rtl_xor(&t0, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t0);
  
  rtl_update_ZFSF(&t0, id_dest->width);
  
  rtl_li(&t1, 0);
  rtl_set_CF(&t1);
  rtl_set_OF(&t1);
  
  print_asm_template2(xor);
}

make_EHelper(or) {
  rtl_or(&t0, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t0);
  
  rtl_update_ZFSF(&t0, id_dest->width);
  
  rtl_li(&t1, 0);
  rtl_set_CF(&t1);
  rtl_set_OF(&t1);
  
  print_asm_template2(or);
}

make_EHelper(sar) {
  // 算术右移 (保留符号位)
  rtl_sar(&t0, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t0);
  
  rtl_update_ZFSF(&t0, id_dest->width);
  
  // unnecessary to update CF and OF in NEMU
  print_asm_template2(sar);
}

make_EHelper(shl) {
  // 逻辑左移
  rtl_shl(&t0, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t0);
  
  rtl_update_ZFSF(&t0, id_dest->width);
  
  // unnecessary to update CF and OF in NEMU
  print_asm_template2(shl);
}

make_EHelper(shr) {
  // 逻辑右移
  rtl_shr(&t0, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t0);
  
  rtl_update_ZFSF(&t0, id_dest->width);
  
  // unnecessary to update CF and OF in NEMU
  print_asm_template2(shr);
}

make_EHelper(setcc) {
  uint8_t subcode = decoding.opcode & 0xf;
  rtl_setcc(&t2, subcode);
  operand_write(id_dest, &t2);
  print_asm("set%s %s", get_cc_name(subcode), id_dest->str);
}

make_EHelper(not) {
  rtl_mv(&t0, &id_dest->val);
  rtl_not(&t0);
  operand_write(id_dest, &t0);
  
  print_asm_template1(not);
}
