/*
 * MIDI Controller - MIDI Serialize / Deserialize Messages Test.
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#include <unity.h>

#include "midi_defs.h"
#include "midi_message.h"
#include "midi_serialize.h"

static uint8_t const kFillerByte = 0x42;
static uint16_t const kFillerWord = 0x2142;

static uint8_t const kNoteOnPacket[] = {
  MIDI_NOTE_ON | MIDI_CHANNEL_4, MIDI_MIDDLE_C,  MIDI_NOTE_ON_VELOCITY
};
static midi_message_t const kNoteOnMessage = {
  .type = MIDI_NOTE_ON,
  .channel = MIDI_CHANNEL_4,
  .note = {
    .key = MIDI_MIDDLE_C,
    .velocity = MIDI_NOTE_ON_VELOCITY
  }
};

static uint8_t const kNoteOffPacket[] = {
  MIDI_NOTE_OFF | MIDI_CHANNEL_5, MIDI_MIDDLE_C,  MIDI_NOTE_ON_VELOCITY
};
static midi_message_t const kNoteOffMessage = {
  .type = MIDI_NOTE_OFF,
  .channel = MIDI_CHANNEL_5,
  .note = {
    .key = MIDI_MIDDLE_C,
    .velocity = MIDI_NOTE_ON_VELOCITY
  }
};

static uint8_t const kKeyPressurePacket[] = {
  MIDI_KEY_PRESSURE | MIDI_CHANNEL_7, MIDI_MIDDLE_C, kFillerByte
};
static midi_message_t const kKeyPressureMessage = {
  .type = MIDI_KEY_PRESSURE,
  .channel = MIDI_CHANNEL_7,
  .note = {
    .key = MIDI_MIDDLE_C,
    .pressure = kFillerByte
  }
};

static uint8_t const kControlChangePacket[] = {
  MIDI_CONTROL_CHANGE | MIDI_CHANNEL_2, MIDI_DECAY_TIME, kFillerByte
};
static midi_message_t const kControlChangeMessage = {
  .type = MIDI_CONTROL_CHANGE,
  .channel = MIDI_CHANNEL_2,
  .control = {
    .number = MIDI_DECAY_TIME,
    .value = kFillerByte
  }
};

static uint8_t const kProgramChangePacket[] = {
  MIDI_PROGRAM_CHANGE | MIDI_CHANNEL_1, MIDI_FLUTE
};
static midi_message_t const kProgramChangeMessage = {
  .type = MIDI_PROGRAM_CHANGE,
  .channel = MIDI_CHANNEL_1,
  .program = MIDI_FLUTE
};

static uint8_t const kChannelPressurePacket[] = {
  MIDI_CHANNEL_PRESSURE | MIDI_CHANNEL_12, kFillerByte,
};
static midi_message_t const kChannelPressureMessage = {
  .type = MIDI_CHANNEL_PRESSURE,
  .channel = MIDI_CHANNEL_12,
  .pressure = kFillerByte
};

static uint8_t const kPitchWheelPacket[] = {
  MIDI_PITCH_WHEEL | MIDI_CHANNEL_16, kFillerByte, kFillerByte
};
static midi_message_t const kPitchWheelMessage = {
  .type = MIDI_PITCH_WHEEL,
  .channel = MIDI_CHANNEL_16,
  .pitch = kFillerWord
};

static uint8_t const kSystemExclusivePacket[] = { MIDI_SYSTEM_EXCLUSIVE };
static midi_message_t const kSystemExclusiveMessage = {
  .type = MIDI_SYSTEM_EXCLUSIVE
};

static uint8_t const kEndSystemExclusivePacket[] = {
  MIDI_END_SYSTEM_EXCLUSIVE
};
static midi_message_t const kEndSystemExclusiveMessage = {
  .type = MIDI_END_SYSTEM_EXCLUSIVE
};

static uint8_t const kTimeCodePacket[] = { MIDI_TIME_CODE,
    MIDI_HOURS_COUNT_LSN | 0x05 };
static midi_message_t const kTimeCodeMessage = {
  .type = MIDI_TIME_CODE,
  .time_code = {
    .type = MIDI_HOURS_COUNT_LSN,
    .value = 0x05
  }
};

static uint8_t const kSongPositionPointerPacket[] = {
  MIDI_SONG_POSITION_POINTER, kFillerByte, kFillerByte
};
static midi_message_t const kSongPositionPointerMessage = {
  .type = MIDI_SONG_POSITION_POINTER,
  /* .song_position = kFillerWord */
};

static uint8_t const kSongSelectPacket[] = { MIDI_SONG_SELECT, kFillerByte };
static midi_message_t const kSongSelectMessage = {
  .type = MIDI_SONG_SELECT,
  /* .song_number = kFillerByte */
};

static uint8_t const kTuneRequestPacket[] = { MIDI_TUNE_REQUEST };
static midi_message_t const kTuneRequestMessage = {
  .type = MIDI_TUNE_REQUEST,
};

static uint8_t const kTimingClockPacket[] = { MIDI_TIMING_CLOCK };
static midi_message_t const kTimingClockMessage = {
  .type = MIDI_TIMING_CLOCK
};
static uint8_t const kStartPacket[] = { MIDI_START };
static midi_message_t const kStartMessage = { .type = MIDI_START };
static uint8_t const kContinuePacket[] = { MIDI_CONTINUE };
static midi_message_t const kContinueMessage = { .type = MIDI_CONTINUE };
static uint8_t const kStopPacket[] = { MIDI_STOP };
static midi_message_t const kStopMessage = { .type = MIDI_STOP };
static uint8_t const kActiveSensingPacket[] = { MIDI_ACTIVE_SENSING };
static midi_message_t const kActiveSensingMessage = {
  .type = MIDI_ACTIVE_SENSING,
};
static uint8_t const kSystemResetPacket[] = { MIDI_SYSTEM_RESET };
static midi_message_t const kSystemResetMessage = {
  .type = MIDI_SYSTEM_RESET,
};

static midi_message_t const kNoneMessage = {
  .type = MIDI_NONE
};
static uint8_t const kUnknownPacket[] = { 0xF4 };

static void TestMidiSerialize_Invalid(void) {
  uint8_t buffer[4];
  TEST_ASSERT_EQUAL(
      0, MidiSerializeMessage(NULL, false, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL(
      0, MidiSerializeMessage(&kNoteOnMessage, false, NULL, sizeof(buffer)));
  TEST_ASSERT_EQUAL(
      0, MidiSerializeMessage(&kNoneMessage, false, buffer, sizeof(buffer)));
}

static void TestMidiSerialize_KnownMessage(void) {
  uint8_t buffer[4];
  TEST_ASSERT_EQUAL(
      sizeof(kNoteOffPacket),
      MidiSerializeMessage(&kNoteOffMessage, false, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_MEMORY(kNoteOffPacket, buffer, sizeof(kNoteOffPacket));

  TEST_ASSERT_EQUAL(
      sizeof(kNoteOnPacket),
      MidiSerializeMessage(&kNoteOnMessage, false, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_MEMORY(kNoteOnPacket, buffer, sizeof(kNoteOnPacket));

  TEST_ASSERT_EQUAL(
      sizeof(kKeyPressurePacket),
      MidiSerializeMessage(
          &kKeyPressureMessage, false, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_MEMORY(kKeyPressurePacket, buffer, sizeof(kKeyPressurePacket));

  TEST_ASSERT_EQUAL(
      sizeof(kControlChangePacket),
      MidiSerializeMessage(
          &kControlChangeMessage, false, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_MEMORY(kControlChangePacket, buffer, sizeof(kControlChangePacket));

  TEST_ASSERT_EQUAL(
      sizeof(kProgramChangePacket),
      MidiSerializeMessage(
          &kProgramChangeMessage, false, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_MEMORY(
      kProgramChangePacket, buffer, sizeof(kProgramChangePacket));

  TEST_ASSERT_EQUAL(
      sizeof(kPitchWheelPacket),
      MidiSerializeMessage(
          &kPitchWheelMessage, false, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_MEMORY(
      kPitchWheelPacket, buffer, sizeof(kPitchWheelPacket));

  // TEST_ASSERT_EQUAL(
  //     sizeof(kSystemExclusivePacket),
  //     MidiSerializeMessage(
  //         &kSystemExclusiveMessage, false, buffer, sizeof(buffer)));
  // TEST_ASSERT_EQUAL_MEMORY(
  //     kSystemExclusivePacket, buffer, sizeof(kSystemExclusivePacket));

  TEST_ASSERT_EQUAL(
      sizeof(kEndSystemExclusivePacket),
      MidiSerializeMessage(
          &kEndSystemExclusiveMessage, false, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_MEMORY(
      kEndSystemExclusivePacket, buffer, sizeof(kEndSystemExclusivePacket));

  TEST_ASSERT_EQUAL(
      sizeof(kTimeCodePacket),
      MidiSerializeMessage(&kTimeCodeMessage, false, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_MEMORY(kTimeCodePacket, buffer, sizeof(kTimeCodePacket));

  // TEST_ASSERT_EQUAL(
  //     sizeof(kSongPositionPointerPacket),
  //     MidiSerializeMessage(
  //         &kSongPositionPointerMessage, false, buffer, sizeof(buffer)));
  // TEST_ASSERT_EQUAL_MEMORY(
  //     kSongPositionPointerPacket, buffer, sizeof(kSongPositionPointerPacket));

  // TEST_ASSERT_EQUAL(
  //     sizeof(kSongSelectPacket),
  //     MidiSerializeMessage(
  //         &kSongSelectMessage, false, buffer, sizeof(buffer)));
  // TEST_ASSERT_EQUAL_MEMORY(
  //     kSongSelectPacket, buffer, sizeof(kSongSelectPacket));

  TEST_ASSERT_EQUAL(
      sizeof(kTuneRequestPacket),
      MidiSerializeMessage(
          &kTuneRequestMessage, false, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_MEMORY(
      kTuneRequestPacket, buffer, sizeof(kTuneRequestPacket));

  TEST_ASSERT_EQUAL(
      sizeof(kTimingClockPacket),
      MidiSerializeMessage(
          &kTimingClockMessage, false, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_MEMORY(
      kTimingClockPacket, buffer, sizeof(kTimingClockPacket));

  TEST_ASSERT_EQUAL(
      sizeof(kStartPacket),
      MidiSerializeMessage(&kStartMessage, false, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_MEMORY(kStartPacket, buffer, sizeof(kStartPacket));

  TEST_ASSERT_EQUAL(
      sizeof(kContinuePacket),
      MidiSerializeMessage(&kContinueMessage, false, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_MEMORY(kContinuePacket, buffer, sizeof(kContinuePacket));

  TEST_ASSERT_EQUAL(
      sizeof(kStopPacket),
      MidiSerializeMessage(&kStopMessage, false, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_MEMORY(kStopPacket, buffer, sizeof(kStopPacket));

  TEST_ASSERT_EQUAL(
      sizeof(kActiveSensingPacket),
      MidiSerializeMessage(
          &kActiveSensingMessage, false, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_MEMORY(
      kActiveSensingPacket, buffer, sizeof(kActiveSensingPacket));

  TEST_ASSERT_EQUAL(
      sizeof(kSystemResetPacket),
      MidiSerializeMessage(
          &kSystemResetMessage, false, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_MEMORY(
      kSystemResetPacket, buffer, sizeof(kSystemResetPacket));
}

static void TestMidiDeserialize_Invalid(void) {
  midi_message_t message;
  TEST_ASSERT_EQUAL(
      0, MidiDeserializeMessage(NULL, 3, MIDI_NONE, &message));
  TEST_ASSERT_EQUAL(
      0, MidiDeserializeMessage(kNoteOffPacket, 0, MIDI_NONE, &message));
  TEST_ASSERT_EQUAL(
      0, MidiDeserializeMessage(
          kNoteOffPacket, sizeof(kNoteOffPacket), MIDI_NONE, NULL));
}

static void TestMidiDeserialize_KnownMessage(void) {
  midi_message_t message;
  TEST_ASSERT_EQUAL(
      sizeof(kNoteOffPacket),
      MidiDeserializeMessage(
          kNoteOffPacket, sizeof(kNoteOffPacket), MIDI_NONE, &message));
  TEST_ASSERT_EQUAL(kNoteOffMessage.type, message.type);
  TEST_ASSERT_EQUAL(kNoteOffMessage.channel, message.channel);
  TEST_ASSERT_EQUAL(kNoteOffMessage.note.key, message.note.key);
  TEST_ASSERT_EQUAL(kNoteOffMessage.note.velocity, message.note.velocity);

  TEST_ASSERT_EQUAL(
      sizeof(kNoteOnPacket),
      MidiDeserializeMessage(
          kNoteOnPacket, sizeof(kNoteOnPacket), MIDI_NONE, &message));
  TEST_ASSERT_EQUAL(kNoteOnMessage.type, message.type);
  TEST_ASSERT_EQUAL(kNoteOnMessage.channel, message.channel);
  TEST_ASSERT_EQUAL(kNoteOnMessage.note.key, message.note.key);
  TEST_ASSERT_EQUAL(kNoteOnMessage.note.velocity, message.note.velocity);

  TEST_ASSERT_EQUAL(
      sizeof(kKeyPressurePacket),
      MidiDeserializeMessage(
          kKeyPressurePacket, sizeof(kKeyPressurePacket), MIDI_NONE, &message));
  TEST_ASSERT_EQUAL(kKeyPressureMessage.type, message.type);
  TEST_ASSERT_EQUAL(kKeyPressureMessage.channel, message.channel);
  TEST_ASSERT_EQUAL(kKeyPressureMessage.note.key, message.note.key);
  TEST_ASSERT_EQUAL(kKeyPressureMessage.note.pressure, message.note.pressure);

  TEST_ASSERT_EQUAL(
      sizeof(kControlChangePacket),
      MidiDeserializeMessage(
          kControlChangePacket, sizeof(kControlChangePacket),
          MIDI_NONE, &message));
  TEST_ASSERT_EQUAL(kControlChangeMessage.type, message.type);
  TEST_ASSERT_EQUAL(kControlChangeMessage.channel, message.channel);
  TEST_ASSERT_EQUAL(kControlChangeMessage.control.number, message.control.number);
  TEST_ASSERT_EQUAL(kControlChangeMessage.control.value, message.control.value);

  TEST_ASSERT_EQUAL(
      sizeof(kProgramChangePacket),
      MidiDeserializeMessage(
          kProgramChangePacket, sizeof(kProgramChangePacket),
          MIDI_NONE, &message));
  TEST_ASSERT_EQUAL(kProgramChangeMessage.type, message.type);
  TEST_ASSERT_EQUAL(kProgramChangeMessage.channel, message.channel);
  TEST_ASSERT_EQUAL(kProgramChangeMessage.program, message.program);

  TEST_ASSERT_EQUAL(
      sizeof(kPitchWheelPacket),
      MidiDeserializeMessage(
          kPitchWheelPacket, sizeof(kPitchWheelPacket),
          MIDI_NONE, &message));
  TEST_ASSERT_EQUAL(kPitchWheelMessage.type, message.type);
  TEST_ASSERT_EQUAL(kPitchWheelMessage.channel, message.channel);
  TEST_ASSERT_EQUAL(kPitchWheelMessage.pitch, message.pitch);

  TEST_ASSERT_EQUAL(
      sizeof(kSystemExclusivePacket),
      MidiDeserializeMessage(
          kSystemExclusivePacket, sizeof(kSystemExclusivePacket),
          MIDI_NONE, &message));
  TEST_ASSERT_EQUAL(kSystemExclusiveMessage.type, message.type);
  // TEST_ASSERT_EQUAL_MEMORY(
  //     kSystemExclusiveMessage.manufacture_id, message.manufacture_id,
  //     sizeof(message.manufacture_id));

  TEST_ASSERT_EQUAL(
      sizeof(kEndSystemExclusivePacket),
      MidiDeserializeMessage(
          kEndSystemExclusivePacket, sizeof(kEndSystemExclusivePacket),
          MIDI_NONE, &message));
  TEST_ASSERT_EQUAL(kEndSystemExclusiveMessage.type, message.type);

  TEST_ASSERT_EQUAL(
      sizeof(kTimeCodePacket),
      MidiDeserializeMessage(
          kTimeCodePacket, sizeof(kTimeCodePacket),
          MIDI_NONE, &message));
  TEST_ASSERT_EQUAL(kTimeCodeMessage.type, message.type);
  TEST_ASSERT_EQUAL_MEMORY(
      &kTimeCodeMessage.time_code, &message.time_code,
      sizeof(midi_time_code_t));

  TEST_ASSERT_EQUAL(
      sizeof(kSongPositionPointerPacket),
      MidiDeserializeMessage(
          kSongPositionPointerPacket, sizeof(kSongPositionPointerPacket),
          MIDI_NONE, &message));
  // TEST_ASSERT_EQUAL(kSongPositionPointerMessage.type, message.type);
  // TEST_ASSERT_EQUAL(
  //     kSongPositionPointerMessage.song_position, message.song_position);

  TEST_ASSERT_EQUAL(
      sizeof(kSongSelectPacket),
      MidiDeserializeMessage(
          kSongSelectPacket, sizeof(kSongSelectPacket),
          MIDI_NONE, &message));
  // TEST_ASSERT_EQUAL(kSongSelectMessage.type, message.type);
  // TEST_ASSERT_EQUAL(kSongSelectMessage.song_number, message.song_number);

  TEST_ASSERT_EQUAL(
      sizeof(kTuneRequestPacket),
      MidiDeserializeMessage(
          kTuneRequestPacket, sizeof(kTuneRequestPacket),
          MIDI_NONE, &message));
  TEST_ASSERT_EQUAL(kTuneRequestMessage.type, message.type);

  TEST_ASSERT_EQUAL(
      sizeof(kTimingClockPacket),
      MidiDeserializeMessage(
          kTimingClockPacket, sizeof(kTimingClockPacket),
          MIDI_NONE, &message));
  TEST_ASSERT_EQUAL(kTimingClockMessage.type, message.type);

  TEST_ASSERT_EQUAL(
      sizeof(kStartPacket),
      MidiDeserializeMessage(
          kStartPacket, sizeof(kStartPacket),
          MIDI_NONE, &message));
  TEST_ASSERT_EQUAL(kStartMessage.type, message.type);

  TEST_ASSERT_EQUAL(
      sizeof(kContinuePacket),
      MidiDeserializeMessage(
          kContinuePacket, sizeof(kContinuePacket),
          MIDI_NONE, &message));
  TEST_ASSERT_EQUAL(kContinueMessage.type, message.type);

  TEST_ASSERT_EQUAL(
      sizeof(kStopPacket),
      MidiDeserializeMessage(
          kStopPacket, sizeof(kStopPacket),
          MIDI_NONE, &message));
  TEST_ASSERT_EQUAL(kStopMessage.type, message.type);

  TEST_ASSERT_EQUAL(
      sizeof(kActiveSensingPacket),
      MidiDeserializeMessage(
          kActiveSensingPacket, sizeof(kActiveSensingPacket),
          MIDI_NONE, &message));
  TEST_ASSERT_EQUAL(kActiveSensingMessage.type, message.type);

  TEST_ASSERT_EQUAL(
      sizeof(kSystemResetPacket),
      MidiDeserializeMessage(
          kSystemResetPacket, sizeof(kSystemResetPacket),
          MIDI_NONE, &message));
  TEST_ASSERT_EQUAL(kSystemResetMessage.type, message.type);
}

void MidiSerializeTest(void) {
  RUN_TEST(TestMidiSerialize_Invalid);
  RUN_TEST(TestMidiSerialize_KnownMessage);
  RUN_TEST(TestMidiDeserialize_Invalid);
  RUN_TEST(TestMidiDeserialize_KnownMessage);
}
