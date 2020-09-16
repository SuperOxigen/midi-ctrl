/*
 * MIDI Controller - MIDI Control Change
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#include "midi_channel.h"
#include "midi_control.h"
#include "midi_defs.h"

bool_t MidiControlNumberIsDefined(midi_control_number_t number) {
  if (!MidiIsValidControlNumber(number)) return false;
  /* These values are taken from a list of known "undefined" values. */
  if (MidiControlNumberIsUndefinedSingleByte(number)) return false;
  /* MSB-LSB checks. */
  if (number == 0x03 || number == 0x23) return false;
  if (number == 0x09 || number == 0x29) return false;
  if (number == 0x0E || number == 0x2E) return false;
  if (number == 0x0F || number == 0x2F) return false;
  if (number >= 0x14 && number <= 0x1F) return false;
  if (number >= 0x34 && number <= 0x3F) return false;
  if (number >= 0x55 && number <= 0x5A) return false;
  return true;
}

bool_t MidiIsValidControlChangeValue(
    midi_control_number_t number, uint8_t value) {
  if (MidiControlNumberIsController(number)) {
    return MidiIsDataByte(value);
  }
  if (!MidiControlNumberIsChannelMode(number)) return false;
  if (number == MIDI_MONO_MODE_ON) {
    /* Number of channels or 0 */
    return value <= 16;
  }
  if (number == MIDI_LOCAL_CONTROL) {
    return value == MIDI_CONTROL_OFF || value == MIDI_CONTROL_ON;
  }
  /* All other channel mode numbers require a value of 0. */
  return value == 0;
}

bool_t MidiIsValidControlChange(midi_control_change_t const *control_change) {
  if (control_change == NULL) return false;
  return MidiIsValidControlChangeValue(
      control_change->number, control_change->value);
}

bool_t MidiControlChange(
    midi_control_change_t *control_change,
    midi_control_number_t number, uint8_t value) {
  if (control_change == NULL) return false;
  if (!MidiIsValidControlChangeValue(number, value)) return false;
  *control_change = (midi_control_change_t) {
    .number = number,
    .value = value
  };
  return true;
}
