/*
 * MIDI Controller - MIDI General MIDI 1
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#ifndef _MIDI_GM1_H_
#define _MIDI_GM1_H_

#include "base.h"

#include "midi_program.h"

C_SECTION_BEGIN;

#define MIDI_GM1_WITH_NUMBER    0x01
#define MIDI_GM1_LOWER_CASE     0x02
#define MIDI_GM1_UPPER_CASE     0x04
#define MIDI_GM1_CAPITALIZATION 0x06

size_t MidiGetGeneralMidiProgramName(
  midi_program_number_t program, uint8_t flags, char *name, size_t name_size);

size_t MidiGetGeneralMidiPercussionName(
  uint8_t key, uint8_t flags, char *name, size_t name_size);

C_SECTION_END;

#endif  /* _MIDI_GM1_H_ */
