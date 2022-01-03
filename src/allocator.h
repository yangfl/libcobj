#ifndef COBJ_ALLOCATOR_H
#define COBJ_ALLOCATOR_H

#include <stddef.h>
#include <stdlib.h>

#include "include/cobj.h"
#include "utils/macro.h"


__attribute__((malloc, warn_unused_result, alloc_size(2), access(read_only, 1)))
/**
 * @memberof CObjAllocator
 * @brief Allocate memory.
 *
 * @param self Allocator object.
 * @param size Memory size.
 * @return Allocated memory.
 */
inline void *CObjAllocator_malloc (
    const struct CObjAllocator *self, size_t size) {
  return unlikely (self != NULL) && likely (self->malloc != NULL) ?
    self->malloc(size) : malloc(size);
}

__attribute__((warn_unused_result, alloc_size(3), access(read_only, 1)))
/**
 * @memberof CObjAllocator
 * @brief Re-allocate the previously allocated block in @p ptr, making the new
 *  block @p size bytes long.
 *
 * @param self Allocator object.
 * @param ptr Previously allocated memory.
 * @param size New memory size.
 * @return Re-allocated memory.
 */
inline void *CObjAllocator_realloc (
    const struct CObjAllocator *self, void *ptr, size_t size) {
  return unlikely (self != NULL) && likely (self->realloc != NULL) ?
    self->realloc(ptr, size) : realloc(ptr, size);
}

__attribute__((access(read_only, 1)))
/**
 * @memberof CObjAllocator
 * @brief Free a block allocated by CObjAllocator_malloc() or
 *  CObjAllocator_realloc().
 *
 * @param self Allocator object.
 * @param ptr Allocated memory.
 */
inline void CObjAllocator_free (const struct CObjAllocator *self, void *ptr) {
  if (unlikely (self != NULL) && likely (self->free != NULL)) {
    self->free(ptr);
  } else {
    free(ptr);
  }
}


#endif /* COBJ_ALLOCATOR_H */
