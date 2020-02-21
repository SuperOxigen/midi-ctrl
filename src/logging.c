/*
 * MIDI Controller - Logging Functionality.
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#ifdef _PLATFORM_NATIVE
#include <stdio.h>
#include <stdarg.h>

#include "logging.h"

char const *const kLogDebug = "DEBUG";
char const *const kLogInfo = "INFO";
char const *const kLogError = "ERROR";

size_t Trace(char const *func, char const *fmt, ...) {
  va_list ap;
  size_t res;
  res = printf("TRACE %s(", func == NULL ? "Unknown" : func);
  va_start(ap, fmt);
  res += vprintf(fmt, ap);
  va_end(ap);
  res += printf(")\n");
  return res;
}

size_t Log(
    char const *filename, uint16_t lineno, char const *level,
    char const *fmt, ...) {
  va_list ap;
  size_t res;
  res = printf("%10s:%-3u ", filename == NULL ? "unknown" : filename, lineno);
  res += printf("%5s ", level == NULL ? "UNK" : level);
  va_start(ap, fmt);
  res += vprintf(fmt, ap);
  va_end(ap);
  res += printf("\n");
  return res;
}

#endif  /* _PLATFORM_NATIVE */
