#include "cpu/exec.h"

void diff_test_skip_qemu();
void diff_test_skip_nemu();

make_EHelper(lidt) {
  // TODO();
  rtl_li(&t0, id_dest->addr);
  rtl_li(&cpu.idtr.limit,vaddr_read(t0,2));
  rtl_li(&cpu.idtr.base,vaddr_read(t0+2,4));
  if(decoding.is_operand_size_16)
    cpu.idtr.base &= 0x00ffffff;

#ifdef DEBUG
  Log("idtr.limit=0x%x", cpu.idtr.limit);
  Log("idtr.base=0x%x", cpu.idtr.base);
#endif
  print_asm_template1(lidt);
}

make_EHelper(mov_r2cr) {
  switch (id_dest->reg) {
	case 0:
		cpu.cr0.val = id_src->val;
		break;
	case 3:
		cpu.cr3.val = id_src->val;
		break;
	default:
		Assert(0, "Shoule reach here for NO cr%d", id_dest->reg);
		break;
  }

  print_asm("movl %%%s,%%cr%d", reg_name(id_src->reg, 4), id_dest->reg);
}

make_EHelper(mov_cr2r) {
  switch (id_src->reg) {
	case 0:
		operand_write(id_dest, &cpu.cr0.val);
		break;
	case 3:
		operand_write(id_dest, &cpu.cr3.val);
		break;
	default:
		Assert(0, "Shoule reach here for NO cr%d", id_dest->reg);
		break;
  }

  print_asm("movl %%cr%d,%%%s", id_src->reg, reg_name(id_dest->reg, 4));

#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}

make_EHelper(int) {
  raise_intr(id_dest->val, decoding.seq_eip);
  print_asm("int %s", id_dest->str);
#ifdef DIFF_TEST
  diff_test_skip_nemu();
#endif
}

make_EHelper(iret) {

  rtl_pop(&decoding.jmp_eip);
  decoding.jmp_eip = true;
  // 使用临时变量解决类型不匹配问题
  rtlreg_t cs_temp;
  rtl_pop(&cs_temp);
  cpu.cs = (uint16_t)cs_temp;
  rtl_pop(&t0);
  memcpy(&cpu.eflags, &t0, sizeof(cpu.eflags));
  
  print_asm("iret");
}

uint32_t pio_read(ioaddr_t, int);
void pio_write(ioaddr_t, int, uint32_t);

make_EHelper(in) {
  uint32_t port_val = pio_read(id_src->val, id_dest->width);
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
