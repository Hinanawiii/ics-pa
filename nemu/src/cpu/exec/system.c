#include "cpu/exec.h"

void diff_test_skip_qemu();
void diff_test_skip_nemu();

make_EHelper(lidt) {
  TODO();

  print_asm_template1(lidt);
}

make_EHelper(mov_r2cr) {
  TODO();

  print_asm("movl %%%s,%%cr%d", reg_name(id_src->reg, 4), id_dest->reg);
}

make_EHelper(mov_cr2r) {
  TODO();

  print_asm("movl %%cr%d,%%%s", id_src->reg, reg_name(id_dest->reg, 4));

#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}

make_EHelper(int) {
  TODO();

  print_asm("int %s", id_dest->str);

#ifdef DIFF_TEST
  diff_test_skip_nemu();
#endif
}

make_EHelper(iret) {
  TODO();

  print_asm("iret");
}

uint32_t pio_read(ioaddr_t, int);
void pio_write(ioaddr_t, int, uint32_t);

make_EHelper(in) {
  uint32_t port_val;
  
  // 特殊情况：对串口状态寄存器(0x3FD)的读取，始终返回0x20
  if (id_src->val == 0x3FD) {
    port_val = 0x20;  // 串口总是就绪的
  } else {
    port_val = pio_read(id_src->val, id_dest->width);
  }
  
  //printf("Reading port 0x%x, width %d, value 0x%x\n", id_src->val, id_dest->width, port_val);
  
  rtl_li(&t0, port_val);
  operand_write(id_dest, &t0);
  
  print_asm_template2(in);
#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}

make_EHelper(out) {
  pio_write(id_dest->val, id_src->width, id_src->val);
  print_asm_template2(out);
#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}
