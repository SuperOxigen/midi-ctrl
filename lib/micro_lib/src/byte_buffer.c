/*
 * MIDI Controller - Byte Buffer
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#include <string.h>

#include "byte_buffer.h"

static inline bool_t ByteBufferIsValid(byte_buffer_t const *buffer) {
  if (buffer == NULL) return false;
  return buffer->data != NULL
      && buffer->capacity > 0
      && buffer->size <= buffer->capacity
      && buffer->ptr < buffer->capacity;
}

bool_t ByteBufferInitialize(
    byte_buffer_t *buffer, uint8_t *data, size_t capacity) {
  if (buffer == NULL || data == NULL || capacity == 0) return false;
  *buffer = (byte_buffer_t) {
    .data = data,
    .capacity = capacity,
    .size = 0,
    .ptr = 0
  };
  return true;
}

bool_t ByteBufferClear(byte_buffer_t *buffer) {
  if (!ByteBufferIsValid(buffer)) return false;
  memset(buffer->data, 0, buffer->capacity);
  buffer->size = 0;
  buffer->ptr = 0;
  return true;
}

size_t ByteBufferClearBytes(byte_buffer_t *buffer, size_t count) {
  if (!ByteBufferIsValid(buffer)) return 0;
  size_t const to_remove = (count > buffer->size) ? buffer->size : count;
  buffer->size -= to_remove;
  if (buffer->size > 0) {
    buffer->ptr = (buffer->ptr + to_remove) % buffer->capacity;
  } else {
    buffer->ptr = 0;
  }
  return to_remove;
}

bool_t ByteBufferIsEmpty(byte_buffer_t const *buffer) {
  if (!ByteBufferIsValid(buffer)) return false;
  return buffer->size == 0;
}

bool_t ByteBufferIsFull(byte_buffer_t const *buffer) {
  if (!ByteBufferIsValid(buffer)) return false;
  return buffer->size == buffer->capacity;
}

bool_t ByteBufferEnqueueByte(byte_buffer_t *buffer, uint8_t byte) {
  if (!ByteBufferIsValid(buffer)) return false;
  if (buffer->size == buffer->capacity) return false;
  buffer->data[(buffer->ptr + buffer->size) % buffer->capacity] = byte;
  ++buffer->size;
  return true;
}

size_t ByteBufferEnqueueBytes(
    byte_buffer_t *buffer, uint8_t const *data, size_t count) {
  if (!ByteBufferIsValid(buffer)) return 0;
  if (data == NULL || count == 0) return 0;
  size_t const to_copy = (count > (buffer->capacity - buffer->size))
      ? (buffer->capacity - buffer->size) : count;
  size_t const end_idx = (buffer->size + buffer->ptr) % buffer->capacity;
  size_t i = 0;
  while (end_idx + i < buffer->capacity && i < to_copy) {
    buffer->data[end_idx + i] = data[i];
    ++i;
  }
  size_t const wrap_offset = buffer->capacity - end_idx;
  while (i < to_copy) {
    buffer->data[i - wrap_offset] = data[i];
    ++i;
  }
  buffer->size += to_copy;
  return to_copy;
}

static size_t ByteBufferPeakBytesInternal(
    byte_buffer_t *buffer, uint8_t *data, size_t count) {
  size_t const to_copy = (count > buffer->size) ? buffer->size : count;
  size_t i = 0;
  while (buffer->ptr + i < buffer->capacity && i < to_copy) {
    data[i] = buffer->data[buffer->ptr + i];
    ++i;
  }
  size_t const wrap_offset = buffer->capacity - buffer->ptr;
  while (i < to_copy) {
    data[i] = buffer->data[i - wrap_offset];
    ++i;
  }
  return to_copy;
}

bool_t ByteBufferPeakByte(byte_buffer_t *buffer, uint8_t *byte) {
  if (!ByteBufferIsValid(buffer) || byte == NULL) return false;
  if (buffer->size == 0) return false;
  *byte = buffer->data[buffer->ptr];
  return true;
}

size_t ByteBufferPeakBytes(
    byte_buffer_t *buffer, uint8_t *data, size_t count) {
  if (!ByteBufferIsValid(buffer)) return 0;
  if (data == NULL || count == 0) return 0;
  return ByteBufferPeakBytesInternal(buffer, data, count);
}

bool_t ByteBufferDequeueByte(byte_buffer_t *buffer, uint8_t *byte) {
  if (!ByteBufferIsValid(buffer) || byte == NULL) return false;
  if (buffer->size == 0) return false;
  *byte = buffer->data[buffer->ptr];
  buffer->ptr = (buffer->ptr + 1) % buffer->capacity;
  --buffer->size;
  if (buffer->size == 0) {
    buffer->ptr = 0;
  }
  return true;
}

size_t ByteBufferDequeueBytes(
    byte_buffer_t *buffer, uint8_t *data, size_t count) {
  if (!ByteBufferIsValid(buffer)) return 0;
  if (data == NULL || count == 0) return 0;
  size_t const copied = ByteBufferPeakBytesInternal(buffer, data, count);
  buffer->size -= copied;
  if (buffer->size == 0) {
    buffer->ptr = 0;
  } else {
    buffer->ptr = (buffer->ptr + copied) % buffer->capacity;
  }
  return copied;
}
