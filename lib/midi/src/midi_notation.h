/*
 * MIDI Controller - Music / Song Notation
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#ifndef _MIDI_NOTATION_H_
#define _MIDI_NOTATION_H_

#include "base.h"
#include "midi_bytes.h"

C_SECTION_BEGIN;

typedef uint16_t midi_song_position_t;
typedef uint8_t midi_song_number_t;

#define MidiIsValidSongPosition(position) \
  (MidiIsDataWord(position) && (((position) % 6) == 0))
#define MidiIsValidSongNumber MidiIsDataByte

C_SECTION_END;

#endif  /* _MIDI_NOTATION_H_ */
