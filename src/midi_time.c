/*
 * MIDI Controller - MIDI Time and Time Code
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#include <string.h>

#include "midi_bytes.h"
#include "midi_defs.h"
#include "midi_time.h"

#define MIDI_TIME_CODE_TYPE_MASK    0x70
#define MIDI_TIME_CODE_VALUE_MASK   0x0F

/* Masks of the nibbles. */
#define MIDI_FRAME_COUNT_LSN_MASK   0x0F
#define MIDI_FRAME_COUNT_MSN_MASK   0x01
#define MIDI_SECONDS_COUNT_LSN_MASK 0x0F
#define MIDI_SECONDS_COUNT_MSN_MASK 0x03
#define MIDI_MINUTES_COUNT_LSN_MASK 0x0F
#define MIDI_MINUTES_COUNT_MSN_MASK 0x03
#define MIDI_HOURS_COUNT_LSN_MASK   0x0F
#define MIDI_HOURS_COUNT_MSN_MASK   0x01
#define MIDI_FPS_MSN_MASK           0x06

/* These time codes are in a specific order for FORWARD time encoding. */
static midi_time_code_type_t const kMidiTimeCodeTypes[] = {
  MIDI_FRAME_COUNT_LSN,   MIDI_FRAME_COUNT_MSN,
  MIDI_SECONDS_COUNT_LSN, MIDI_SECONDS_COUNT_MSN,
  MIDI_MINUTES_COUNT_LSN, MIDI_MINUTES_COUNT_MSN,
  MIDI_HOURS_COUNT_LSN,   MIDI_HOURS_COUNT_MSN
};
static size_t const kMidiTimeCodeTypeCount = sizeof(kMidiTimeCodeTypes);

/*
 *  MIDI Time Code.
 */
#define MidiIsValidTimeCodeType(type) (!((type) & 0x8F))
#define MidiIsValidTimeCodeValue(value) (!((value) & 0xF0))

#define MidiTimeCodeDataToType(type) (type & MIDI_TIME_CODE_TYPE_MASK)
#define MidiTimeCodeDataToValue(value) (value & MIDI_TIME_CODE_VALUE_MASK)
#define MidiTimeCodeDataToFrameRate(value) (value & MIDI_FPS_MSN_MASK)

static bool_t MidiIsValidTimeCodePair(
    midi_time_code_type_t type, uint8_t value) {
  if (!MidiIsValidTimeCodeType(type) || !MidiIsValidTimeCodeValue(value))
      return false;
  uint8_t mask = 0;
  switch (type) {
    case MIDI_FRAME_COUNT_LSN:
      mask = MIDI_FRAME_COUNT_LSN_MASK;
      break;
    case MIDI_FRAME_COUNT_MSN:
      mask = MIDI_FRAME_COUNT_MSN_MASK;
      break;
    case MIDI_SECONDS_COUNT_LSN:
      mask = MIDI_SECONDS_COUNT_LSN_MASK;
      break;
    case MIDI_SECONDS_COUNT_MSN:
      mask = MIDI_SECONDS_COUNT_MSN_MASK;
      break;
    case MIDI_MINUTES_COUNT_LSN:
      mask = MIDI_MINUTES_COUNT_LSN_MASK;
      break;
    case MIDI_MINUTES_COUNT_MSN:
      mask = MIDI_MINUTES_COUNT_MSN_MASK;
      break;
    case MIDI_HOURS_COUNT_LSN:
      mask = MIDI_HOURS_COUNT_LSN_MASK;
      break;
    case MIDI_HOURS_COUNT_MSN:
      mask = MIDI_HOURS_COUNT_MSN_MASK | MIDI_FPS_MSN_MASK;
      break;
    default:
      /* WUT? */
      return false;
  }
  return (value & mask) == value;
}

bool_t MidiIsValidTimeCode(midi_time_code_t const  *time_code) {
  if (time_code == NULL) return false;
  return MidiIsValidTimeCodePair(time_code->type, time_code->value);
}

bool_t MidiInitializeTimeCode(
    midi_time_code_t *time_code, uint8_t type, uint8_t value) {
  if (time_code == NULL) return false;
  if (!MidiIsValidTimeCodePair(type, value)) return false;
  memset(time_code, 0, sizeof(midi_time_code_t));
  time_code->type = type;
  time_code->value = value;
  return true;
}

bool_t MidiSerializeTimeCode(
    midi_time_code_t const *time_code, uint8_t *data) {
  if (time_code == NULL || data == NULL) return false;
  if (!MidiIsValidTimeCodePair(time_code->type, time_code->value)) return false;
  *data = (time_code->type & MIDI_TIME_CODE_TYPE_MASK)
        | (time_code->value & MIDI_TIME_CODE_VALUE_MASK);
  return true;
}

bool_t MidiDeserializeTimeCode(
    midi_time_code_t *time_code, uint8_t data) {
  if (time_code == NULL) return false;
  if (!MidiIsDataByte(data)) return false;
  time_code->type = MidiTimeCodeDataToType(data);
  time_code->value = MidiTimeCodeDataToValue(data);
  switch (time_code->type) {
    case MIDI_FRAME_COUNT_LSN:
      time_code->value &= MIDI_FRAME_COUNT_LSN_MASK;
      break;
    case MIDI_FRAME_COUNT_MSN:
      time_code->value &= MIDI_FRAME_COUNT_MSN_MASK;
      break;
    case MIDI_SECONDS_COUNT_LSN:
      time_code->value &= MIDI_SECONDS_COUNT_LSN_MASK;
      break;
    case MIDI_SECONDS_COUNT_MSN:
      time_code->value &= MIDI_SECONDS_COUNT_MSN_MASK;
      break;
    case MIDI_MINUTES_COUNT_LSN:
      time_code->value &= MIDI_MINUTES_COUNT_LSN_MASK;
      break;
    case MIDI_MINUTES_COUNT_MSN:
      time_code->value &= MIDI_MINUTES_COUNT_MSN_MASK;
      break;
    case MIDI_HOURS_COUNT_LSN:
      time_code->value &= MIDI_HOURS_COUNT_LSN_MASK;
      break;
    case MIDI_HOURS_COUNT_MSN:
      time_code->value &= (MIDI_HOURS_COUNT_MSN_MASK | MIDI_FPS_MSN_MASK);
      break;
    default:
      return false;
  }
  return true;
}

/*
 *  MIDI Time.
 */
#define MIDI_FRAME_COUNT_MAX        29
#define MIDI_SECONDS_COUNT_MAX      59
#define MIDI_MINUTES_COUNT_MAX      59
#define MIDI_HOURS_COUNT_MAX        23
#define MIDI_FPS_MASK               0x60

#define MidiIsValidTimeFrame(frame) \
    ((frame) <= MIDI_FRAME_COUNT_MAX)
#define MidiIsValidTimeSeconds(seconds) \
    ((seconds) <= MIDI_SECONDS_COUNT_MAX)
#define MidiIsValidTimeMinutes(minutes) \
    ((minutes) <= MIDI_MINUTES_COUNT_MAX)
#define MidiIsValidTimeHours(hours) \
    ((hours) <= MIDI_HOURS_COUNT_MAX)
#define MidiIsValidTimeFps(fps) \
    (((fps) & MIDI_FPS_MASK) == (fps))

bool_t MidiIsValidTime(midi_time_t const *time) {
  if (time == NULL) return false;
  return MidiIsValidTimeFrame(time->frame)
      && MidiIsValidTimeSeconds(time->seconds)
      && MidiIsValidTimeMinutes(time->minutes)
      && MidiIsValidTimeHours(time->hours)
      && MidiIsValidTimeFps(time->fps);
}

bool_t MidiInitializeTime(midi_time_t *time) {
  if (time == NULL) return false;
  memset(time, 0, sizeof(midi_time_t));
  return true;
}

static uint8_t MidiFpsValue(uint8_t fps) {
  switch (fps) {
    case MIDI_24_FPS: return 24;
    case MIDI_25_FPS: return 25;
    case MIDI_30_FPS_NON_DROP:
    case MIDI_30_FPS_DROP_FRAME: return 30;
  }
  return 0;
}

bool_t MidiUpdateTime(midi_time_t *time, midi_time_code_t const *time_code) {
  if (!MidiIsValidTime(time) || !MidiIsValidTimeCode(time_code)) return false;
  switch (time_code->type) {
    case MIDI_FRAME_COUNT_LSN: {
      uint8_t const frame = (time->frame & (MIDI_FRAME_COUNT_MSN_MASK << 4))
                          | (time_code->value & MIDI_FRAME_COUNT_LSN_MASK);
      if (!MidiIsValidTimeFrame(frame)) return false;
      time->frame = frame;
    } break;
    case MIDI_FRAME_COUNT_MSN: {
      uint8_t const frame =
          ((time_code->value & MIDI_FRAME_COUNT_MSN_MASK) << 4)
          | (time->frame & MIDI_FRAME_COUNT_LSN_MASK);
      if (!MidiIsValidTimeFrame(frame)) return false;
      time->frame = frame;
    } break;
    case MIDI_SECONDS_COUNT_LSN: {
      uint8_t const seconds = (time->seconds & (MIDI_SECONDS_COUNT_MSN_MASK << 4))
                            | (time_code->value & MIDI_SECONDS_COUNT_LSN_MASK);
      if (!MidiIsValidTimeSeconds(seconds)) return false;
      time->seconds = seconds;
    } break;
    case MIDI_SECONDS_COUNT_MSN: {
      uint8_t const seconds =
          ((time_code->value & MIDI_SECONDS_COUNT_MSN_MASK) << 4)
          | (time->seconds & MIDI_SECONDS_COUNT_LSN_MASK);
      if (!MidiIsValidTimeSeconds(seconds)) return false;
      time->seconds = seconds;
    } break;
    case MIDI_MINUTES_COUNT_LSN: {
      uint8_t const minutes =
          (time->minutes & (MIDI_MINUTES_COUNT_MSN_MASK << 4))
          | (time_code->value & MIDI_MINUTES_COUNT_LSN_MASK);
      if (!MidiIsValidTimeMinutes(minutes)) return false;
      time->minutes = minutes;
    } break;
    case MIDI_MINUTES_COUNT_MSN: {
      uint8_t const minutes =
          ((time_code->value & MIDI_MINUTES_COUNT_MSN_MASK) << 4)
          | (time->minutes & MIDI_MINUTES_COUNT_LSN_MASK);
      if (!MidiIsValidTimeMinutes(minutes)) return false;
      time->minutes = minutes;
    } break;
    case MIDI_HOURS_COUNT_LSN: {
      uint8_t const hours =
          (time->hours & (MIDI_HOURS_COUNT_MSN_MASK << 4))
          | (time_code->value & MIDI_HOURS_COUNT_LSN_MASK);
      if (!MidiIsValidTimeHours(hours)) return false;
      time->hours = hours;
    } break;
    case MIDI_HOURS_COUNT_MSN: {
      uint8_t const hours =
          ((time_code->value & MIDI_HOURS_COUNT_MSN_MASK) << 4)
          | (time->hours & MIDI_HOURS_COUNT_LSN_MASK);
      uint8_t const fps = ((time_code->value & MIDI_FPS_MSN_MASK) << 4);
      if (!MidiIsValidTimeHours(hours) || !MidiIsValidTimeFps(fps))
        return false;
      time->hours = hours;
      /* Handle frame rolling. */
      time->fps = fps;
      if (time->frame >= MidiFpsValue(fps)) {
        time->frame = 0;
      }
    } break;
    default:
      /* WUT? */
      return false;
  }
  return true;
}  /* MidiUpdateTime */

bool_t MidiExtractTimeCode(
    midi_time_t const *time, midi_time_code_type_t type,
    midi_time_code_t *time_code) {
  if (!MidiIsValidTime(time) || !MidiIsValidTimeCodeType(type)) return false;
  if (time_code == NULL) return false;
  time_code->type = type;
  switch (type) {
    case MIDI_FRAME_COUNT_LSN:
      time_code->value = time->frame & MIDI_FRAME_COUNT_LSN_MASK;
      break;
    case MIDI_FRAME_COUNT_MSN:
      time_code->value = (time->frame >> 4) & MIDI_FRAME_COUNT_MSN_MASK;
      break;
    case MIDI_SECONDS_COUNT_LSN:
      time_code->value = time->seconds & MIDI_SECONDS_COUNT_LSN_MASK;
      break;
    case MIDI_SECONDS_COUNT_MSN:
      time_code->value = (time->seconds >> 4) & MIDI_SECONDS_COUNT_MSN_MASK;
      break;
    case MIDI_MINUTES_COUNT_LSN:
      time_code->value = time->minutes & MIDI_MINUTES_COUNT_LSN_MASK;
      break;
    case MIDI_MINUTES_COUNT_MSN:
      time_code->value = (time->minutes >> 4) & MIDI_MINUTES_COUNT_MSN_MASK;
      break;
    case MIDI_HOURS_COUNT_LSN:
      time_code->value = time->hours & MIDI_HOURS_COUNT_LSN_MASK;
      break;
    case MIDI_HOURS_COUNT_MSN:
      time_code->value = ((time->hours >> 4) & MIDI_HOURS_COUNT_MSN_MASK)
                       | ((time->fps >> 4) & MIDI_FPS_MSN_MASK);
      break;
    default:
      /* WUT? */
      return false;
  }
  return true;
}

size_t MidiSerializeTime(
    midi_time_t const *time, bool_t backwards, uint8_t *data, size_t data_size) {
  if (data ==  NULL) return 0;
  if (!MidiIsValidTime(time)) return 0;
  /* Time code index, data index, data used (if fully serialize) */
  size_t tci = 0, di = 0, data_used = 0;
  while (tci < kMidiTimeCodeTypeCount) {
    midi_time_code_type_t const type = backwards
        ? kMidiTimeCodeTypes[kMidiTimeCodeTypeCount - tci - 1]
        : kMidiTimeCodeTypes[tci];
    ++tci;
    midi_time_code_t time_code;
    if (!MidiExtractTimeCode(time, type, &time_code)) {
      /* WUT? */
      return 0;
    }
    if (data_used < data_size) {
      if (!MidiSerializeTimeCode(&time_code, &data[di++])) {
        /* WUT? */
        return 0;
      }
    }
    ++data_used;
  }
  return data_used;
}

bool_t MidiIncrementTimeFrame(midi_time_t *time) {
  if (!MidiIsValidTime(time)) return false;
  ++time->frame;
  if (time->frame >= MidiFpsValue(time->fps)) {
    time->frame = 0;
    return MidiIncrementTimeSeconds(time);
  }
  return true;
}

bool_t MidiIncrementTimeSeconds(midi_time_t *time) {
  if (!MidiIsValidTime(time)) return false;
  ++time->seconds;
  if (time->seconds > MIDI_SECONDS_COUNT_MAX) {
    time->seconds = 0;
    return MidiIncrementTimeMinutes(time);
  }
  return true;
}

bool_t MidiIncrementTimeMinutes(midi_time_t *time) {
  if (!MidiIsValidTime(time)) return false;
  ++time->minutes;
  if (time->minutes > MIDI_MINUTES_COUNT_MAX) {
    time->minutes = 0;
    return MidiIncrementTimeHours(time);
  }
  return true;
}

bool_t MidiIncrementTimeHours(midi_time_t *time) {
  if (!MidiIsValidTime(time)) return false;
  ++time->hours;
  if (time->hours > MIDI_HOURS_COUNT_MAX) {
    time->hours = 0;
  }
  return true;
}
