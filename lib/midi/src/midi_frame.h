/*
 * MIDI Controller - MIDI Frame
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#ifndef _MIDI_FRAME_H_
#define _MIDI_FRAME_H_

#include "base.h"

C_SECTION_BEGIN;

/* Size of the largest MIDI message. */
#define MIDI_FRAME_BUFFER_SIZE   128

/* A circular buffer for frame data. */
typedef struct {
  uint8_t buffer[MIDI_FRAME_BUFFER_SIZE];
  uint8_t front;
  uint8_t size;
} midi_frame_buffer_t;

bool_t MidiInitializeFrameBuffer(midi_frame_buffer_t *frame);
bool_t MidiFrameBufferFull(midi_frame_buffer_t const *frame);
bool_t MidiFrameBufferEmpty(midi_frame_buffer_t const *frame);
#define MidiFrameBufferSize(frame) ((size_t) (frame)->size)
#define MidiFrameBufferCapacity(frame) ((size_t) MIDI_FRAME_BUFFER_SIZE)

size_t MidiPutFrameBufferData(
    uint8_t const *data, size_t data_size, midi_frame_buffer_t *frame);
bool_t MidiPutFrameBufferByte(uint8_t byte, midi_frame_buffer_t *frame);
size_t MidiPeakFrameBufferData(
    midi_frame_buffer_t const *frame, uint8_t *data, size_t data_size);
size_t MidiTakeFrameBufferData(
    midi_frame_buffer_t *frame, uint8_t *data, size_t data_size);
size_t MidiClearFrameBufferData(
    midi_frame_buffer_t *frame, size_t data_size);
size_t MidiClearAllFrameBufferData(
    midi_frame_buffer_t *frame);

C_SECTION_END;

#endif  /* _MIDI_FRAME_H_ */
