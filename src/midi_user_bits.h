/*
 * MIDI Controller - MIDI User Bits.
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#ifndef _MIDI_USER_BITS_H_
#define _MIDI_USER_BITS_H_

#include "base.h"

C_SECTION_BEGIN;

#define MIDI_USER_BITS_DATA_SIZE 4

typedef struct {
  uint8_t data[MIDI_USER_BITS_DATA_SIZE];
  uint8_t bg_flags;
} midi_user_bits_t;

bool_t MidiIsValidUserBits(midi_user_bits_t const *user_bits);

bool_t MidiInitializeUserBits(
  midi_user_bits_t *user_bits, uint8_t const *data, size_t data_size);
size_t MidiSerializeUserBits(
  midi_user_bits_t const *user_bits, uint8_t *data, size_t data_size);
size_t MidiDeserializeUserBits(
  uint8_t const *data, size_t data_size, midi_user_bits_t *user_bits);

C_SECTION_END;

#endif  /*  _MIDI_USER_BITS_H_ */
