#include <stddef.h>

#include "include/cmethod.h"
#include "utils/macro.h"
#include "tag.h"
#include "property.h"


long CObjTagArray_get (
    const struct CObjTag *self, const struct CObjSlot *slot, const void *obj,
    long default_) {
  struct CMethodContext context;
  const struct CObjVariant *v =
    CObjTagArray_resolve(self, slot, &context.target, &context.offset);
  return_if_fail (v != NULL) default_;
  switch (__builtin_expect(v->type, COBJ_TYPE_UNDEFINED)) {
    case COBJ_TYPE_UNDEFINED:
      return v->value;
    case COBJ_TYPE_FUNC:
      context.func = v->func;
      context.userdata = NULL;
      context.traits = NULL;
      context.types = &self;
      context.len = 1;
      context.msg = NULL;
      return ((long (*) ()) v->func)((char *) obj + context.offset, &context);
    default:
      return default_;
  }
}
