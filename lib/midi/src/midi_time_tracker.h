/*
 * MIDI Controller - MIDI Time Tracker
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#ifndef _MIDI_TIME_TRACKER_H_
#define _MIDI_TIME_TRACKER_H_

#include "base.h"

#include "midi_time.h"

C_SECTION_BEGIN;

/*
 *  MIDI Time Tracker.
 *    Takes time codes in sequence received to produce the clock
 *    synchronization.  The order of time codes determines the
 *    time direction.
 */
typedef struct {
  midi_time_t time;
  midi_time_direction_t direction;
  uint8_t tracking;  /* Internally used to determine direction. */
} midi_time_tracker_t;

bool_t MidiInitializeTimeTracker(midi_time_tracker_t *tracker);

bool_t MidiIsValidTimeTracker(midi_time_tracker_t const *tracker);

bool_t MidiIsSynchronized(midi_time_tracker_t const *tracker);

/* This feeds the tracker time codes.  After each synchronization,
 * the function sets |updated| to true and |direction| is set.  if
 * If |updated| is true and |direction| is not UNKNOWN, then |time|
 * is updated. */
bool_t MidiUpdateTimeTracker(
  midi_time_tracker_t *tracker, midi_time_code_t const *time_code,
  bool_t *updated, midi_time_t *time, midi_time_direction_t *direction);

bool_t MidiGetTimeTrackerTime(
  midi_time_tracker_t const *tracker, midi_time_t *time);
bool_t MidiSetTimeTrackerTime(
  midi_time_tracker_t *tracker, midi_time_t const *time);

/* When setting the direction to FORWARD or REVERSE, then the tracking
 * set to expect the next time code to be begining of a FORWARD or
 * REVERSE sequence, respectively. */
bool MidiSetTimeTracker(
  midi_time_tracker_t *tracker, midi_time_t const *time,
  midi_time_direction_t direction);

C_SECTION_END;

#endif  /* _MIDI_TIME_TRACKER_H_ */
