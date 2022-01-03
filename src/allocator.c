#include "allocator.h"


extern inline void *CObjAllocator_malloc (
  const struct CObjAllocator *self, size_t size);
extern inline void *CObjAllocator_realloc (
  const struct CObjAllocator *self, void *ptr, size_t size);
extern inline void CObjAllocator_free (
  const struct CObjAllocator *self, void *ptr);
