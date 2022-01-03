#ifndef COBJ_TAG_H
#define COBJ_TAG_H

#include <stdbool.h>

#include "include/cobj.h"
#include "slot.h"


__attribute__((copy(CObjSlot_isnull)))
/**
 * @memberof CObjTag
 * @brief Test if a tag is empty.
 *
 * @param self Tag.
 * @return @c true if empty.
 */
static inline bool CObjTag_isnull (const struct CObjTag *self) {
  return CObjSlot_isnull(&self->slot);
}
__attribute__((copy(CObjSlot_equal)))
/**
 * @memberof CObjTag
 * @brief Test if tag has the given slot.
 *
 * @param self Tag.
 * @param slot Slot.
 * @return @c true if slot is equal.
 */
static inline bool CObjTag_match (
    const struct CObjTag *self, const struct CObjSlot *slot) {
  return CObjSlot_equal(&self->slot, slot);
}
__attribute__((copy(CObjSlot_equal_static)))
/**
 * @memberof CObjTag
 * @brief Test if tag has the given slot. Only useful for static constants.
 *
 * @param self Tag.
 * @param name Name.
 * @param ns Namespace.
 * @return @c true if slot is equal.
 */
static inline bool CObjTag_match_static (
    const struct CObjTag *self, const char *name, int ns) {
  return CObjSlot_equal_static(&self->slot, name, ns);
}

__attribute__((pure, warn_unused_result, nonnull, access(read_only, 1)))
/**
 * @memberof CObjTagArray
 * @brief Get the first public tag.
 *
 * @param self Tag set.
 * @return First public tag, or @c NULL if not found.
 */
const struct CObjTag *CObjTagArray_find_public (const struct CObjTag *self);
__attribute__((pure, warn_unused_result, access(read_only, 1),
               access(read_only, 2)))
/**
 * @memberof CObjTagArray
 * @brief Test if tag set is a subtype of another tag set.
 *
 * @param self Tag set.
 * @param base Base tag set.
 * @return @c true if @p base is a subtype of @p self.
 */
bool CObjTagArray_is_derived (
  const struct CObjTag *self, const struct CObjTag *base);


#endif /* COBJ_TAG_H */
