/*
 * MIDI Controller - MIDI Frame Buffer
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#include <string.h>

#include "midi_frame.h"

bool_t MidiInitializeFrameBuffer(midi_frame_buffer_t *frame) {
  if (frame == NULL) return false;
  memset(frame, 0, sizeof(midi_frame_buffer_t));
  return true;
}

bool_t MidiFrameBufferFull(midi_frame_buffer_t const *frame) {
  if (frame == NULL) return false;
  return frame->size >= MIDI_FRAME_BUFFER_SIZE;
}

bool_t MidiFrameBufferEmpty(midi_frame_buffer_t const *frame) {
  if (frame == NULL) return false;
  return frame->size == 0;
}

size_t MidiPutFrameBufferData(
    uint8_t const *data, size_t data_size,
    midi_frame_buffer_t *frame) {
  if (data == NULL || frame == NULL) return 0;
  if (data_size == 0) return 0;
  if (data_size >= MIDI_FRAME_BUFFER_SIZE) {
    size_t const di = data_size - MIDI_FRAME_BUFFER_SIZE;
    memcpy(frame->buffer, &data[di], MIDI_FRAME_BUFFER_SIZE);
    frame->front = 0;
    frame->size = MIDI_FRAME_BUFFER_SIZE;
    return data_size;
  }
  size_t const size = (size_t) frame->size;
  size_t const front = (size_t) frame->front;
  size_t const back = (front + size) % MIDI_FRAME_BUFFER_SIZE;
  for (size_t di = 0; di < data_size; ++di) {
    uint8_t const fi = (uint8_t) ((back + di) % MIDI_FRAME_BUFFER_SIZE);
    frame->buffer[fi] = data[di];
  }
  if (size + data_size > MIDI_FRAME_BUFFER_SIZE) {
    frame->front = (front + (MIDI_FRAME_BUFFER_SIZE - data_size))
        % MIDI_FRAME_BUFFER_SIZE;
    frame->size = MIDI_FRAME_BUFFER_SIZE;
  } else {
    frame->size += data_size;
  }
  return data_size;
}

bool_t MidiPutFrameBufferByte(uint8_t byte, midi_frame_buffer_t *frame) {
  return MidiPutFrameBufferData(&byte, 1, frame) > 0;
}

size_t MidiPeakFrameBufferData(
    midi_frame_buffer_t const *frame, uint8_t *data, size_t data_size) {
  if (frame == NULL || data == NULL) return 0;
  if (data_size == 0 || frame->size == 0) return 0;
  size_t const size = (size_t) frame->size;
  size_t const front = (size_t) frame->front;
  for (size_t di = 0; di < data_size && di < size; ++di) {
    size_t const fi = (di + front) % MIDI_FRAME_BUFFER_SIZE;
    data[di] = frame->buffer[fi];
  }
  return (data_size >= size) ? size : data_size;
}

size_t MidiTakeFrameBufferData(
    midi_frame_buffer_t *frame, uint8_t *data, size_t data_size) {
  size_t const taken = MidiPeakFrameBufferData(frame, data, data_size);
  if (taken == 0) return 0;
  size_t const size = frame->size;
  if (taken == size) {
    MidiClearAllFrameBufferData(frame);
  } else {
    frame->front = (taken + frame->front) % MIDI_FRAME_BUFFER_SIZE;
    frame->size -= taken;
  }
  return taken;
}

size_t MidiClearFrameBufferData(
    midi_frame_buffer_t *frame, size_t data_size) {
  if (frame == NULL || data_size == 0) return 0;
  size_t const size = frame->size;
  if (data_size >= size) {
    MidiClearAllFrameBufferData(frame);
    return size;
  }
  frame->front = (data_size + frame->front) % MIDI_FRAME_BUFFER_SIZE;
  frame->size -= data_size;
  return data_size;
}

size_t MidiClearAllFrameBufferData(
    midi_frame_buffer_t *frame) {
  if (frame == NULL) return 0;
  size_t const size = frame->size;
  memset(frame, 0, sizeof(midi_frame_buffer_t));
  return size;
}
