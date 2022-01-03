#ifndef COBJ_MESSAGE_H
#define COBJ_MESSAGE_H

#include <stddef.h>

#include "include/cmethod.h"
#include "utils/macro.h"


__attribute__((warn_unused_result, nonnull(2)))
/**
 * @memberof CObjMsgArray
 * @brief Finds a message for the given method and removes it from the list.
 *
 * @param self List of messages. Can be @c NULL.
 * @param func Method to find.
 * @return Message for the given method, or @c NULL if not found.
 */
static inline void *CObjMsg_popany (struct CObjMsg *self, CObjFunc func) {
  return likely (self == NULL) ? NULL : CObjMsgArray_pop(self, func);
}


#endif /* COBJ_MESSAGE_H */
