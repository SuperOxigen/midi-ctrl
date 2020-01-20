/*
 * MIDI Controller - MIDI Bytes Info
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#include "midi_bytes.h"

#define MIDI_WORD_LSB_MASK   0x007F
#define MIDI_WORD_MSB_MASK   0x3F80
/* Number of bytes to shift. */
#define MIDI_WORD_MSB_OFFSET      7

uint16_t MidiDataWordFromBytes(uint8_t msb, uint8_t lsb) {
  if (!MidiIsDataByte(msb) || !MidiIsDataByte(lsb)) return 0;
  return ((msb << MIDI_WORD_MSB_OFFSET) & MIDI_WORD_MSB_MASK) |
         (lsb & MIDI_WORD_LSB_MASK);
}

uint8_t MidiGetDataWordMsb(uint16_t word) {
   return ((word & MIDI_WORD_MSB_MASK) >> MIDI_WORD_MSB_OFFSET)
        & MIDI_WORD_LSB_MASK;
}

uint8_t MidiGetDataWordLsb(uint16_t word) {
  return word & MIDI_WORD_LSB_MASK;
}

bool_t MidiSetDataWordMsb(uint16_t *word, uint8_t msb) {
  if (word == NULL) return false;
  if (!MidiIsDataByte(msb)) return false;
  *word = (*word & MIDI_WORD_LSB_MASK) |
      ((((uint16_t) msb) << MIDI_WORD_MSB_OFFSET) & MIDI_WORD_MSB_MASK);
  return true;
}

bool_t MidiSetDataWordLsb(uint16_t *word, uint8_t lsb) {
  if (word == NULL) return false;
  if (!MidiIsDataByte(lsb)) return false;
  *word = (*word & MIDI_WORD_MSB_MASK) |
      (((uint16_t) lsb) & MIDI_WORD_LSB_MASK);
  return true;
}
