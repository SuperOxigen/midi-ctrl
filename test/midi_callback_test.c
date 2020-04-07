/*
 * MIDI Controller - MIDI Callback Test.
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#include <string.h>
#include <unity.h>

#include "midi_callback.h"
#include "midi_callback_internal.h"
#include "midi_defs.h"

static void TestMidiCallbackInitializer(void) {
  TEST_ASSERT_FALSE(MidiInitializeCallbacks(NULL));
  midi_callbacks_t callbacks;
  memset(&callbacks, 0xFF, sizeof(midi_callbacks_t));
  TEST_ASSERT_TRUE(MidiInitializeCallbacks(&callbacks));
  TEST_ASSERT_EQUAL(1, callbacks.next_event_id);
  TEST_ASSERT_EQUAL(1, callbacks.rx.next_rx_event_id);
  TEST_ASSERT_EQUAL(1, callbacks.rx.next_sys_ex_rx_event_id);
  TEST_ASSERT_EQUAL(1, callbacks.tx.next_tx_event_id);
}

typedef struct {
  /* Set by generic message callback. */
  midi_time_t const *time;
  uint32_t event_id;
  midi_message_t const *message;
  uint32_t rx_event_id;
} midi_message_ctx_t;

typedef struct {
  midi_message_ctx_t *message_ctx;
  /* Set by generic sys ex message callback. */
  midi_sys_ex_t const *sys_ex;
  uint32_t sys_ex_rx_event_id;
  midi_device_id_t device_id;
  /* Set by the specialized callback. */
  midi_message_type_t type;
  uint8_t arg_one;
  uint8_t arg_two;
  uint16_t wide_arg;
  void const *pointer_arg;
  uint8_t sub_id;
} midi_specialized_ctx_t;

static void ClearCtx(midi_specialized_ctx_t *ctx) {
  if (ctx == NULL) return;
  midi_message_ctx_t *message_ctx = ctx->message_ctx;
  memset(ctx, 0, sizeof(midi_specialized_ctx_t));
  ctx->message_ctx = message_ctx;
}

/* These are error codes used for debugging certain issues with the
 * callback. */
#define MESSAGE_NULL      0x01
#define MESSAGE_NOT_NULL  0x02
#define UNEXPECTED_TYPE   0x03
#define SYS_EX_NULL       0x04
#define BAD_DEVICE_ID     0x05
#define TIME_NULL         0x06
#define FLAG_NULL         0x07

/* Used as an argument where the callback has none. */
#define FILLER_ARG        0x30
#define INVALID_ARG       0xFF

/*
 *  Message Callbacks
 */

static void MessageCallback(midi_rx_event_t const *rx_event) {
  if (rx_event == NULL || rx_event->user_ctx == NULL) return;
  midi_message_ctx_t *ctx =
      (midi_message_ctx_t *) rx_event->user_ctx;
  memset(ctx, 0, sizeof(midi_message_ctx_t));
  ctx->time = rx_event->general.time;
  ctx->event_id = rx_event->general.event_id;
  ctx->rx_event_id = rx_event->rx_event_id;
  ctx->message = rx_event->message;
}

static void NoteCallback(
    midi_rx_event_t const *rx_event, midi_channel_number_t channel,
    midi_note_t const *note) {
  if (rx_event == NULL || rx_event->user_ctx == NULL) return;
  midi_specialized_ctx_t *ctx =
      (midi_specialized_ctx_t *) rx_event->user_ctx;
  ClearCtx(ctx);
  midi_message_t const *message = rx_event->message;
  if (message == NULL) {
    ctx->type = MESSAGE_NULL;
    return;
  }
  if (message->type != MIDI_NOTE_OFF && message->type != MIDI_NOTE_ON &&
      message->type != MIDI_KEY_PRESSURE) {
    ctx->type = UNEXPECTED_TYPE;
    return;
  }
  ctx->type = message->type;
  ctx->arg_one = channel;
  ctx->pointer_arg = note;
}

static void ControlChangeCallback(
    midi_rx_event_t const *rx_event, midi_channel_number_t channel,
    midi_control_change_t const *control) {
  if (rx_event == NULL || rx_event->user_ctx == NULL) return;
  midi_specialized_ctx_t *ctx =
      (midi_specialized_ctx_t *) rx_event->user_ctx;
  ClearCtx(ctx);
  midi_message_t const *message = rx_event->message;
  if (message == NULL) {
    ctx->type = MESSAGE_NULL;
    return;
  }
  if (message->type != MIDI_CONTROL_CHANGE) {
    ctx->type = UNEXPECTED_TYPE;
    return;
  }
  ctx->type = message->type;
  ctx->arg_one = channel;
  ctx->pointer_arg = control;
}

static void ProgramChangeCallback(
    midi_rx_event_t const *rx_event, midi_channel_number_t channel,
    midi_program_number_t program) {
  if (rx_event == NULL || rx_event->user_ctx == NULL) return;
  midi_specialized_ctx_t *ctx =
      (midi_specialized_ctx_t *) rx_event->user_ctx;
  ClearCtx(ctx);
  midi_message_t const *message = rx_event->message;
  if (message == NULL) {
    ctx->type = MESSAGE_NULL;
    return;
  }
  if (message->type != MIDI_PROGRAM_CHANGE) {
    ctx->type = UNEXPECTED_TYPE;
    return;
  }
  ctx->type = message->type;
  ctx->arg_one = channel;
  ctx->arg_two = program;
}

static void PressureChangeCallback(
    midi_rx_event_t const *rx_event, midi_channel_number_t channel,
    uint8_t pressure) {
  if (rx_event == NULL || rx_event->user_ctx == NULL) return;
  midi_specialized_ctx_t *ctx =
      (midi_specialized_ctx_t *) rx_event->user_ctx;
  ClearCtx(ctx);
  midi_message_t const *message = rx_event->message;
  if (message == NULL) {
    ctx->type = MESSAGE_NULL;
    return;
  }
  if (message->type != MIDI_CHANNEL_PRESSURE) {
    ctx->type = UNEXPECTED_TYPE;
    return;
  }
  ctx->type = message->type;
  ctx->arg_one = channel;
  ctx->arg_two = pressure;
}

static void PitchWheelChangeCallback(
    midi_rx_event_t const *rx_event, midi_channel_number_t channel,
    uint16_t pitch) {
  if (rx_event == NULL || rx_event->user_ctx == NULL) return;
  midi_specialized_ctx_t *ctx =
      (midi_specialized_ctx_t *) rx_event->user_ctx;
  ClearCtx(ctx);
  midi_message_t const *message = rx_event->message;
  if (message == NULL) {
    ctx->type = MESSAGE_NULL;
    return;
  }
  if (message->type != MIDI_PITCH_WHEEL) {
    ctx->type = UNEXPECTED_TYPE;
    return;
  }
  ctx->type = message->type;
  ctx->arg_one = channel;
  ctx->wide_arg = pitch;
}

static void TimeUpdateCallback(
    midi_rx_event_t const *rx_event, midi_time_direction_t direction) {
  if (rx_event == NULL || rx_event->user_ctx == NULL) return;
  midi_specialized_ctx_t *ctx =
      (midi_specialized_ctx_t *) rx_event->user_ctx;
  ClearCtx(ctx);
  if (rx_event->message != NULL) {
    ctx->type = MESSAGE_NOT_NULL;
    return;
  }
  midi_time_t const *time = rx_event->general.time;
  if (time == NULL) {
    ctx->type = TIME_NULL;
    return;
  }
  ctx->arg_one = (uint8_t) direction;
  ctx->pointer_arg = time;
}

static void SongPositionCallback(
    midi_rx_event_t const *rx_event, midi_song_position_t position) {
  if (rx_event == NULL || rx_event->user_ctx == NULL) return;
  midi_specialized_ctx_t *ctx =
      (midi_specialized_ctx_t *) rx_event->user_ctx;
  ClearCtx(ctx);
  midi_message_t const *message = rx_event->message;
  if (message == NULL) {
    ctx->type = MESSAGE_NULL;
    return;
  }
  if (message->type != MIDI_SONG_POSITION_POINTER) {
    ctx->type = UNEXPECTED_TYPE;
    return;
  }
  ctx->type = message->type;
  ctx->wide_arg = position;
}

static void SongSelectCallback(
    midi_rx_event_t const *rx_event, midi_song_number_t number) {
  if (rx_event == NULL || rx_event->user_ctx == NULL) return;
  midi_specialized_ctx_t *ctx =
      (midi_specialized_ctx_t *) rx_event->user_ctx;
  ClearCtx(ctx);
  midi_message_t const *message = rx_event->message;
  if (message == NULL) {
    ctx->type = MESSAGE_NULL;
    return;
  }
  if (message->type != MIDI_SONG_SELECT) {
    ctx->type = UNEXPECTED_TYPE;
    return;
  }
  ctx->type = message->type;
  ctx->arg_one = number;
}

static void TuneRequestCallback(midi_rx_event_t const *rx_event) {
  if (rx_event == NULL || rx_event->user_ctx == NULL) return;
  midi_specialized_ctx_t *ctx =
      (midi_specialized_ctx_t *) rx_event->user_ctx;
  ClearCtx(ctx);
  midi_message_t const *message = rx_event->message;
  if (message == NULL) {
    ctx->type = MESSAGE_NULL;
    return;
  }
  if (message->type != MIDI_TUNE_REQUEST) {
    ctx->type = UNEXPECTED_TYPE;
    return;
  }
  ctx->type = message->type;
  ctx->arg_one = FILLER_ARG;
}

static void TimingClockCallback(midi_rx_event_t const *rx_event) {
  if (rx_event == NULL || rx_event->user_ctx == NULL) return;
  midi_specialized_ctx_t *ctx =
      (midi_specialized_ctx_t *) rx_event->user_ctx;
  ClearCtx(ctx);
  midi_message_t const *message = rx_event->message;
  if (message == NULL) {
    ctx->type = MESSAGE_NULL;
    return;
  }
  if (message->type != MIDI_TIMING_CLOCK) {
    ctx->type = UNEXPECTED_TYPE;
    return;
  }
  ctx->type = message->type;
  ctx->arg_one = FILLER_ARG;
}

static void PlaybackCallback(midi_rx_event_t const *rx_event) {
  if (rx_event == NULL || rx_event->user_ctx == NULL) return;
  midi_specialized_ctx_t *ctx =
      (midi_specialized_ctx_t *) rx_event->user_ctx;
  ClearCtx(ctx);
  midi_message_t const *message = rx_event->message;
  if (message == NULL) {
    ctx->type = MESSAGE_NULL;
    return;
  }
  if (message->type != MIDI_START && message->type != MIDI_CONTINUE &&
      message->type != MIDI_STOP) {
    ctx->type = UNEXPECTED_TYPE;
    return;
  }
  ctx->type = message->type;
  ctx->arg_one = FILLER_ARG;
}

static void ActiveSensingCallback(midi_rx_event_t const *rx_event) {
  if (rx_event == NULL || rx_event->user_ctx == NULL) return;
  midi_specialized_ctx_t *ctx =
      (midi_specialized_ctx_t *) rx_event->user_ctx;
  ClearCtx(ctx);
  midi_message_t const *message = rx_event->message;
  if (message == NULL) {
    ctx->type = MESSAGE_NULL;
    return;
  }
  if (message->type != MIDI_ACTIVE_SENSING) {
    ctx->type = UNEXPECTED_TYPE;
    return;
  }
  ctx->type = message->type;
  ctx->arg_one = FILLER_ARG;
}

static void SystemResetCallback(
    midi_rx_event_t const *rx_event, bool_t *soft_reset) {
  if (rx_event == NULL || rx_event->user_ctx == NULL) return;
  midi_specialized_ctx_t *ctx =
      (midi_specialized_ctx_t *) rx_event->user_ctx;
  ClearCtx(ctx);
  midi_message_t const *message = rx_event->message;
  if (message == NULL) {
    ctx->type = MESSAGE_NULL;
    return;
  }
  if (message->type != MIDI_SYSTEM_RESET) {
    ctx->type = UNEXPECTED_TYPE;
    return;
  }
  if (soft_reset == NULL) {
    ctx->type = FLAG_NULL;
  }
  ctx->type = message->type;
  ctx->pointer_arg = soft_reset;
  *soft_reset = !*soft_reset;
}

static void TestMidiCallback_AllMessages(void) {
  midi_callbacks_t callbacks;
  TEST_ASSERT_TRUE(MidiInitializeCallbacks(&callbacks));
  callbacks.next_event_id = 1000;

  TEST_ASSERT_FALSE(MidiCallOnMessageCallback(NULL, NULL, NULL));
  TEST_ASSERT_FALSE(MidiCallOnMessageCallback(&callbacks, NULL, NULL));
  TEST_ASSERT_EQUAL(1000, callbacks.next_event_id);
  TEST_ASSERT_EQUAL(1, callbacks.rx.next_rx_event_id);

  midi_time_t const kTime;
  midi_message_t const kNoteOnMessage = {
    .type = MIDI_NOTE_ON,
    .channel = MIDI_CHANNEL_8,
    .note = {}
  };
  TEST_ASSERT_TRUE(MidiCallOnMessageCallback(
      &callbacks, NULL, &kNoteOnMessage));
  TEST_ASSERT_EQUAL(1001, callbacks.next_event_id);
  TEST_ASSERT_EQUAL(2, callbacks.rx.next_rx_event_id);

  midi_message_ctx_t message_ctx = {};
  callbacks.rx.OnMessage = MessageCallback;
  callbacks.rx.message_ctx = &message_ctx;
  /* Calling without specialized method. */
  TEST_ASSERT_TRUE(MidiCallOnMessageCallback(
      &callbacks, NULL, &kNoteOnMessage));
  /* Check the generic message set value. */
  TEST_ASSERT_NULL(message_ctx.time);
  TEST_ASSERT_EQUAL(1001, message_ctx.event_id);
  TEST_ASSERT_EQUAL(2, message_ctx.rx_event_id);
  TEST_ASSERT_EQUAL(&kNoteOnMessage, message_ctx.message);

  /* Test that the wrong callback isn't called. */
  midi_specialized_ctx_t note_ctx = { .message_ctx = &message_ctx };
  callbacks.rx.OnNoteOff = NoteCallback;
  callbacks.rx.OnKeyPressure = NoteCallback;
  callbacks.rx.note_ctx = &note_ctx;
  TEST_ASSERT_TRUE(MidiCallOnMessageCallback(
      &callbacks, NULL, &kNoteOnMessage));
  TEST_ASSERT_EQUAL(0, note_ctx.type);

  callbacks.rx.OnNoteOn = NoteCallback;
  TEST_ASSERT_TRUE(MidiCallOnMessageCallback(
      &callbacks, NULL, &kNoteOnMessage));
  /* Check the generic message set value. */
  TEST_ASSERT_NULL(message_ctx.time);
  TEST_ASSERT_EQUAL(1003, message_ctx.event_id);
  TEST_ASSERT_EQUAL(4, message_ctx.rx_event_id);
  TEST_ASSERT_EQUAL(&kNoteOnMessage, message_ctx.message);
  /* Check the specialized message set values. */
  TEST_ASSERT_EQUAL(MIDI_NOTE_ON, note_ctx.type);
  TEST_ASSERT_EQUAL(MIDI_CHANNEL_8, note_ctx.arg_one);
  TEST_ASSERT_EQUAL(&kNoteOnMessage.note, note_ctx.pointer_arg);

  midi_message_t const kNoteOffMessage = {
    .type = MIDI_NOTE_OFF,
    .channel = MIDI_CHANNEL_3,
    .note = {}
  };
  TEST_ASSERT_TRUE(MidiCallOnMessageCallback(
      &callbacks, &kTime, &kNoteOffMessage));
  /* Check the generic message set value. */
  TEST_ASSERT_EQUAL(&kTime, message_ctx.time);
  TEST_ASSERT_EQUAL(1004, message_ctx.event_id);
  TEST_ASSERT_EQUAL(5, message_ctx.rx_event_id);
  TEST_ASSERT_EQUAL(&kNoteOffMessage, message_ctx.message);
  /* Check the specialized message set values. */
  TEST_ASSERT_EQUAL(MIDI_NOTE_OFF, note_ctx.type);
  TEST_ASSERT_EQUAL(MIDI_CHANNEL_3, note_ctx.arg_one);
  TEST_ASSERT_EQUAL(&kNoteOffMessage.note, note_ctx.pointer_arg);

  midi_message_t const kKeyPressureMessage = {
    .type = MIDI_KEY_PRESSURE,
    .channel = MIDI_CHANNEL_11,
    .note = {}
  };
  TEST_ASSERT_TRUE(MidiCallOnMessageCallback(
      &callbacks, NULL, &kKeyPressureMessage));
  /* Check the generic message set value. */
  TEST_ASSERT_NULL(message_ctx.time);
  TEST_ASSERT_EQUAL(1005, message_ctx.event_id);
  TEST_ASSERT_EQUAL(6, message_ctx.rx_event_id);
  TEST_ASSERT_EQUAL(&kKeyPressureMessage, message_ctx.message);
  /* Check the specialized message set values. */
  TEST_ASSERT_EQUAL(MIDI_KEY_PRESSURE, note_ctx.type);
  TEST_ASSERT_EQUAL(MIDI_CHANNEL_11, note_ctx.arg_one);
  TEST_ASSERT_EQUAL(&kKeyPressureMessage.note, note_ctx.pointer_arg);

  midi_message_t const kControlChangeMessage = {
    .type = MIDI_CONTROL_CHANGE,
    .channel = MIDI_CHANNEL_6,
    .control = {}
  };
  midi_specialized_ctx_t control_ctx = { .message_ctx = &message_ctx };
  callbacks.rx.OnControlChange = ControlChangeCallback;
  callbacks.rx.control_change_ctx = &control_ctx;
  TEST_ASSERT_TRUE(MidiCallOnMessageCallback(
      &callbacks, NULL, &kControlChangeMessage));
  /* Check the generic message set value. */
  TEST_ASSERT_NULL(message_ctx.time);
  TEST_ASSERT_EQUAL(1006, message_ctx.event_id);
  TEST_ASSERT_EQUAL(7, message_ctx.rx_event_id);
  TEST_ASSERT_EQUAL(&kControlChangeMessage, message_ctx.message);
  /* Check the specialized message set values. */
  TEST_ASSERT_EQUAL(MIDI_CONTROL_CHANGE, control_ctx.type);
  TEST_ASSERT_EQUAL(MIDI_CHANNEL_6, control_ctx.arg_one);
  TEST_ASSERT_EQUAL(&kControlChangeMessage.control, control_ctx.pointer_arg);

  midi_message_t const kProgramChangeMessage = {
    .type = MIDI_PROGRAM_CHANGE,
    .channel = MIDI_CHANNEL_2,
    .program = 56
  };
  midi_specialized_ctx_t program_ctx = { .message_ctx = &message_ctx };
  callbacks.rx.OnProgramChange = ProgramChangeCallback;
  callbacks.rx.program_change_ctx = &program_ctx;
  TEST_ASSERT_TRUE(MidiCallOnMessageCallback(
      &callbacks, NULL, &kProgramChangeMessage));
  /* Check the generic message set value. */
  TEST_ASSERT_NULL(message_ctx.time);
  TEST_ASSERT_EQUAL(1007, message_ctx.event_id);
  TEST_ASSERT_EQUAL(8, message_ctx.rx_event_id);
  TEST_ASSERT_EQUAL(&kProgramChangeMessage, message_ctx.message);
  /* Check the specialized message set values. */
  TEST_ASSERT_EQUAL(MIDI_PROGRAM_CHANGE, program_ctx.type);
  TEST_ASSERT_EQUAL(MIDI_CHANNEL_2, program_ctx.arg_one);
  TEST_ASSERT_EQUAL(56, program_ctx.arg_two);

  midi_message_t const kChannelPressureMessage = {
    .type = MIDI_CHANNEL_PRESSURE,
    .channel = MIDI_CHANNEL_7,
    .pressure = 97
  };
  midi_specialized_ctx_t channel_pressure_ctx = { .message_ctx = &message_ctx };
  callbacks.rx.OnChannelPressureChange = PressureChangeCallback;
  callbacks.rx.channel_pressure_ctx = &channel_pressure_ctx;
  TEST_ASSERT_TRUE(MidiCallOnMessageCallback(
      &callbacks, NULL, &kChannelPressureMessage));
  /* Check the generic message set value. */
  TEST_ASSERT_NULL(message_ctx.time);
  TEST_ASSERT_EQUAL(1008, message_ctx.event_id);
  TEST_ASSERT_EQUAL(9, message_ctx.rx_event_id);
  TEST_ASSERT_EQUAL(&kChannelPressureMessage, message_ctx.message);
  /* Check the specialized message set values. */
  TEST_ASSERT_EQUAL(MIDI_CHANNEL_PRESSURE, channel_pressure_ctx.type);
  TEST_ASSERT_EQUAL(MIDI_CHANNEL_7, channel_pressure_ctx.arg_one);
  TEST_ASSERT_EQUAL(97, channel_pressure_ctx.arg_two);

  midi_message_t const kPitchWheelMessage = {
    .type = MIDI_PITCH_WHEEL,
    .channel = MIDI_CHANNEL_13,
    .pitch = 888
  };
  midi_specialized_ctx_t pitch_wheel_ctx = { .message_ctx = &message_ctx };
  callbacks.rx.OnPitchWheelChange = PitchWheelChangeCallback;
  callbacks.rx.pitch_wheel_ctx = &pitch_wheel_ctx;
  TEST_ASSERT_TRUE(MidiCallOnMessageCallback(
      &callbacks, NULL, &kPitchWheelMessage));
  /* Check the generic message set value. */
  TEST_ASSERT_NULL(message_ctx.time);
  TEST_ASSERT_EQUAL(1009, message_ctx.event_id);
  TEST_ASSERT_EQUAL(10, message_ctx.rx_event_id);
  TEST_ASSERT_EQUAL(&kPitchWheelMessage, message_ctx.message);
  /* Check the specialized message set values. */
  TEST_ASSERT_EQUAL(MIDI_PITCH_WHEEL, pitch_wheel_ctx.type);
  TEST_ASSERT_EQUAL(MIDI_CHANNEL_13, pitch_wheel_ctx.arg_one);
  TEST_ASSERT_EQUAL(888, pitch_wheel_ctx.wide_arg);

  midi_message_t const kSongPositionMessage = {
    .type = MIDI_SONG_POSITION_POINTER,
    .song_position = 767
  };
  midi_specialized_ctx_t song_position_ctx = { .message_ctx = &message_ctx };
  callbacks.rx.OnSongPosition = SongPositionCallback;
  callbacks.rx.song_position_ctx = &song_position_ctx;
  TEST_ASSERT_TRUE(MidiCallOnMessageCallback(
      &callbacks, NULL, &kSongPositionMessage));
  /* Check the generic message set value. */
  TEST_ASSERT_NULL(message_ctx.time);
  TEST_ASSERT_EQUAL(1010, message_ctx.event_id);
  TEST_ASSERT_EQUAL(11, message_ctx.rx_event_id);
  TEST_ASSERT_EQUAL(&kSongPositionMessage, message_ctx.message);
  /* Check the specialized message set values. */
  TEST_ASSERT_EQUAL(MIDI_SONG_POSITION_POINTER, song_position_ctx.type);
  TEST_ASSERT_EQUAL(767, song_position_ctx.wide_arg);

  midi_message_t const kSongSelectMessage = {
    .type = MIDI_SONG_SELECT,
    .song_position = 80
  };
  midi_specialized_ctx_t song_select_ctx = { .message_ctx = &message_ctx };
  callbacks.rx.OnSongSelect = SongSelectCallback;
  callbacks.rx.song_select_ctx = &song_select_ctx;
  TEST_ASSERT_TRUE(MidiCallOnMessageCallback(
      &callbacks, NULL, &kSongSelectMessage));
  /* Check the generic message set value. */
  TEST_ASSERT_NULL(message_ctx.time);
  TEST_ASSERT_EQUAL(1011, message_ctx.event_id);
  TEST_ASSERT_EQUAL(12, message_ctx.rx_event_id);
  TEST_ASSERT_EQUAL(&kSongSelectMessage, message_ctx.message);
  /* Check the specialized message set values. */
  TEST_ASSERT_EQUAL(MIDI_SONG_SELECT, song_select_ctx.type);
  TEST_ASSERT_EQUAL(80, song_select_ctx.arg_one);

  midi_message_t const kTuneRequestMessage = {
    .type = MIDI_TUNE_REQUEST
  };
  midi_specialized_ctx_t tune_request_ctx = { .message_ctx = &message_ctx };
  callbacks.rx.OnTuneRequest = TuneRequestCallback;
  callbacks.rx.tune_request_ctx = &tune_request_ctx;
  TEST_ASSERT_TRUE(MidiCallOnMessageCallback(
      &callbacks, NULL, &kTuneRequestMessage));
  /* Check the generic message set value. */
  TEST_ASSERT_NULL(message_ctx.time);
  TEST_ASSERT_EQUAL(1012, message_ctx.event_id);
  TEST_ASSERT_EQUAL(13, message_ctx.rx_event_id);
  TEST_ASSERT_EQUAL(&kTuneRequestMessage, message_ctx.message);
  /* Check the specialized message set values. */
  TEST_ASSERT_EQUAL(MIDI_TUNE_REQUEST, tune_request_ctx.type);
  TEST_ASSERT_EQUAL(FILLER_ARG, tune_request_ctx.arg_one);

  midi_message_t const kTimingClockMessage = {
    .type = MIDI_TIMING_CLOCK
  };
  midi_specialized_ctx_t timing_clock_ctx = { .message_ctx = &message_ctx };
  callbacks.rx.OnTimingClock = TimingClockCallback;
  callbacks.rx.timing_clock_ctx = &timing_clock_ctx;
  TEST_ASSERT_TRUE(MidiCallOnMessageCallback(
      &callbacks, NULL, &kTimingClockMessage));
  /* Check the generic message set value. */
  TEST_ASSERT_NULL(message_ctx.time);
  TEST_ASSERT_EQUAL(1013, message_ctx.event_id);
  TEST_ASSERT_EQUAL(14, message_ctx.rx_event_id);
  TEST_ASSERT_EQUAL(&kTimingClockMessage, message_ctx.message);
  /* Check the specialized message set values. */
  TEST_ASSERT_EQUAL(MIDI_TIMING_CLOCK, timing_clock_ctx.type);
  TEST_ASSERT_EQUAL(FILLER_ARG, timing_clock_ctx.arg_one);

  midi_message_t const kContinueMessage = {
    .type = MIDI_CONTINUE
  };
  midi_specialized_ctx_t playback_ctx = { .message_ctx = &message_ctx };
  callbacks.rx.OnContinuePlayback = PlaybackCallback;
  callbacks.rx.playback_ctx = &playback_ctx;
  TEST_ASSERT_TRUE(MidiCallOnMessageCallback(
      &callbacks, NULL, &kContinueMessage));
  /* Check the generic message set value. */
  TEST_ASSERT_NULL(message_ctx.time);
  TEST_ASSERT_EQUAL(1014, message_ctx.event_id);
  TEST_ASSERT_EQUAL(15, message_ctx.rx_event_id);
  TEST_ASSERT_EQUAL(&kContinueMessage, message_ctx.message);
  /* Check the specialized message set values. */
  TEST_ASSERT_EQUAL(MIDI_CONTINUE, playback_ctx.type);
  TEST_ASSERT_EQUAL(FILLER_ARG, playback_ctx.arg_one);

  midi_message_t const kActiveSensingMessage = {
    .type = MIDI_ACTIVE_SENSING
  };
  midi_specialized_ctx_t active_sensing_ctx = { .message_ctx = &message_ctx };
  callbacks.rx.OnActiveSensing = ActiveSensingCallback;
  callbacks.rx.active_sensing_ctx = &active_sensing_ctx;
  TEST_ASSERT_TRUE(MidiCallOnMessageCallback(
      &callbacks, NULL, &kActiveSensingMessage));
  /* Check the generic message set value. */
  TEST_ASSERT_NULL(message_ctx.time);
  TEST_ASSERT_EQUAL(1015, message_ctx.event_id);
  TEST_ASSERT_EQUAL(16, message_ctx.rx_event_id);
  TEST_ASSERT_EQUAL(&kActiveSensingMessage, message_ctx.message);
  /* Check the specialized message set values. */
  TEST_ASSERT_EQUAL(MIDI_ACTIVE_SENSING, active_sensing_ctx.type);
  TEST_ASSERT_EQUAL(FILLER_ARG, active_sensing_ctx.arg_one);
}  /* TestMidiCallback_AllMessages */

static void TestMidiCallback_TimeUpdate(void) {
  midi_callbacks_t callbacks;
  TEST_ASSERT_TRUE(MidiInitializeCallbacks(&callbacks));
  callbacks.next_event_id = 1000;

  midi_message_ctx_t message_ctx = {};
  callbacks.rx.OnMessage = MessageCallback;
  callbacks.rx.message_ctx = &message_ctx;
  midi_specialized_ctx_t time_update_ctx = { .message_ctx = &message_ctx };
  callbacks.rx.OnTimeUpdate = TimeUpdateCallback;
  callbacks.rx.time_update_ctx = &time_update_ctx;

  midi_time_t const kTime = {};
  midi_message_t const  kTimeCodeMessage = {
    .type = MIDI_TIME_CODE,
    .time_code = {}
  };
  TEST_ASSERT_FALSE(MidiCallOnMessageCallback(
      &callbacks, &kTime, &kTimeCodeMessage))
  TEST_ASSERT_FALSE(MidiCallOnTimeSynchronizeCallback(
      &callbacks, NULL, MIDI_TIME_FORWARD));
  TEST_ASSERT_EQUAL(1000, callbacks.next_event_id);
  TEST_ASSERT_EQUAL(1, callbacks.rx.next_rx_event_id);
  /* Check the generic message set value. */
  TEST_ASSERT_EQUAL(&kTime, message_ctx.time);
  TEST_ASSERT_EQUAL(1000, message_ctx.event_id);
  TEST_ASSERT_EQUAL(1, message_ctx.rx_event_id);
  TEST_ASSERT_EQUAL(&kTimeCodeMessage, message_ctx.message);
  memset(&message_ctx, 0, sizeof(message_ctx));

  TEST_ASSERT_TRUE(MidiCallOnTimeSynchronizeCallback(
      &callbacks, &kTime, MIDI_TIME_REVERSE));
  /* Check the specialized message set values. */
  TEST_ASSERT_EQUAL(MIDI_NONE, time_update_ctx.type);
  TEST_ASSERT_EQUAL(MIDI_TIME_REVERSE, time_update_ctx.arg_one);
  TEST_ASSERT_EQUAL(&kTime, time_update_ctx.pointer_arg);
}

static void TestMidiCallback_SystemReset(void) {
  midi_callbacks_t callbacks;
  TEST_ASSERT_TRUE(MidiInitializeCallbacks(&callbacks));
  callbacks.next_event_id = 1000;

  midi_message_ctx_t message_ctx = {};
  callbacks.rx.OnMessage = MessageCallback;
  callbacks.rx.message_ctx = &message_ctx;
  midi_specialized_ctx_t system_reset_ctx = { .message_ctx = &message_ctx };
  callbacks.rx.OnSystemReset = SystemResetCallback;
  callbacks.rx.system_reset_ctx = &system_reset_ctx;

  midi_message_t const kSystemResetMessage = {
    .type = MIDI_SYSTEM_RESET
  };
  midi_message_t const kNotSystemResetMessage = {
    .type = MIDI_NOTE_ON,
    .channel = MIDI_CHANNEL_14
  };
  bool_t soft_reset = false;
  memset(&message_ctx, 0, sizeof(midi_message_t));
  TEST_ASSERT_FALSE(MidiCallOnMessageCallback(
      &callbacks, NULL, &kSystemResetMessage));
  TEST_ASSERT_NULL(message_ctx.message);
  TEST_ASSERT_EQUAL(0, message_ctx.event_id);

  TEST_ASSERT_FALSE(MidiCallOnSystemResetCallback(
      &callbacks, NULL, NULL, &soft_reset));
  TEST_ASSERT_FALSE(MidiCallOnSystemResetCallback(
      &callbacks, NULL, &kSystemResetMessage, NULL));
  TEST_ASSERT_FALSE(MidiCallOnSystemResetCallback(
      &callbacks, NULL, &kNotSystemResetMessage, &soft_reset));

  memset(&message_ctx, 0, sizeof(message_ctx));
  soft_reset = false;
  TEST_ASSERT_TRUE(MidiCallOnSystemResetCallback(
      &callbacks, NULL, &kSystemResetMessage, &soft_reset));
  /* Check the generic message set value. */
  TEST_ASSERT_NULL(message_ctx.time);
  TEST_ASSERT_EQUAL(1000, message_ctx.event_id);
  TEST_ASSERT_EQUAL(1, message_ctx.rx_event_id);
  TEST_ASSERT_EQUAL(&kSystemResetMessage, message_ctx.message);
  /* Check the specialized message set values. */
  TEST_ASSERT_EQUAL(MIDI_SYSTEM_RESET, system_reset_ctx.type);
  TEST_ASSERT_TRUE(soft_reset);
  TEST_ASSERT_EQUAL(&soft_reset, system_reset_ctx.pointer_arg);
}

/*
 *  System Exclusive Callbacks
 */

static void SysExMessageCallback(midi_sys_ex_rx_event_t const *sys_ex_event) {
  if (sys_ex_event == NULL || sys_ex_event->rx_event.user_ctx == NULL) return;
  midi_specialized_ctx_t *ctx =
      (midi_specialized_ctx_t *) sys_ex_event->rx_event.user_ctx;
  ctx->sys_ex = sys_ex_event->sys_ex;
  ctx->sys_ex_rx_event_id = sys_ex_event->sys_ex_rx_event_id;
  ctx->device_id = sys_ex_event->device_id;
  midi_message_t const *message = sys_ex_event->rx_event.message;
  if (message == NULL) {
    ctx->type = MESSAGE_NULL;
    return;
  }
  if (message->type != MIDI_SYSTEM_EXCLUSIVE) {
    ctx->type = UNEXPECTED_TYPE;
    return;
  }
  midi_sys_ex_t const *sys_ex = sys_ex_event->sys_ex;
  if (sys_ex == NULL) {
    ctx->type = SYS_EX_NULL;
    return;
  }
  if (sys_ex->device_id != ctx->device_id) {
    ctx->type = BAD_DEVICE_ID;
    return;
  }
  ctx->type = MIDI_SYSTEM_EXCLUSIVE;
}

void MidiCallbackTest(void) {
  RUN_TEST(TestMidiCallbackInitializer);
  RUN_TEST(TestMidiCallback_AllMessages);
  RUN_TEST(TestMidiCallback_TimeUpdate);
  RUN_TEST(TestMidiCallback_SystemReset);
}
