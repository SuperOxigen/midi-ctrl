/*
 * MIDI Controller - MIDI Message
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#include <string.h>
#include <unity.h>

#include "midi_defs.h"
#include "midi_message.h"

static void TestMidiStatusByte_Validator(void) {
  TEST_ASSERT_FALSE(MidiIsStatusByte(0x00));
  TEST_ASSERT_FALSE(MidiIsStatusByte(0x01));
  TEST_ASSERT_FALSE(MidiIsStatusByte(0x10));
  TEST_ASSERT_FALSE(MidiIsStatusByte(0x7F));

  TEST_ASSERT_TRUE(MidiIsStatusByte(0x80));
  TEST_ASSERT_TRUE(MidiIsStatusByte(0xF0));
  TEST_ASSERT_TRUE(MidiIsStatusByte(0xFF));
}

static void TestMidiMessageType_FromStatus(void) {
  TEST_ASSERT_EQUAL(MIDI_NONE, MidiStatusToMessageType(MIDI_NONE));
  for (midi_channel_number_t i = MIDI_CHANNEL_1; i <= MIDI_CHANNEL_16; ++i) {
    TEST_ASSERT_EQUAL(MIDI_NOTE_OFF, MidiStatusToMessageType(MIDI_NOTE_OFF | i));
    TEST_ASSERT_EQUAL(MIDI_PITCH_WHEEL,
        MidiStatusToMessageType(MIDI_PITCH_WHEEL | i));
  }
  TEST_ASSERT_EQUAL(MIDI_SYSTEM_EXCLUSIVE,
      MidiStatusToMessageType(MIDI_SYSTEM_EXCLUSIVE));
  TEST_ASSERT_EQUAL(MIDI_TUNE_REQUEST,
      MidiStatusToMessageType(MIDI_TUNE_REQUEST));
  TEST_ASSERT_EQUAL(MIDI_SYSTEM_RESET,
      MidiStatusToMessageType(MIDI_SYSTEM_RESET));
}

static void TestMidiMessageType_Validator(void) {
  TEST_ASSERT_FALSE(MidiIsValidMessageType(MIDI_NONE));
  /* Reject channels */
  TEST_ASSERT_FALSE(MidiIsValidMessageType(MIDI_NOTE_ON | MIDI_CHANNEL_2));
  TEST_ASSERT_FALSE(MidiIsValidMessageType(MIDI_NOTE_OFF | MIDI_CHANNEL_16));

  TEST_ASSERT_TRUE(MidiIsValidMessageType(MIDI_NOTE_ON));
  TEST_ASSERT_TRUE(MidiIsValidMessageType(MIDI_NOTE_OFF));
  TEST_ASSERT_TRUE(MidiIsValidMessageType(MIDI_PROGRAM_CHANGE));
}

static void TestMidiMessageType_ChannelValidator(void) {
  TEST_ASSERT_FALSE(MidiIsChannelMessageType(MIDI_NONE));

  TEST_ASSERT_TRUE(MidiIsChannelMessageType(MIDI_NOTE_OFF));
  TEST_ASSERT_TRUE(MidiIsChannelMessageType(MIDI_NOTE_ON));
  TEST_ASSERT_TRUE(MidiIsChannelMessageType(MIDI_KEY_PRESSURE));
  TEST_ASSERT_TRUE(MidiIsChannelMessageType(MIDI_PITCH_WHEEL));

  TEST_ASSERT_FALSE(MidiIsChannelMessageType(MIDI_TIME_CODE));
  TEST_ASSERT_FALSE(MidiIsChannelMessageType(MIDI_SYSTEM_RESET));
  TEST_ASSERT_FALSE(MidiIsChannelMessageType(MIDI_TUNE_REQUEST));
}

static void TestMidiStatusByte_FromChannelMessage(void) {
  TEST_ASSERT_EQUAL(
      MIDI_NONE,
      MidiChannelStatusByte(MIDI_NONE, MIDI_CHANNEL_2));
  TEST_ASSERT_EQUAL(
      MIDI_NONE,
      MidiChannelStatusByte(MIDI_SYSTEM_EXCLUSIVE, MIDI_CHANNEL_16));
  TEST_ASSERT_EQUAL(
      MIDI_NONE,
      MidiChannelStatusByte(MIDI_SYSTEM_RESET, MIDI_CHANNEL_3));

  TEST_ASSERT_EQUAL(
      MIDI_NOTE_ON | MIDI_CHANNEL_4,
      MidiChannelStatusByte(MIDI_NOTE_ON, MIDI_CHANNEL_4));
  TEST_ASSERT_EQUAL(
      MIDI_KEY_PRESSURE | MIDI_CHANNEL_8,
      MidiChannelStatusByte(MIDI_KEY_PRESSURE, MIDI_CHANNEL_8));
  TEST_ASSERT_EQUAL(
      MIDI_PROGRAM_CHANGE | MIDI_CHANNEL_12,
      MidiChannelStatusByte(MIDI_PROGRAM_CHANGE, MIDI_CHANNEL_12));
}

static void TestMidiStatusByte_FromMessage(void) {
  midi_message_t message = {};
  message.type = MIDI_NOTE_ON;
  message.channel = MIDI_CHANNEL_8;
  TEST_ASSERT_EQUAL(
      MIDI_NOTE_ON | MIDI_CHANNEL_8,
      MidiMessageStatus(&message));

  message.type = MIDI_KEY_PRESSURE;
  message.channel = MIDI_CHANNEL_4;
  TEST_ASSERT_EQUAL(
      MIDI_KEY_PRESSURE | MIDI_CHANNEL_4,
      MidiMessageStatus(&message));

  message.type = MIDI_PITCH_WHEEL;
  message.channel = MIDI_CHANNEL_14;
  TEST_ASSERT_EQUAL(
      MIDI_PITCH_WHEEL | MIDI_CHANNEL_14,
      MidiMessageStatus(&message));

  message.type = MIDI_TIME_CODE;
  MidiInitializeTimeCode(&message.time_code, MIDI_FRAME_COUNT_LSN, 0x00);
  TEST_ASSERT_EQUAL(MIDI_TIME_CODE, MidiMessageStatus(&message));

  message.type = MIDI_START;
  TEST_ASSERT_EQUAL(MIDI_START, MidiMessageStatus(&message));

  message.type = MIDI_SYSTEM_RESET;
  TEST_ASSERT_EQUAL(MIDI_SYSTEM_RESET, MidiMessageStatus(&message));
}

static void TestMidiMessage_Initializer(void) {
  midi_message_t message;
  midi_note_t note = {
    .key = MIDI_MIDDLE_C,
    .velocity = MIDI_NOTE_ON_VELOCITY
  };
  TEST_ASSERT_FALSE(MidiNoteOnMessage(NULL, MIDI_CHANNEL_6, &note));
  TEST_ASSERT_FALSE(MidiNoteOnMessage(&message, 0x16, &note));
  TEST_ASSERT_FALSE(MidiNoteOnMessage(&message, MIDI_CHANNEL_6, NULL));
  note.velocity = 0x80;
  TEST_ASSERT_FALSE(MidiNoteOnMessage(&message, MIDI_CHANNEL_6, &note));
  note.velocity = MIDI_NOTE_ON_VELOCITY;
  TEST_ASSERT_TRUE(MidiNoteOnMessage(&message, MIDI_CHANNEL_6, &note));
  TEST_ASSERT_EQUAL(MIDI_NOTE_ON, message.type);
  TEST_ASSERT_EQUAL(MIDI_CHANNEL_6, message.channel);
  TEST_ASSERT_EQUAL(MIDI_MIDDLE_C, message.note.key);
  TEST_ASSERT_EQUAL(MIDI_NOTE_ON_VELOCITY, message.note.velocity);

  memset(&message, 0, sizeof(message));
  TEST_ASSERT_TRUE(MidiNoteOffMessage(&message, MIDI_CHANNEL_6, &note));
  TEST_ASSERT_EQUAL(MIDI_NOTE_OFF, message.type);
  TEST_ASSERT_EQUAL(MIDI_CHANNEL_6, message.channel);
  TEST_ASSERT_EQUAL(MIDI_MIDDLE_C, message.note.key);
  TEST_ASSERT_EQUAL(MIDI_NOTE_ON_VELOCITY, message.note.velocity);

  note = (midi_note_t) {
    .key = MIDI_MIDDLE_C,
    .pressure = 0x10
  };
  TEST_ASSERT_FALSE(MidiKeyPressureMessage(NULL, MIDI_CHANNEL_11, &note));
  TEST_ASSERT_FALSE(MidiKeyPressureMessage(&message, 0x16, &note));
  TEST_ASSERT_FALSE(MidiKeyPressureMessage(&message, MIDI_CHANNEL_11, NULL));
  note.pressure = 0x80;
  TEST_ASSERT_FALSE(MidiKeyPressureMessage(
      &message, MIDI_CHANNEL_11, &note));
  note.pressure = 0x20;
  TEST_ASSERT_TRUE(MidiKeyPressureMessage(
      &message, MIDI_CHANNEL_11, &note));
  TEST_ASSERT_EQUAL(MIDI_KEY_PRESSURE, message.type);
  TEST_ASSERT_EQUAL(MIDI_CHANNEL_11, message.channel);
  TEST_ASSERT_EQUAL(MIDI_MIDDLE_C, message.note.key);
  TEST_ASSERT_EQUAL(0x20, message.note.pressure);

  midi_control_change_t control = {
    .number = MIDI_PORTAMENTO_CONTROL,
    .value = MIDI_CONTROL_ON
  };
  TEST_ASSERT_FALSE(MidiControlChangeMessage(NULL, MIDI_CHANNEL_7, &control));
  TEST_ASSERT_FALSE(MidiControlChangeMessage(&message, 0x16, &control));
  TEST_ASSERT_FALSE(MidiControlChangeMessage(&message, MIDI_CHANNEL_7, NULL));
  control.value = 0x80;
  TEST_ASSERT_FALSE(MidiControlChangeMessage(
      &message, MIDI_CHANNEL_7, &control));
  control.value = MIDI_CONTROL_ON;
  TEST_ASSERT_TRUE(MidiControlChangeMessage(
      &message, MIDI_CHANNEL_7, &control));
  TEST_ASSERT_EQUAL(MIDI_CONTROL_CHANGE, message.type);
  TEST_ASSERT_EQUAL(MIDI_CHANNEL_7, message.channel);
  TEST_ASSERT_EQUAL(MIDI_PORTAMENTO_CONTROL, message.control.number);
  TEST_ASSERT_EQUAL(MIDI_CONTROL_ON, message.control.value);

  TEST_ASSERT_FALSE(MidiProgramChangeMessage(
      NULL, MIDI_CHANNEL_13, MIDI_TUBULAR_BELLS));
  TEST_ASSERT_FALSE(MidiProgramChangeMessage(
      &message, 0x16, MIDI_TUBULAR_BELLS));
  TEST_ASSERT_FALSE(MidiProgramChangeMessage(
      &message, MIDI_CHANNEL_13, MIDI_TUBULAR_BELLS | 0x80));
  TEST_ASSERT_TRUE(MidiProgramChangeMessage(
      &message, MIDI_CHANNEL_13, MIDI_TUBULAR_BELLS));
  TEST_ASSERT_EQUAL(MIDI_PROGRAM_CHANGE, message.type);
  TEST_ASSERT_EQUAL(MIDI_CHANNEL_13, message.channel);
  TEST_ASSERT_EQUAL(MIDI_TUBULAR_BELLS, message.program);

  TEST_ASSERT_FALSE(MidiChannelPressureMessage(
      NULL, MIDI_CHANNEL_6, 0x0A));
  TEST_ASSERT_FALSE(MidiChannelPressureMessage(
      &message, 0x16, 0x0A));
  TEST_ASSERT_FALSE(MidiChannelPressureMessage(
      &message, MIDI_CHANNEL_6, 0x8A));
  TEST_ASSERT_TRUE(MidiChannelPressureMessage(
      &message, MIDI_CHANNEL_6, 0x0A));
  TEST_ASSERT_EQUAL(MIDI_CHANNEL_PRESSURE, message.type);
  TEST_ASSERT_EQUAL(MIDI_CHANNEL_6, message.channel);
  TEST_ASSERT_EQUAL(0x0A, message.pressure);

  TEST_ASSERT_FALSE(MidiPitchWheelMessage(
      NULL, MIDI_CHANNEL_3, 0x0ABC));
  TEST_ASSERT_FALSE(MidiPitchWheelMessage(
      &message, 0x16, 0x0ABC));
  TEST_ASSERT_FALSE(MidiPitchWheelMessage(
      &message, MIDI_CHANNEL_3, 0x9ABC));
  TEST_ASSERT_TRUE(MidiPitchWheelMessage(
      &message, MIDI_CHANNEL_3, 0x0ABC));
  TEST_ASSERT_EQUAL(MIDI_PITCH_WHEEL, message.type);
  TEST_ASSERT_EQUAL(MIDI_CHANNEL_3, message.channel);
  TEST_ASSERT_EQUAL(0x0ABC, message.pitch);

  uint8_t const id[] = {0x00, 0x50, 0x00};
  TEST_ASSERT_FALSE(MidiSystemExclusiveMessage(NULL, id));
  TEST_ASSERT_TRUE(MidiSystemExclusiveMessage(&message, id));
  TEST_ASSERT_EQUAL(MIDI_SYSTEM_EXCLUSIVE, message.type);
  TEST_ASSERT_EQUAL_MEMORY(id, message.sys_ex.id, sizeof(id));

  midi_time_code_t time_code = {
    .type = MIDI_SECONDS_COUNT_MSN,
    .value = 0x3  /*0x30 = 48*/
  };
  TEST_ASSERT_FALSE(MidiTimeCodeMessage(NULL, &time_code));
  TEST_ASSERT_FALSE(MidiTimeCodeMessage(&message, NULL));
  time_code.value = 0x4;
  TEST_ASSERT_FALSE(MidiTimeCodeMessage(&message, &time_code));
  time_code.value = 0x3;
  TEST_ASSERT_TRUE(MidiTimeCodeMessage(&message, &time_code));
  TEST_ASSERT_EQUAL(MIDI_TIME_CODE, message.type);
  TEST_ASSERT_EQUAL(MIDI_SECONDS_COUNT_MSN, message.time_code.type);
  TEST_ASSERT_EQUAL(0x3, message.time_code.value);
}

void MidiMessageTest(void) {
  RUN_TEST(TestMidiStatusByte_Validator);
  RUN_TEST(TestMidiMessageType_FromStatus);
  RUN_TEST(TestMidiMessageType_Validator);
  RUN_TEST(TestMidiMessageType_ChannelValidator);
  RUN_TEST(TestMidiStatusByte_FromChannelMessage);
  RUN_TEST(TestMidiStatusByte_FromMessage);

  RUN_TEST(TestMidiMessage_Initializer);
}
