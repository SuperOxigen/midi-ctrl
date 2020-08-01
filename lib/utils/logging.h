/*
 * MIDI Controller - Logging Functionality.
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#ifndef _LOGGING_H_
#define _LOGGING_H_

#include "base.h"

#ifdef _TRACE_ENABLED
size_t Trace(char const *func, char const *fmt, ...)
  __attribute__((format (printf, 2, 3)));
# define LOG_TRACE(fmt, ...) Trace(__func__, fmt, __VA_ARGS__)
#else
# define LOG_TRACE(...)
#endif  /* _TRACE_ENABLED */

#ifdef _LOG_ENABLED
extern char const *const kLogDebug;
extern char const *const kLogInfo;
extern char const *const kLogError;

size_t Log(
    char const *filename, uint16_t lineno, char const *level,
    char const *fmt, ...)
  __attribute__((format (printf, 4, 5)));
# define LOG_DEBUG(fmt, ...) \
  Log(__FILE__, __LINE__, kLogDebug, fmt, ##__VA_ARGS__)
# define LOG_INFO(fmt, ...) \
  Log(__FILE__, __LINE__, kLogInfo, fmt, ##__VA_ARGS__)
# define LOG_ERROR(fmt, ...) \
  Log(__FILE__, __LINE__, kLogError, fmt, ##__VA_ARGS__)
#else
# define LOG_ERROR(...)
# define LOG_INFO(...)
# define LOG_DEBUG(...)
#endif

#endif  /* _LOGGING_H_ */
