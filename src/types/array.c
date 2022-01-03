#include <stdbool.h>
#include <string.h>

#include "include/cmethod.h"
#include "utils/macro.h"
#include "allocator.h"



static const struct CObjSlot slot_size = {.name = "size"};
static const struct CObjSlot slot_isnull = {.name = "isnull"};
static const struct CObjSlot slot_allocator = {.name = "allocator"};
static const struct CObjSlot slot_super = {.name = "super"};
static const struct CObjSlot slot_init = {.name = "init"};
static const struct CObjSlot slot_destroy = {.name = "destroy"};

static const struct CObjSlot path_1_super_size[] = {
  {.name = {1}}, {.name = "super"}, {.name = "size"}, {{0}}};
static const struct CObjSlot path_1_super[] = {
  {.name = {1}}, {.name = "super"}, {{0}}};
static const struct CObjSlot path_2_super[] = {
  {.name = {2}}, {.name = "super"}, {{0}}};
static const struct CObjSlot path_2_super_super[] = {
  {.name = {2}}, {.name = "super"}, {.name = "super"}, {{0}}};

static const struct CObjTrait trait_AsuperPeqBsuper_AsuperIsize[] = {
  {.path = path_1_super, .value = {.path = path_2_super},
   .cmp = COBJ_TRAIT_EQUAL},
  {.path = path_1_super_size},
  {0}
};
#define trait_AsuperIsize (trait_AsuperPeqBsuper_AsuperIsize + 1)
static const struct CObjTrait trait_AsuperPeqBsuperIsuper_AsuperIsize[] = {
  {.path = path_1_super, .value = {.path = path_2_super_super},
   .cmp = COBJ_TRAIT_EQUAL},
  {.path = path_1_super_size},
  {0}
};


const struct CObjTag *CMethodContext_super (const struct CMethodContext *self) {
  return_if_fail (self->types != NULL) NULL;
  const struct CObjTag *type = self->types[0];
  return_if_fail (type != NULL) NULL;
  const struct CObjVariant *v = CObjTagArray_find(type, &slot_super);
  return_if_fail (v != NULL && v->type == COBJ_TYPE_TAGS) NULL;
  return v->tags;
}


static bool memnull (const void *self, int len) {
  int i;
  for (i = 0; i < (len & ~7); i += 8) {
    return_if_fail (((const uint64_t *) self)[i] == 0) false;
  }
  i -= 8;
  for (; i < len; i++) {
    return_if_fail (((const char *) self)[i] == 0) false;
  }
  return true;
}


static int CMethodContext_init_copyer (
    struct CMethodContext *self, const struct CObjTag *type,
    struct CObjMsg *msg) {
  const struct CObjTag *types[] = {type, type};
  self->types = types;
  self->len = 2;
  self->msg = msg;
  return CMethodContext_init(self, &slot_init);
}


int Pointer_init_copy (
    void **self, const void **other, const struct CMethodContext *ctx) {
  const struct CObjTag *super = CMethodContext_super(ctx);
  return_if_fail (super != NULL) 255;

  int size = CObjTagArray_get0(super, &slot_size, *other);
  return_if_fail (size >= 0) 255;

  if unlikely (size == 0) {
    *self = NULL;
  } else {
    const struct CObjAllocator *allocator =
      (void *) CObjTagArray_get0(super, &slot_allocator, self);
    *self = CObjAllocator_malloc(allocator, size);
    return_if_fail (*self != NULL) -1;
    struct CMethodContext context;
    if (CMethodContext_init_copyer(&context, super, ctx->msg) != 0) {
      memcpy(*self, *other, size);
    } else {
      int res = ((int (*) ()) context.func)(*self, *other, &context);
      should (res == 0) otherwise {
        CObjAllocator_free(allocator, *self);
        return res;
      }
    }
  }
  return 0;
}
const struct CMethod PointerType_init[] = {
  {.func = (CObjFunc) Pointer_init_copy,
   .traits = trait_AsuperPeqBsuper_AsuperIsize},
  {0}
};


static int Array_len_ (
    const void *self, int size, const struct CObjTag *super,
    struct CObjMsg *msg) {
  struct CMethodContext context;
  context.types = &super;
  context.len = 1;
  context.msg = msg;
  bool has_isnull = CMethodContext_init(&context, &slot_isnull) == 0;
  int n;
  for (n = 0; has_isnull ?
        !((bool (*) ()) context.func)((char *) self + size * n, &context) :
        !memnull((char *) self + size * n, size); n++) { }
  return n;
}
int Array_len (const void *self, const struct CMethodContext *ctx) {
  const struct CObjTag *super = CMethodContext_super(ctx);
  return_if_fail (super != NULL) -1;

  int size = CObjTagArray_get0(super, &slot_size, self);
  return_if_fail (size > 0) -1;

  return Array_len_(self, size, super, ctx->msg);
}
const struct CMethod ArrayType_len[] = {
  {.func = (CObjFunc) Array_len, .traits = trait_AsuperIsize},
  {0}
};


int Array_size (const void *self, const struct CMethodContext *ctx) {
  const struct CObjTag *super = CMethodContext_super(ctx);
  return_if_fail (super != NULL) -1;

  int size = CObjTagArray_get0(super, &slot_size, self);
  return_if_fail (size > 0) -1;

  int n = Array_len_(self, size, super, ctx->msg);
  return_if_fail (n >= 0) -1;

  return size * n;
}
const struct CMethod ArrayType_size[] = {
  {.func = (CObjFunc) Array_size, .traits = trait_AsuperIsize},
  {0}
};


static void Array_destroy_ (void *self, int size, struct CMethodContext *ctx) {
  ctx->len = 1;
  if (CMethodContext_init(ctx, &slot_destroy) != 0) {
    for (int i = 0; i < size; i++) {
      ctx->func((char *) self + size * i, ctx);
    }
  }
}
void Array_destroy (void *self, struct CMethodContext *ctx) {
  const struct CObjTag *super = CMethodContext_super(ctx);
  return_if_fail (super != NULL);

  int size = CObjTagArray_get0(super, &slot_size, self);
  return_if_fail (size > 0);

  struct CMethodContext context;
  context.types = &super;
  context.msg = ctx->msg;
  Array_destroy_(self, size, &context);
}
const struct CMethod ArrayType_destroy[] = {
  {.func = (CObjFunc) Array_destroy, .traits = trait_AsuperIsize},
  {0}
};


int Array_init_copy (
    void *self, const void *other, const struct CMethodContext *ctx) {
  const struct CObjTag *super = CMethodContext_super(ctx);
  return_if_fail (super != NULL) 255;

  int size = CObjTagArray_get0(super, &slot_size, other);
  return_if_fail (size > 0) 255;

  int n = Array_len_(other, size, super, ctx->msg);
  return_if (n == 0) 0;

  struct CMethodContext context;
  if (CMethodContext_init_copyer(&context, super, ctx->msg) != 0) {
    memcpy(self, other, size * n);
  } else {
    for (int i = 0; i < n; i++) {
      int res = ((int (*) ()) context.func)(
        (char *) self + size * i, (char *) other + size * i, &context);
      should (res == 0) otherwise {
        Array_destroy_(self, size, &context);
        return res;
      }
    }
  }
  return 0;
}
int Array_init_flatten (
    void *self, const void **other, const struct CMethodContext *ctx) {
  const struct CObjTag *super = CMethodContext_super(ctx);
  return_if_fail (super != NULL) 255;

  int size = CObjTagArray_get0(super, &slot_size, other[0]);
  return_if_fail (size > 0) 255;

  int n;
  for (n = 0; other[n] != NULL; n++) { }
  return_if (n == 0) 0;

  struct CMethodContext context;
  bool has_copyer = CMethodContext_init_copyer(&context, super, ctx->msg) != 0;
  for (int i = 0; i < n; i++) {
    if (!has_copyer) {
      memcpy((char *) self + size * i, other[i], size);
    } else {
      int res = ((int (*) ()) context.func)(
        (char *) self + size * i, other[i], &context);
      should (res == 0) otherwise {
        Array_destroy_(self, size, &context);
        return res;
      }
    }
  }
  return 0;
}
const struct CMethod ArrayType_init[] = {
  {.func = (CObjFunc) Array_init_copy,
   .traits = trait_AsuperPeqBsuper_AsuperIsize},
  {.func = (CObjFunc) Array_init_flatten,
   .traits = trait_AsuperPeqBsuperIsuper_AsuperIsize},
  {0}
};
