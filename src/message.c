#include <stddef.h>

#include "include/cmethod.h"
#include "message.h"


void *CObjMsgArray_pop (struct CObjMsg *self, CObjFunc func) {
  for (; self->func != NULL; self++) {
    if (self->func == func && self->data != NULL) {
      void *data = self->data;
      self->data = NULL;
      return data;
    }
  }
  return NULL;
}
