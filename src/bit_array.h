/*
 * MIDI Controller - Bit Array.
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#ifndef _BIT_ARRAY_H_
#define _BIT_ARRAY_H_

#include "base.h"

C_SECTION_BEGIN;

#define BIT_ARRAY_MAX_BUFFER_SIZE 128

/* A special array for storing bits.  The actual buffer is provided by
 * the user and is expected to exists for the life time of the
 * bit_array_t. */
typedef struct {
  uint8_t *buffer;
  size_t bit_size;
} bit_array_t;

/* |buffer_size| is in bytes. */
bool_t BitArrayInitialize(
  bit_array_t *array, uint8_t *buffer, size_t buffer_size);

bool_t BitArrayClear(bit_array_t *array);

/* Single bit. */
bool_t BitArraySetBit(bit_array_t *array, size_t index);
bool_t BitArrayClearBit(bit_array_t *array, size_t index);
bool_t BitArrayTestBit(bit_array_t const *array, size_t index);

/* Set testers. */
bool_t BitArrayNone(bit_array_t const *array);
bool_t BitArrayAny(bit_array_t const *array);
bool_t BitArrayAll(bit_array_t const *array);

C_SECTION_END;

#endif  /* _BIT_ARRAY_H_ */
