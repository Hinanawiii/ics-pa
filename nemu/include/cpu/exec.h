#ifndef __CPU_EXEC_H__
#define __CPU_EXEC_H__

#include "nemu.h"

#define make_EHelper(name) void concat(exec_, name) (vaddr_t *eip)
typedef void (*EHelper) (vaddr_t *);

#include "cpu/decode.h"

static inline uint32_t instr_fetch(vaddr_t *eip, int len) {
  uint32_t instr = vaddr_read(*eip, len);
#ifdef DEBUG
  uint8_t *p_instr = (void *)&instr;
  int i;
  for (i = 0; i < len; i ++) {
    decoding.p += sprintf(decoding.p, "%02x ", p_instr[i]);
  }
#endif
  (*eip) += len;
  return instr;
}

void rtl_setcc(rtlreg_t*, uint8_t);
void raise_intr(uint8_t NO, vaddr_t ret_addr);

static inline const char* get_cc_name(int subcode) {
  static const char *cc_name[] = {
    "o", "no", "b", "nb",
    "e", "ne", "be", "nbe",
    "s", "ns", "p", "np",
    "l", "nl", "le", "nle"
  };
  return cc_name[subcode];
}

#ifdef DEBUG
#define print_asm(...) Assert(snprintf(decoding.assembly, 80, __VA_ARGS__) < 80, "buffer overflow!")
#else
#define print_asm(...)
#endif

#define suffix_char(width) ((width) == 4 ? 'l' : ((width) == 1 ? 'b' : ((width) == 2 ? 'w' : '?')))

#define print_asm_template1(instr) \
  print_asm(str(instr) "%c %s", suffix_char(id_dest->width), id_dest->str)

#define print_asm_template2(instr) \
  print_asm(str(instr) "%c %s,%s", suffix_char(id_dest->width), id_src->str, id_dest->str)

#define print_asm_template3(instr) \
  print_asm(str(instr) "%c %s,%s,%s", suffix_char(id_dest->width), id_src->str, id_src2->str, id_dest->str)

make_EHelper(add);
make_EHelper(inc);
make_EHelper(sub);
make_EHelper(dec);
make_EHelper(cmp);
make_EHelper(neg);
make_EHelper(adc);
make_EHelper(sbb);
make_EHelper(mul);
make_EHelper(imul1);
make_EHelper(imul2);
make_EHelper(imul3);
make_EHelper(div);
make_EHelper(idiv);


make_EHelper(push);
make_EHelper(push_rm);
make_EHelper(pop);
make_EHelper(leave);
make_EHelper(cwtl);
make_EHelper(cltd);
make_EHelper(movzx);
make_EHelper(movsx);

make_EHelper(lea);
make_EHelper(ret);
make_EHelper(call);
make_EHelper(jmp);
make_EHelper(call_rm);
make_EHelper(jmp_rm);
make_EHelper(jcc);
make_EHelper(none);
make_EHelper(nop);

make_EHelper(not);
make_EHelper(and);
make_EHelper(or);
make_EHelper(xor);
make_EHelper(shl);
make_EHelper(shr);
make_EHelper(sar);
make_EHelper(setcc);
make_EHelper(test);

make_EHelper(in);
make_EHelper(out);



#endif
