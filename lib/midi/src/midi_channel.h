/*
 * MIDI Controller - MIDI Channel Context
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#ifndef _MIDI_CHANNEL_H_
#define _MIDI_CHANNEL_H_

#include "base.h"
#include "midi_program.h"

C_SECTION_BEGIN;

/* MIDI channel number 0-15 */
typedef uint8_t midi_channel_number_t;

#define MidiIsValidChannelNumber(channel_number) \
    (((channel_number) & 0xF) == (channel_number))

/* MIDI channel properties. */
typedef struct {
  /* TODO: Implement channel properties. */
} midi_channel_properties_t;

/* Channel Context */
typedef struct {
  midi_channel_number_t number;
  midi_program_number_t program;
  midi_channel_properties_t properties;
} midi_channel_ctx_t;

C_SECTION_END;

#endif  /* _MIDI_CHANNEL_H_ */
