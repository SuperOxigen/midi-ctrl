/*
 * MIDI Controller - MIDI User Bits.
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#include <string.h>

#include "midi_user_bits.h"

/* Raw binary group flags. */
#define MIDI_BGF0 0x01
#define MIDI_BGF1 0x02

#define MIDI_BINARY_GROUP_FLAG_MASK  0x03

#define MIDI_USER_BITS_SIZE 9

#define MidiIsValidBinaryGroupFlags(flags) \
    (((flags) & MIDI_BINARY_GROUP_FLAG_MASK) == (flags))

#define MidiIsValidUserBitsGroup(group) (((group) & 0x0F) == (group))

bool_t MidiIsValidUserBits(midi_user_bits_t const *user_bits) {
  if (user_bits == NULL) return false;
  return MidiIsValidBinaryGroupFlags(user_bits->bg_flags);
}

bool_t MidiInitializeUserBits(
    midi_user_bits_t *user_bits, uint8_t const *data, size_t data_size) {
  if (user_bits == NULL) return false;
  if (data == NULL || data_size != MIDI_USER_BITS_DATA_SIZE) return false;
  memset(user_bits, 0, sizeof(midi_user_bits_t));
  memcpy(user_bits->data, data, MIDI_USER_BITS_DATA_SIZE);
  user_bits->bg_flags = MIDI_BGF0;
  return true;
}
size_t MidiSerializeUserBits(
    midi_user_bits_t const *user_bits, uint8_t *data, size_t data_size) {
  if (data == NULL && data_size > 0) return 0;
  if (!MidiIsValidUserBits(user_bits)) return 0;
  if (data_size >= MIDI_USER_BITS_SIZE) {
    for (size_t i = 0; i < MIDI_USER_BITS_DATA_SIZE; ++i) {
      data[2 * i] =
          user_bits->data[(MIDI_USER_BITS_DATA_SIZE - 1) - i] & 0x0F;
      data[(2 * i) + 1] =
          (user_bits->data[(MIDI_USER_BITS_DATA_SIZE - 1) - i] >> 4) & 0x0F;
    }
    data[8] = user_bits->bg_flags;
  }
  return MIDI_USER_BITS_SIZE;
}

size_t MidiDeserializeUserBits(
    uint8_t const *data, size_t data_size, midi_user_bits_t *user_bits) {
  if (user_bits == NULL) return 0;
  if (data == NULL && data_size > 0) return 0;
  if (data_size >= MIDI_USER_BITS_SIZE) {
    memset(user_bits, 0, sizeof(midi_user_bits_t));
    if (!MidiIsValidBinaryGroupFlags(data[8])) return 0;
    user_bits->bg_flags = data[8];
    for (size_t i = 0; i < MIDI_USER_BITS_DATA_SIZE; ++i) {
      if (!MidiIsValidUserBitsGroup(data[2 * i])) return 0;
      if (!MidiIsValidUserBitsGroup(data[(2 * i) + 1])) return 0;
      user_bits->data[(MIDI_USER_BITS_DATA_SIZE - 1) - i] =
          data[2 * i] | ((data[(2 * i) + 1] << 4) & 0xF0);
    }
  }
  return MIDI_USER_BITS_SIZE;
}
