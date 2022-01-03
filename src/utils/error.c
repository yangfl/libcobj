#include <execinfo.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "include/cobj.h"
#include "utils/macro.h"
#include "../tag.h"
#include "error.h"


static void print_backtrace (int skip, FILE *stream, bool with_header) {
  void *symbols[64];
  register const int symbols_size = arraysize(symbols);
  skip++;
  if unlikely (skip >= symbols_size) {
    fputs("(too many frames skipped)\n", stream);
    return;
  }
  int size = backtrace(symbols, symbols_size);
  should (size > 0) otherwise {
    fputs("(no available backtrace)\n", stream);
    return;
  }

  if (with_header) {
    fputs("Backtrace:\n", stream);
  }

  int skipped = size;
  if likely (size < symbols_size) {
    skipped -= 2;
  }
  skipped -= skip;
  if unlikely (skipped < 0) {
    skipped = 0;
  }
  backtrace_symbols_fd(symbols + skip, skipped, fileno(stream));
  if (size == symbols_size) {
    fputs("...and possibly more\n", stream);
  }
}


int CObjTagArray_putname (const struct CObjTag *self, FILE *stream) {
  static const struct CObjSlot slot = {.name = "name"};
  const struct CObjVariant *v = CObjTagArray_find(self, &slot);
  int ret = v != NULL && v->type == COBJ_TYPE_UNDEFINED ?
    fputs(v->ptr, stream) : fprintf(stream, "<%p>", (void *) self);

  const struct CObjTag *super = CObjTagArray_find_public(self);
  if (super != NULL) {
    ret += fputs("(", stream);
    while (1) {
      ret += CObjTagArray_putname(super, stream);
      super = CObjTagArray_find_public(self + 1);
      if (super == NULL) {
        ret += fputs(")", stream);
        break;
      }
      ret += fputs(", ", stream);
    }
  }

  return ret;
}


void CObjTagArray_TypeError (
    const struct CObjTag *self, const char *format, ...) {
  fputs("TypeError at ", stderr);
  CObjTagArray_putname(self, stderr);
  fprintf(stderr, "(%p): ", (void *) self);
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  fputc('\n', stderr);
  print_backtrace(0, stderr, true);
  exit(EXIT_FAILURE);
}
