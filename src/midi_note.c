/*
 * MIDI Controller - MIDI Note
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#include "midi_note.h"

bool_t MidiIsValidNote(midi_note_t const *note) {
  if (note == NULL) return false;
  return MidiIsValidKey(note->key)
      && MidiIsValidVelocity(note->velocity)
      && MidiIsValidPressure(note->pressure);
}

bool_t MidiNote(midi_note_t *note, uint8_t key, uint8_t velocity) {
  if (note == NULL) return false;
  if (!MidiIsValidKey(key) || !MidiIsValidVelocity(velocity)) return false;
  *note = (midi_note_t) {
    .key = key,
    .velocity = velocity
  };
  return true;
}

bool_t MidiNotePressure(midi_note_t *note, uint8_t key, uint8_t pressure) {
  if (note == NULL) return false;
  if (!MidiIsValidKey(key) || !MidiIsValidPressure(pressure)) return false;
  *note = (midi_note_t) {
    .key = key,
    .pressure = pressure
  };
  return true;
}

bool_t MidiSetNoteKey(midi_note_t *note, uint8_t key) {
  if (note == NULL) return false;
  if (!MidiIsValidKey(key)) return false;
  note->key = key;
  return true;
}

bool_t MidiSetNoteVelocity(midi_note_t *note, uint8_t velocity) {
  if (note == NULL) return false;
  if (!MidiIsValidVelocity(velocity)) return false;
  note->velocity = velocity;
  return true;
}

bool_t MidiSetNotePressure(midi_note_t *note, uint8_t pressure) {
  if (note == NULL) return false;
  if (!MidiIsValidPressure(pressure)) return false;
  note->pressure = pressure;
  return true;
}
