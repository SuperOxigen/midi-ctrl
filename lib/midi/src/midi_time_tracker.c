/*
 * MIDI Controller - MIDI Time Tracker
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#include <string.h>

#include "midi_defs.h"
#include "midi_time_tracker.h"

#define TRACKING_NONE           0xFF
/* Bit flag */
#define TRACKING_FORWARD        0x80
#define TRACKING_TO_CODE(tracking) ((tracking) & 0x7F)

#define BEGIN_FORWARD_TRACKING  (MIDI_FRAME_COUNT_LSN | TRACKING_FORWARD)
#define END_FORWARD_TRACKING    (MIDI_HOURS_COUNT_MSN | TRACKING_FORWARD)
#define BEGIN_REVERSE_TRACKING  MIDI_HOURS_COUNT_MSN
#define END_REVERSE_TRACKING    MIDI_FRAME_COUNT_LSN

bool_t MidiInitializeTimeTracker(midi_time_tracker_t *tracker) {
  if (tracker == NULL) return false;
  *tracker = (midi_time_tracker_t) {
    .direction = MIDI_TIME_UNKNOWN,
    .tracking = TRACKING_NONE
  };
  return MidiInitializeTime(&tracker->time);
}

static inline uint8_t MidiNextForwardTracking(uint8_t tracking) {
  if (tracking == TRACKING_NONE) return BEGIN_FORWARD_TRACKING;
  if (!(tracking & TRACKING_FORWARD)) return TRACKING_NONE;
  if (tracking == END_FORWARD_TRACKING) return BEGIN_FORWARD_TRACKING;
  uint8_t const type = TRACKING_TO_CODE(tracking);
  return ((type + 0x10) | TRACKING_FORWARD) & 0xF0;
}

static inline uint8_t MidiNextReverseTracking(uint8_t tracking) {
  if (tracking == TRACKING_NONE) return BEGIN_REVERSE_TRACKING;
  if (tracking & TRACKING_FORWARD) return TRACKING_NONE;
  if (tracking == END_REVERSE_TRACKING) return BEGIN_REVERSE_TRACKING;
  uint8_t const type = TRACKING_TO_CODE(tracking);
  return ((type - 0x10)) & 0x70;
}

static inline bool_t MidiIsValidTracking(uint8_t tracking) {
  return (tracking == TRACKING_NONE) || ((tracking & 0xF0) == tracking);
}

bool_t MidiIsValidTimeTracker(midi_time_tracker_t const *tracker) {
  if (tracker == NULL) return false;
  if (!MidiIsValidTracking(tracker->tracking))
    return false;
  if (tracker->tracking == TRACKING_NONE) {
    return tracker->direction == MIDI_TIME_UNKNOWN;
  }
  switch (tracker->direction) {
    case MIDI_TIME_UNKNOWN: return true;
    case MIDI_TIME_FORWARD: {
      return !!(tracker->tracking & TRACKING_FORWARD);
    }
    case MIDI_TIME_REVERSE: {
      return !(tracker->tracking & TRACKING_FORWARD);
    }
    default:
      return false;
  }
}

bool_t MidiIsSynchronized(midi_time_tracker_t const *tracker) {
  if (tracker == NULL) return false;
  if (tracker->direction == MIDI_TIME_UNKNOWN ||
      tracker->tracking == TRACKING_NONE ||
      !MidiIsValidTracking(tracker->tracking)) return false;
  switch (tracker->direction) {
    case MIDI_TIME_FORWARD:
      return !!(tracker->tracking & TRACKING_FORWARD);
    case MIDI_TIME_REVERSE:
      return !(tracker->tracking & TRACKING_FORWARD);
    default:
      return false;
  }
}

static inline void MidiInvalidateTracker(midi_time_tracker_t *tracker) {
  MidiInitializeTime(&tracker->time);
  tracker->direction = MIDI_TIME_UNKNOWN;
  tracker->tracking = TRACKING_NONE;
}

bool_t MidiUpdateTimeTracker(
    midi_time_tracker_t *tracker, midi_time_code_t const *time_code,
    bool_t *updated, midi_time_t *time, midi_time_direction_t *direction) {
  if (tracker == NULL || !MidiIsValidTimeCode(time_code))
    return false;
  if (updated == NULL || time == NULL || direction == NULL)
    return false;
  *updated = false;
  if (!MidiIsValidTimeTracker(tracker)) {
    MidiInvalidateTracker(tracker);
    *updated = true;
    *direction = MIDI_TIME_UNKNOWN;
  }

  /* Tracking is NONE (and direction is UNKNOWN), then begin tracking
   * if the code is a possible begining code, otherwise, drop the code
   * and wait for the next one. */
  if (tracker->tracking == TRACKING_NONE) {
    if (time_code->type == MIDI_FRAME_COUNT_LSN) {
      MidiUpdateTime(&tracker->time, time_code);
      tracker->tracking = TRACKING_FORWARD | MIDI_FRAME_COUNT_LSN;
    } else if (time_code->type == MIDI_HOURS_COUNT_MSN) {
      MidiUpdateTime(&tracker->time, time_code);
      tracker->tracking = MIDI_HOURS_COUNT_MSN;
    }
    return true;
  }

  if (tracker->tracking & TRACKING_FORWARD) {
    uint8_t const new_tracking = time_code->type | TRACKING_FORWARD;
    uint8_t const expected_tracking =
        MidiNextForwardTracking(tracker->tracking);

    if (new_tracking == expected_tracking) {
      MidiUpdateTime(&tracker->time, time_code);
      tracker->tracking = new_tracking;
      if (new_tracking == END_FORWARD_TRACKING) {
        *updated = true;
        tracker->direction = *direction = MIDI_TIME_FORWARD;
        memcpy(time, &tracker->time, sizeof(midi_time_t));
      }
      return true;
    }
  } else { /* If reverse tracking. */
    uint8_t const new_tracking = time_code->type;
    uint8_t const expected_tracking =
        MidiNextReverseTracking(tracker->tracking);

    if (new_tracking == expected_tracking) {
      MidiUpdateTime(&tracker->time, time_code);
      tracker->tracking = new_tracking;
      if (new_tracking == END_REVERSE_TRACKING) {
        *updated = true;
        tracker->direction = *direction = MIDI_TIME_REVERSE;
        memcpy(time, &tracker->time, sizeof(midi_time_t));
      }
      return true;
    }
  }

  /* If previously synced, set as unsynced. */
  if (tracker->direction != MIDI_TIME_UNKNOWN) {
    *updated = true;
    *direction = MIDI_TIME_UNKNOWN;
  }
  MidiInvalidateTracker(tracker);

  /* Check if possible to reset tracking. */
  if (time_code->type == MIDI_FRAME_COUNT_LSN) {
    MidiUpdateTime(&tracker->time, time_code);
    tracker->tracking = BEGIN_FORWARD_TRACKING;
  } else if (time_code->type == MIDI_HOURS_COUNT_MSN) {
    MidiUpdateTime(&tracker->time, time_code);
    tracker->tracking = BEGIN_REVERSE_TRACKING;
  }
  return true;
}

bool_t MidiGetTimeTrackerTime(
    midi_time_tracker_t const *tracker, midi_time_t *time) {
  if (!MidiIsValidTimeTracker(tracker) || time == NULL) return false;
  if (tracker->direction != MIDI_TIME_FORWARD &&
      tracker->direction != MIDI_TIME_REVERSE) return false;
  memcpy(time, &tracker->time, sizeof(midi_time_t));
  return true;
}

bool_t MidiSetTimeTrackerTime(
    midi_time_tracker_t *tracker, midi_time_t const *time) {
  if (tracker == NULL || !MidiIsValidTime(time)) return false;
  memcpy(&tracker->time, time, sizeof(midi_time_t));
  tracker->tracking = TRACKING_NONE;
  tracker->direction = MIDI_TIME_UNKNOWN;
  return true;
}

bool MidiSetTimeTracker(
    midi_time_tracker_t *tracker, midi_time_t const *time,
    midi_time_direction_t direction) {
  if (!MidiSetTimeTrackerTime(tracker, time)) return false;
  /* If the new direction is forward or reverse, the tracking
   * is set to the END of the sequence so the next call to update
   * expects the begining of the tracking sequence. */
  switch (direction) {
    case MIDI_TIME_FORWARD:
      tracker->direction = MIDI_TIME_FORWARD;
      tracker->tracking = END_FORWARD_TRACKING;
      break;
    case MIDI_TIME_REVERSE:
      tracker->direction = MIDI_TIME_REVERSE;
      tracker->tracking = END_REVERSE_TRACKING;
      break;
    default:
      tracker->direction = MIDI_TIME_UNKNOWN;
      break;
  }
  return true;
}
