/*
 * MIDI Controller - MIDI Control Change
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#include "midi_control.h"

bool_t MidiControlNumberIsDefined(midi_control_number_t control_number) {
  if (!MidiIsValidControlNumber(control_number)) return false;
  /* These values are taken from a list of known "undefined" values. */
  if (MidiControlNumberIsUndefinedSingleByte(control_number)) return false;
  /* MSB-LSB checks. */
  if (control_number == 0x03 || control_number == 0x23) return false;
  if (control_number == 0x09 || control_number == 0x29) return false;
  if (control_number == 0x0E || control_number == 0x2E) return false;
  if (control_number == 0x0F || control_number == 0x2F) return false;
  if (control_number >= 0x14 && control_number <= 0x1F) return false;
  if (control_number >= 0x34 && control_number <= 0x3F) return false;
  if (control_number >= 0x55 && control_number <= 0x5A) return false;
  return true;
}

bool_t MidiIsValidControlChange(midi_control_change_t const *control_change) {
  if (control_change == NULL) return false;
  return MidiIsValidControlNumber(control_change->number)
      && MidiIsValidControlValue(control_change->value);
}

bool_t MidiControlChange(
    midi_control_change_t *control_change,
    midi_control_number_t number, uint8_t value) {
  if (control_change == NULL) return false;
  if (!MidiIsValidControlNumber(number) ||
      !MidiIsValidControlValue(value)) return false;
  *control_change = (midi_control_change_t) {
    .number = number,
    .value = value
  };
  return true;
}
