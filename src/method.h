#ifndef COBJ_METHOD_H
#define COBJ_METHOD_H

#include <stdbool.h>
#include <stddef.h>

#include "include/cmethod.h"
#include "tag.h"


__attribute__((pure, warn_unused_result, nonnull, access(read_only, 1)))
/**
 * @memberof CObjTrait
 * @brief Test if argument qualifier is empty.
 *
 * @param self Argument qualifier.
 * @return @c true if empty.
 */
static inline bool CObjTrait_isnull (const struct CObjTrait *self) {
  return self->path == NULL;
}

__attribute__((pure, warn_unused_result, nonnull(1),
               access(read_only, 1), access(read_only, 2)))
/**
 * @memberof CMethod
 * @brief Test if method can be called with given types.
 *
 * @param self Method descriptor.
 * @param types Type objects.
 * @param len Length of @p types.
 * @return @c true if match.
 */
bool CMethod_match (
  const struct CMethod *self, const struct CObjTag **types, int len);


#endif /* COBJ_METHOD_H */
