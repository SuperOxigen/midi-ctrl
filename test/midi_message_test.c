/*
 * MIDI Controller - MIDI Message
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
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

void MidiMessageTest(void) {
  RUN_TEST(TestMidiStatusByte_Validator);
  RUN_TEST(TestMidiMessageType_FromStatus);
  RUN_TEST(TestMidiMessageType_Validator);
  RUN_TEST(TestMidiMessageType_ChannelValidator);
  RUN_TEST(TestMidiStatusByte_FromChannelMessage);
  RUN_TEST(TestMidiStatusByte_FromMessage);
}
