/*
 * MIDI Controller - MIDI Time Tracker Test
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#include <string.h>
#include <unity.h>

#include "midi_defs.h"
#include "midi_time_tracker.h"

#define TRACKING_NONE           0xFF
#define TRACKING_FORWARD        0x80

static void TestMidiTimeTracker_Initializer(void) {
  TEST_ASSERT_FALSE(MidiInitializeTimeTracker(NULL));
  midi_time_tracker_t tracker;
  TEST_ASSERT_TRUE(MidiInitializeTimeTracker(&tracker));
  TEST_ASSERT_EQUAL(tracker.direction, MIDI_TIME_UNKNOWN);
}

static void TestMidiTimeTracker_Validator(void) {
  TEST_ASSERT_FALSE(MidiIsValidTimeTracker(NULL));
  midi_time_tracker_t tracker;
  MidiInitializeTimeTracker(&tracker);
  TEST_ASSERT_TRUE(MidiIsValidTimeTracker(&tracker));

  /* Tracking must be valid. */
  tracker.direction = MIDI_TIME_UNKNOWN;
  tracker.tracking = MIDI_HOURS_COUNT_LSN | TRACKING_FORWARD | 0x02;
  TEST_ASSERT_FALSE(MidiIsValidTimeTracker(&tracker));

  MidiInitializeTimeTracker(&tracker);
  /* If the tracker is NONE, then the direction must be UNKNOWN */
  tracker.direction = MIDI_TIME_FORWARD;
  tracker.tracking = TRACKING_NONE;
  TEST_ASSERT_FALSE(MidiIsValidTimeTracker(&tracker));
  tracker.direction = MIDI_TIME_REVERSE;
  TEST_ASSERT_FALSE(MidiIsValidTimeTracker(&tracker));

  /* If direction is forward, then the tracking must be a forward type. */
  MidiInitializeTimeTracker(&tracker);
  tracker.direction = MIDI_TIME_FORWARD;
  tracker.tracking = MIDI_HOURS_COUNT_LSN;
  TEST_ASSERT_FALSE(MidiIsValidTimeTracker(&tracker));
  tracker.tracking = MIDI_HOURS_COUNT_LSN | TRACKING_FORWARD;
  TEST_ASSERT_TRUE(MidiIsValidTimeTracker(&tracker));

  /* If direction is reverse, then the tracking must be a reverse type. */
  MidiInitializeTimeTracker(&tracker);
  tracker.direction = MIDI_TIME_REVERSE;
  tracker.tracking = MIDI_HOURS_COUNT_LSN | TRACKING_FORWARD;
  TEST_ASSERT_FALSE(MidiIsValidTimeTracker(&tracker));
  tracker.tracking = MIDI_HOURS_COUNT_LSN;
  TEST_ASSERT_TRUE(MidiIsValidTimeTracker(&tracker));
}

static void TestMidiTimeTracker_IsSynchronized(void) {
  TEST_ASSERT_FALSE(MidiIsSynchronized(NULL));
  midi_time_tracker_t tracker;
  MidiInitializeTimeTracker(&tracker);
  TEST_ASSERT_FALSE(MidiIsSynchronized(&tracker));

  tracker.direction = MIDI_TIME_FORWARD;
  tracker.tracking = TRACKING_NONE;
  TEST_ASSERT_FALSE(MidiIsSynchronized(&tracker));

  tracker.tracking = MIDI_HOURS_COUNT_LSN;
  TEST_ASSERT_FALSE(MidiIsSynchronized(&tracker));

  tracker.tracking = MIDI_HOURS_COUNT_LSN | TRACKING_FORWARD;
  TEST_ASSERT_TRUE(MidiIsSynchronized(&tracker));

  tracker.direction = MIDI_TIME_REVERSE;
  TEST_ASSERT_FALSE(MidiIsSynchronized(&tracker));

  tracker.tracking = MIDI_HOURS_COUNT_LSN;
  TEST_ASSERT_TRUE(MidiIsSynchronized(&tracker));
}

static void TestMidiTimeTrackerUpdate_Invalid(void) {
  static midi_time_code_t const kValidTimeCode = {
    .type = MIDI_SECONDS_COUNT_LSN,
    .value = 10
  };
  static midi_time_code_t const kInvalidTimeCodeType = {
    .type = MIDI_HOURS_COUNT_LSN | 0x01,
    .value = 1
  };
  static midi_time_code_t const kInvalidTimeCodeValue = {
    .type = MIDI_MINUTES_COUNT_LSN,
    .value = 0xA0
  };
  midi_time_tracker_t tracker;
  MidiInitializeTimeTracker(&tracker);
  bool_t updated = false;
  midi_time_t time = {};
  midi_time_direction_t direction = MIDI_TIME_UNKNOWN;

  TEST_ASSERT_FALSE(MidiUpdateTimeTracker(
      NULL, &kValidTimeCode, &updated, &time, &direction));
  TEST_ASSERT_FALSE(MidiUpdateTimeTracker(
      &tracker, NULL, &updated, &time, &direction));
  TEST_ASSERT_FALSE(MidiUpdateTimeTracker(
      &tracker, &kInvalidTimeCodeType, &updated, &time, &direction));
  TEST_ASSERT_FALSE(MidiUpdateTimeTracker(
      &tracker, &kInvalidTimeCodeValue, &updated, &time, &direction));
  TEST_ASSERT_FALSE(MidiUpdateTimeTracker(
      &tracker, &kValidTimeCode, NULL, &time, &direction));
  TEST_ASSERT_FALSE(MidiUpdateTimeTracker(
      &tracker, &kValidTimeCode, &updated, NULL, &direction));
  TEST_ASSERT_FALSE(MidiUpdateTimeTracker(
      &tracker, &kValidTimeCode, &updated, &time, NULL));

  TEST_ASSERT_TRUE(MidiUpdateTimeTracker(
      &tracker, &kValidTimeCode, &updated, &time, &direction));
}

static midi_time_code_t const kForwardSequence[8] = {
  { .type = MIDI_FRAME_COUNT_LSN, .value = 4 },
  { .type = MIDI_FRAME_COUNT_MSN, .value = 1 },
  { .type = MIDI_SECONDS_COUNT_LSN, .value = 1 },
  { .type = MIDI_SECONDS_COUNT_MSN, .value = 2 },
  { .type = MIDI_MINUTES_COUNT_LSN, .value = 3 },
  { .type = MIDI_MINUTES_COUNT_MSN, .value = 3 },
  { .type = MIDI_HOURS_COUNT_LSN, .value = 5 },
  { .type = MIDI_HOURS_COUNT_MSN, .value = (MIDI_24_FPS >> 4) }
};

static midi_time_t const kExpectedFowardTime = {
  .frame = 20,
  .seconds = 33,
  .minutes = 51,
  .hours = 5,
  .fps = MIDI_24_FPS
};

static midi_time_code_t const kReverseSequence[8] = {
  { .type = MIDI_HOURS_COUNT_MSN,
    .value = (MIDI_30_FPS_DROP_FRAME >> 4) | 1 },
  { .type = MIDI_HOURS_COUNT_LSN, .value = 6 },
  { .type = MIDI_MINUTES_COUNT_MSN, .value = 1 },
  { .type = MIDI_MINUTES_COUNT_LSN, .value = 12 },
  { .type = MIDI_SECONDS_COUNT_MSN, .value = 0 },
  { .type = MIDI_SECONDS_COUNT_LSN, .value = 10 },
  { .type = MIDI_FRAME_COUNT_MSN, .value = 1 },
  { .type = MIDI_FRAME_COUNT_LSN, .value = 13 }
};

static midi_time_t const kExpectedReverseTime = {
  .frame = 29,
  .seconds = 10,
  .minutes = 28,
  .hours = 22,
  .fps = MIDI_30_FPS_DROP_FRAME
};

static midi_time_t const kInvalidTime = {
  .frame = 29,
  .seconds = 63,
  .minutes = 28,
  .hours = 22,
  .fps = MIDI_30_FPS_DROP_FRAME
};

static void TestMidiTimeTrackerUpdate_Forward(void) {
  midi_time_tracker_t tracker;
  bool_t updated = false;
  midi_time_t time = {};
  midi_time_direction_t direction = MIDI_TIME_UNKNOWN;
  MidiInitializeTimeTracker(&tracker);

  for (size_t i = 0; i < 7; ++i) {
    TEST_ASSERT_TRUE(MidiUpdateTimeTracker(
        &tracker, &kForwardSequence[i], &updated, &time, &direction));
    TEST_ASSERT_FALSE(updated);
  }
  TEST_ASSERT_EQUAL(MIDI_TIME_UNKNOWN, tracker.direction);

  TEST_ASSERT_FALSE(MidiIsSynchronized(&tracker));
  TEST_ASSERT_TRUE(MidiUpdateTimeTracker(
      &tracker, &kForwardSequence[7], &updated, &time, &direction));

  TEST_ASSERT_TRUE(updated);
  TEST_ASSERT_EQUAL(kExpectedFowardTime.frame, time.frame);
  TEST_ASSERT_EQUAL(kExpectedFowardTime.seconds, time.seconds);
  TEST_ASSERT_EQUAL(kExpectedFowardTime.minutes, time.minutes);
  TEST_ASSERT_EQUAL(kExpectedFowardTime.hours, time.hours);
  TEST_ASSERT_EQUAL(kExpectedFowardTime.fps, time.fps);
  TEST_ASSERT_EQUAL(MIDI_TIME_FORWARD, direction);
  TEST_ASSERT_EQUAL(MIDI_TIME_FORWARD, tracker.direction);

  TEST_ASSERT_TRUE(MidiIsSynchronized(&tracker));
}

static void TestMidiTimeTrackerUpdate_Reverse(void) {
  midi_time_tracker_t tracker;
  bool_t updated = false;
  midi_time_t time = {};
  midi_time_direction_t direction = MIDI_TIME_UNKNOWN;
  MidiInitializeTimeTracker(&tracker);

  for (size_t i = 0; i < 7; ++i) {
    TEST_ASSERT_TRUE(MidiUpdateTimeTracker(
        &tracker, &kReverseSequence[i], &updated, &time, &direction));
    TEST_ASSERT_FALSE(updated);
  }
  TEST_ASSERT_EQUAL(MIDI_TIME_UNKNOWN, tracker.direction);

  TEST_ASSERT_FALSE(MidiIsSynchronized(&tracker));
  TEST_ASSERT_TRUE(MidiUpdateTimeTracker(
      &tracker, &kReverseSequence[7], &updated, &time, &direction));

  TEST_ASSERT_TRUE(updated);
  TEST_ASSERT_EQUAL(kExpectedReverseTime.frame, time.frame);
  TEST_ASSERT_EQUAL(kExpectedReverseTime.seconds, time.seconds);
  TEST_ASSERT_EQUAL(kExpectedReverseTime.minutes, time.minutes);
  TEST_ASSERT_EQUAL(kExpectedReverseTime.hours, time.hours);
  TEST_ASSERT_EQUAL(kExpectedReverseTime.fps, time.fps);
  TEST_ASSERT_EQUAL(MIDI_TIME_REVERSE, direction);
  TEST_ASSERT_EQUAL(MIDI_TIME_REVERSE, tracker.direction);

  TEST_ASSERT_TRUE(MidiIsSynchronized(&tracker));
}

static void TestMidiTimeTrackerUpdate_UpdateOnNextComplete(void) {
  midi_time_tracker_t tracker;
  bool_t updated = false;
  midi_time_t time = {};
  midi_time_direction_t direction = MIDI_TIME_UNKNOWN;
  MidiInitializeTimeTracker(&tracker);

  for (size_t i = 0; i < 8; ++i) {
    TEST_ASSERT_TRUE(MidiUpdateTimeTracker(
        &tracker, &kForwardSequence[i], &updated, &time, &direction));
  }
  TEST_ASSERT_TRUE(MidiIsSynchronized(&tracker));
  for (size_t i = 0; i < 7; ++i) {
    TEST_ASSERT_TRUE(MidiUpdateTimeTracker(
        &tracker, &kForwardSequence[i], &updated, &time, &direction));
    TEST_ASSERT_FALSE(updated);
    TEST_ASSERT_TRUE(MidiIsSynchronized(&tracker));
  }
  memset(&time, 0, sizeof(midi_time_t));
  direction = MIDI_TIME_UNKNOWN;
  TEST_ASSERT_TRUE(MidiUpdateTimeTracker(
      &tracker, &kForwardSequence[7], &updated, &time, &direction));
  TEST_ASSERT_TRUE(updated);
  TEST_ASSERT_TRUE(updated);
  TEST_ASSERT_EQUAL(kExpectedFowardTime.frame, time.frame);
  TEST_ASSERT_EQUAL(kExpectedFowardTime.seconds, time.seconds);
  TEST_ASSERT_EQUAL(kExpectedFowardTime.minutes, time.minutes);
  TEST_ASSERT_EQUAL(kExpectedFowardTime.hours, time.hours);
  TEST_ASSERT_EQUAL(kExpectedFowardTime.fps, time.fps);
  TEST_ASSERT_EQUAL(MIDI_TIME_FORWARD, direction);
  TEST_ASSERT_EQUAL(MIDI_TIME_FORWARD, tracker.direction);
  TEST_ASSERT_TRUE(MidiIsSynchronized(&tracker));
}

static void TestMidiTimeTrackerUpdate_BadStart(void) {
  midi_time_tracker_t tracker;
  bool_t updated = false;
  midi_time_t time = {};
  midi_time_direction_t direction = MIDI_TIME_UNKNOWN;
  MidiInitializeTimeTracker(&tracker);

  for (size_t i = 2; i < 8; ++i) {
    TEST_ASSERT_TRUE(MidiUpdateTimeTracker(
        &tracker, &kForwardSequence[i], &updated, &time, &direction));
    TEST_ASSERT_FALSE(updated);
  }
  TEST_ASSERT_FALSE(MidiIsSynchronized(&tracker));

  for (size_t i = 0; i < 7; ++i) {
    TEST_ASSERT_TRUE(MidiUpdateTimeTracker(
        &tracker, &kForwardSequence[i], &updated, &time, &direction));
    TEST_ASSERT_FALSE(updated);
  }
  TEST_ASSERT_TRUE(MidiUpdateTimeTracker(
      &tracker, &kForwardSequence[7], &updated, &time, &direction));
  TEST_ASSERT_TRUE(updated);
  TEST_ASSERT_EQUAL(kExpectedFowardTime.frame, time.frame);
  TEST_ASSERT_EQUAL(kExpectedFowardTime.seconds, time.seconds);
  TEST_ASSERT_EQUAL(kExpectedFowardTime.minutes, time.minutes);
  TEST_ASSERT_EQUAL(kExpectedFowardTime.hours, time.hours);
  TEST_ASSERT_EQUAL(kExpectedFowardTime.fps, time.fps);
  TEST_ASSERT_EQUAL(MIDI_TIME_FORWARD, direction);
  TEST_ASSERT_EQUAL(MIDI_TIME_FORWARD, tracker.direction);
}

static void TestMidiTimeTrackerUpdate_ChangeDirection(void) {
  midi_time_tracker_t tracker;
  bool_t updated = false;
  midi_time_t time = {};
  midi_time_direction_t direction = MIDI_TIME_UNKNOWN;
  MidiInitializeTimeTracker(&tracker);

  for (size_t i = 0; i < 8; ++i) {
    TEST_ASSERT_TRUE(MidiUpdateTimeTracker(
        &tracker, &kForwardSequence[i], &updated, &time, &direction));
  }
  TEST_ASSERT_TRUE(updated);
  TEST_ASSERT_EQUAL(MIDI_TIME_FORWARD, tracker.direction);
  updated = false;

  TEST_ASSERT_TRUE(MidiUpdateTimeTracker(
      &tracker, &kReverseSequence[0], &updated, &time, &direction));
  TEST_ASSERT_TRUE(updated);
  TEST_ASSERT_EQUAL(MIDI_TIME_UNKNOWN, direction);
  TEST_ASSERT_EQUAL(MIDI_TIME_UNKNOWN, tracker.direction);

  for (size_t i = 1; i < 8; ++i) {
    TEST_ASSERT_TRUE(MidiUpdateTimeTracker(
        &tracker, &kReverseSequence[i], &updated, &time, &direction));
  }
  TEST_ASSERT_TRUE(updated);
  TEST_ASSERT_EQUAL(kExpectedReverseTime.frame, time.frame);
  TEST_ASSERT_EQUAL(kExpectedReverseTime.seconds, time.seconds);
  TEST_ASSERT_EQUAL(kExpectedReverseTime.minutes, time.minutes);
  TEST_ASSERT_EQUAL(kExpectedReverseTime.hours, time.hours);
  TEST_ASSERT_EQUAL(kExpectedReverseTime.fps, time.fps);
  TEST_ASSERT_EQUAL(MIDI_TIME_REVERSE, direction);
  TEST_ASSERT_EQUAL(MIDI_TIME_REVERSE, tracker.direction);
}

static void TestMidiTimeTracker_GetTime(void) {
  midi_time_tracker_t tracker;
  midi_time_t time = {};
  MidiInitializeTimeTracker(&tracker);
  TEST_ASSERT_FALSE(MidiGetTimeTrackerTime(&tracker, &time));

  memcpy(&tracker.time, &kExpectedFowardTime, sizeof(midi_time_t));
  tracker.tracking = MIDI_MINUTES_COUNT_LSN | TRACKING_FORWARD;
  tracker.direction = MIDI_TIME_FORWARD;

  TEST_ASSERT_FALSE(MidiGetTimeTrackerTime(NULL, &time));
  TEST_ASSERT_FALSE(MidiGetTimeTrackerTime(&tracker, NULL));

  memset(&time, 0, sizeof(midi_time_t));
  TEST_ASSERT_TRUE(MidiGetTimeTrackerTime(&tracker, &time));
  TEST_ASSERT_EQUAL(kExpectedFowardTime.frame, time.frame);
  TEST_ASSERT_EQUAL(kExpectedFowardTime.seconds, time.seconds);
  TEST_ASSERT_EQUAL(kExpectedFowardTime.minutes, time.minutes);
  TEST_ASSERT_EQUAL(kExpectedFowardTime.hours, time.hours);
  TEST_ASSERT_EQUAL(kExpectedFowardTime.fps, time.fps);
}

static void TestMidiTimeTracker_SetTime(void) {
  midi_time_tracker_t tracker;
  MidiInitializeTimeTracker(&tracker);

  TEST_ASSERT_FALSE(MidiSetTimeTrackerTime(NULL, &kExpectedFowardTime));
  TEST_ASSERT_FALSE(MidiSetTimeTrackerTime(&tracker, NULL));
  TEST_ASSERT_FALSE(MidiSetTimeTrackerTime(&tracker, &kInvalidTime));

  TEST_ASSERT_TRUE(MidiSetTimeTrackerTime(&tracker, &kExpectedFowardTime));
  tracker.tracking = MIDI_MINUTES_COUNT_LSN | TRACKING_FORWARD;
  tracker.direction = MIDI_TIME_FORWARD;

  midi_time_t time;
  TEST_ASSERT_TRUE(MidiGetTimeTrackerTime(&tracker, &time));
  TEST_ASSERT_EQUAL(kExpectedFowardTime.frame, time.frame);
  TEST_ASSERT_EQUAL(kExpectedFowardTime.seconds, time.seconds);
  TEST_ASSERT_EQUAL(kExpectedFowardTime.minutes, time.minutes);
  TEST_ASSERT_EQUAL(kExpectedFowardTime.hours, time.hours);
  TEST_ASSERT_EQUAL(kExpectedFowardTime.fps, time.fps);
}

static void TestMidiTimeTracker_Set(void) {
  midi_time_tracker_t tracker;
  MidiInitializeTimeTracker(&tracker);

  TEST_ASSERT_FALSE(MidiSetTimeTracker(
      NULL, &kExpectedFowardTime, MIDI_TIME_FORWARD));
  TEST_ASSERT_FALSE(MidiSetTimeTracker(
      &tracker, NULL, MIDI_TIME_FORWARD));
  TEST_ASSERT_FALSE(MidiSetTimeTracker(
      &tracker, &kInvalidTime, MIDI_TIME_FORWARD));

  TEST_ASSERT_TRUE(MidiSetTimeTracker(
      &tracker, &kExpectedFowardTime, MIDI_TIME_UNKNOWN));
  TEST_ASSERT_EQUAL(kExpectedFowardTime.frame, tracker.time.frame);
  TEST_ASSERT_EQUAL(kExpectedFowardTime.seconds, tracker.time.seconds);
  TEST_ASSERT_EQUAL(kExpectedFowardTime.minutes, tracker.time.minutes);
  TEST_ASSERT_EQUAL(kExpectedFowardTime.hours, tracker.time.hours);
  TEST_ASSERT_EQUAL(kExpectedFowardTime.fps, tracker.time.fps);
  TEST_ASSERT_EQUAL(MIDI_TIME_UNKNOWN, tracker.direction);

  TEST_ASSERT_TRUE(MidiSetTimeTracker(
      &tracker, &kExpectedReverseTime, MIDI_TIME_REVERSE));
  TEST_ASSERT_EQUAL(MIDI_TIME_REVERSE, tracker.direction);
  midi_time_t time;
  TEST_ASSERT_TRUE(MidiGetTimeTrackerTime(&tracker, &time));
  TEST_ASSERT_EQUAL(kExpectedReverseTime.frame, time.frame);
  TEST_ASSERT_EQUAL(kExpectedReverseTime.seconds, time.seconds);
  TEST_ASSERT_EQUAL(kExpectedReverseTime.minutes, time.minutes);
  TEST_ASSERT_EQUAL(kExpectedReverseTime.hours, time.hours);
  TEST_ASSERT_EQUAL(kExpectedReverseTime.fps, time.fps);

  TEST_ASSERT_TRUE(MidiSetTimeTracker(
      &tracker, &kExpectedFowardTime, MIDI_TIME_FORWARD));
  TEST_ASSERT_EQUAL(MIDI_TIME_FORWARD, tracker.direction);
  TEST_ASSERT_TRUE(MidiGetTimeTrackerTime(&tracker, &time));
  TEST_ASSERT_EQUAL(kExpectedFowardTime.frame, time.frame);
  TEST_ASSERT_EQUAL(kExpectedFowardTime.seconds, time.seconds);
  TEST_ASSERT_EQUAL(kExpectedFowardTime.minutes, time.minutes);
  TEST_ASSERT_EQUAL(kExpectedFowardTime.hours, time.hours);
  TEST_ASSERT_EQUAL(kExpectedFowardTime.fps, time.fps);
}

void MidiTimeTrackerTest(void) {
  RUN_TEST(TestMidiTimeTracker_Initializer);
  RUN_TEST(TestMidiTimeTracker_Validator);
  RUN_TEST(TestMidiTimeTracker_IsSynchronized);
  RUN_TEST(TestMidiTimeTrackerUpdate_Invalid);
  RUN_TEST(TestMidiTimeTrackerUpdate_Forward);
  RUN_TEST(TestMidiTimeTrackerUpdate_Reverse);
  RUN_TEST(TestMidiTimeTrackerUpdate_UpdateOnNextComplete);
  RUN_TEST(TestMidiTimeTrackerUpdate_BadStart);
  RUN_TEST(TestMidiTimeTrackerUpdate_ChangeDirection);
  RUN_TEST(TestMidiTimeTracker_GetTime);
  RUN_TEST(TestMidiTimeTracker_SetTime);
  RUN_TEST(TestMidiTimeTracker_Set);
}

// static void TestMidiTimeTracker_(void) {}
