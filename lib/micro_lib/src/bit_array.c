/*
 * MIDI Controller - Bit Array.
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#include <string.h>

#include "bit_array.h"

bool_t BitArrayInitialize(
    bit_array_t *array, uint8_t *buffer, size_t buffer_size) {
  if (!BitArrayInitializeAsIs(array, buffer, buffer_size))
    return false;
  memset(buffer, 0, buffer_size);
  return true;
}

bool_t BitArrayInitializeAsIs(
    bit_array_t *array, uint8_t *buffer, size_t buffer_size) {
  if (array == NULL || buffer == NULL) return false;
  if (buffer_size == 0 || buffer_size > BIT_ARRAY_MAX_BUFFER_SIZE)
    return false;
  *array = (bit_array_t) {
    .buffer = buffer,
    .bit_size = (buffer_size << 3)
  };
  return true;
}

inline static bool_t BitArrayInBound(bit_array_t const *array, size_t index) {
  if (array == NULL || array->buffer == NULL) return false;
  return index < array->bit_size;
}

bool_t BitArrayClear(bit_array_t *array) {
  if (array == NULL || array->buffer == NULL) return false;
  memset(array->buffer, 0, (array->bit_size + 7) / 8);
  return true;
}

bool_t BitArraySetBit(bit_array_t *array, size_t index) {
  if (!BitArrayInBound(array, index)) return false;
  size_t const major_index = index >> 3;
  size_t const minor_index = index & 0x7;
  array->buffer[major_index] |= (1 << minor_index);
  return true;
}

bool_t BitArrayClearBit(bit_array_t *array, size_t index) {
  if (!BitArrayInBound(array, index)) return false;
  size_t const major_index = index >> 3;
  size_t const minor_index = index & 0x7;
  array->buffer[major_index] &= ~(1 << minor_index);
  return true;
}

bool_t BitArrayTestBit(bit_array_t const *array, size_t index) {
  if (!BitArrayInBound(array, index)) return false;
  size_t const major_index = index >> 3;
  size_t const minor_index = index & 0x7;
  return (array->buffer[major_index] & (1 << minor_index)) != 0;
}

bool_t BitArrayNone(bit_array_t const *array) {
  if (array == NULL || array->buffer == NULL) return false;
  for (size_t i = 0; i < (array->bit_size >> 3); ++i) {
    if (array->buffer[i] != 0x00) return false;
  }
  return true;
}

bool_t BitArrayAny(bit_array_t const *array) {
  if (array == NULL || array->buffer == NULL) return false;
  for (size_t i = 0; i < (array->bit_size >> 3); ++i) {
    if (array->buffer[i] != 0x00) return true;
  }
  return false;
}

bool_t BitArrayAll(bit_array_t const *array) {
  if (array == NULL || array->buffer == NULL) return false;
  for (size_t i = 0; i < (array->bit_size >> 3); ++i) {
    if (array->buffer[i] != 0xFF) return false;
  }
  return true;
}
