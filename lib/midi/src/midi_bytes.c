/*
 * MIDI Controller - MIDI Bytes Info
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#include "midi_bytes.h"

#define MIDI_BYTE_MASK   0x7F
#define MIDI_WORD_MASK   0x3F80
/* Number of bytes to shift. */
#define MIDI_WORD_OFFSET      7

#define MIDI_TRI_BYTE_MASK     0x001FC000
#define MIDI_TRI_BYTE_OFFSET   14

#define MIDI_QUAD_BYTE_MASK     0x0FE00000
#define MIDI_QUAD_BYTE_OFFSET   21

uint16_t MidiDataWordFromBytes(uint8_t msb, uint8_t lsb) {
  if (!MidiIsDataByte(msb) || !MidiIsDataByte(lsb)) return 0;
  return ((msb << MIDI_WORD_OFFSET) & MIDI_WORD_MASK) |
         (lsb & MIDI_BYTE_MASK);
}

uint8_t MidiGetDataWordMsb(uint16_t word) {
   return ((word & MIDI_WORD_MASK) >> MIDI_WORD_OFFSET)
        & MIDI_BYTE_MASK;
}

uint8_t MidiGetDataWordLsb(uint16_t word) {
  return word & MIDI_BYTE_MASK;
}

bool_t MidiSetDataWordMsb(uint16_t *word, uint8_t msb) {
  if (word == NULL) return false;
  if (!MidiIsDataByte(msb)) return false;
  *word = (*word & MIDI_BYTE_MASK) |
      ((((uint16_t) msb) << MIDI_WORD_OFFSET) & MIDI_WORD_MASK);
  return true;
}

bool_t MidiSetDataWordLsb(uint16_t *word, uint8_t lsb) {
  if (word == NULL) return false;
  if (!MidiIsDataByte(lsb)) return false;
  *word = (*word & MIDI_WORD_MASK) |
      (((uint16_t) lsb) & MIDI_BYTE_MASK);
  return true;
}

uint32_t MidiDataTriByteFromBytes(uint8_t msb, uint8_t mid, uint8_t lsb) {
  if (!MidiIsDataByte(msb) || !MidiIsDataByte(mid) ||
      !MidiIsDataByte(lsb)) return 0;
  return ((((uint32_t) msb) << MIDI_TRI_BYTE_OFFSET) & MIDI_TRI_BYTE_MASK) |
         ((((uint32_t) mid) << MIDI_WORD_OFFSET) & MIDI_WORD_MASK) |
         (((uint32_t) lsb) & MIDI_BYTE_MASK);
}
uint8_t MidiGetDataTriByteMsb(uint32_t tri) {
  return (tri >> MIDI_TRI_BYTE_OFFSET) & MIDI_BYTE_MASK;
}

uint8_t MidiGetDataTriByteMid(uint32_t tri) {
  return (tri >> MIDI_WORD_OFFSET) & MIDI_BYTE_MASK;
}

uint8_t MidiGetDataTriByteLsb(uint32_t tri) {
  return tri & MIDI_BYTE_MASK;
}

bool_t MidiSerializeTriByte(uint32_t tri, uint8_t *data) {
  if (!MidiIsDataTriByte(tri)) return false;
  if (data == NULL) return false;
  data[0] = tri & MIDI_BYTE_MASK;
  data[1] = (tri >> MIDI_WORD_OFFSET) & MIDI_BYTE_MASK;
  data[2] = (tri >> MIDI_TRI_BYTE_OFFSET) & MIDI_BYTE_MASK;
  return true;
}

bool_t MidiDeserializeTriByte(uint8_t const *data, uint32_t *tri) {
  if (data == NULL || tri == NULL) return false;
  if (!MidiIsDataByte(data[2]) || !MidiIsDataByte(data[1]) ||
      !MidiIsDataByte(data[0])) return 0;
  *tri = ((((uint32_t) data[2]) << MIDI_TRI_BYTE_OFFSET) &
            MIDI_TRI_BYTE_MASK) |
         ((((uint32_t) data[1]) << MIDI_WORD_OFFSET) &
            MIDI_WORD_MASK) |
         (((uint32_t) data[0]) & MIDI_BYTE_MASK);
  return true;
}

uint32_t MidiDataQuadByteFromBytes(
    uint8_t msb, uint8_t mmid, uint8_t lmid, uint8_t lsb) {
  if (!MidiIsDataByte(msb) || !MidiIsDataByte(mmid) ||
      !MidiIsDataByte(lmid) || !MidiIsDataByte(lsb)) return 0;
  return ((((uint32_t) msb) << MIDI_QUAD_BYTE_OFFSET) & MIDI_QUAD_BYTE_MASK) |
         ((((uint32_t) mmid) << MIDI_TRI_BYTE_OFFSET) & MIDI_TRI_BYTE_MASK) |
         ((((uint32_t) lmid) << MIDI_WORD_OFFSET) & MIDI_WORD_MASK) |
         (((uint32_t) lsb) & MIDI_BYTE_MASK);
}

uint8_t MidiGetDataQuadByteMsb(uint32_t quad) {
  return (quad >> MIDI_QUAD_BYTE_OFFSET) & MIDI_BYTE_MASK;
}

uint8_t MidiGetDataQuadByteMMid(uint32_t quad) {
  return (quad >> MIDI_TRI_BYTE_OFFSET) & MIDI_BYTE_MASK;
}

uint8_t MidiGetDataQuadByteLMid(uint32_t quad) {
  return (quad >> MIDI_WORD_OFFSET) & MIDI_BYTE_MASK;
}

uint8_t MidiGetDataQuadByteLsb(uint32_t quad) {
  return quad & MIDI_BYTE_MASK;
}

bool_t MidiSerializeQuadByte(uint32_t quad, uint8_t *data) {
  if (!MidiIsDataQuadByte(quad)) return false;
  if (data == NULL) return false;
  data[0] = quad & MIDI_BYTE_MASK;
  data[1] = (quad >> MIDI_WORD_OFFSET) & MIDI_BYTE_MASK;
  data[2] = (quad >> MIDI_TRI_BYTE_OFFSET) & MIDI_BYTE_MASK;
  data[3] = (quad >> MIDI_QUAD_BYTE_OFFSET) & MIDI_BYTE_MASK;
  return true;
}

bool_t MidiDeserializeQuadByte(uint8_t const *data, uint32_t *quad) {
  if (data == NULL || quad == NULL) return false;
  if (!MidiIsDataByte(data[3]) || !MidiIsDataByte(data[2]) ||
      !MidiIsDataByte(data[1]) || !MidiIsDataByte(data[0])) return 0;
  *quad = ((((uint32_t) data[3]) << MIDI_QUAD_BYTE_OFFSET) &
            MIDI_QUAD_BYTE_MASK) |
          ((((uint32_t) data[2]) << MIDI_TRI_BYTE_OFFSET) &
            MIDI_TRI_BYTE_MASK) |
          ((((uint32_t) data[1]) << MIDI_WORD_OFFSET) &
            MIDI_WORD_MASK) |
          (((uint32_t) data[0]) & MIDI_BYTE_MASK);
  return true;
}

bool_t MidiIsDataArray(uint8_t const *data, size_t data_size) {
  if (data == NULL || data_size == 0) return false;
  for (size_t i = 0; i < data_size; ++i) {
    if (!MidiIsDataByte(data[i])) return false;
  }
  return true;
}
