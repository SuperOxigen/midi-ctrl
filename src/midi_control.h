/*
 * MIDI Controller - MIDI Control Change
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#ifndef _MIDI_CONTROL_H_
#define _MIDI_CONTROL_H_

#include "base.h"
#include "midi_bytes.h"

C_SECTION_BEGIN;

typedef uint8_t midi_control_number_t;

/* Data from a control change message. */
typedef struct {
  midi_control_number_t number;
  uint8_t value;
} midi_control_change_t;

/* Validity checks. */
#define MidiIsValidControlNumber(number) MidiIsDataByte(number)
#define MidiIsValidControlValue(value) MidiIsDataByte(value)
bool_t MidiIsValidControlChange(midi_control_change_t const *control_change);

/* Checks if the control number is defined in MIDI standard. */
bool_t MidiControlNumberIsDefined(midi_control_number_t control_number);

/* Inintializes control so long as the parameters are valid. */
bool_t MidiControlChange(
    midi_control_change_t *control_change,
    midi_control_number_t number, uint8_t value);

C_SECTION_END;

#endif  /* _MIDI_CONTROL_H_ */
