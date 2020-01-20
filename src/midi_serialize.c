/*
 * MIDI Controller - MIDI Serializing.
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#include "midi_defs.h"
#include "midi_serialize.h"

size_t MidiMessageDataSize(midi_message_type_t type) {
  if (!MidiIsValidMessageType(type)) return 0;
  if (type == MIDI_SYSTEM_EXCLUSIVE) return 0;
  switch (type) {
    case MIDI_TUNE_REQUEST:
    case MIDI_END_SYSTEM_EXCLUSIVE:
    case MIDI_TIMING_CLOCK:
    case MIDI_START:
    case MIDI_CONTINUE:
    case MIDI_STOP:
    case MIDI_ACTIVE_SENSING:
    case MIDI_SYSTEM_RESET:
      return 0;
    case MIDI_PROGRAM_CHANGE:
    case MIDI_CHANNEL_PRESSURE:
    case MIDI_TIME_CODE:
    case MIDI_SONG_SELECT:
      return 1;
    case MIDI_NOTE_OFF:
    case MIDI_NOTE_ON:
    case MIDI_KEY_PRESSURE:
    case MIDI_CONTROL_CHANGE:
    case MIDI_PITCH_WHEEL:
    case MIDI_SONG_POSITION_POINTER:
      return 2;
  }
  return 0;
}

/* MIDI Serializing Functions */

size_t MidiSerializeMessage(
    midi_message_t const *message, bool_t skip_status,
    uint8_t *data, size_t data_size) {
  if (message == NULL || data == NULL) return 0;
  if (message->type == MIDI_NONE) return 0;
  if (!MidiMessageIsValid(message)) return 0;
  uint8_t message_buffer[4];
  uint8_t *message_data = message_buffer;
  if (!skip_status) {
    /* Status Byte. */
    message_buffer[0] = MidiIsChannelMessageType(message->type)
        ? MidiChannelStatusByte(message->type, message->channel)
        : message->type;
    message_data = &message_buffer[1];
  }
  /* 2 is the most common value, switch will change if needed. */
  size_t message_data_size = 2;
  /* Data Bytes */
  switch (message->type) {
    case MIDI_NOTE_OFF:
    case MIDI_NOTE_ON:
      message_data[0] = message->note.key;
      message_data[1] = message->note.velocity;
      break;
    case MIDI_KEY_PRESSURE:
      message_data[0] = message->note.key;
      message_data[1] = message->note.pressure;
      break;
    case MIDI_CONTROL_CHANGE:
      message_data[0] = message->control.number;
      message_data[1] = message->control.value;
      break;
    case MIDI_PROGRAM_CHANGE:
      message_data[0] = message->program;
      message_data_size = 1;
      break;
    case MIDI_CHANNEL_PRESSURE:
      message_data[0] = message->pressure;
      message_data_size = 1;
      break;
    case MIDI_PITCH_WHEEL:
      message_data[0] = MidiGetDataWordLsb(message->pitch);
      message_data[1] = MidiGetDataWordMsb(message->pitch);
      break;
    /* Data-less */
    case MIDI_TUNE_REQUEST:
    case MIDI_END_SYSTEM_EXCLUSIVE:
    case MIDI_TIMING_CLOCK:
    case MIDI_START:
    case MIDI_CONTINUE:
    case MIDI_STOP:
    case MIDI_ACTIVE_SENSING:
    case MIDI_SYSTEM_RESET:
      message_data_size = 0;
      break;
    /* Unsupported */
    case MIDI_SYSTEM_EXCLUSIVE:
    case MIDI_TIME_CODE:
    case MIDI_SONG_POSITION_POINTER:
    case MIDI_SONG_SELECT:
    default:
      return 0;
  }  /* switch (message->type) */
  /* Transfer to output buffer. */
  size_t const message_size = message_data_size + (skip_status ? 0 : 1);
  for (size_t i = 0; i < data_size && i < message_size; ++i) {
    data[i] = message_buffer[i];
  }
  return message_size;
}

size_t MidiSerializeMessages(
    midi_message_t const *messages, size_t message_count,
    uint8_t *data, size_t data_size) {
  if (messages == NULL || data == NULL || message_count == 0) return 0;
  size_t mi = 0, di = 0;
  size_t total_message_size = 0;
  while (mi < message_count) {
    size_t const message_size = MidiSerializeMessage(
        &messages[mi++], false, &data[di], di - data_size);
    if (message_size == 0) continue;
    total_message_size += message_size;
    if (total_message_size >= data_size) {
      di = data_size;
    } else {
      di += message_size;
    }
  }
  return total_message_size;
}

size_t MidiDeserializeMessage(
    uint8_t const *data, size_t data_size,
    midi_status_t status_override, midi_message_t *message) {
  if (data == NULL || message == NULL) return 0;
  size_t data_used = 0;
  if (status_override != MIDI_NONE) {
    if (!MidiIsStatusByte(status_override)) return 0;
    message->type = MidiStatusToMessageType(status_override);
    message->channel = MidiChannelFromStatusByte(status_override);
  } else {
    if (data_size == 0) return 0;
    midi_status_t const status_byte = data[0];
    data_used = 1;
    if (!MidiIsStatusByte(status_byte)) return 0;
    message->type = MidiStatusToMessageType(status_byte);
    message->channel = MidiChannelFromStatusByte(status_byte);
  }
  /* Check that enough data is avaliable. */
  size_t const message_data_size = MidiMessageDataSize(message->type);
  /* TODO: Handel case of manufacture ID. */
  if (message_data_size == 0) return data_used;
  size_t const expected_data_size = data_used + message_data_size;
  if (expected_data_size < data_size) {
    message->type = MIDI_NONE;
    return expected_data_size;
  }
  /* Check that all of the data required are valid data bytes. */
  for (size_t i = data_used; i < expected_data_size; ++i) {
    if (!MidiIsDataByte(data[i])) {
      message->type = MIDI_NONE;
      return i;
    }
  }

  switch (message->type) {
    case MIDI_NOTE_OFF:
    case MIDI_NOTE_ON:
      MidiNote(&message->note, data[data_used], data[data_used + 1]);
      data_used += 2;
      break;
    case MIDI_KEY_PRESSURE:
      MidiNotePressure(&message->note, data[data_used], data[data_used + 1]);
      data_used += 2;
      break;
    case MIDI_CONTROL_CHANGE:
      MidiControlChange(
          &message->control, data[data_used], data[data_used + 1]);
      data_used += 2;
      break;
    case MIDI_PROGRAM_CHANGE:
      message->program = data[data_used];
      data_used += 1;
      break;
    case MIDI_CHANNEL_PRESSURE:
      message->pressure = data[data_used];
      data_used += 1;
      break;
    case MIDI_PITCH_WHEEL:
      message->pitch =
          MidiDataWordFromBytes(data[data_used + 1], data[data_used]);
      data_used += 2;
      break;
    case MIDI_TIME_CODE:
      /* TODO: Parse |time_code| */
      message->type = MIDI_NONE;
      data_used += 1;
      break;
    case MIDI_SONG_POSITION_POINTER:
      message->type = MIDI_NONE;
      data_used += 2;
      break;
    case MIDI_SONG_SELECT:
      message->type = MIDI_NONE;
      data_used += 1;
      break;
    default:
      /* WUT? */
      message->type = MIDI_NONE;
      return expected_data_size;
  }

  if (data_used != expected_data_size) {
    /* WUT? */
    return expected_data_size;
  }
  return data_used;
}
