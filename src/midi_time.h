/*
 * MIDI Controller - MIDI Time and Time Code
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#ifndef _MIDI_TIME_H_
#define _MIDI_TIME_H_

#include "base.h"

C_SECTION_BEGIN;

/*
 *  MIDI Time Code.
 */
typedef uint8_t midi_time_code_type_t;

typedef struct {
  midi_time_code_type_t type;
  uint8_t value;
} midi_time_code_t;

/* uint8_t -> bool_t */

bool_t MidiIsValidTimeCode(midi_time_code_t const  *time_code);

bool_t MidiInitializeTimeCode(
  midi_time_code_t *time_code, midi_time_code_type_t type, uint8_t value);

bool_t MidiSerializeTimeCode(
  midi_time_code_t const *time_code, uint8_t *data);

bool_t MidiDeserializeTimeCode(
  midi_time_code_t *time_code, uint8_t data);

/*
 *  MIDI Time Code.
 */

typedef struct {
  uint8_t frame;
  uint8_t seconds;
  uint8_t minutes;
  uint8_t hours;
  uint8_t fps;
} midi_time_t;

bool_t MidiIsValidTime(midi_time_t const *time);

bool_t MidiInitializeTime(midi_time_t *time);
bool_t MidiUpdateTime(midi_time_t *time, midi_time_code_t const *time_code);
bool_t MidiExtractTimeCode(
  midi_time_t const *time, midi_time_code_type_t type,
  midi_time_code_t *time_code);

size_t MidiSerializeTime(
  midi_time_t const *time, bool_t backwards, uint8_t *data, size_t data_size);

bool_t MidiIncrementTimeFrame(midi_time_t *time);
bool_t MidiIncrementTimeSeconds(midi_time_t *time);
bool_t MidiIncrementTimeMinutes(midi_time_t *time);
bool_t MidiIncrementTimeHours(midi_time_t *time);

C_SECTION_END;

#endif  /* _MIDI_TIME_H_ */
