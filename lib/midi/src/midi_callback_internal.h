/*
 * MIDI Controller - MIDI Callbacks - Internal
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#ifndef _MIDI_CALLBACK_INTERNAL_H_
#define _MIDI_CALLBACK_INTERNAL_H_

#include "base.h"

#include "midi_callback.h"

C_SECTION_BEGIN;

/*  This is an internal API for the midi_callbacks_t module.  This is
 *  intended to be used by the midi_system_t module. */

bool_t MidiInitializeCallbacks(midi_callbacks_t *callbacks);

bool_t MidiIsValidCallbacks(midi_callbacks_t const *callbacks);

bool_t MidiCallOnMessageCallback(
  midi_callbacks_t *callbacks, midi_time_t const *time,
  midi_message_t const *message);

bool_t MidiCallOnTimeSynchronizeCallback(
  midi_callbacks_t *callbacks, midi_time_t const *time,
  midi_time_direction_t direction);

bool_t MidiCallOnSystemResetCallback(
  midi_callbacks_t *callbacks, midi_time_t const *time,
  midi_message_t const *message, bool_t *soft_reset);

C_SECTION_END;

#endif  /* _MIDI_CALLBACK_INTERNAL_H_ */
