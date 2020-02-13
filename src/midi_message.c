/*
 * MIDI Controller - MIDI Message
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#include <string.h>

#include "midi_defs.h"
#include "midi_message.h"

/* MIDI Message Functions. */

midi_message_type_t MidiStatusToMessageType(midi_status_t status_byte) {
  if (!MidiIsStatusByte(status_byte)) return MIDI_NONE;
  /* Non-channel based messages. */
  if ((status_byte & 0xF0) == 0xF0) return (midi_message_type_t) status_byte;
  /* Channel-base (remove lower 4 bits) */
  return status_byte & 0xF0;
}

bool_t MidiIsValidMessageType(midi_message_type_t message_type) {
  if (!MidiIsStatusByte(message_type)) return false;
  if (MidiIsChannelMessageType(message_type)) {
    /* For channel messages, the message type should not have a channel
     * value set. */
    return (message_type & 0x0F) == 0x00;
  }
  return true;
}

bool_t MidiIsChannelMessageType(midi_message_type_t message_type) {
  if (!MidiIsStatusByte(message_type)) return false;
  return (message_type & 0xF0) != 0xF0;
}

midi_status_t MidiChannelStatusByte(
    midi_message_type_t message_type,
    midi_channel_number_t channel) {
  if (!MidiIsChannelMessageType(message_type)) return MIDI_NONE;
  if (!MidiIsValidChannelNumber(channel)) return MIDI_NONE;
  return (message_type & 0xF0) | (channel & 0x0F);
}

midi_channel_number_t MidiChannelFromStatusByte(midi_status_t status_byte) {
  if (!MidiIsChannelMessageType(MidiStatusToMessageType(status_byte))) return 0;
  return status_byte & 0x0F;
}

bool_t MidiMessageIsValid(midi_message_t const *message) {
  if (message == NULL) return false;
  if (!MidiIsValidMessageType(message->type)) return false;
  if (MidiIsChannelMessageType(message->type) &&
      !MidiIsValidChannelNumber(message->channel)) return false;
  switch (message->type) {
    case MIDI_NOTE_OFF:
    case MIDI_NOTE_ON:
    case MIDI_KEY_PRESSURE:
      return MidiIsValidNote(&message->note);
    case MIDI_CONTROL_CHANGE:
      return MidiIsValidControlChange(&message->control);
    case MIDI_PROGRAM_CHANGE:
      return MidiIsValidProgramNumber(message->program);
    case MIDI_CHANNEL_PRESSURE:
      return MidiIsValidChannelPressure(message->pressure);
    case MIDI_PITCH_WHEEL:
      return MidiIsValidPitchWheel(message->pitch);
    /* No Data */
    case MIDI_TUNE_REQUEST:
    case MIDI_END_SYSTEM_EXCLUSIVE:
    case MIDI_TIMING_CLOCK:
    case MIDI_START:
    case MIDI_CONTINUE:
    case MIDI_STOP:
    case MIDI_ACTIVE_SENSING:
    case MIDI_SYSTEM_RESET:
      return true;
    case MIDI_SYSTEM_EXCLUSIVE:
      return MidiIsValidSysEx(&message->sys_ex);
    case MIDI_TIME_CODE:
      return MidiIsValidTimeCode(&message->time_code);
    case MIDI_SONG_POSITION_POINTER:
      /* TODO: Check |song_position| */
      return false;
    case MIDI_SONG_SELECT:
      /* TODO: Check |song_number| */
      return false;
  }
  return false;
}

midi_status_t MidiMessageStatus(midi_message_t const *message) {
  if (!MidiMessageIsValid(message)) return MIDI_NONE;
  return MidiIsChannelMessageType(message->type)
      ? MidiChannelStatusByte(message->type, message->channel)
      : message->type;
}

static bool_t MidiInitializeMessage(midi_message_t *message) {
  if (message == NULL) return false;
  memset(message, 0, sizeof(midi_message_t));
  return true;
}

bool_t MidiNoteMessage(
    midi_message_t *message, midi_channel_number_t channel_number,
    bool_t on, midi_note_t const *note) {
  if (!MidiInitializeMessage(message)) return false;
  if (!MidiIsValidChannelNumber(channel_number)) return false;
  if (!MidiIsValidNote(note)) return false;
  message->type = on ? MIDI_NOTE_ON : MIDI_NOTE_OFF;
  message->channel = channel_number;
  message->note = (midi_note_t) {
    .key = note->key,
    .velocity = note->velocity
  };
  return true;
}

bool_t MidiKeyPressureMessage(
    midi_message_t *message, midi_channel_number_t channel_number,
    midi_note_t const *note) {
  if (!MidiInitializeMessage(message)) return false;
  if (!MidiIsValidChannelNumber(channel_number)) return false;
  if (!MidiIsValidNote(note)) return false;
  message->type = MIDI_KEY_PRESSURE;
  message->channel = channel_number;
  message->note = (midi_note_t) {
    .key = note->key,
    .pressure = note->pressure
  };
  return true;
}

bool_t MidiControlChangeMessage(
    midi_message_t *message, midi_channel_number_t channel_number,
    midi_control_change_t const *control_change) {
  if (!MidiInitializeMessage(message)) return false;
  if (!MidiIsValidChannelNumber(channel_number)) return false;
  if (!MidiIsValidControlChange(control_change)) return false;
  message->type = MIDI_CONTROL_CHANGE;
  message->channel = channel_number;
  message->control = *control_change;
  return true;
}

bool_t MidiProgramChangeMessage(
    midi_message_t *message, midi_channel_number_t channel_number,
    midi_program_number_t program) {
  if (!MidiInitializeMessage(message)) return false;
  if (!MidiIsValidChannelNumber(channel_number)) return false;
  if (!MidiIsValidProgramNumber(program)) return false;
  message->type = MIDI_PROGRAM_CHANGE;
  message->channel = channel_number;
  message->program = program;
  return true;
}

bool_t MidiChannelPressureMessage(
    midi_message_t *message, midi_channel_number_t channel_number,
    uint8_t channel_pressure) {
  if (!MidiInitializeMessage(message)) return false;
  if (!MidiIsValidChannelNumber(channel_number)) return false;
  if (!MidiIsValidChannelPressure(channel_pressure)) return false;
  message->type = MIDI_CHANNEL_PRESSURE;
  message->channel = channel_number;
  message->pressure = channel_pressure;
  return true;
}

bool_t MidiPitchWheelMessage(
    midi_message_t *message, midi_channel_number_t channel_number,
    uint16_t pitch) {
  if (!MidiInitializeMessage(message)) return false;
  if (!MidiIsValidChannelNumber(channel_number)) return false;
  if (!MidiIsValidPitchWheel(pitch)) return false;
  message->type = MIDI_PITCH_WHEEL;
  message->channel = channel_number;
  message->pitch = pitch;
  return true;
}

bool_t MidiSystemExclusiveMessage(
    midi_message_t *message, midi_manufacturer_id_cref_t man_id) {
  if (!MidiInitializeMessage(message)) return false;
  if (!MidiInitializeSysEx(&message->sys_ex, man_id)) return false;
  message->type = MIDI_SYSTEM_EXCLUSIVE;
  return true;
}

bool_t MidiTimeCodeMessage(
    midi_message_t *message, midi_time_code_t const *time_code) {
  if (!MidiInitializeMessage(message)) return false;
  if (!MidiIsValidTimeCode(time_code)) return false;
  message->type = MIDI_TIME_CODE;
  memcpy(&message->time_code, time_code, sizeof(midi_time_code_t));
  return true;
}
