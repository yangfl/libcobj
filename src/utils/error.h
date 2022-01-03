#ifndef COBJ_UTILS_ERROR_H
#define COBJ_UTILS_ERROR_H

#include <stdio.h>

#include "include/cobj.h"


__attribute__((access(read_only, 1)))
/**
 * @memberof CObjTagArray
 * @brief Print the name of the given object.
 *
 * @param self Tag set.
 * @param stream The stream to print to.
 * @return Number of characters printed.
 */
int CObjTagArray_putname (const struct CObjTag *self, FILE *stream);

__attribute__((noreturn, format(printf, 2, 3)))
/**
 * @memberof CObjTagArray
 * @brief Print error message of tag set to the stream @c stderr and terminate
 *  the program. This function does not return.
 *
 * @param self Tag set.
 * @param format Format string.
 * @param ... Format arguments.
 */
void CObjTagArray_TypeError (
  const struct CObjTag *self, const char *format, ...);


#endif /* COBJ_UTILS_ERROR_H */
