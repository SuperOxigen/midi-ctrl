/*
 * MIDI Controller - MIDI Program Numbers.
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#ifndef _MIDI_PROGRAM_H_
#define _MIDI_PROGRAM_H_

#include "base.h"
#include "midi_bytes.h"

C_SECTION_BEGIN;

/* Standard MIDI program number "instrument" fall into the following
 * family groups. */
typedef enum {
  MIDI_FAMILY_NONE,
  MIDI_FAMILY_PIANO = 1,
  MIDI_FAMILY_CHROMATIC_PERCUSSION = 2,
  MIDI_FAMILY_ORGAN = 3,
  MIDI_FAMILY_GUITAR = 4,
  MIDI_FAMILY_BASE = 5,
  MIDI_FAMILY_STRINGS = 6,
  MIDI_FAMILY_ENSEMBLE = 7,
  MIDI_FAMILY_BRASS = 8,
  MIDI_FAMILY_REED = 9,
  MIDI_FAMILY_PIPE = 10,
  MIDI_FAMILY_SYNTH_LEAD = 11,
  MIDI_FAMILY_SYNTH_PAD = 12,
  MIDI_FAMILY_SYNTH_EFFECTS = 13,
  MIDI_FAMILY_ETHNIC = 14,
  MIDI_FAMILY_PERCUSSIVE = 15,
  MIDI_FAMILY_SOUND_EFFECTS = 16,
  MIDI_FAMILY_MAX
} midi_program_family_t;

typedef uint8_t midi_program_number_t;
#define MidiIsValidProgramNumber(number) MidiIsDataByte(number)

/* Converts MIDI program numbers to their instrument family. */
midi_program_family_t MidiProgramNumberToFamily(
    midi_program_number_t program_number);

C_SECTION_END;

#endif  /* _MIDI_PROGRAM_H_ */
