/*
 * MIDI Controller - MIDI Program Numbers.
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#include "midi_program.h"

midi_program_family_t MidiProgramNumberToFamily(
    midi_program_number_t program_number) {
  static const uint8_t kMidiProgramsPerFamily = 8;
  if (!MidiIsValidProgramNumber(program_number)) return MIDI_FAMILY_NONE;
  midi_program_family_t const family =
      (program_number / kMidiProgramsPerFamily) + 1;
  return (family < MIDI_FAMILY_MAX) ? family : MIDI_FAMILY_NONE;
}
