#ifndef CMETHOD_H
#define CMETHOD_H

#ifdef __cplusplus
extern "C" {
#else
#include <stdbool.h>
#endif

#include "cobj.h"

/**
 * @file
 * Functions and structures for finding and calling class methods.
 */


/// Auxiliary message to method functions.
struct CObjMsg {
  /// destination function
  CObjFunc func;
  /// data to be passed
  void *data;
};

#ifdef DOXYGEN
/// Virtual type of CObjMsg[], only for documentation.
struct CObjMsgArray {
  /// message array
  struct CObjMsg msgs[];
};
#endif

__attribute__((warn_unused_result, nonnull))
/**
 * @memberof CObjMsgArray
 * @brief Finds a message for the given method and removes it from the list.
 *
 * @param self List of messages.
 * @param func Method to find.
 * @return Message for the given method, or @c NULL if not found.
 */
COBJ_API void *CObjMsgArray_pop (struct CObjMsg *self, CObjFunc func);

struct CObjTrait;
/// type of function to test if a trait is satisfied
typedef bool (*CMethodTraitTestFunc) (
  const struct CObjVariant *var1, const struct CObjVariant *var2,
  const struct CObjTrait *traits, const struct CObjTag **types, int len);

/// CObjTrait comparison type.
enum CObjTraitType {
  /// no comparison; only the presence of the slot is required
  COBJ_TRAIT_NONE = 0,
  /// comparison is done by user-defined function
  COBJ_TRAIT_FUNC,
  /// test equality of types and values of two variants
  COBJ_TRAIT_EQUAL,
  /// test equality of types of two variants
  COBJ_TRAIT_OFTYPE,
  /// test if two variants contain tag sets, and one tag set is a subtype of another
  COBJ_TRAIT_SUBTYPE,
};

/// Argument trait for calling the method.
struct CObjTrait {
  /// left value of the comparison; extracted by resolving the slot path, where
  /// `path[0].name[0]` indicates the index of the argument (starting from 1)
  const struct CObjSlot *path;
  /// right value of the comparison, if any
  struct CObjVariant value;
  /// comparison type, see #CObjTraitType
  unsigned char cmp;
  /// test function; valid when CObjTrait::cmp == ::COBJ_TRAIT_FUNC
  CMethodTraitTestFunc func;
  /// data to be passed to CObjTrait::func; valid when CObjTrait::cmp == ::COBJ_TRAIT_FUNC
  void *userdata;
};

__attribute__((
  warn_unused_result, nonnull(1, 2), access(read_only, 1),
  access(read_only, 2), access(write_only, 4), access(write_only, 5)))
/**
 * @memberof CObjTrait
 * @brief Return tag data that matches argument qualifier.
 *
 * @param self Argument qualifier.
 * @param types Type objects.
 * @param len Length of @p types.
 * @param[out] var1 Matched tag data.
 * @param[out] var2 Matched tag data.
 * @return @c true if matches.
 */
COBJ_API bool CObjTrait_match2 (
  const struct CObjTrait *self, const struct CObjTag **types, int len,
  struct CObjVariant *var1, struct CObjVariant *var2);
__attribute__((pure, warn_unused_result, nonnull, access(read_only, 1),
               access(read_only, 2)))
/**
 * @memberof CObjTrait
 * @brief Return tag data that matches argument qualifier.
 *
 * @param self Argument qualifier.
 * @param types Type objects.
 * @param len Length of @p types.
 * @return @c true if matches.
 */
COBJ_API bool CObjTrait_match (
  const struct CObjTrait *self, const struct CObjTag **types, int len);

/// CObj method.
struct CMethod {
  /// method function
  CObjFunc func;
  /// user data
  void *userdata;
  /// traits required for calling the method
  const struct CObjTrait *traits;
};

__attribute__((pure, warn_unused_result, nonnull,
               access(read_only, 1), access(read_only, 2)))
/**
 * @memberof CMethod
 * @brief Find a suitable method.
 *
 * @param self Method descriptor array.
 * @param types Type objects.
 * @param len Length of @p types.
 * @return Method descriptor, or @c NULL if not found.
 */
COBJ_API const struct CMethod *CMethodArray_find (
  const struct CMethod *self, const struct CObjTag **types, int len);
__attribute__((pure, warn_unused_result, nonnull, sentinel(2),
               access(read_only, 1)))
/**
 * @memberof CMethod
 * @brief Find a suitable method.
 *
 * @param self Method descriptor array.
 * @param ... Type objects, terminated by @c NULL.
 * @return Method descriptor, or @c NULL if not found.
 */
COBJ_API const struct CMethod *CMethodArray_finds (
  const struct CMethod *self, ...);

/// Context for calling CMethod.
struct CMethodContext {
  /// method function
  CObjFunc func;
  /// user data
  void *userdata;
  /// traits for the method
  const struct CObjTrait *traits;

  /// tag sets that own the method
  const struct CObjTag *target;
  /// absloute offset of the structure corresponding to CMethodContext::target
  int offset;

  /// length of CMethodContext::types
  int len;
  /// tag sets of arguments
  const struct CObjTag **types;
  /// auxiliary data array
  struct CObjMsg *msg;
};

__attribute__((nonnull))
/**
 * @memberof CMethodContext
 * @brief Resolve method of given slot and initialize a method context object.
 *
 * @p self->types and @p self->len should be set before calling. @p self->msg is
 *  untouched.
 *
 * @param[in,out] self Method context.
 * @param slot Slot of method.
 * @return 0 on success, 1 if no suitable method found, 255 if @p self->types
 *  or @p self->len invalid.
 */
COBJ_API int CMethodContext_init (
  struct CMethodContext *self, const struct CObjSlot *slot);
__attribute__((pure, warn_unused_result, nonnull, access(read_only, 1)))
/**
 * @memberof CMethodContext
 * @brief Find the `super` tag of `self->types[0]`.
 *
 * @param self Method context.
 * @return `super` tag of `self->types[0]`, or @c NULL if not found.
 */
COBJ_API const struct CObjTag *CMethodContext_super (
  const struct CMethodContext *self);

COBJ_API extern const struct CMethod PointerType_init[];
#define COBJ_TAG_POINTER_INIT { \
  .name = "init", .methods = PointerType_init, .type = COBJ_TYPE_CMETHODS}
COBJ_API extern const struct CMethod ArrayType_len[];
#define COBJ_TAG_ARRAY_LEN { \
  .name = "len", .methods = ArrayType_len, .type = COBJ_TYPE_CMETHODS}
COBJ_API extern const struct CMethod ArrayType_size[];
#define COBJ_TAG_ARRAY_SIZE { \
  .name = "size", .methods = ArrayType_size, .type = COBJ_TYPE_CMETHODS}
COBJ_API extern const struct CMethod ArrayType_destroy[];
#define COBJ_TAG_ARRAY_DESTROY { \
  .name = "destroy", .methods = ArrayType_destroy, .type = COBJ_TYPE_CMETHODS}
COBJ_API extern const struct CMethod ArrayType_init[];
#define COBJ_TAG_ARRAY_INIT { \
  .name = "init", .methods = ArrayType_init, .type = COBJ_TYPE_CMETHODS}


#ifdef __cplusplus
}
#endif

#endif /* CMETHOD_H */
