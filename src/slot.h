#ifndef COBJ_SLOT_H
#define COBJ_SLOT_H

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "include/cobj.h"


__attribute__((pure, warn_unused_result, access(read_only, 1)))
/**
 * @memberof CObjSlot
 * @brief Test if slot is empty.
 *
 * @param self Slot.
 * @return @c true if empty.
 */
static inline bool CObjSlot_isnull (const struct CObjSlot *self) {
  return *(void **) self == NULL;
}
__attribute__((pure, warn_unused_result,
               access(read_only, 1), access(read_only, 2)))
/**
 * @memberof CObjSlot
 * @brief Test if two slots are equal.
 *
 * @param self Slot.
 * @param other Slot.
 * @return @c true if equal.
 */
static inline bool CObjSlot_equal (
    const struct CObjSlot *self, const struct CObjSlot *other) {
  return memcmp(self, other, sizeof(*self)) == 0;
}
__attribute__((always_inline, pure, warn_unused_result,
               access(read_only, 1), access(read_only, 2)))
/**
 * @memberof CObjSlot
 * @brief Test if slot is equal to given condition. Only useful for static
 *  constants.
 *
 * @param self Slot.
 * @param name Name.
 * @param ns Namespace.
 * @return @c true if equal.
 */
static inline bool CObjSlot_equal_static (
    const struct CObjSlot *self, const char *name, int ns) {
  struct CObjSlot other = {.ns = ns};
  strncpy(other.name, name, sizeof(other.name));
  return memcmp(self, &other, sizeof(*self)) == 0;
}


#endif /* COBJ_SLOT_H */
