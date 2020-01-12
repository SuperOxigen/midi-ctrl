/*
 * MIDI Controller - MIDI Note
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#ifndef _MIDI_NOTE_H_
#define _MIDI_NOTE_H_

#include "base.h"
#include "midi_bytes.h"

typedef struct {
  uint8_t key;
  uint8_t velocity;
  uint8_t pressure;
} midi_note_t;

/* Validity checks. */
#define MidiIsValidKey(key) MidiIsDataByte(key)
#define MidiIsValidVelocity(velocity) MidiIsDataByte(velocity)
#define MidiIsValidPressure(pressure) MidiIsDataByte(pressure)
bool_t MidiIsValidNote(midi_note_t const *note);

/* Initialize note struct as a specific type. */
bool_t MidiNote(midi_note_t *note, uint8_t key, uint8_t velocity);
bool_t MidiNotePressure(midi_note_t *note, uint8_t key, uint8_t pressure);

/* Sets the note struct, only if the provided value is a valid data byte. */
bool_t MidiSetNoteKey(midi_note_t *note, uint8_t key);
bool_t MidiSetNoteVelocity(midi_note_t *note, uint8_t velocity);
bool_t MidiSetNotePressure(midi_note_t *note, uint8_t pressure);

#endif  /* _MIDI_NOTE_H_ */
