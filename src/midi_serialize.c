/*
 * MIDI Controller - MIDI Serializing.
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#include <string.h>

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
  if (data == NULL && data_size > 0) return 0;
  if (!MidiMessageIsValid(message)) return 0;
  if (message->type == MIDI_NONE) return 0;
  uint8_t *message_data = data;
  size_t message_data_size = data_size;
  size_t data_used = 0;
  if (!skip_status) {
    /* Status Byte. */
    if (message_data_size >= 1) {
      *message_data++ = MidiMessageStatus(message);
      --message_data_size;
    }
    data_used = 1;
  }
  /* Data Bytes */
  switch (message->type) {
    case MIDI_NOTE_OFF:
    case MIDI_NOTE_ON:
      if (message_data_size >= 2) {
        message_data[0] = message->note.key;
        message_data[1] = message->note.velocity;
      }
      data_used += 2;
      break;
    case MIDI_KEY_PRESSURE:
      if (message_data_size >= 2) {
        message_data[0] = message->note.key;
        message_data[1] = message->note.pressure;
      }
      data_used += 2;
      break;
    case MIDI_CONTROL_CHANGE:
      if (message_data_size >= 2) {
        message_data[0] = message->control.number;
        message_data[1] = message->control.value;
      }
      data_used += 2;
      break;
    case MIDI_PROGRAM_CHANGE:
      if (message_data_size >= 1)
        message_data[0] = message->program;
      ++data_used;
      break;
    case MIDI_CHANNEL_PRESSURE:
      if (message_data_size >= 1)
        message_data[0] = message->pressure;
      ++data_used;
      break;
    case MIDI_PITCH_WHEEL:
      if (message_data_size >= 2) {
        message_data[0] = MidiGetDataWordLsb(message->pitch);
        message_data[1] = MidiGetDataWordMsb(message->pitch);
      }
      data_used += 2;
      break;
    case MIDI_SYSTEM_EXCLUSIVE: {
      size_t const sys_ex_size = MidiSerializeSysEx(
          &message->sys_ex, message_data, message_data_size);
      if (sys_ex_size == 0) return 0;
      data_used += sys_ex_size;
      if (message_data_size >= (sys_ex_size + 1)) {
        message_data[sys_ex_size] = MIDI_END_SYSTEM_EXCLUSIVE;
      }
      ++data_used;
    } break;
    case MIDI_TIME_CODE:
      if (message_data_size >= 1) {
        if (!MidiSerializeTimeCode(&message->time_code, message_data))
          return 0;
      }
      ++data_used;
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
    case MIDI_SONG_POSITION_POINTER:
    case MIDI_SONG_SELECT:
    default:
      return 0;
  }  /* switch (message->type) */
  return data_used;
}

size_t MidiSerializeTimeAsPacket(
    midi_time_t const *time, bool_t backwards,
    uint8_t *data, size_t data_size) {
  if (!MidiIsValidTime(time) || data == NULL) return 0;
  if (data_size > 0) {
    data[0] = MIDI_TIME_CODE;
  }
  return MidiSerializeTime(
      time, backwards, &data[1], data_size > 0 ? data_size - 1 : 0) + 1;
}

size_t MidiDeserializeMessage(
    uint8_t const *data, size_t data_size,
    midi_status_t status_override, midi_message_t *message) {
  if (data == NULL && data_size > 0) return 0;
  if (message == NULL) return 0;
  memset(message, 0, sizeof(*message));
  size_t data_used = 0;
  if (status_override != MIDI_NONE) {
    if (!MidiIsStatusByte(status_override)) return 0;
    message->type = MidiStatusToMessageType(status_override);
    message->channel = MidiChannelFromStatusByte(status_override);
  } else {
    if (data_size == 0) return 1;
    midi_status_t const status_byte = data[0];
    data_used = 1;
    if (!MidiIsStatusByte(status_byte)) return 0;
    message->type = MidiStatusToMessageType(status_byte);
    message->channel = MidiChannelFromStatusByte(status_byte);
  }
  uint8_t const *message_data = (data == NULL) ? NULL : &data[data_used];
  size_t message_data_size = data_size - data_used;

  if (message->type == MIDI_SYSTEM_EXCLUSIVE) {
    size_t const sys_ex_size = MidiDeserializeSysEx(
        message_data, message_data_size, &message->sys_ex);
    if (sys_ex_size == 0) goto deserialize_error;
    data_used += sys_ex_size;
    if (message_data_size >= (sys_ex_size + 1)) {
      /* This condition implies that SysEx deserialized correctly, and
       * that this must be the end of the SysEx packet. */
      if (message_data[sys_ex_size] != MIDI_END_SYSTEM_EXCLUSIVE)
        return 0;
      /* Drop byte. */
    }
    ++data_used;
  } else switch (message->type) {
    case MIDI_NOTE_OFF:
    case MIDI_NOTE_ON:
      if (message_data_size >=  2) {
        if (!MidiNote(&message->note, message_data[0], message_data[1]))
          goto deserialize_error;
      }
      data_used += 2;
      break;
    case MIDI_KEY_PRESSURE:
      if (message_data_size >=  2) {
        if (!MidiNotePressure(&message->note, message_data[0], message_data[1]))
          goto deserialize_error;
      }
      data_used += 2;
      break;
    case MIDI_CONTROL_CHANGE:
      if (message_data_size >=  2) {
        if (!MidiControlChange(&message->control, message_data[0], message_data[1]))
          goto deserialize_error;
      }
      data_used += 2;
      break;
    case MIDI_PROGRAM_CHANGE:
      if (message_data_size >= 1) {
        if (!MidiIsValidProgramNumber(message_data[0]))
          goto deserialize_error;
        message->program = message_data[0];
      }
      ++data_used;
      break;
    case MIDI_CHANNEL_PRESSURE:
      if (message_data_size >= 1) {
        if (!MidiIsValidChannelPressure(message_data[0]))
          goto deserialize_error;
        message->pressure = message_data[0];
      }
      ++data_used;
      break;
    case MIDI_PITCH_WHEEL:
      if (message_data_size >=  2) {
        if (!MidiIsDataArray(message_data, 2))
          goto deserialize_error;
        message->pitch =
            MidiDataWordFromBytes(message_data[1], message_data[0]);
      }
      data_used += 2;
      break;
    case MIDI_TIME_CODE:
      if (message_data_size >= 1) {
        if (!MidiDeserializeTimeCode(&message->time_code, message_data[0]))
          goto deserialize_error;
      }
      ++data_used;
      break;
    case MIDI_SONG_POSITION_POINTER:
      if (message_data_size >= 2) {
        if (!MidiIsDataArray(message_data, 2))
          goto deserialize_error;
        /* TODO: Support song position pointer. */
      }
      message->type = MIDI_NONE;
      data_used += 2;
      break;
    case MIDI_SONG_SELECT:
      /* TODO: Support song position pointer. */
      if (!MidiIsDataByte(message_data[0]))
        goto deserialize_error;
      message->type = MIDI_NONE;
      ++data_used;
      break;
    case MIDI_TUNE_REQUEST:
    case MIDI_END_SYSTEM_EXCLUSIVE:
    case MIDI_TIMING_CLOCK:
    case MIDI_START:
    case MIDI_CONTINUE:
    case MIDI_STOP:
    case MIDI_ACTIVE_SENSING:
    case MIDI_SYSTEM_RESET:
      break;
    default:
      /* WUT? Reserved maybe. */
      goto deserialize_error;
  }
  return data_used;
deserialize_error:
  if (message != NULL) message->type = MIDI_NONE;
  return 0;
}
