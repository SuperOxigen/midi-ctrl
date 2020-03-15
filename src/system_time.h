/*
 * MIDI Controller - System Time
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#ifndef _SYSTEM_TIME_H_
#define _SYSTEM_TIME_H_

#include "base.h"

C_SECTION_BEGIN;

/* The system time is a platform-specific monotonically increasing
 * time measure.  This is intended to be used to calculate durations
 * and used for timer events. */
typedef struct {
  uint32_t seconds;  /* Actual max value will depend on the system. */
  uint32_t nanoseconds;  /* 0 to 999,999,999 */
} system_time_t;

/* Implementation is platform specific. */
bool_t SystemTimeNow(system_time_t *time);

/* Total Orderings */
bool_t SystemTimeLessThan(
  system_time_t const *time_a, system_time_t const *time_b);
bool_t SystemTimeLessThanOrEqual(
  system_time_t const *time_a, system_time_t const *time_b);
bool_t SystemTimeEqual(
  system_time_t const *time_a, system_time_t const *time_b);
bool_t SystemTimeGreaterThanOrEqual(
  system_time_t const *time_a, system_time_t const *time_b);
bool_t SystemTimeGreaterThan(
  system_time_t const *time_a, system_time_t const *time_b);
/* Ordering aliases. */
#define SystemTimeLT SystemTimeLessThan
#define SystemTimeLE SystemTimeLessThanOrEqual
#define SystemTimeEQ SystemTimeEqual
#define SystemTimeGE SystemTimeGreaterThanOrEqual
#define SystemTimeGT SystemTimeGreaterThan

/* Time deltas are in absolute values.  Returns false if the inputs are
 * invalid or if the time differences are too large to fit in a uint32_t
 * value.  All deltas will be rounded down. */
bool_t SystemTimeSecondsDelta(
  system_time_t const *time_a, system_time_t const *time_b, uint32_t *s);
bool_t SystemTimeMillisecondsDelta(
  system_time_t const *time_a, system_time_t const *time_b, uint32_t *ms);
bool_t SystemTimeMicrosecondsDelta(
  system_time_t const *time_a, system_time_t const *time_b, uint32_t *us);
bool_t SystemTimeNanosecondsDelta(
  system_time_t const *time_a, system_time_t const *time_b, uint32_t *ns);

/* The arithmetic functions will NOT rollover.  Adding beyond max will
 * simply set the time to its maximum possible value.  Subtracting below
 * zero will result in a zero time. */
bool_t SystemTimeIncrementSeconds(system_time_t *time, uint32_t s);
bool_t SystemTimeIncrementMilliseconds(system_time_t *time, uint32_t ms);
bool_t SystemTimeIncrementMicroseconds(system_time_t *time, uint32_t us);
bool_t SystemTimeIncrementNanoseconds(system_time_t *time, uint32_t ns);

bool_t SystemTimeDecrementSeconds(system_time_t *time, uint32_t s);
bool_t SystemTimeDecrementMilliseconds(system_time_t *time, uint32_t ms);
bool_t SystemTimeDecrementMicroseconds(system_time_t *time, uint32_t us);
bool_t SystemTimeDecrementNanoseconds(system_time_t *time, uint32_t ns);

C_SECTION_END;

#endif  /* _SYSTEM_TIME_H_ */
