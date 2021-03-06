/*
 * MIDI Controller - MIDI Message
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#ifndef _MIDI_MESSAGE_H_
#define _MIDI_MESSAGE_H_

#include "base.h"
#include "midi_bytes.h"
#include "midi_channel.h"
#include "midi_control.h"
#include "midi_man_id.h"
#include "midi_notation.h"
#include "midi_note.h"
#include "midi_program.h"
#include "midi_sys_ex.h"
#include "midi_time.h"

C_SECTION_BEGIN;

/* MIDI message related types. */
typedef uint8_t midi_status_t;
typedef uint8_t midi_message_type_t;

/* Strips out channel information from status byte. */
midi_message_type_t MidiStatusToMessageType(midi_status_t status_byte);

bool_t MidiIsValidMessageType(midi_message_type_t message_type);
/* Checks if the message type is related to a channel. */
bool_t MidiIsChannelMessageType(midi_message_type_t message_type);

/* Creates a channel-based MIDI status byte from the message type and
 * channel number.  Will return MIDI_NONE if |message_type| does not
 * specify a channel-based message. */
midi_status_t MidiChannelStatusByte(
  midi_message_type_t message_type,
  midi_channel_number_t channel);
/* Returns 0 if not a channel status type. */
midi_channel_number_t MidiChannelFromStatusByte(midi_status_t status_byte);

/* Master MIDI Message */
typedef struct {
  midi_message_type_t type;
  union {
    /* Channel Based */
    struct {
      midi_channel_number_t channel;
      union {
        /* Note-On/Off, Key pressure. */
        midi_note_t note;
        midi_control_change_t control;
        midi_program_number_t program;
        /* Chanel pressure. */
        uint8_t pressure;
        /* Pitch wheel change. */
        uint16_t pitch;
      };
    };
    /* System exclusive. */
    midi_sys_ex_t sys_ex;
    /* Non-Channel Based */
    midi_time_code_t time_code;
    midi_song_position_t song_position;
    midi_song_number_t song_number;
  };
} midi_message_t;

bool_t MidiIsValidMessage(midi_message_t const *message);

/* Validators for stand along message values. */
#define MidiIsValidChannelPressure(pressure) MidiIsDataByte(pressure)
#define MidiIsValidPitchWheel(pitch) MidiIsDataWord(pitch)

/* Message Constructions */

midi_status_t MidiMessageStatus(midi_message_t const *message);

bool_t MidiNoteMessage(
  midi_message_t *message, midi_channel_number_t channel_number,
  bool_t on, midi_note_t const *note);
#define MidiNoteOnMessage(message, channel_number, note) \
    MidiNoteMessage(message, channel_number, true, note)
#define MidiNoteOffMessage(message, channel_number, note) \
    MidiNoteMessage(message, channel_number, false, note)

bool_t MidiKeyPressureMessage(
  midi_message_t *message, midi_channel_number_t channel_number,
  midi_note_t const *note);

bool_t MidiControlChangeMessage(
  midi_message_t *message, midi_channel_number_t channel_number,
  midi_control_change_t const *control_change);

bool_t MidiProgramChangeMessage(
  midi_message_t *message, midi_channel_number_t channel_number,
  midi_program_number_t program);

bool_t MidiChannelPressureMessage(
  midi_message_t *message, midi_channel_number_t channel_number,
  uint8_t channel_pressure);

bool_t MidiPitchWheelMessage(
  midi_message_t *message, midi_channel_number_t channel_number,
  uint16_t pitch);

bool_t MidiSystemExclusiveMessage(
  midi_message_t *message, midi_manufacturer_id_cref_t man_id);

bool_t MidiTimeCodeMessage(
  midi_message_t *message, midi_time_code_t const *time_code);

bool_t MidiSongPositionMessage(
  midi_message_t *message, midi_song_position_t song_position);

bool_t MidiSongSelectMessage(
  midi_message_t *message, midi_song_number_t song_number);

C_SECTION_END;

#endif  /* _MIDI_MESSAGE_H_ */
