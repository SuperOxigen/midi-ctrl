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

/* MIDI data tribyte (21-bits) */
#define MidiIsDataTriByte(tri) \
  (!((tri) & 0xFFE00000))  /* uint32_t -> bool_t */

/* MIDI data quad */
#define MidiIsDataQuadByte(quad) \
  (!((quad) & 0xF0000000))  /* uint32_t -> bool_t */

uint16_t MidiDataWordFromBytes(uint8_t msb, uint8_t lsb);
uint8_t MidiGetDataWordMsb(uint16_t word);
uint8_t MidiGetDataWordLsb(uint16_t word);
bool_t MidiSetDataWordMsb(uint16_t *word, uint8_t msb);
bool_t MidiSetDataWordLsb(uint16_t *word, uint8_t lsb);

uint32_t MidiDataTriByteFromBytes(uint8_t msb, uint8_t mid, uint8_t lsb);
uint8_t MidiGetDataTriByteMsb(uint32_t tri);
uint8_t MidiGetDataTriByteMid(uint32_t tri);
uint8_t MidiGetDataTriByteLsb(uint32_t tri);

uint32_t MidiDataQuadByteFromBytes(
  uint8_t msb, uint8_t mmid, uint8_t lmid, uint8_t lsb);
uint8_t MidiGetDataQuadByteMsb(uint32_t quad);
uint8_t MidiGetDataQuadByteMMid(uint32_t quad);
uint8_t MidiGetDataQuadByteLMid(uint32_t quad);
uint8_t MidiGetDataQuadByteLsb(uint32_t quad);

C_SECTION_END;

#endif  /* _MIDI_BYTES_H_ */
