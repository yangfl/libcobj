#ifndef COBJ_H
#define COBJ_H

#ifdef __cplusplus
extern "C" {
#else
#include <stdbool.h>
#endif

#include <stddef.h>
#include <stdint.h>

/**
 * @file
 * Functions and structures for class and properties.
 */


/** @cond GARBAGE */
#ifdef COBJ_BUILD
#define COBJ_API __attribute__((visibility("default")))
#else
#define COBJ_API
#endif
/** @endcond */

/// Memory allocator.
struct CObjAllocator {
  /// memory allocator, if @c NULL, standard `malloc` is used
  void *(*malloc) (size_t size);
  /// memory reallocator, if @c NULL, standard `realloc` is used
  void *(*realloc) (void *ptr, size_t size);
  /// memory deallocator, if @c NULL, standard `free` is used
  void (*free) (void *ptr);
};

struct CObjTag;
struct CMethod;
/// universal type of function
typedef void (*CObjFunc) ();

/// Type of variable.
enum CObjVariantType {
  /// type unknown
  COBJ_TYPE_UNDEFINED = 0,
  /// data type is CObjSlot[]
  COBJ_TYPE_PATH,
  /// data type is CObjTag[]
  COBJ_TYPE_TAGS,
  /// data type is CMethod[]
  COBJ_TYPE_CMETHODS,
  /// data type is ::CObjFunc
  COBJ_TYPE_FUNC,
};

/// Variable container.
struct CObjVariant {
  union {
    /// user-defined data; valid when CObjVariant::type == ::COBJ_TYPE_UNDEFINED
    void *ptr;
    /// user-defined data; valid when CObjVariant::type == ::COBJ_TYPE_UNDEFINED
    long value;
    /// slot path; valid when CObjVariant::type == ::COBJ_TYPE_PATH
    const struct CObjSlot *path;
    /// tags; valid when CObjVariant::type == ::COBJ_TYPE_TAGS
    const struct CObjTag *tags;
    /// methods; valid when CObjVariant::type == ::COBJ_TYPE_CMETHODS
    const struct CMethod *methods;
    /// function; valid when CObjVariant::type == ::COBJ_TYPE_FUNC
    CObjFunc func;
  };
  /// CObjVariantType indicating the type of data
  unsigned char type;
};

/// Slot name.
struct CObjSlot {
  /// name, the first 4 bytes cannot be all zero
  char name[14];
  /// namespace, to avoid name collision
  int16_t ns;
};

/// Tag.
struct CObjTag {
  union {
    /// tag slot
    struct CObjSlot slot;
    struct {
      /// name of tag slot, see CObjSlot::name
      char name[14];
      /// namespace of tag slot, see CObjSlot::ns
      int16_t ns;
    };
  };
  union {
    /// tag value
    struct CObjVariant data;
    struct {
      union {
        /// user-defined data; valid when CObjTag::type == ::COBJ_TYPE_UNDEFINED
        void *ptr;
        /// user-defined data; valid when CObjTag::type == ::COBJ_TYPE_UNDEFINED
        long value;
        /// slot path; valid when CObjTag::type == ::COBJ_TYPE_PATH
        const struct CObjSlot *path;
        /// tags; valid when CObjTag::type == ::COBJ_TYPE_TAGS
        const struct CObjTag *tags;
        /// methods; valid when CObjTag::type == ::COBJ_TYPE_CMETHODS
        const struct CMethod *methods;
        /// function; valid when CObjTag::type == ::COBJ_TYPE_FUNC
        CObjFunc func;
      };
      /// CObjVariantType indicating the type of data
      unsigned char type;
      union {
        /// whether to expose the tag set to the outside; valid when CObjTag::type == ::COBJ_TYPE_TAGS
        bool public_;
        /// whether to re-resolve the path from the top tag set; valid when CObjTag::type == ::COBJ_TYPE_PATH
        bool virtual_;
      };
      /// offset of the structure associated with the tag set; valid when CObjTag::type == ::COBJ_TYPE_TAGS
      unsigned short offset;
    };
  };
};

#ifdef DOXYGEN
/// Virtual type of CObjTag[], only for documentation.
struct CObjTagArray {
  /// tag set
  struct CObjTag tags[];
};
#endif

__attribute__((warn_unused_result, nonnull, access(read_only, 1),
               access(read_only, 2)))
/**
 * @memberof CObjTagArray
 * @brief Find a tag by name.
 *
 * @param self Tag set.
 * @param slot Slot name.
 * @return Tag data, or @c NULL if not found.
 */
COBJ_API const struct CObjVariant *CObjTagArray_find (
  const struct CObjTag *self, const struct CObjSlot *slot);
__attribute__((
  warn_unused_result, nonnull(1, 2), access(read_only, 1),
  access(read_only, 2), access(write_only, 3), access(write_only, 4)))
/**
 * @memberof CObjTagArray
 * @brief Resolve a slot name to a tag.
 *
 * @param self Tag set.
 * @param slot Slot name.
 * @param[out] target The target tag set.
 * @param[out] offset Offset of the target tag set.
 * @return Tag data, or @c NULL if not found.
 */
COBJ_API const struct CObjVariant *CObjTagArray_resolve (
  const struct CObjTag *self, const struct CObjSlot *slot,
  const struct CObjTag **target, int *offset);
__attribute__((
  warn_unused_result, nonnull(1, 2), access(read_only, 1),
  access(read_only, 2), access(write_only, 3), access(write_only, 4)))
/**
 * @memberof CObjTagArray
 * @brief Resolve a slot path to a tag.
 *
 * @param self Tag set.
 * @param path Slot path.
 * @param[out] target The target tag set.
 * @param[out] offset Offset of the target tag set.
 * @return Tag data, or @c NULL if not found.
 */
COBJ_API const struct CObjVariant *CObjTagArray_resolves (
  const struct CObjTag *self, const struct CObjSlot *path,
  const struct CObjTag **target, int *offset);
__attribute__((pure, warn_unused_result, nonnull, access(read_only, 1),
               access(read_only, 2), access(read_only, 3)))
/**
 * @memberof CObjTagArray
 * @brief Get property of object. If property is not found, return default.
 *
 * @param self Tag set.
 * @param slot Slot name.
 * @param obj Object.
 * @param default_ Default value.
 * @return Property value, or @p default_ if not found.
 */
COBJ_API long CObjTagArray_get (
  const struct CObjTag *self, const struct CObjSlot *slot, const void *obj,
  long default_);
__attribute__((copy(CObjTagArray_get)))
/**
 * @memberof CObjTagArray
 * @brief Get property of object. If property is not found, return 0.
 *
 * @param self Tag set.
 * @param slot Slot name.
 * @param obj Object.
 * @return Property value, or 0 if not found.
 */
static inline long CObjTagArray_get0 (
    const struct CObjTag *self, const struct CObjSlot *slot, const void *obj) {
  return CObjTagArray_get (self, slot, obj, 0);
}

/// tag initializer of `name = s`
#define COBJ_TAG_NAME(s) {.name = "name", .ptr = s}
/// tag initializer of `size = n`
#define COBJ_TAG_SIZE(n) {.name = "size", .value = n}
/// tag initializer of `size = sizeof(t)`
#define COBJ_TAG_SIZEOF(t) COBJ_TAG_SIZE(sizeof(t))

#define COBJ_TYPE(n) COBJ_API extern const struct CObjTag n[]
COBJ_TYPE(Imm1Type);
COBJ_TYPE(Imm2Type);
COBJ_TYPE(Imm4Type);
COBJ_TYPE(Imm8Type);
COBJ_TYPE(Imm16Type);
COBJ_TYPE(PointerType);


#ifdef __cplusplus
}
#endif

#endif /* COBJ_H */
