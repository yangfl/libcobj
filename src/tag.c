#include <stddef.h>

#include "include/cobj.h"
#include "utils/macro.h"
#include "utils/error.h"
#include "slot.h"
#include "variant.h"
#include "tag.h"


static const struct CObjTag *_CObjTagArray_find (
    const struct CObjTag *self, const struct CObjSlot *slot) {
  for (; !CObjTag_isnull(self); self++) {
    return_if (CObjTag_match(self, slot)) self;
  }
  return NULL;
}


const struct CObjVariant *CObjTagArray_find (
    const struct CObjTag *self, const struct CObjSlot *slot) {
  const struct CObjTag *tag = _CObjTagArray_find(self, slot);
  return tag == NULL ? NULL : &tag->data;
}


const struct CObjTag *CObjTagArray_find_public (const struct CObjTag *self) {
  for (; !CObjTag_isnull(self); self++) {
    return_if (self->public_ && self->type == COBJ_TYPE_TAGS &&
               self->tags != NULL) self;
  }
  return NULL;
}


static const struct CObjTag *CObjTagArray_next_public (
    const struct CObjTag *self) {
  return likely (self != NULL) ? CObjTagArray_find_public(self + 1) : NULL;
}


// non-recursively resolve single slot
static const struct CObjTag *CObjTagArray_resolve_simple (
    const struct CObjTag *self, const struct CObjSlot *slot,
    const struct CObjTag **target, int *offset, int cur_offset) {
  // find in self
  const struct CObjTag *tag = _CObjTagArray_find(self, slot);
  if (tag != NULL) {
    if (target != NULL) {
      *target = self;
    }
    if (offset != NULL) {
      *offset = cur_offset;
    }
    return tag;
  }

  // find in super class(es)
  const struct CObjTag *super = CObjTagArray_next_public(self - 1);
  return_if_fail (super != NULL) NULL;
  for (const struct CObjTag *next_super = super; ; super = next_super) {
    next_super = CObjTagArray_next_public(next_super);
    return_if_fail (next_super != NULL) CObjTagArray_resolve_simple(
      super, slot, target, offset, cur_offset + super->offset);
    const struct CObjTag *tag = CObjTagArray_resolve_simple(
      super, slot, target, offset, cur_offset + super->offset);
    return_if (tag != NULL) tag;
  }
}


// recursively resolve slot name
const struct CObjVariant *CObjTagArray_resolve (
    const struct CObjTag *self, const struct CObjSlot *slot,
    const struct CObjTag **target, int *offset) {
  const struct CObjTag *tgt;
  if (target == NULL) {
    target = &tgt;
  }
  const struct CObjTag *tag = CObjTagArray_resolve_simple(
    self, slot, target, offset, 0);
  return_if_fail (tag != NULL) NULL;
  const struct CObjVariant *v = &tag->data;
  return_if_fail (!CObjVariant_isvalid(v)) v;
  should (v->path != NULL && !CObjSlot_isnull(v->path)) otherwise {
    CObjTagArray_TypeError(*target, "Alias tag %s@%d has invalid path",
                    slot->name, slot->ns);
  }
  should (!CObjSlot_equal(slot, v->path)) otherwise {
    CObjTagArray_TypeError(*target, "Alias tag %s@%d points to itself",
                    slot->name, slot->ns);
  }
  return CObjTagArray_resolves(
    tgt->virtual_ ? self : *target, v->path, target, offset);
}


// recursively resolve slot path
const struct CObjVariant *CObjTagArray_resolves (
    const struct CObjTag *self, const struct CObjSlot *path,
    const struct CObjTag **target, int *offset) {
  return_if_fail (CObjSlot_isnull(path)) NULL;
  for (; ; path++) {
    const struct CObjVariant *v = CObjTagArray_resolve(
      self, path, target, offset);
    return_if_fail (v != NULL && !CObjSlot_isnull(path + 1)) v;
    return_if_fail (v->type == COBJ_TYPE_TAGS && v->tags != NULL) NULL;
    self = v->tags;
  }
}


bool _CObjTagArray_is_derived (
    const struct CObjTag *self, const struct CObjTag *base) {
  const struct CObjTag *super = CObjTagArray_next_public(self - 1);
  return_if_fail (super != NULL) false;
  for (const struct CObjTag *next_super = super; ; super = next_super) {
    next_super = CObjTagArray_next_public(next_super);
    return_if_fail (next_super != NULL) _CObjTagArray_is_derived(super, base);
    return_if_fail (self != base) true;
    return_if (_CObjTagArray_is_derived(super, base)) true;
  }
}


bool CObjTagArray_is_derived (
    const struct CObjTag *self, const struct CObjTag *base) {
  return_if_fail (base != NULL && self != base) true;
  return_if_fail (self != NULL) false;
  return _CObjTagArray_is_derived(self, base);
}
