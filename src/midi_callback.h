/*
 * MIDI Controller - MIDI Callbacks
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#ifndef _MIDI_CALLBACK_H_
#define _MIDI_CALLBACK_H_

#include "base.h"

#include "midi_message.h"
#include "midi_time.h"

C_SECTION_BEGIN;

/* Generic information about any event. */
typedef struct {
  /* A counter for the current event.  Starts at zero for the first
   * system event. */
  uint32_t event_id;
  /* Current MIDI time when the event occures.  This will be NULL
   * if the internal clock has not been set / synced by the system
   * or the receiver.  */
  midi_time_t const *time;
} midi_event_t;

/* MIDI Transmitter Events */
typedef struct {
  midi_event_t general;
  /* A counter for the current transmitter event. */
  uint32_t tx_event_id;
  /* The message that is being sent. */
  midi_message_t const *message;
  /* User context.  A pointer to some generic context that was provided
   * by the MIDI system user when the callback was registered. */
  void *user_ctx;
} midi_tx_event_t;

/* This callback is used by the MIDI system to send bytes out the
 * MIDI port.
 *
 * Args:
 *  void *
 *    A user specific writer context.  This  is provided to the MIDI
 *    system when registering the callback.
 *  uint8_t const *
 *    Pointer to the raw bytes that are to be written.
 *  size_t
 *    The number of bytes to be written.
 */
typedef void (*midi_data_writer_t) (
  midi_tx_event_t const *, uint8_t const *, size_t);

/* Transmitter event callback set. */
typedef struct {
  uint32_t next_tx_event_id;
  midi_data_writer_t WriteData;
  void *data_writer_ctx;
} midi_tx_callbacks_t;

/* MIDI Receiver Events
 *  Not all messages have callbacks, some will be combined. */

/* Generic information about a receiver event. */
typedef struct {
  midi_event_t general;
  /* A counter for the current receiver event. */
  uint32_t rx_event_id;
  /* The message struct that was received (if applicable).
   * Each receiver event callback will have the important parameters
   * passed in separately. */
  midi_message_t const *message;
  /* User context.  A pointer to some generic context that was provided
   * by the MIDI system user when the callback was registered. */
  void *user_ctx;
} midi_rx_event_t;

/* Called upon receiving any MIDI message *before* message specific
 * callbacks. */
typedef void (*midi_message_callback_t) (midi_rx_event_t const *);

/* Channel-based callbacks. */

/* Called when a note on message is received. */
typedef void (*midi_note_on_callback_t) (
  midi_rx_event_t const *, midi_channel_number_t, midi_note_t const *);

/* Called when a note on message is received. */
typedef void (*midi_note_off_callback_t) (
  midi_rx_event_t const *, midi_channel_number_t, midi_note_t const *);

/* Called when key pressure change message is received. */
typedef void (*midi_key_pressure_callback_t) (
  midi_rx_event_t const *, midi_channel_number_t, midi_note_t const *);

/* Called when control change message is received. */
typedef void (*midi_control_change_callback_t) (
  midi_rx_event_t const *, midi_channel_number_t,
  midi_control_change_t const *);

/* Called when a program change message is received. */
typedef void (*midi_program_change_callback_t) (
  midi_rx_event_t const *, midi_channel_number_t, midi_program_number_t);

/* Called when a channel pressure change message is received. */
typedef void (*midi_channel_pressure_change_callback_t) (
  midi_rx_event_t const *, midi_channel_number_t, uint8_t);

/* Called when a pitch wheel change message is received. */
typedef void (*midi_pitch_wheel_change_callback_t) (
  midi_rx_event_t const *, midi_channel_number_t, uint16_t);

/* The following are called when receiving a Song Position or Song
 * Select message. */
typedef void (*midi_song_position_callback_t) (
  midi_rx_event_t const *, midi_song_position_t);
typedef void (*midi_song_select_callback_t) (
  midi_rx_event_t const *, midi_song_number_t);

/* Called when receiving a Tune  Request message. */
typedef void (*midi_tune_request_callback_t) (midi_rx_event_t const *);

/* Time code based messages. */

/* Called when the MIDI system time is updated or synchronized.  This
 * occurs after receiving a Full Time Code message (unsynchronized),
 * and after a complete sequence of Quarter Frame Time Code (QFTC)
 * messages.
 *
 * The direction will be UNKNOWN upon a Full Time Code, and the direction
 * will be set based on the QFTC.  Invalid or incomplete sequences of QFTC
 * will not result in a call. */
typedef void (*midi_time_update_callback_t) (
  midi_rx_event_t const *, midi_time_direction_t);

/* System Realtime Messages */

/* Called when the MIDI system receives a Timing Clock message.  The
 * MIDI system does not track the number of occurrences or any tempo
 * info.  The user is expected to handle this message. */
typedef void (*midi_timing_clock_callback_t) (midi_rx_event_t const *);

/* This is called if any of the Start, Continue, or Stop are received. */
typedef void (*midi_playback_callback_t) (midi_rx_event_t const *, uint8_t);
/* These three are called when Start, Continue, or Stop messages are
 * received.  Note that the Start and Continue callback will be called
 * immidiately, and *not* after a Start/Continue + Timing Clock as
 * suggested by the MIDI standard.  This is to allow the user to run
 * any setup before actually starting or continuing. */
typedef void (*midi_start_callback_t) (midi_rx_event_t const *);
typedef void (*midi_continue_callback_t) (midi_rx_event_t const *);
typedef void (*midi_stop_callback_t) (midi_rx_event_t const *);

/* Called when receiving an Active Sensing message. */
typedef void (*midi_active_sensing_callback_t) (midi_rx_event_t const *);

/* Called when received a System Reset message.  This has an added
 * boolean argument.  If the callback sets this value to true, then
 * the MIDI system will clear/reset several internal values.  This is
 * the equivelent to calling MidiSystemSoftReset().  This is provided
 * as calling MidiSystemSoftReset() will cause unexpected behavior. */
typedef void (*midi_system_reset_callback_t) (
  midi_rx_event_t const *, bool_t *);

/* Standard SysEx callbacks. */
typedef struct {
  midi_rx_event_t rx_event;
  /* A counter for the current received SysEx event. */
  uint32_t sys_ex_rx_event_id;
  midi_device_id_t device_id;
  midi_sys_ex_t const *sys_ex;
} midi_sys_ex_rx_event_t;

/* These are called when received any handshake message (ACK, NAK,
 * CANCEL, WAIT, EOF). */
typedef void (*midi_ack_callback_t) (
  midi_sys_ex_rx_event_t const *, midi_packet_number_t);
typedef void (*midi_nak_callback_t) (
  midi_sys_ex_rx_event_t const *, midi_packet_number_t);
typedef void (*midi_cancel_callback_t) (
  midi_sys_ex_rx_event_t const *, midi_packet_number_t);
typedef void (*midi_wait_callback_t) (
  midi_sys_ex_rx_event_t const *, midi_packet_number_t);
typedef void (*midi_eof_callback_t) (midi_sys_ex_rx_event_t const *);

/* Called upon receiving any System Exclusive message, this is called
 * after the generic message callback, and before the specialized
 * system exclusive callbacks. */
typedef void (*midi_sys_ex_message_callback_t) (midi_sys_ex_rx_event_t const *);

/* Receiver event callback set. */
typedef struct {
  uint32_t next_rx_event_id;
  /* Any message. */
  midi_message_callback_t OnMessage;
  void *message_ctx;
  /* Channel-based callbacks. */
  /* Note callback. */
  midi_note_on_callback_t OnNoteOn;
  midi_note_off_callback_t OnNoteOff;
  midi_key_pressure_callback_t OnKeyPressure;
  void *note_ctx;
  midi_control_change_callback_t OnControlChange;
  void *control_change_ctx;
  midi_program_change_callback_t OnProgramChange;
  void *program_change_ctx;
  midi_channel_pressure_change_callback_t OnChannelPressureChange;
  void *channel_pressure_ctx;
  midi_pitch_wheel_change_callback_t OnPitchWheelChange;
  void *pitch_wheel_ctx;
  /* Time Code / Timing Clock. */
  midi_time_update_callback_t OnTimeUpdate;
  void *time_update_ctx;
  midi_timing_clock_callback_t OnTimingClock;
  void *timing_clock_ctx;
  /* Song Select/Position. */
  midi_song_position_callback_t OnSongPosition;
  void *song_position_ctx;
  midi_song_select_callback_t OnSongSelect;
  void *song_select_ctx;
  /* Tune Request. */
  midi_tune_request_callback_t OnTuneRequest;
  void *tune_request_ctx;
  /* Playback Controls. */
  midi_playback_callback_t OnPlayback;
  midi_start_callback_t OnStartPlayback;
  midi_continue_callback_t OnContinuePlayback;
  midi_stop_callback_t OnStopPlayback;
  void *playback_ctx;
  /* Active Sensing. */
  midi_active_sensing_callback_t OnActiveSensing;
  void *active_sensing_ctx;
  /* System Reset. */
  midi_system_reset_callback_t OnSystemReset;
  void *system_reset_ctx;

  /* System Exclusive callbacks. */
  uint32_t next_sys_ex_rx_event_id;
  midi_sys_ex_message_callback_t OnSysExMessage;
  void *sys_ex_message_ctx;
  /* Handshake callbacks. */
  midi_eof_callback_t OnEof;
  midi_wait_callback_t OnWait;
  midi_cancel_callback_t OnCancel;
  midi_nak_callback_t OnNak;
  midi_ack_callback_t OnAck;
  void *handshake_ctx;
} midi_rx_callbacks_t;

typedef struct {
  uint32_t next_event_id;
  midi_rx_callbacks_t rx;
  midi_tx_callbacks_t tx;
} midi_callbacks_t;

C_SECTION_END;

#endif  /* _MIDI_CALLBACK_H_ */
