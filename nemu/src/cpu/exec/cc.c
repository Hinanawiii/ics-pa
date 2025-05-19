#include "cpu/rtl.h"

/* Condition Code */

void rtl_setcc(rtlreg_t* dest, uint8_t subcode) {
  bool invert = subcode & 0x1;
  enum {
    CC_O, CC_NO, CC_B,  CC_NB,
    CC_E, CC_NE, CC_BE, CC_NBE,
    CC_S, CC_NS, CC_P,  CC_NP,
    CC_L, CC_NL, CC_LE, CC_NLE
  };

  // TODO: Query EFLAGS to determine whether the condition code is satisfied.
  // dest <- ( cc is satisfied ? 1 : 0)
  switch (subcode & 0xe) {
    case CC_O:  // OF=1
      rtl_get_OF(dest);
      break;
    case CC_B:  // CF=1
      rtl_get_CF(dest);
      break;
    case CC_E:  // ZF=1
      rtl_get_ZF(dest);
      break;
    case CC_BE: // CF=1 || ZF=1
      rtl_get_CF(&t0);
      rtl_get_ZF(&t1);
      rtl_or(dest, &t0, &t1);
      break;
    case CC_S:  // SF=1
      rtl_get_SF(dest);
      break;
    case CC_L:  // SF != OF
      rtl_get_SF(&t0);
      rtl_get_OF(&t1);
      rtl_xor(dest, &t0, &t1);
      break;
    case CC_LE: // ZF=1 || SF != OF
      rtl_get_SF(&t0);
      rtl_get_OF(&t1);
      rtl_xor(&t2, &t0, &t1);
      rtl_get_ZF(&t0);
      rtl_or(dest, &t0, &t2);
      break;
    case CC_NO: // OF=0
      rtl_get_OF(dest);
      rtl_xori(dest, dest, 1);
      break;
    case CC_NB: // CF=0
      rtl_get_CF(dest);
      rtl_xori(dest, dest, 1);
      break;
    case CC_NE: // ZF=0
      rtl_get_ZF(dest);
      rtl_xori(dest, dest, 1);
      break;
    case CC_NBE: // CF=0 && ZF=0
      rtl_get_CF(&t0);
      rtl_get_ZF(&t1);
      rtl_or(&t2, &t0, &t1);
      rtl_xori(dest, &t2, 1);
      break;
    case CC_NS: // SF=0
      rtl_get_SF(dest);
      rtl_xori(dest, dest, 1);
      break;
    case CC_NL: // SF == OF
      rtl_get_SF(&t0);
      rtl_get_OF(&t1);
      rtl_xor(dest, &t0, &t1);
      rtl_xori(dest, dest, 1);
      break;
    case CC_NLE: // ZF=0 && SF == OF
      rtl_get_ZF(&t0);
      rtl_get_SF(&t1);
      rtl_get_OF(&t2);
      rtl_xor(&t3, &t1, &t2);
      rtl_xori(&t3, &t3, 1);
      rtl_xori(&t0, &t0, 1);
      rtl_and(dest, &t0, &t3);
      break;
    case CC_P:  // PF=1 (未实现)
      rtl_li(dest, 0);
      break;
    default: 
      panic("should not reach here");
  }

  if (invert) {
    rtl_xori(dest, dest, 0x1);
  }
}
