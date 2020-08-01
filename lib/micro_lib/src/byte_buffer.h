/*
 * MIDI Controller - Byte Buffer
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#ifndef _BYTE_BUFFER_H_
#define _BYTE_BUFFER_H_

#include "base.h"

C_SECTION_BEGIN;

typedef struct {
  uint8_t *data;
  size_t capacity;
  size_t size;
  size_t ptr;
} byte_buffer_t;

bool_t ByteBufferInitialize(
  byte_buffer_t *buffer, uint8_t *data, size_t capacity);

bool_t ByteBufferClear(byte_buffer_t *buffer);
size_t ByteBufferClearBytes(byte_buffer_t *buffer, size_t count);

bool_t ByteBufferIsEmpty(byte_buffer_t const *buffer);
bool_t ByteBufferIsFull(byte_buffer_t const *buffer);

bool_t ByteBufferEnqueueByte(byte_buffer_t *buffer, uint8_t byte);
size_t ByteBufferEnqueueBytes(
  byte_buffer_t *buffer, uint8_t *data, size_t count);

bool_t ByteBufferPeakByte(byte_buffer_t *buffer, uint8_t *byte);
size_t ByteBufferPeakBytes(byte_buffer_t *buffer, uint8_t *data, size_t count);

bool_t ByteBufferDequeueByte(byte_buffer_t *buffer, uint8_t *byte);
size_t ByteBufferDequeueBytes(
  byte_buffer_t *buffer, uint8_t *data, size_t count);

C_SECTION_END;

#endif  /* _BYTE_BUFFER_H_ */
