/*
 * MIDI Controller - System Time Test
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#include <string.h>
#include <unity.h>

#include "system_time.h"

#define MAX_U32 4294967295u
#define MAX_NANOSECONDS 999999999u
#define MAX_SECONDS MAX_U32

static system_time_t const kZeroTime = {
  .seconds = 0u,
  .nanoseconds = 0u
};

static system_time_t const kMaxTime = {
  .seconds = MAX_SECONDS,
  .nanoseconds = MAX_NANOSECONDS
};

static system_time_t const kMaxMilliDeltaTime = {
  .seconds = 4294967u,
  .nanoseconds = 295999999u
};

static system_time_t const kMaxMicroDeltaTime = {
  .seconds = 4294u,
  .nanoseconds = 967295999u
};

static system_time_t const kMaxNanoDeltaTime = {
  .seconds = 4u,
  .nanoseconds = 294967295u
};

static system_time_t const kInvalidTime = {
  .seconds = 5000u,
  .nanoseconds = MAX_NANOSECONDS + 24u
};

#ifdef _PLATFORM_NATIVE
#include <stdio.h>
#include <unistd.h>

static void TestSystemTime_NativeMonotonic(void)  {
  TEST_ASSERT_FALSE(SystemTimeNow(NULL));
  system_time_t start = {};
  TEST_ASSERT_TRUE(SystemTimeNow(&start));
  static unsigned int const kOneSecond = 1;
  printf("Sleeping for %u seconds...\n", kOneSecond);
  unsigned int const res = sleep(kOneSecond);
  TEST_ASSERT_EQUAL(0, res);
  system_time_t end = {};
  TEST_ASSERT_TRUE(SystemTimeNow(&end));
  TEST_ASSERT_TRUE(SystemTimeLessThan(&start, &end));
}
#endif  /* _PLATFORM_NATIVE */

static void TestSystemTime_LessThan(void) {
  TEST_ASSERT_FALSE(SystemTimeLessThan(NULL, &kZeroTime));
  TEST_ASSERT_FALSE(SystemTimeLessThan(&kInvalidTime, &kZeroTime));
  TEST_ASSERT_FALSE(SystemTimeLessThan(&kZeroTime, NULL));
  TEST_ASSERT_FALSE(SystemTimeLessThan(&kZeroTime, &kInvalidTime));
  TEST_ASSERT_FALSE(SystemTimeLessThan(NULL, NULL));
  TEST_ASSERT_FALSE(SystemTimeLessThan(&kInvalidTime, &kInvalidTime));

  TEST_ASSERT_TRUE(SystemTimeLessThan(&kZeroTime, &kMaxTime));
  TEST_ASSERT_FALSE(SystemTimeLessThan(&kMaxTime, &kZeroTime));

  TEST_ASSERT_FALSE(SystemTimeLessThan(&kZeroTime, &kZeroTime));
  TEST_ASSERT_FALSE(SystemTimeLessThan(&kMaxTime, &kMaxTime));

  system_time_t a = {
    .seconds = 5000,
    .nanoseconds = 750000
  };
  TEST_ASSERT_TRUE(SystemTimeLessThan(&kZeroTime, &a));
  TEST_ASSERT_FALSE(SystemTimeLessThan(&kMaxTime, &a));
  TEST_ASSERT_FALSE(SystemTimeLessThan(&a, &kZeroTime));
  TEST_ASSERT_TRUE(SystemTimeLessThan(&a, &kMaxTime));
  TEST_ASSERT_FALSE(SystemTimeLessThan(&a, &a));

  system_time_t b = {
    .seconds = a.seconds,
    .nanoseconds = a.nanoseconds + 1
  };
  TEST_ASSERT_TRUE(SystemTimeLessThan(&a, &b));
  TEST_ASSERT_FALSE(SystemTimeLessThan(&b, &a));

  b.nanoseconds = a.nanoseconds - 1;
  TEST_ASSERT_FALSE(SystemTimeLessThan(&a, &b));
  TEST_ASSERT_TRUE(SystemTimeLessThan(&b, &a));

  b.seconds = a.seconds + 1;
  b.nanoseconds = a.nanoseconds;
  TEST_ASSERT_TRUE(SystemTimeLessThan(&a, &b));
  TEST_ASSERT_FALSE(SystemTimeLessThan(&b, &a));

  b.seconds = a.seconds - 1;
  TEST_ASSERT_FALSE(SystemTimeLessThan(&a, &b));
  TEST_ASSERT_TRUE(SystemTimeLessThan(&b, &a));
}

static void TestSystemTime_LessThanOrEqual(void) {
  TEST_ASSERT_FALSE(SystemTimeLessThanOrEqual(NULL, &kZeroTime));
  TEST_ASSERT_FALSE(SystemTimeLessThanOrEqual(&kInvalidTime, &kZeroTime));
  TEST_ASSERT_FALSE(SystemTimeLessThanOrEqual(&kZeroTime, NULL));
  TEST_ASSERT_FALSE(SystemTimeLessThanOrEqual(&kZeroTime, &kInvalidTime));
  TEST_ASSERT_FALSE(SystemTimeLessThanOrEqual(NULL, NULL));
  TEST_ASSERT_FALSE(SystemTimeLessThanOrEqual(&kInvalidTime, &kInvalidTime));

  TEST_ASSERT_TRUE(SystemTimeLessThanOrEqual(&kZeroTime, &kMaxTime));
  TEST_ASSERT_FALSE(SystemTimeLessThanOrEqual(&kMaxTime, &kZeroTime));

  TEST_ASSERT_TRUE(SystemTimeLessThanOrEqual(&kZeroTime, &kZeroTime));
  TEST_ASSERT_TRUE(SystemTimeLessThanOrEqual(&kMaxTime, &kMaxTime));

  system_time_t a = {
    .seconds = 5000,
    .nanoseconds = 750000
  };
  TEST_ASSERT_TRUE(SystemTimeLessThanOrEqual(&kZeroTime, &a));
  TEST_ASSERT_FALSE(SystemTimeLessThanOrEqual(&kMaxTime, &a));
  TEST_ASSERT_FALSE(SystemTimeLessThanOrEqual(&a, &kZeroTime));
  TEST_ASSERT_TRUE(SystemTimeLessThanOrEqual(&a, &kMaxTime));
  TEST_ASSERT_TRUE(SystemTimeLessThanOrEqual(&a, &a));

  system_time_t b = {
    .seconds = a.seconds,
    .nanoseconds = a.nanoseconds + 1
  };
  TEST_ASSERT_TRUE(SystemTimeLessThanOrEqual(&a, &b));
  TEST_ASSERT_FALSE(SystemTimeLessThanOrEqual(&b, &a));

  b.nanoseconds = a.nanoseconds - 1;
  TEST_ASSERT_FALSE(SystemTimeLessThanOrEqual(&a, &b));
  TEST_ASSERT_TRUE(SystemTimeLessThanOrEqual(&b, &a));

  b.seconds = a.seconds + 1;
  b.nanoseconds = a.nanoseconds;
  TEST_ASSERT_TRUE(SystemTimeLessThanOrEqual(&a, &b));
  TEST_ASSERT_FALSE(SystemTimeLessThanOrEqual(&b, &a));

  b.seconds = a.seconds - 1;
  TEST_ASSERT_FALSE(SystemTimeLessThanOrEqual(&a, &b));
  TEST_ASSERT_TRUE(SystemTimeLessThanOrEqual(&b, &a));
}

static void TestSystemTime_Equal(void) {
  TEST_ASSERT_FALSE(SystemTimeEqual(NULL, &kZeroTime));
  TEST_ASSERT_FALSE(SystemTimeEqual(&kInvalidTime, &kZeroTime));
  TEST_ASSERT_FALSE(SystemTimeEqual(&kZeroTime, NULL));
  TEST_ASSERT_FALSE(SystemTimeEqual(&kZeroTime, &kInvalidTime));
  TEST_ASSERT_FALSE(SystemTimeEqual(NULL, NULL));
  TEST_ASSERT_FALSE(SystemTimeEqual(&kInvalidTime, &kInvalidTime));

  TEST_ASSERT_FALSE(SystemTimeEqual(&kZeroTime, &kMaxTime));
  TEST_ASSERT_FALSE(SystemTimeEqual(&kMaxTime, &kZeroTime));

  TEST_ASSERT_TRUE(SystemTimeEqual(&kZeroTime, &kZeroTime));
  TEST_ASSERT_TRUE(SystemTimeEqual(&kMaxTime, &kMaxTime));

  system_time_t a = {
    .seconds = 5000,
    .nanoseconds = 750000
  };
  TEST_ASSERT_FALSE(SystemTimeEqual(&kZeroTime, &a));
  TEST_ASSERT_FALSE(SystemTimeEqual(&kMaxTime, &a));
  TEST_ASSERT_FALSE(SystemTimeEqual(&a, &kZeroTime));
  TEST_ASSERT_FALSE(SystemTimeEqual(&a, &kMaxTime));
  TEST_ASSERT_TRUE(SystemTimeEqual(&a, &a));

  system_time_t b = {
    .seconds = a.seconds,
    .nanoseconds = a.nanoseconds + 1
  };
  TEST_ASSERT_FALSE(SystemTimeEqual(&a, &b));
  TEST_ASSERT_FALSE(SystemTimeEqual(&b, &a));

  b.nanoseconds = a.nanoseconds - 1;
  TEST_ASSERT_FALSE(SystemTimeEqual(&a, &b));
  TEST_ASSERT_FALSE(SystemTimeEqual(&b, &a));

  b.seconds = a.seconds + 1;
  b.nanoseconds = a.nanoseconds;
  TEST_ASSERT_FALSE(SystemTimeEqual(&a, &b));
  TEST_ASSERT_FALSE(SystemTimeEqual(&b, &a));

  b.seconds = a.seconds - 1;
  TEST_ASSERT_FALSE(SystemTimeEqual(&a, &b));
  TEST_ASSERT_FALSE(SystemTimeEqual(&b, &a));
}

static void TestSystemTime_GreaterThan(void) {
  TEST_ASSERT_FALSE(SystemTimeGreaterThan(NULL, &kZeroTime));
  TEST_ASSERT_FALSE(SystemTimeGreaterThan(&kInvalidTime, &kZeroTime));
  TEST_ASSERT_FALSE(SystemTimeGreaterThan(&kZeroTime, NULL));
  TEST_ASSERT_FALSE(SystemTimeGreaterThan(&kZeroTime, &kInvalidTime));
  TEST_ASSERT_FALSE(SystemTimeGreaterThan(NULL, NULL));
  TEST_ASSERT_FALSE(SystemTimeGreaterThan(&kInvalidTime, &kInvalidTime));

  TEST_ASSERT_FALSE(SystemTimeGreaterThan(&kZeroTime, &kMaxTime));
  TEST_ASSERT_TRUE(SystemTimeGreaterThan(&kMaxTime, &kZeroTime));

  TEST_ASSERT_FALSE(SystemTimeGreaterThan(&kZeroTime, &kZeroTime));
  TEST_ASSERT_FALSE(SystemTimeGreaterThan(&kMaxTime, &kMaxTime));

  system_time_t a = {
    .seconds = 5000,
    .nanoseconds = 750000
  };
  TEST_ASSERT_FALSE(SystemTimeGreaterThan(&kZeroTime, &a));
  TEST_ASSERT_TRUE(SystemTimeGreaterThan(&kMaxTime, &a));
  TEST_ASSERT_TRUE(SystemTimeGreaterThan(&a, &kZeroTime));
  TEST_ASSERT_FALSE(SystemTimeGreaterThan(&a, &kMaxTime));
  TEST_ASSERT_FALSE(SystemTimeGreaterThan(&a, &a));

  system_time_t b = {
    .seconds = a.seconds,
    .nanoseconds = a.nanoseconds + 1
  };
  TEST_ASSERT_FALSE(SystemTimeGreaterThan(&a, &b));
  TEST_ASSERT_TRUE(SystemTimeGreaterThan(&b, &a));

  b.nanoseconds = a.nanoseconds - 1;
  TEST_ASSERT_TRUE(SystemTimeGreaterThan(&a, &b));
  TEST_ASSERT_FALSE(SystemTimeGreaterThan(&b, &a));

  b.seconds = a.seconds + 1;
  b.nanoseconds = a.nanoseconds;
  TEST_ASSERT_FALSE(SystemTimeGreaterThan(&a, &b));
  TEST_ASSERT_TRUE(SystemTimeGreaterThan(&b, &a));

  b.seconds = a.seconds - 1;
  TEST_ASSERT_TRUE(SystemTimeGreaterThan(&a, &b));
  TEST_ASSERT_FALSE(SystemTimeGreaterThan(&b, &a));
}

static void TestSystemTime_GreaterThanOrEqual(void) {
  TEST_ASSERT_FALSE(SystemTimeGreaterThanOrEqual(NULL, &kZeroTime));
  TEST_ASSERT_FALSE(SystemTimeGreaterThanOrEqual(&kInvalidTime, &kZeroTime));
  TEST_ASSERT_FALSE(SystemTimeGreaterThanOrEqual(&kZeroTime, NULL));
  TEST_ASSERT_FALSE(SystemTimeGreaterThanOrEqual(&kZeroTime, &kInvalidTime));
  TEST_ASSERT_FALSE(SystemTimeGreaterThanOrEqual(NULL, NULL));
  TEST_ASSERT_FALSE(SystemTimeGreaterThanOrEqual(&kInvalidTime, &kInvalidTime));

  TEST_ASSERT_FALSE(SystemTimeGreaterThanOrEqual(&kZeroTime, &kMaxTime));
  TEST_ASSERT_TRUE(SystemTimeGreaterThanOrEqual(&kMaxTime, &kZeroTime));

  TEST_ASSERT_TRUE(SystemTimeGreaterThanOrEqual(&kZeroTime, &kZeroTime));
  TEST_ASSERT_TRUE(SystemTimeGreaterThanOrEqual(&kMaxTime, &kMaxTime));

  system_time_t a = {
    .seconds = 5000,
    .nanoseconds = 750000
  };
  TEST_ASSERT_FALSE(SystemTimeGreaterThanOrEqual(&kZeroTime, &a));
  TEST_ASSERT_TRUE(SystemTimeGreaterThanOrEqual(&kMaxTime, &a));
  TEST_ASSERT_TRUE(SystemTimeGreaterThanOrEqual(&a, &kZeroTime));
  TEST_ASSERT_FALSE(SystemTimeGreaterThanOrEqual(&a, &kMaxTime));
  TEST_ASSERT_TRUE(SystemTimeGreaterThanOrEqual(&a, &a));

  system_time_t b = {
    .seconds = a.seconds,
    .nanoseconds = a.nanoseconds + 1
  };
  TEST_ASSERT_FALSE(SystemTimeGreaterThanOrEqual(&a, &b));
  TEST_ASSERT_TRUE(SystemTimeGreaterThanOrEqual(&b, &a));

  b.nanoseconds = a.nanoseconds - 1;
  TEST_ASSERT_TRUE(SystemTimeGreaterThanOrEqual(&a, &b));
  TEST_ASSERT_FALSE(SystemTimeGreaterThanOrEqual(&b, &a));

  b.seconds = a.seconds + 1;
  b.nanoseconds = a.nanoseconds;
  TEST_ASSERT_FALSE(SystemTimeGreaterThanOrEqual(&a, &b));
  TEST_ASSERT_TRUE(SystemTimeGreaterThanOrEqual(&b, &a));

  b.seconds = a.seconds - 1;
  TEST_ASSERT_TRUE(SystemTimeGreaterThanOrEqual(&a, &b));
  TEST_ASSERT_FALSE(SystemTimeGreaterThanOrEqual(&b, &a));
}

static void TestSystemTime_SecondsDelta(void) {
  uint32_t delta = 0;
  TEST_ASSERT_FALSE(SystemTimeSecondsDelta(NULL, &kZeroTime, &delta));
  TEST_ASSERT_FALSE(SystemTimeSecondsDelta(&kInvalidTime, &kZeroTime, &delta));
  TEST_ASSERT_FALSE(SystemTimeSecondsDelta(&kZeroTime, NULL, &delta));
  TEST_ASSERT_FALSE(SystemTimeSecondsDelta(&kZeroTime, &kInvalidTime, &delta));
  TEST_ASSERT_FALSE(SystemTimeSecondsDelta(&kZeroTime, &kZeroTime, NULL));
  TEST_ASSERT_FALSE(SystemTimeSecondsDelta(NULL, NULL, NULL));
  TEST_ASSERT_FALSE(SystemTimeSecondsDelta(NULL, NULL, &delta));
  TEST_ASSERT_FALSE(SystemTimeSecondsDelta(
      &kInvalidTime, &kInvalidTime, NULL));
  TEST_ASSERT_FALSE(SystemTimeSecondsDelta(
      &kInvalidTime, &kInvalidTime, &delta));

  delta = 0;
  TEST_ASSERT_TRUE(SystemTimeSecondsDelta(&kZeroTime, &kMaxTime, &delta));
  TEST_ASSERT_EQUAL(MAX_SECONDS, delta);
  delta = 0;
  TEST_ASSERT_TRUE(SystemTimeSecondsDelta(&kMaxTime, &kZeroTime, &delta));
  TEST_ASSERT_EQUAL(MAX_SECONDS, delta);
  TEST_ASSERT_TRUE(SystemTimeSecondsDelta(&kZeroTime, &kZeroTime, &delta));
  TEST_ASSERT_EQUAL(0, delta);
  delta = MAX_SECONDS;
  TEST_ASSERT_TRUE(SystemTimeSecondsDelta(&kMaxTime, &kMaxTime, &delta));
  TEST_ASSERT_EQUAL(0, delta);

  system_time_t const kA = {
    .seconds = 5000,
    .nanoseconds = 750000
  };
  TEST_ASSERT_TRUE(SystemTimeSecondsDelta(&kZeroTime, &kA, &delta));
  TEST_ASSERT_EQUAL(5000, delta);
  TEST_ASSERT_TRUE(SystemTimeSecondsDelta(&kMaxTime, &kA, &delta));
  TEST_ASSERT_EQUAL(MAX_SECONDS - 5000, delta);
  TEST_ASSERT_TRUE(SystemTimeSecondsDelta(&kA, &kZeroTime, &delta));
  TEST_ASSERT_EQUAL(5000, delta);
  TEST_ASSERT_TRUE(SystemTimeSecondsDelta(&kA, &kMaxTime, &delta));
  TEST_ASSERT_EQUAL(MAX_SECONDS - 5000, delta);
  TEST_ASSERT_TRUE(SystemTimeSecondsDelta(&kA, &kA, &delta));
  TEST_ASSERT_EQUAL(0, delta);

  /* Nanoseconds borrow. */
  system_time_t const kB = {
    .seconds = kA.seconds + 5,
    .nanoseconds = kA.nanoseconds - 5
  };
  TEST_ASSERT_TRUE(SystemTimeSecondsDelta(&kA, &kB, &delta));
  TEST_ASSERT_EQUAL(4, delta);
  delta = 0;
  TEST_ASSERT_TRUE(SystemTimeSecondsDelta(&kB, &kA, &delta));
  TEST_ASSERT_EQUAL(4, delta);

  /* Nanosecond round down. */
  system_time_t const kC = {
    .seconds = kA.seconds - 5,
    .nanoseconds = 0
  };
  TEST_ASSERT_TRUE(SystemTimeSecondsDelta(&kA, &kC, &delta));
  TEST_ASSERT_EQUAL(5, delta);
  delta = 0;
  TEST_ASSERT_TRUE(SystemTimeSecondsDelta(&kC, &kA, &delta));
  TEST_ASSERT_EQUAL(5, delta);

  /* Near Max */
  system_time_t const kD = {
    .seconds = MAX_SECONDS,
    .nanoseconds = 0
  };
  system_time_t const kE = {
    .seconds = 0,
    .nanoseconds = MAX_NANOSECONDS
  };
  TEST_ASSERT_TRUE(SystemTimeSecondsDelta(&kD, &kE, &delta));
  TEST_ASSERT_EQUAL(MAX_SECONDS - 1, delta);
}

static void TestSystemTime_MillisecondsDelta(void) {
  uint32_t delta = 0;
  TEST_ASSERT_FALSE(SystemTimeMillisecondsDelta(
      NULL, &kZeroTime, &delta));
  TEST_ASSERT_FALSE(SystemTimeMillisecondsDelta(
      &kInvalidTime, &kZeroTime, &delta));
  TEST_ASSERT_FALSE(SystemTimeMillisecondsDelta(
      &kZeroTime, NULL, &delta));
  TEST_ASSERT_FALSE(SystemTimeMillisecondsDelta(
      &kZeroTime, &kInvalidTime, &delta));
  TEST_ASSERT_FALSE(SystemTimeMillisecondsDelta(
      &kZeroTime, &kZeroTime, NULL));
  TEST_ASSERT_FALSE(SystemTimeMillisecondsDelta(
      NULL, NULL, NULL));
  TEST_ASSERT_FALSE(SystemTimeMillisecondsDelta(
      NULL, NULL, &delta));
  TEST_ASSERT_FALSE(SystemTimeMillisecondsDelta(
      &kInvalidTime, &kInvalidTime, NULL));
  TEST_ASSERT_FALSE(SystemTimeMillisecondsDelta(
      &kInvalidTime, &kInvalidTime, &delta));

  /* Overflows */
  TEST_ASSERT_FALSE(SystemTimeMillisecondsDelta(
      &kZeroTime, &kMaxTime, &delta));
  TEST_ASSERT_FALSE(SystemTimeMillisecondsDelta(
      &kMaxTime, &kZeroTime, &delta));

  /* Equal */
  TEST_ASSERT_TRUE(SystemTimeMillisecondsDelta(
      &kZeroTime, &kZeroTime, &delta));
  TEST_ASSERT_EQUAL(0, delta);
  delta = MAX_SECONDS;
  TEST_ASSERT_TRUE(SystemTimeMillisecondsDelta(&kMaxTime, &kMaxTime, &delta));
  TEST_ASSERT_EQUAL(0, delta);

  /* Max delta. */
  system_time_t time = kMaxMilliDeltaTime;
  TEST_ASSERT_TRUE(SystemTimeMillisecondsDelta(
      &kZeroTime, &time, &delta));
  TEST_ASSERT_EQUAL(MAX_U32, delta);
  delta = 0;
  TEST_ASSERT_TRUE(SystemTimeMillisecondsDelta(
      &time, &kZeroTime, &delta));
  TEST_ASSERT_EQUAL(MAX_U32, delta);
  ++time.nanoseconds;
  TEST_ASSERT_FALSE(SystemTimeMillisecondsDelta(
      &kZeroTime, &time, &delta));
  TEST_ASSERT_FALSE(SystemTimeMillisecondsDelta(
      &time, &kZeroTime, &delta));

  system_time_t const kA = {
    .seconds = 4,
    .nanoseconds = 750000000
  };
  TEST_ASSERT_TRUE(SystemTimeMillisecondsDelta(&kZeroTime, &kA, &delta));
  TEST_ASSERT_EQUAL(4750, delta);
  delta = 0;
  TEST_ASSERT_TRUE(SystemTimeMillisecondsDelta(&kA, &kZeroTime, &delta));
  TEST_ASSERT_EQUAL(4750, delta);
  TEST_ASSERT_TRUE(SystemTimeMillisecondsDelta(&kA, &kA, &delta));
  TEST_ASSERT_EQUAL(0, delta);

  /* Milliseconds borrow. */
  system_time_t const kB = {
    .seconds = kA.seconds + 5,
    .nanoseconds = kA.nanoseconds - 5000000
  };
  TEST_ASSERT_TRUE(SystemTimeMillisecondsDelta(&kA, &kB, &delta));
  TEST_ASSERT_EQUAL(4995, delta);
  delta = 0;
  TEST_ASSERT_TRUE(SystemTimeMillisecondsDelta(&kB, &kA, &delta));
  TEST_ASSERT_EQUAL(4995, delta);

  /* Milliseconds round down. */
  system_time_t const kC = {
    .seconds = kA.seconds + 5,
    .nanoseconds = kA.nanoseconds - 40
  };
  TEST_ASSERT_TRUE(SystemTimeMillisecondsDelta(&kA, &kC, &delta));
  TEST_ASSERT_EQUAL(4999, delta);
  delta = 0;
  TEST_ASSERT_TRUE(SystemTimeMillisecondsDelta(&kC, &kA, &delta));
  TEST_ASSERT_EQUAL(4999, delta);

  /* Milliseconds only. */
  system_time_t const kD = {
    .seconds = kA.seconds,
    .nanoseconds = kA.nanoseconds + 5000000
  };
  TEST_ASSERT_TRUE(SystemTimeMillisecondsDelta(&kA, &kD, &delta));
  TEST_ASSERT_EQUAL(5, delta);
  delta = 0;
  TEST_ASSERT_TRUE(SystemTimeMillisecondsDelta(&kD, &kA, &delta));
  TEST_ASSERT_EQUAL(5, delta);

  /* Milliseconds only round down. */
  system_time_t const kE = {
    .seconds = kA.seconds,
    .nanoseconds = kA.nanoseconds + 1999999
  };
  TEST_ASSERT_TRUE(SystemTimeMillisecondsDelta(&kA, &kE, &delta));
  TEST_ASSERT_EQUAL(1, delta);
  delta = 0;
  TEST_ASSERT_TRUE(SystemTimeMillisecondsDelta(&kE, &kA, &delta));
  TEST_ASSERT_EQUAL(1, delta);

  /* Seconds only. */
  system_time_t const kF = {
    .seconds = kA.seconds + 5,
    .nanoseconds = kA.nanoseconds
  };
  TEST_ASSERT_TRUE(SystemTimeMillisecondsDelta(&kA, &kF, &delta));
  TEST_ASSERT_EQUAL(5000, delta);
  delta = 0;
  TEST_ASSERT_TRUE(SystemTimeMillisecondsDelta(&kF, &kA, &delta));
  TEST_ASSERT_EQUAL(5000, delta);
}

static void TestSystemTime_MicrosecondsDelta(void) {
  uint32_t delta = 0;
  TEST_ASSERT_FALSE(SystemTimeMicrosecondsDelta(
      NULL, &kZeroTime, &delta));
  TEST_ASSERT_FALSE(SystemTimeMicrosecondsDelta(
      &kInvalidTime, &kZeroTime, &delta));
  TEST_ASSERT_FALSE(SystemTimeMicrosecondsDelta(
      &kZeroTime, NULL, &delta));
  TEST_ASSERT_FALSE(SystemTimeMicrosecondsDelta(
      &kZeroTime, &kInvalidTime, &delta));
  TEST_ASSERT_FALSE(SystemTimeMicrosecondsDelta(
      &kZeroTime, &kZeroTime, NULL));
  TEST_ASSERT_FALSE(SystemTimeMicrosecondsDelta(
      NULL, NULL, NULL));
  TEST_ASSERT_FALSE(SystemTimeMicrosecondsDelta(
      NULL, NULL, &delta));
  TEST_ASSERT_FALSE(SystemTimeMicrosecondsDelta(
      &kInvalidTime, &kInvalidTime, NULL));
  TEST_ASSERT_FALSE(SystemTimeMicrosecondsDelta(
      &kInvalidTime, &kInvalidTime, &delta));

  /* Overflows */
  TEST_ASSERT_FALSE(SystemTimeMicrosecondsDelta(
      &kZeroTime, &kMaxTime, &delta));
  TEST_ASSERT_FALSE(SystemTimeMicrosecondsDelta(
      &kMaxTime, &kZeroTime, &delta));

  /* Equal */
  TEST_ASSERT_TRUE(SystemTimeMicrosecondsDelta(
      &kZeroTime, &kZeroTime, &delta));
  TEST_ASSERT_EQUAL(0, delta);
  delta = MAX_SECONDS;
  TEST_ASSERT_TRUE(SystemTimeMicrosecondsDelta(&kMaxTime, &kMaxTime, &delta));
  TEST_ASSERT_EQUAL(0, delta);

  /* Max delta. */
  system_time_t time = kMaxMicroDeltaTime;
  TEST_ASSERT_TRUE(SystemTimeMicrosecondsDelta(
      &kZeroTime, &time, &delta));
  TEST_ASSERT_EQUAL(MAX_U32, delta);
  delta = 0;
  TEST_ASSERT_TRUE(SystemTimeMicrosecondsDelta(
      &time, &kZeroTime, &delta));
  TEST_ASSERT_EQUAL(MAX_U32, delta);
  ++time.nanoseconds;
  TEST_ASSERT_FALSE(SystemTimeMicrosecondsDelta(
      &kZeroTime, &time, &delta));
  TEST_ASSERT_FALSE(SystemTimeMicrosecondsDelta(
      &time, &kZeroTime, &delta));

  system_time_t const kA = {
    .seconds = 4,
    .nanoseconds = 750250000
  };
  TEST_ASSERT_TRUE(SystemTimeMicrosecondsDelta(&kZeroTime, &kA, &delta));
  TEST_ASSERT_EQUAL(4750250, delta);
  delta = 0;
  TEST_ASSERT_TRUE(SystemTimeMicrosecondsDelta(&kA, &kZeroTime, &delta));
  TEST_ASSERT_EQUAL(4750250, delta);
  TEST_ASSERT_TRUE(SystemTimeMicrosecondsDelta(&kA, &kA, &delta));
  TEST_ASSERT_EQUAL(0, delta);

  /* Microseconds borrow. */
  system_time_t const kB = {
    .seconds = kA.seconds + 5,
    .nanoseconds = kA.nanoseconds - 5000
  };
  TEST_ASSERT_TRUE(SystemTimeMicrosecondsDelta(&kA, &kB, &delta));
  TEST_ASSERT_EQUAL(4999995, delta);
  delta = 0;
  TEST_ASSERT_TRUE(SystemTimeMicrosecondsDelta(&kB, &kA, &delta));
  TEST_ASSERT_EQUAL(4999995, delta);

  /* Microseconds round down. */
  system_time_t const kC = {
    .seconds = kA.seconds + 5,
    .nanoseconds = kA.nanoseconds - 40
  };
  TEST_ASSERT_TRUE(SystemTimeMicrosecondsDelta(&kA, &kC, &delta));
  TEST_ASSERT_EQUAL(4999999, delta);
  delta = 0;
  TEST_ASSERT_TRUE(SystemTimeMicrosecondsDelta(&kC, &kA, &delta));
  TEST_ASSERT_EQUAL(4999999, delta);

  /* Microseconds only. */
  system_time_t const kD = {
    .seconds = kA.seconds,
    .nanoseconds = kA.nanoseconds + 5000
  };
  TEST_ASSERT_TRUE(SystemTimeMicrosecondsDelta(&kA, &kD, &delta));
  TEST_ASSERT_EQUAL(5, delta);
  delta = 0;
  TEST_ASSERT_TRUE(SystemTimeMicrosecondsDelta(&kD, &kA, &delta));
  TEST_ASSERT_EQUAL(5, delta);

  /* Microseconds only round down. */
  system_time_t const kE = {
    .seconds = kA.seconds,
    .nanoseconds = kA.nanoseconds + 1999
  };
  TEST_ASSERT_TRUE(SystemTimeMicrosecondsDelta(&kA, &kE, &delta));
  TEST_ASSERT_EQUAL(1, delta);
  delta = 0;
  TEST_ASSERT_TRUE(SystemTimeMicrosecondsDelta(&kE, &kA, &delta));
  TEST_ASSERT_EQUAL(1, delta);

  /* Seconds only. */
  system_time_t const kF = {
    .seconds = kA.seconds + 5,
    .nanoseconds = kA.nanoseconds
  };
  TEST_ASSERT_TRUE(SystemTimeMicrosecondsDelta(&kA, &kF, &delta));
  TEST_ASSERT_EQUAL(5000000, delta);
  delta = 0;
  TEST_ASSERT_TRUE(SystemTimeMicrosecondsDelta(&kF, &kA, &delta));
  TEST_ASSERT_EQUAL(5000000, delta);
}

static void TestSystemTime_NanosecondsDelta(void) {
  uint32_t delta = 0;
  TEST_ASSERT_FALSE(SystemTimeNanosecondsDelta(
      NULL, &kZeroTime, &delta));
  TEST_ASSERT_FALSE(SystemTimeNanosecondsDelta(
      &kInvalidTime, &kZeroTime, &delta));
  TEST_ASSERT_FALSE(SystemTimeNanosecondsDelta(
      &kZeroTime, NULL, &delta));
  TEST_ASSERT_FALSE(SystemTimeNanosecondsDelta(
      &kZeroTime, &kInvalidTime, &delta));
  TEST_ASSERT_FALSE(SystemTimeNanosecondsDelta(
      &kZeroTime, &kZeroTime, NULL));
  TEST_ASSERT_FALSE(SystemTimeNanosecondsDelta(
      NULL, NULL, NULL));
  TEST_ASSERT_FALSE(SystemTimeNanosecondsDelta(
      NULL, NULL, &delta));
  TEST_ASSERT_FALSE(SystemTimeNanosecondsDelta(
      &kInvalidTime, &kInvalidTime, NULL));
  TEST_ASSERT_FALSE(SystemTimeNanosecondsDelta(
      &kInvalidTime, &kInvalidTime, &delta));

  /* Overflows */
  TEST_ASSERT_FALSE(SystemTimeNanosecondsDelta(&kZeroTime, &kMaxTime, &delta));
  TEST_ASSERT_FALSE(SystemTimeNanosecondsDelta(&kMaxTime, &kZeroTime, &delta));

  /* Equal */
  TEST_ASSERT_TRUE(SystemTimeNanosecondsDelta(&kZeroTime, &kZeroTime, &delta));
  TEST_ASSERT_EQUAL(0, delta);
  delta = MAX_SECONDS;
  TEST_ASSERT_TRUE(SystemTimeNanosecondsDelta(&kMaxTime, &kMaxTime, &delta));
  TEST_ASSERT_EQUAL(0, delta);

  /* Max delta. */
  system_time_t time = kMaxNanoDeltaTime;
  TEST_ASSERT_TRUE(SystemTimeNanosecondsDelta(
      &kZeroTime, &time, &delta));
  TEST_ASSERT_EQUAL(MAX_U32, delta);
  delta = 0;
  TEST_ASSERT_TRUE(SystemTimeNanosecondsDelta(
      &time, &kZeroTime, &delta));
  TEST_ASSERT_EQUAL(MAX_U32, delta);
  ++time.nanoseconds;
  TEST_ASSERT_FALSE(SystemTimeNanosecondsDelta(
      &kZeroTime, &time, &delta));
  TEST_ASSERT_FALSE(SystemTimeNanosecondsDelta(
      &time, &kZeroTime, &delta));

  system_time_t const kA = {
    .seconds = 4,
    .nanoseconds = 250500750
  };
  TEST_ASSERT_TRUE(SystemTimeNanosecondsDelta(&kZeroTime, &kA, &delta));
  TEST_ASSERT_EQUAL(4250500750, delta);
  delta = 0;
  TEST_ASSERT_TRUE(SystemTimeNanosecondsDelta(&kA, &kZeroTime, &delta));
  TEST_ASSERT_EQUAL(4250500750, delta);
  TEST_ASSERT_TRUE(SystemTimeNanosecondsDelta(&kA, &kA, &delta));
  TEST_ASSERT_EQUAL(0, delta);

  /* Nanoseconds borrow. */
  system_time_t const kB = {
    .seconds = kA.seconds + 2,
    .nanoseconds = kA.nanoseconds - 400
  };
  TEST_ASSERT_TRUE(SystemTimeNanosecondsDelta(&kA, &kB, &delta));
  TEST_ASSERT_EQUAL(1999999600, delta);
  delta = 0;
  TEST_ASSERT_TRUE(SystemTimeNanosecondsDelta(&kB, &kA, &delta));
  TEST_ASSERT_EQUAL(1999999600, delta);

  /* Nanoseconds no borrow. */
  system_time_t const kC = {
    .seconds = kA.seconds + 2,
    .nanoseconds = kA.nanoseconds + 400
  };
  TEST_ASSERT_TRUE(SystemTimeNanosecondsDelta(&kA, &kC, &delta));
  TEST_ASSERT_EQUAL(2000000400, delta);
  delta = 0;
  TEST_ASSERT_TRUE(SystemTimeNanosecondsDelta(&kC, &kA, &delta));
  TEST_ASSERT_EQUAL(2000000400, delta);

  /* Nanoseconds only. */
  system_time_t const kD = {
    .seconds = kA.seconds,
    .nanoseconds = kA.nanoseconds + 10500
  };
  TEST_ASSERT_TRUE(SystemTimeNanosecondsDelta(&kA, &kD, &delta));
  TEST_ASSERT_EQUAL(10500, delta);
  delta = 0;
  TEST_ASSERT_TRUE(SystemTimeNanosecondsDelta(&kD, &kA, &delta));
  TEST_ASSERT_EQUAL(10500, delta);
}

static void TestSystemTime_IncrementSeconds(void) {
  TEST_ASSERT_FALSE(SystemTimeIncrementSeconds(NULL, 0));

  system_time_t time = kMaxTime;
  TEST_ASSERT_TRUE(SystemTimeIncrementSeconds(&time, 0));
  TEST_ASSERT_TRUE(SystemTimeEqual(&kMaxTime, &time));

  TEST_ASSERT_TRUE(SystemTimeIncrementSeconds(&time, MAX_SECONDS));
  TEST_ASSERT_TRUE(SystemTimeEqual(&kMaxTime, &time));

  time = kZeroTime;
  TEST_ASSERT_TRUE(SystemTimeIncrementSeconds(&time, 0));
  TEST_ASSERT_TRUE(SystemTimeEqual(&kZeroTime, &time));

  TEST_ASSERT_TRUE(SystemTimeIncrementSeconds(&time, 1));
  TEST_ASSERT_EQUAL(1, time.seconds);
  TEST_ASSERT_EQUAL(0, time.nanoseconds);

  time = kZeroTime;
  TEST_ASSERT_TRUE(SystemTimeIncrementSeconds(&time, MAX_SECONDS));
  TEST_ASSERT_EQUAL(MAX_SECONDS, time.seconds);
  TEST_ASSERT_EQUAL(0, time.nanoseconds);

  time = (system_time_t) {
    .seconds = 5000,
    .nanoseconds = 750000
  };
  TEST_ASSERT_TRUE(SystemTimeIncrementSeconds(&time, 0));
  TEST_ASSERT_EQUAL(5000, time.seconds);
  TEST_ASSERT_EQUAL(750000, time.nanoseconds);

  TEST_ASSERT_TRUE(SystemTimeIncrementSeconds(&time, 4500));
  TEST_ASSERT_EQUAL(9500, time.seconds);
  TEST_ASSERT_EQUAL(750000, time.nanoseconds);

  TEST_ASSERT_TRUE(SystemTimeIncrementSeconds(&time, MAX_SECONDS));
  TEST_ASSERT_TRUE(SystemTimeEqual(&kMaxTime, &time));
}

static void TestSystemTime_IncrementMilliseconds(void) {
  TEST_ASSERT_FALSE(SystemTimeIncrementMilliseconds(NULL, 0));

  system_time_t time = kMaxTime;
  TEST_ASSERT_TRUE(SystemTimeIncrementMilliseconds(&time, 0));
  TEST_ASSERT_TRUE(SystemTimeEqual(&kMaxTime, &time));

  TEST_ASSERT_TRUE(SystemTimeIncrementMilliseconds(&time, MAX_SECONDS));
  TEST_ASSERT_TRUE(SystemTimeEqual(&kMaxTime, &time));

  time = kZeroTime;
  TEST_ASSERT_TRUE(SystemTimeIncrementMilliseconds(&time, 0));
  TEST_ASSERT_TRUE(SystemTimeEqual(&kZeroTime, &time));

  TEST_ASSERT_TRUE(SystemTimeIncrementMilliseconds(&time, 1001));
  TEST_ASSERT_EQUAL(1, time.seconds);
  TEST_ASSERT_EQUAL(1000000, time.nanoseconds);

  time = kZeroTime;
  TEST_ASSERT_TRUE(SystemTimeIncrementMilliseconds(&time, MAX_U32));
  TEST_ASSERT_EQUAL(kMaxMilliDeltaTime.seconds, time.seconds);
  TEST_ASSERT_EQUAL(kMaxMilliDeltaTime.nanoseconds - 999999u, time.nanoseconds);

  time = (system_time_t) {
    .seconds = 5000,
    .nanoseconds = 750000150
  };
  TEST_ASSERT_TRUE(SystemTimeIncrementMilliseconds(&time, 0));
  TEST_ASSERT_EQUAL(5000, time.seconds);
  TEST_ASSERT_EQUAL(750000150, time.nanoseconds);

  TEST_ASSERT_TRUE(SystemTimeIncrementMilliseconds(&time, 4150));
  TEST_ASSERT_EQUAL(5004, time.seconds);
  TEST_ASSERT_EQUAL(900000150, time.nanoseconds);

  /* Seconds carry. */
  TEST_ASSERT_TRUE(SystemTimeIncrementMilliseconds(&time, 5100));
  TEST_ASSERT_EQUAL(5010, time.seconds);
  TEST_ASSERT_EQUAL(150, time.nanoseconds);

  TEST_ASSERT_TRUE(SystemTimeIncrementMilliseconds(&time, 500));
  TEST_ASSERT_EQUAL(5010, time.seconds);
  TEST_ASSERT_EQUAL(500000150, time.nanoseconds);

  TEST_ASSERT_TRUE(SystemTimeIncrementMilliseconds(&time, 500));
  TEST_ASSERT_EQUAL(5011, time.seconds);
  TEST_ASSERT_EQUAL(150, time.nanoseconds);

  /* Overflow. */
  time = (system_time_t) {
    .seconds = MAX_SECONDS - 50,
    .nanoseconds = 0
  };
  TEST_ASSERT_TRUE(SystemTimeIncrementMilliseconds(&time, 51000));
  TEST_ASSERT_TRUE(SystemTimeEqual(&kMaxTime, &time));

  time = (system_time_t) {
    .seconds = MAX_SECONDS - 50,
    .nanoseconds = 5000000
  };
  TEST_ASSERT_TRUE(SystemTimeIncrementMilliseconds(&time, 50995));
  TEST_ASSERT_TRUE(SystemTimeEqual(&kMaxTime, &time));
}

static void TestSystemTime_IncrementMicroseconds(void) {
  TEST_ASSERT_FALSE(SystemTimeIncrementMicroseconds(NULL, 0));

  system_time_t time = kMaxTime;
  TEST_ASSERT_TRUE(SystemTimeIncrementMicroseconds(&time, 0));
  TEST_ASSERT_TRUE(SystemTimeEqual(&kMaxTime, &time));

  TEST_ASSERT_TRUE(SystemTimeIncrementMicroseconds(&time, MAX_SECONDS));
  TEST_ASSERT_TRUE(SystemTimeEqual(&kMaxTime, &time));

  time = kZeroTime;
  TEST_ASSERT_TRUE(SystemTimeIncrementMicroseconds(&time, 0));
  TEST_ASSERT_TRUE(SystemTimeEqual(&kZeroTime, &time));

  TEST_ASSERT_TRUE(SystemTimeIncrementMicroseconds(&time, 1001002));
  TEST_ASSERT_EQUAL(1, time.seconds);
  TEST_ASSERT_EQUAL(1002000, time.nanoseconds);

  time = kZeroTime;
  TEST_ASSERT_TRUE(SystemTimeIncrementMicroseconds(&time, MAX_U32));
  TEST_ASSERT_EQUAL(kMaxMicroDeltaTime.seconds, time.seconds);
  TEST_ASSERT_EQUAL(kMaxMicroDeltaTime.nanoseconds - 999u, time.nanoseconds);

  time = (system_time_t) {
    .seconds = 5000,
    .nanoseconds = 750150
  };
  TEST_ASSERT_TRUE(SystemTimeIncrementMicroseconds(&time, 0));
  TEST_ASSERT_EQUAL(5000, time.seconds);
  TEST_ASSERT_EQUAL(750150, time.nanoseconds);

  TEST_ASSERT_TRUE(SystemTimeIncrementMicroseconds(&time, 4000150));
  TEST_ASSERT_EQUAL(5004, time.seconds);
  TEST_ASSERT_EQUAL(900150, time.nanoseconds);

  /* Seconds carry. */
  TEST_ASSERT_TRUE(SystemTimeIncrementMicroseconds(&time, 5999100));
  TEST_ASSERT_EQUAL(5010, time.seconds);
  TEST_ASSERT_EQUAL(150, time.nanoseconds);

  TEST_ASSERT_TRUE(SystemTimeIncrementMicroseconds(&time, 500000));
  TEST_ASSERT_EQUAL(5010, time.seconds);
  TEST_ASSERT_EQUAL(500000150, time.nanoseconds);

  TEST_ASSERT_TRUE(SystemTimeIncrementMicroseconds(&time, 500000));
  TEST_ASSERT_EQUAL(5011, time.seconds);
  TEST_ASSERT_EQUAL(150, time.nanoseconds);

  /* Overflow. */
  time = (system_time_t) {
    .seconds = MAX_SECONDS - 50,
    .nanoseconds = 0
  };
  TEST_ASSERT_TRUE(SystemTimeIncrementMicroseconds(&time, 51000000));
  TEST_ASSERT_TRUE(SystemTimeEqual(&kMaxTime, &time));

  time = (system_time_t) {
    .seconds = MAX_SECONDS - 50,
    .nanoseconds = 5000
  };
  TEST_ASSERT_TRUE(SystemTimeIncrementMicroseconds(&time, 50999995));
  TEST_ASSERT_TRUE(SystemTimeEqual(&kMaxTime, &time));
}

static void TestSystemTime_IncrementNanoseconds(void) {
  TEST_ASSERT_FALSE(SystemTimeIncrementNanoseconds(NULL, 0));

  system_time_t time = kMaxTime;
  TEST_ASSERT_TRUE(SystemTimeIncrementNanoseconds(&time, 0));
  TEST_ASSERT_TRUE(SystemTimeEqual(&kMaxTime, &time));

  TEST_ASSERT_TRUE(SystemTimeIncrementNanoseconds(&time, MAX_SECONDS));
  TEST_ASSERT_TRUE(SystemTimeEqual(&kMaxTime, &time));

  time = kZeroTime;
  TEST_ASSERT_TRUE(SystemTimeIncrementNanoseconds(&time, 0));
  TEST_ASSERT_TRUE(SystemTimeEqual(&kZeroTime, &time));

  TEST_ASSERT_TRUE(SystemTimeIncrementNanoseconds(&time, 1001001001));
  TEST_ASSERT_EQUAL(1, time.seconds);
  TEST_ASSERT_EQUAL(1001001, time.nanoseconds);

  time = kZeroTime;
  TEST_ASSERT_TRUE(SystemTimeIncrementNanoseconds(&time, MAX_U32));
  TEST_ASSERT_TRUE(SystemTimeEqual(&kMaxNanoDeltaTime, &time));

  time = (system_time_t) {
    .seconds = 5000,
    .nanoseconds = 750150250
  };
  TEST_ASSERT_TRUE(SystemTimeIncrementNanoseconds(&time, 0));
  TEST_ASSERT_EQUAL(5000, time.seconds);
  TEST_ASSERT_EQUAL(750150250, time.nanoseconds);

  TEST_ASSERT_TRUE(SystemTimeIncrementNanoseconds(&time, 249849750));
  TEST_ASSERT_EQUAL(5001, time.seconds);
  TEST_ASSERT_EQUAL(0, time.nanoseconds);

  TEST_ASSERT_TRUE(SystemTimeIncrementNanoseconds(&time, 1750250500));
  TEST_ASSERT_EQUAL(5002, time.seconds);
  TEST_ASSERT_EQUAL(750250500, time.nanoseconds);

  time = (system_time_t) {
    .seconds = MAX_SECONDS - 2,
    .nanoseconds = 0
  };
  TEST_ASSERT_TRUE(SystemTimeIncrementNanoseconds(&time, 2000000000));
  TEST_ASSERT_EQUAL(MAX_SECONDS, time.seconds);
  TEST_ASSERT_EQUAL(0, time.nanoseconds);

  time = (system_time_t) {
    .seconds = MAX_SECONDS - 1,
    .nanoseconds = 500000000
  };
  TEST_ASSERT_TRUE(SystemTimeIncrementNanoseconds(&time, 500000001));
  TEST_ASSERT_EQUAL(MAX_SECONDS, time.seconds);
  TEST_ASSERT_EQUAL(1, time.nanoseconds);

  time = (system_time_t) {
    .seconds = MAX_SECONDS - 1,
    .nanoseconds = 500000000
  };
  TEST_ASSERT_TRUE(SystemTimeIncrementNanoseconds(&time, 2000000000));
  TEST_ASSERT_TRUE(SystemTimeEqual(&kMaxTime, &time));

  time = (system_time_t) {
    .seconds = MAX_SECONDS,
    .nanoseconds = 500000000
  };
  TEST_ASSERT_TRUE(SystemTimeIncrementNanoseconds(&time, 500000000));
  TEST_ASSERT_TRUE(SystemTimeEqual(&kMaxTime, &time));
}

static void TestSystemTime_DecrementSeconds(void) {
  TEST_ASSERT_FALSE(SystemTimeDecrementSeconds(NULL, 0));

  system_time_t time = kZeroTime;
  TEST_ASSERT_TRUE(SystemTimeDecrementSeconds(&time, 0));
  TEST_ASSERT_TRUE(SystemTimeEqual(&kZeroTime, &time));

  TEST_ASSERT_TRUE(SystemTimeDecrementSeconds(&time, MAX_SECONDS));
  TEST_ASSERT_TRUE(SystemTimeEqual(&kZeroTime, &time));

  time = kMaxTime;
  TEST_ASSERT_TRUE(SystemTimeDecrementSeconds(&time, 0));
  TEST_ASSERT_TRUE(SystemTimeEqual(&kMaxTime, &time));

  TEST_ASSERT_TRUE(SystemTimeDecrementSeconds(&time, 1));
  TEST_ASSERT_EQUAL(MAX_SECONDS - 1, time.seconds);
  TEST_ASSERT_EQUAL(MAX_NANOSECONDS, time.nanoseconds);

  time = kMaxTime;
  TEST_ASSERT_TRUE(SystemTimeDecrementSeconds(&time, MAX_SECONDS));
  TEST_ASSERT_EQUAL(0, time.seconds);
  TEST_ASSERT_EQUAL(MAX_NANOSECONDS, time.nanoseconds);

  time = (system_time_t) {
    .seconds = 5000,
    .nanoseconds = 750000
  };
  TEST_ASSERT_TRUE(SystemTimeDecrementSeconds(&time, 0));
  TEST_ASSERT_EQUAL(5000, time.seconds);
  TEST_ASSERT_EQUAL(750000, time.nanoseconds);

  TEST_ASSERT_TRUE(SystemTimeDecrementSeconds(&time, 4500));
  TEST_ASSERT_EQUAL(500, time.seconds);
  TEST_ASSERT_EQUAL(750000, time.nanoseconds);

  TEST_ASSERT_TRUE(SystemTimeDecrementSeconds(&time, MAX_SECONDS));
  TEST_ASSERT_TRUE(SystemTimeEqual(&kZeroTime, &time));
}

static void TestSystemTime_DecrementMilliseconds(void) {
  TEST_ASSERT_FALSE(SystemTimeDecrementMilliseconds(NULL, 0));

  system_time_t time = kZeroTime;
  TEST_ASSERT_TRUE(SystemTimeDecrementMilliseconds(&time, 0));
  TEST_ASSERT_TRUE(SystemTimeEqual(&kZeroTime, &time));

  TEST_ASSERT_TRUE(SystemTimeDecrementMilliseconds(&time, MAX_SECONDS));
  TEST_ASSERT_TRUE(SystemTimeEqual(&kZeroTime, &time));

  time = kMaxTime;
  TEST_ASSERT_TRUE(SystemTimeDecrementMilliseconds(&time, 0));
  TEST_ASSERT_TRUE(SystemTimeEqual(&kMaxTime, &time));

  TEST_ASSERT_TRUE(SystemTimeDecrementMilliseconds(&time, 1001));
  TEST_ASSERT_EQUAL(MAX_SECONDS - 1, time.seconds);
  TEST_ASSERT_EQUAL(MAX_NANOSECONDS - 1000000, time.nanoseconds);

  time = kMaxMilliDeltaTime;
  ++time.seconds;
  TEST_ASSERT_TRUE(SystemTimeDecrementMilliseconds(&time, MAX_U32));
  TEST_ASSERT_EQUAL(1, time.seconds);
  TEST_ASSERT_EQUAL(999999, time.nanoseconds);

  time = (system_time_t) {
    .seconds = 5000,
    .nanoseconds = 750000150
  };
  TEST_ASSERT_TRUE(SystemTimeDecrementMilliseconds(&time, 0));
  TEST_ASSERT_EQUAL(5000, time.seconds);
  TEST_ASSERT_EQUAL(750000150, time.nanoseconds);

  TEST_ASSERT_TRUE(SystemTimeDecrementMilliseconds(&time, 4150));
  TEST_ASSERT_EQUAL(4996, time.seconds);
  TEST_ASSERT_EQUAL(600000150, time.nanoseconds);

  /* Seconds carry. */
  TEST_ASSERT_TRUE(SystemTimeDecrementMilliseconds(&time, 800));
  TEST_ASSERT_EQUAL(4995, time.seconds);
  TEST_ASSERT_EQUAL(800000150, time.nanoseconds);

  TEST_ASSERT_TRUE(SystemTimeDecrementMilliseconds(&time, 4900));
  TEST_ASSERT_EQUAL(4990, time.seconds);
  TEST_ASSERT_EQUAL(900000150, time.nanoseconds);

  /* Overflow. */
  time = (system_time_t) {
    .seconds = 50,
    .nanoseconds = 0
  };
  TEST_ASSERT_TRUE(SystemTimeDecrementMilliseconds(&time, 51000));
  TEST_ASSERT_TRUE(SystemTimeEqual(&kZeroTime, &time));

  time = (system_time_t) {
    .seconds = 50,
    .nanoseconds = 5000000
  };
  TEST_ASSERT_TRUE(SystemTimeDecrementMilliseconds(&time, 50004));
  TEST_ASSERT_EQUAL(0, time.seconds);
  TEST_ASSERT_EQUAL(1000000, time.nanoseconds);

  TEST_ASSERT_TRUE(SystemTimeDecrementMilliseconds(&time, 1));
  TEST_ASSERT_TRUE(SystemTimeEqual(&kZeroTime, &time));
}

static void TestSystemTime_DecrementMicroseconds(void) {
  TEST_ASSERT_FALSE(SystemTimeDecrementMicroseconds(NULL, 0));

  system_time_t time = kZeroTime;
  TEST_ASSERT_TRUE(SystemTimeDecrementMicroseconds(&time, 0));
  TEST_ASSERT_TRUE(SystemTimeEqual(&kZeroTime, &time));

  TEST_ASSERT_TRUE(SystemTimeDecrementMicroseconds(&time, MAX_SECONDS));
  TEST_ASSERT_TRUE(SystemTimeEqual(&kZeroTime, &time));

  time = kMaxTime;
  TEST_ASSERT_TRUE(SystemTimeDecrementMicroseconds(&time, 0));
  TEST_ASSERT_TRUE(SystemTimeEqual(&kMaxTime, &time));

  TEST_ASSERT_TRUE(SystemTimeDecrementMicroseconds(&time, 1001001));
  TEST_ASSERT_EQUAL(MAX_SECONDS - 1, time.seconds);
  TEST_ASSERT_EQUAL(MAX_NANOSECONDS - 1001000, time.nanoseconds);

  time = kMaxMicroDeltaTime;
  ++time.seconds;
  TEST_ASSERT_TRUE(SystemTimeDecrementMicroseconds(&time, MAX_U32));
  TEST_ASSERT_EQUAL(1, time.seconds);
  TEST_ASSERT_EQUAL(999, time.nanoseconds);

  time = (system_time_t) {
    .seconds = 5000,
    .nanoseconds = 750350150
  };
  TEST_ASSERT_TRUE(SystemTimeDecrementMicroseconds(&time, 0));
  TEST_ASSERT_EQUAL(5000, time.seconds);
  TEST_ASSERT_EQUAL(750350150, time.nanoseconds);

  TEST_ASSERT_TRUE(SystemTimeDecrementMicroseconds(&time, 4150250));
  TEST_ASSERT_EQUAL(4996, time.seconds);
  TEST_ASSERT_EQUAL(600100150, time.nanoseconds);

  /* Seconds carry. */
  TEST_ASSERT_TRUE(SystemTimeDecrementMicroseconds(&time, 800050));
  TEST_ASSERT_EQUAL(4995, time.seconds);
  TEST_ASSERT_EQUAL(800050150, time.nanoseconds);

  TEST_ASSERT_TRUE(SystemTimeDecrementMicroseconds(&time, 4900050));
  TEST_ASSERT_EQUAL(4990, time.seconds);
  TEST_ASSERT_EQUAL(900000150, time.nanoseconds);

  /* Overflow. */
  time = (system_time_t) {
    .seconds = 50,
    .nanoseconds = 0
  };
  TEST_ASSERT_TRUE(SystemTimeDecrementMicroseconds(&time, 51000000));
  TEST_ASSERT_TRUE(SystemTimeEqual(&kZeroTime, &time));

  time = (system_time_t) {
    .seconds = 50,
    .nanoseconds = 5000
  };
  TEST_ASSERT_TRUE(SystemTimeDecrementMicroseconds(&time, 50000004));
  TEST_ASSERT_EQUAL(0, time.seconds);
  TEST_ASSERT_EQUAL(1000, time.nanoseconds);

  TEST_ASSERT_TRUE(SystemTimeDecrementMicroseconds(&time, 1));
  TEST_ASSERT_TRUE(SystemTimeEqual(&kZeroTime, &time));
}

static void TestSystemTime_DecrementNanoseconds(void) {
  TEST_ASSERT_FALSE(SystemTimeDecrementNanoseconds(NULL, 0));

  system_time_t time = kZeroTime;
  TEST_ASSERT_TRUE(SystemTimeDecrementNanoseconds(&time, 0));
  TEST_ASSERT_TRUE(SystemTimeEqual(&kZeroTime, &time));

  TEST_ASSERT_TRUE(SystemTimeDecrementNanoseconds(&time, MAX_SECONDS));
  TEST_ASSERT_TRUE(SystemTimeEqual(&kZeroTime, &time));

  time = kMaxTime;
  TEST_ASSERT_TRUE(SystemTimeDecrementNanoseconds(&time, 0));
  TEST_ASSERT_TRUE(SystemTimeEqual(&kMaxTime, &time));

  TEST_ASSERT_TRUE(SystemTimeDecrementNanoseconds(&time, 1001001001));
  TEST_ASSERT_EQUAL(MAX_SECONDS - 1, time.seconds);
  TEST_ASSERT_EQUAL(MAX_NANOSECONDS - 1001001, time.nanoseconds);

  time = kMaxNanoDeltaTime;
  ++time.seconds;
  ++time.nanoseconds;
  TEST_ASSERT_TRUE(SystemTimeDecrementNanoseconds(&time, MAX_U32));
  TEST_ASSERT_EQUAL(1, time.seconds);
  TEST_ASSERT_EQUAL(1, time.nanoseconds);

  time = (system_time_t) {
    .seconds = 5000,
    .nanoseconds = 750150250
  };
  TEST_ASSERT_TRUE(SystemTimeDecrementNanoseconds(&time, 0));
  TEST_ASSERT_EQUAL(5000, time.seconds);
  TEST_ASSERT_EQUAL(750150250, time.nanoseconds);

  TEST_ASSERT_TRUE(SystemTimeDecrementNanoseconds(&time, 750150251));
  TEST_ASSERT_EQUAL(4999, time.seconds);
  TEST_ASSERT_EQUAL(MAX_NANOSECONDS, time.nanoseconds);

  TEST_ASSERT_TRUE(SystemTimeDecrementNanoseconds(&time, 2000000500));
  TEST_ASSERT_EQUAL(4997, time.seconds);
  TEST_ASSERT_EQUAL(MAX_NANOSECONDS - 500, time.nanoseconds);

  time = (system_time_t) {
    .seconds = 1,
    .nanoseconds = MAX_NANOSECONDS
  };
  TEST_ASSERT_TRUE(SystemTimeDecrementNanoseconds(&time, 1000000000));
  TEST_ASSERT_EQUAL(0, time.seconds);
  TEST_ASSERT_EQUAL(MAX_NANOSECONDS, time.nanoseconds);

  time = (system_time_t) {
    .seconds = 1,
    .nanoseconds = 500000000
  };
  TEST_ASSERT_TRUE(SystemTimeDecrementNanoseconds(&time, 500000001));
  TEST_ASSERT_EQUAL(0, time.seconds);
  TEST_ASSERT_EQUAL(MAX_NANOSECONDS, time.nanoseconds);

  time = (system_time_t) {
    .seconds = 1,
    .nanoseconds = 500000000
  };
  TEST_ASSERT_TRUE(SystemTimeDecrementNanoseconds(&time, 1500000001));
  TEST_ASSERT_TRUE(SystemTimeEqual(&kZeroTime, &time));

  time = (system_time_t) {
    .seconds = 1,
    .nanoseconds = 500000000
  };
  TEST_ASSERT_TRUE(SystemTimeDecrementNanoseconds(&time, 1500000000));
  TEST_ASSERT_TRUE(SystemTimeEqual(&kZeroTime, &time));
}


void SystemTimeTest(void) {
  RUN_TEST(TestSystemTime_LessThan);
  RUN_TEST(TestSystemTime_LessThanOrEqual);
  RUN_TEST(TestSystemTime_Equal);
  RUN_TEST(TestSystemTime_GreaterThan);
  RUN_TEST(TestSystemTime_GreaterThanOrEqual);

  RUN_TEST(TestSystemTime_SecondsDelta);
  RUN_TEST(TestSystemTime_MillisecondsDelta);
  RUN_TEST(TestSystemTime_MicrosecondsDelta);
  RUN_TEST(TestSystemTime_NanosecondsDelta);

  RUN_TEST(TestSystemTime_IncrementSeconds);
  RUN_TEST(TestSystemTime_IncrementMilliseconds);
  RUN_TEST(TestSystemTime_IncrementMicroseconds);
  RUN_TEST(TestSystemTime_IncrementNanoseconds);

  RUN_TEST(TestSystemTime_DecrementSeconds);
  RUN_TEST(TestSystemTime_DecrementMilliseconds);
  RUN_TEST(TestSystemTime_DecrementMicroseconds);
  RUN_TEST(TestSystemTime_DecrementNanoseconds);

#ifdef _PLATFORM_NATIVE
  RUN_TEST(TestSystemTime_NativeMonotonic);
#endif  /* _PLATFORM_NATIVE */
}
