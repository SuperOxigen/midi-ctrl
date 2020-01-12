/*
 * MIDI Controller - MIDI Bytes Info
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#ifndef _MIDI_BYTES_H_
#define _MIDI_BYTES_H_

#include "base.h"

C_SECTION_BEGIN;

#define MidiIsStatusByte(byte) ((byte) & 0x80)    /* uint8_t -> bool_t */

/* MIDI data byte (7-bit) */
#define MidiIsDataByte(byte) (!((byte) & 0x80))   /* uint8_t -> bool_t */

/* MIDI data word (14-bit) */
#define MidiIsDataWord(word) (!((word) & 0xC000))  /* uint16_t -> bool_t */
uint8_t MidiGetDataWordMsb(uint16_t word);
uint8_t MidiGetDataWordLsb(uint16_t word);
bool_t MidiSetDataWordMsb(uint16_t *word, uint8_t msb);
bool_t MidiSetDataWordLsb(uint16_t *word, uint8_t lsb);

C_SECTION_END;

#endif  /* _MIDI_BYTES_H_ */
