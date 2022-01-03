#include "include/cmethod.h"


#define COBJ_TYPEDEF_IMM(n) \
  const struct CObjTag Imm ## n ## Type[] = { \
    COBJ_TAG_SIZE(n), \
    COBJ_TAG_NAME("Imm" # n), \
  }
COBJ_TYPEDEF_IMM(1);
COBJ_TYPEDEF_IMM(2);
COBJ_TYPEDEF_IMM(4);
COBJ_TYPEDEF_IMM(8);
COBJ_TYPEDEF_IMM(16);
