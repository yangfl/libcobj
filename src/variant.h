#ifndef COBJ_VARIANT_H
#define COBJ_VARIANT_H

#include <stdbool.h>

#include "include/cobj.h"


__attribute__((pure, warn_unused_result, nonnull, access(read_only, 1)))
/**
 * @memberof CObjVariant
 * @brief Test if a variant contains final value (do not require further
 *  resolving).
 *
 * @param self Variant.
 * @return @c true if contains final value.
 */
static inline bool CObjVariant_isvalid (const struct CObjVariant *self) {
  return self->type != COBJ_TYPE_PATH;
}
__attribute__((pure, warn_unused_result, nonnull,
               access(read_only, 1), access(read_only, 2)))
/**
 * @memberof CObjVariant
 * @brief Test if two variants are equal.
 *
 * @param self Variant.
 * @param other Variant.
 * @return @c true if equal.
 */
static inline bool CObjVariant_equal (
    const struct CObjVariant *self, const struct CObjVariant *other) {
  return self->value == other->value && self->type == other->type;
}


#endif /* COBJ_VARIANT_H */
