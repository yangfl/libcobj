#include <stdarg.h>
#include <stdbool.h>

#include "include/cmethod.h"
#include "utils/macro.h"
#include "slot.h"
#include "variant.h"
#include "method.h"


static const struct CObjVariant *CObjSlotArray_resolves_args (
    const struct CObjSlot *path, const struct CObjTag **types, int len,
    struct CObjVariant *buf) {
  return_if_fail
    (path != NULL && 0 < path[0].name[0] && path[0].name[0] <= len) NULL;
  const struct CObjTag *type = types[path[0].name[0] - 1];
  if unlikely (CObjSlot_isnull(path + 1)) {
    buf->tags = type;
    buf->type = COBJ_TYPE_TAGS;
  } else {
    return_if_fail (type != NULL) NULL;
    const struct CObjVariant *v =
      CObjTagArray_resolves(type, path + 1, NULL, NULL);
    return_if_fail (v != NULL) NULL;
    *buf = *v;
  }
  return buf;
}


bool CObjTrait_match2 (
    const struct CObjTrait *self, const struct CObjTag **types, int len,
    struct CObjVariant *var1, struct CObjVariant *var2) {
  struct CObjVariant buf1;
  const struct CObjVariant *v1 =
    CObjSlotArray_resolves_args(self->path, types, len, &buf1);

  bool ret;
  if likely (self->cmp == COBJ_TRAIT_NONE) {
    ret = v1 != NULL;
  } else {
    struct CObjVariant buf2;
    const struct CObjVariant *v2 = likely (self->value.type != COBJ_TYPE_PATH) ?
      &self->value :
      CObjSlotArray_resolves_args(self->value.path, types, len, &buf2);
    if unlikely (self->cmp == COBJ_TRAIT_FUNC) {
      ret = self->func(v1, v2, self, types, len);
    } else if unlikely (v1 == NULL || v2 == NULL) {
      ret = v1 == NULL && v2 == NULL;
    } else {
      switch (self->cmp) {
        case COBJ_TRAIT_EQUAL:
          ret = CObjVariant_equal(v1, v2);
          break;
        case COBJ_TRAIT_OFTYPE:
          ret = v1->type == v2->type;
          break;
        case COBJ_TRAIT_SUBTYPE:
          ret = v1->type == COBJ_TYPE_TAGS && v1->type == v2->type &&
                CObjTagArray_is_derived(v1->tags, v2->tags);
          break;
        default:
          ret = false;
          break;
      }
    }
    if unlikely (var2 != NULL && v2 != NULL) {
      *var2 = *v2;
    }
  }

  if unlikely (var1 != NULL && v1 != NULL) {
    *var1 = *v1;
  }
  return ret;
}


bool CObjTrait_match (
    const struct CObjTrait *self, const struct CObjTag **types, int len) {
  return CObjTrait_match2(self, types, len, NULL, NULL);
}


bool CMethod_match (
    const struct CMethod *self, const struct CObjTag **types, int len) {
  for (const struct CObjTrait *trait = self->traits;
       !CObjTrait_isnull(trait); trait++) {
    return_if_fail (CObjTrait_match(trait, types, len)) false;
  }
  return true;
}


const struct CMethod *CMethodArray_find (
    const struct CMethod *self, const struct CObjTag **types, int len) {
  for (; self->func != NULL; self++) {
    return_if (CMethod_match(self, types, len)) self;
  }
  return NULL;
}


const struct CMethod *CMethodArray_finds (const struct CMethod *self, ...) {
  int len;
  {
    va_list ap;
    va_start(ap, self);
    for (len = 0; va_arg(ap, const struct CObjTag *) != NULL; len++) { }
    va_end(ap);
  }

  const struct CObjTag *types[len + 1];
  va_list ap;
  va_start(ap, self);
  for (int i = 0; i <= len; i++) {
    types[i] = va_arg(ap, const struct CObjTag *);
  }
  va_end(ap);

  return CMethodArray_find(self, types, len);
}


int CMethodContext_init (
    struct CMethodContext *self, const struct CObjSlot *slot) {
  const struct CObjTag **types = self->types;
  int len = self->len;

  const struct CObjTag *base = types[0];
  return_if_fail (len > 0 && base != NULL) 255;
  const struct CObjVariant *v =
    CObjTagArray_resolve(base, slot, &self->target, &self->offset);
  return_if_fail (v != NULL) 1;
  switch (__builtin_expect(v->type, COBJ_TYPE_CMETHODS)) {
    default:
      return 1;
    case COBJ_TYPE_FUNC:
      return_if_fail (v->func != NULL) 2;
      self->func = v->func;
      self->userdata = NULL;
      self->traits = NULL;
      break;
    case COBJ_TYPE_CMETHODS:
      return_if_fail (v->methods != NULL) 2;
      const struct CMethod *method = CMethodArray_find(v->methods, types, len);
      return_if_fail (method != NULL) 2;
      self->func = method->func;
      self->userdata = method->userdata;
      self->traits = method->traits;
      break;
  }
  return 0;
}
