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

/*
 * The control number type represents both controller numbers and
 * channel mode numbers.  Sub-ranges of controller numbers have
 * specific meaning/function.  Not all controller numbers are defined
 * in MIDI 1.0, but can be used by device manufacturers at their
 * discretion.
 *
 * Ranges:
 *  0 to 119   - Controller Numbers
 *    0 to 31    - MSB of continuous controller data
 *    32 to 63   - LSB for controllers from 0 to 31
 *    64 to 95   - Single-byte controllers
 *    96 to 101  - Increment/decrement and parameter numbers
 *    102 to 119 - Undefined single-byte controllers
 * 120 to 127  - Channel Mode
 *    120        - All sound off
 *    121        - Reset all controllers
 *    122        - Local control (value is on (0x7F) or off (0x00))
 *    123        - All notes off
 *    124/125    - Omni mode off/on
 *    126/127    - Mono/poly mode (mono mode value is number of channels)
 */
typedef uint8_t midi_control_number_t;
#define MidiIsValidControlNumber(number) MidiIsDataByte(number)

/* Ranges of controller numbers. */
#define MidiControlNumberIsController(control_number) ((control_number) < 0x78)
#define MidiControlNumberIsMsb(control_number) ((control_number) < 0x20)
#define MidiControlNumberIsLsb(control_number) \
  ((control_number) >= 0x20 && (control_number) < 0x40)
#define MidiControlNumberIsSingleByte(control_number) \
  ((control_number) >= 0x40 && (control_number) < 0x60)
#define MidiControlNumberIsIncDec(control_number) \
  ((control_number) >= 0x60 && (control_number) < 0x66)
#define MidiControlNumberIsUndefinedSingleByte(control_number) \
  ((control_number) >= 0x66 && (control_number) < 0x78)
#define MidiControlNumberIsChannelMode(control_number) \
  ((control_number) >= 0x78 && (control_number) < 0x80)

/* Checks if the control number is defined in MIDI standard. */
bool_t MidiControlNumberIsDefined(midi_control_number_t control_number);

/* Most control change values just need to be a MIDI data byte, however,
 * channel mode messages have specific values/ranges that are allowed. */
bool_t MidiIsValidControlChangeValue(
    midi_control_number_t number, uint8_t value);

/* Data from a control change message. */
typedef struct {
  midi_control_number_t number;
  uint8_t value;
} midi_control_change_t;

bool_t MidiIsValidControlChange(midi_control_change_t const *control_change);

/* Inintializes control so long as the parameters are valid. */
bool_t MidiControlChange(
  midi_control_change_t *control_change,
  midi_control_number_t number, uint8_t value);

C_SECTION_END;

#endif  /* _MIDI_CONTROL_H_ */
