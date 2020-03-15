/*
 * MIDI Controller - System Time - Native
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#ifdef _PLATFORM_NATIVE

#include <errno.h>
#include <time.h>

#include "logging.h"
#include "system_time.h"

bool_t SystemTimeNow(system_time_t *system_time) {
  if (system_time == NULL) return false;
  struct timespec ts;
  if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
    int const err = errno;
    if (err == EINVAL) {
      LOG_ERROR("Linux CLOCK_MONOTONIC is unavailable");
    } else {
      LOG_ERROR("Unknown error: errno = %d", err);
    }
    return false;
  }
  system_time->seconds = ts.tv_sec;
  system_time->nanoseconds = ts.tv_nsec;
  LOG_DEBUG(
      "Now: time = %u.%09u", system_time->seconds, system_time->nanoseconds);
  return true;
}

#endif  /* _PLATFORM_NATIVE */
