/*
 * MIDI Controller - Project Base
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#ifndef _MIDI_BASE_H_
#define _MIDI_BASE_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef bool bool_t;

typedef struct {
  uint8_t number;
  union {
    uint8_t velocity;
    uint8_t pressure;
  };
} note_t;

typedef uint8_t channel_t;

typedef uint16_t pitch_t;

#endif  /* _MIDI_BASE_H_ */
