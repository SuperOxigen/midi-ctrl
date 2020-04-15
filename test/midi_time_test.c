/*
 * MIDI Controller - MIDI Time and Time Code Test
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#include <unity.h>

#include "midi_defs.h"
#include "midi_time.h"

/* MIDI Time Code Test */

static void TestMidiTimeCode_Validators(void) {
  TEST_ASSERT_FALSE(MidiIsValidTimeCode(NULL));
  midi_time_code_t time_code = {};
  TEST_ASSERT_TRUE(MidiIsValidTimeCode(&time_code));
  time_code.type = 0x04;  /* Bad type code. */
  TEST_ASSERT_FALSE(MidiIsValidTimeCode(&time_code));
  time_code.type = MIDI_MINUTES_COUNT_MSN;
  TEST_ASSERT_TRUE(MidiIsValidTimeCode(&time_code));
  time_code.type = MIDI_FRAME_COUNT_LSN;
  TEST_ASSERT_TRUE(MidiIsValidTimeCode(&time_code));
  time_code.value = 0x0F;  /* Max value for frame count LSN */
  TEST_ASSERT_TRUE(MidiIsValidTimeCode(&time_code));
  /* Bad value for any type. */
  time_code.value = 0x10;
  TEST_ASSERT_FALSE(MidiIsValidTimeCode(&time_code));
  time_code.value = 0xF4;
  TEST_ASSERT_FALSE(MidiIsValidTimeCode(&time_code));

  time_code.type = MIDI_FRAME_COUNT_MSN;
  time_code.value = 0x01;  /* Max value for frame count MSN */
  TEST_ASSERT_TRUE(MidiIsValidTimeCode(&time_code));
  time_code.value = 0x02;  /* Bad value for frame count MSN */
  TEST_ASSERT_FALSE(MidiIsValidTimeCode(&time_code));

  time_code.type = MIDI_SECONDS_COUNT_MSN;
  time_code.value = 0x03;  /* Max value for seconds count MSN */
  TEST_ASSERT_TRUE(MidiIsValidTimeCode(&time_code));
  time_code.value = 0x04;  /* Bad value for seconds count MSN */
  TEST_ASSERT_FALSE(MidiIsValidTimeCode(&time_code));

  time_code.type = MIDI_MINUTES_COUNT_MSN;
  time_code.value = 0x03;  /* Max value for minutes count MSN */
  TEST_ASSERT_TRUE(MidiIsValidTimeCode(&time_code));
  time_code.value = 0x04;  /* Bad value for minutes count MSN */
  TEST_ASSERT_FALSE(MidiIsValidTimeCode(&time_code));

  time_code.type = MIDI_HOURS_COUNT_MSN;
  time_code.value = 0x01;  /* Max value for hours count MSN */
  TEST_ASSERT_TRUE(MidiIsValidTimeCode(&time_code));
  time_code.value = 0x07;  /* Max value for hours count MSN with FPS */
  TEST_ASSERT_TRUE(MidiIsValidTimeCode(&time_code));
  time_code.value = 0x08;  /* Bad value for frame count MSN with FPS */
  TEST_ASSERT_FALSE(MidiIsValidTimeCode(&time_code));
}

static void TestMidiTimeCode_Initializer_Invalid(void) {
  TEST_ASSERT_FALSE(MidiInitializeTimeCode(NULL, MIDI_FRAME_COUNT_LSN, 0x00));
  midi_time_code_t time_code = {};
  TEST_ASSERT_FALSE(MidiInitializeTimeCode(
      &time_code, MIDI_FRAME_COUNT_LSN, 0xF0 /* Bad value */));
  TEST_ASSERT_FALSE(MidiInitializeTimeCode(
      &time_code, 0xF0 /* Bad value */, 0x00));
  TEST_ASSERT_FALSE(MidiInitializeTimeCode(
      &time_code, 0x0F /* Bad value */, 0x00));
}

static void TestMidiTimeCode_Initializer_Valid(void) {
  midi_time_code_t time_code = {};
  TEST_ASSERT_TRUE(MidiInitializeTimeCode(
      &time_code, MIDI_FRAME_COUNT_LSN, 0x00));
  TEST_ASSERT_EQUAL(MIDI_FRAME_COUNT_LSN, time_code.type);
  TEST_ASSERT_EQUAL(0x00, time_code.value);

  TEST_ASSERT_TRUE(MidiInitializeTimeCode(
      &time_code, MIDI_SECONDS_COUNT_LSN, 0x0F));
  TEST_ASSERT_EQUAL(MIDI_SECONDS_COUNT_LSN, time_code.type);
  TEST_ASSERT_EQUAL(0x0F, time_code.value);

  TEST_ASSERT_TRUE(MidiInitializeTimeCode(
      &time_code, MIDI_SECONDS_COUNT_MSN, 0x03));
  TEST_ASSERT_EQUAL(MIDI_SECONDS_COUNT_MSN, time_code.type);
  TEST_ASSERT_EQUAL(0x03, time_code.value);

  TEST_ASSERT_TRUE(MidiInitializeTimeCode(
      &time_code, MIDI_HOURS_COUNT_MSN, MIDI_30_FPS_DROP_FRAME >> 4));
  TEST_ASSERT_EQUAL(MIDI_HOURS_COUNT_MSN, time_code.type);
  TEST_ASSERT_EQUAL(MIDI_30_FPS_DROP_FRAME >> 4, time_code.value);
}

static void TestMidiTimeCode_Serialize_Invalid(void) {
  midi_time_code_t time_code = {};
  uint8_t data;
  /* Invalid parameters. */
  TEST_ASSERT_FALSE(MidiSerializeTimeCode(NULL, NULL));
  TEST_ASSERT_FALSE(MidiSerializeTimeCode(&time_code, NULL));
  TEST_ASSERT_FALSE(MidiSerializeTimeCode(NULL, &data));
  /* Invalid time code. */
  time_code.type = 0xF0;
  time_code.value = 0x00;
  TEST_ASSERT_FALSE(MidiSerializeTimeCode(&time_code, &data));
  time_code.type = MIDI_HOURS_COUNT_MSN;
  time_code.value = 0x0F;
  TEST_ASSERT_FALSE(MidiSerializeTimeCode(&time_code, &data));
}

static void TestMidiTimeCode_Serialize_Valid(void) {
  midi_time_code_t time_code = {};
  uint8_t data;
  time_code.type = MIDI_SECONDS_COUNT_LSN;
  time_code.value = 0x0F;
  TEST_ASSERT_TRUE(MidiSerializeTimeCode(&time_code, &data));
  TEST_ASSERT_EQUAL(MIDI_SECONDS_COUNT_LSN | 0x0F, data);

  time_code.type = MIDI_HOURS_COUNT_MSN;
  time_code.value = (MIDI_30_FPS_DROP_FRAME >> 4) | 0x01;
  TEST_ASSERT_TRUE(MidiSerializeTimeCode(&time_code, &data));
  TEST_ASSERT_EQUAL(
      MIDI_HOURS_COUNT_MSN | (MIDI_30_FPS_DROP_FRAME >> 4) | 0x01, data);
}

static void TestMidiTimeCode_Deserialize_Invalid(void) {
  midi_time_code_t time_code = {};
  TEST_ASSERT_FALSE(MidiDeserializeTimeCode(NULL, 0x00));
  TEST_ASSERT_FALSE(MidiDeserializeTimeCode(&time_code, 0x80));
  TEST_ASSERT_FALSE(MidiDeserializeTimeCode(&time_code, 0xF0));
}

static void TestMidiTimeCode_Deserialize_Valid(void) {
  midi_time_code_t time_code = {};
  TEST_ASSERT_TRUE(
      MidiDeserializeTimeCode(&time_code, MIDI_FRAME_COUNT_LSN | 0x0F));
  TEST_ASSERT_EQUAL(MIDI_FRAME_COUNT_LSN, time_code.type);
  TEST_ASSERT_EQUAL(0x0F, time_code.value);

  TEST_ASSERT_TRUE(MidiDeserializeTimeCode(
      &time_code,
      MIDI_HOURS_COUNT_MSN | (MIDI_30_FPS_DROP_FRAME >> 4) | 1));
  TEST_ASSERT_EQUAL(MIDI_HOURS_COUNT_MSN, time_code.type);
  TEST_ASSERT_EQUAL((MIDI_30_FPS_DROP_FRAME >> 4) | 1, time_code.value);
}

/* All of the MSN time code types have some "reserved" bits that should
 * be ignored, and parsed as zero. */
static void TestMidiTimeCode_Deserialize_IgnoreReservedBits(void) {
  midi_time_code_t time_code = {};
  TEST_ASSERT_TRUE(MidiDeserializeTimeCode(
      &time_code, MIDI_FRAME_COUNT_MSN | 0x0F));
  TEST_ASSERT_EQUAL(MIDI_FRAME_COUNT_MSN, time_code.type);
  TEST_ASSERT_EQUAL(0x01, time_code.value);

  TEST_ASSERT_TRUE(MidiDeserializeTimeCode(
      &time_code, MIDI_SECONDS_COUNT_MSN | 0x0F));
  TEST_ASSERT_EQUAL(MIDI_SECONDS_COUNT_MSN, time_code.type);
  TEST_ASSERT_EQUAL(0x03, time_code.value);

  TEST_ASSERT_TRUE(MidiDeserializeTimeCode(
      &time_code, MIDI_MINUTES_COUNT_MSN | 0x0F));
  TEST_ASSERT_EQUAL(MIDI_MINUTES_COUNT_MSN, time_code.type);
  TEST_ASSERT_EQUAL(0x03, time_code.value);

  TEST_ASSERT_TRUE(MidiDeserializeTimeCode(
      &time_code, MIDI_HOURS_COUNT_MSN | 0x0F));
  TEST_ASSERT_EQUAL(MIDI_HOURS_COUNT_MSN, time_code.type);
  TEST_ASSERT_EQUAL(0x07, time_code.value);
}

/* MIDI Time Test */

static void TestMidiTime_Validators(void) {
  TEST_ASSERT_FALSE(MidiIsValidTime(NULL));
  midi_time_t time = {};
  TEST_ASSERT_TRUE(MidiIsValidTime(&time));

  time.frame = 30; /* Above max */
  TEST_ASSERT_FALSE(MidiIsValidTime(&time));
  time.frame = 0xFF;
  TEST_ASSERT_FALSE(MidiIsValidTime(&time));
  time.frame = 29;
  TEST_ASSERT_TRUE(MidiIsValidTime(&time));

  time.seconds = 60; /* Above max */
  TEST_ASSERT_FALSE(MidiIsValidTime(&time));
  time.seconds = 0xFF;
  TEST_ASSERT_FALSE(MidiIsValidTime(&time));
  time.seconds = 59;
  TEST_ASSERT_TRUE(MidiIsValidTime(&time));

  time.minutes = 60; /* Above max */
  TEST_ASSERT_FALSE(MidiIsValidTime(&time));
  time.minutes = 0xFF;
  TEST_ASSERT_FALSE(MidiIsValidTime(&time));
  time.minutes = 59;
  TEST_ASSERT_TRUE(MidiIsValidTime(&time));

  time.hours = 24; /* Above max */
  TEST_ASSERT_FALSE(MidiIsValidTime(&time));
  time.hours = 0xFF;
  TEST_ASSERT_FALSE(MidiIsValidTime(&time));
  time.hours = 23;
  TEST_ASSERT_TRUE(MidiIsValidTime(&time));

  time.fps = 0x9F;  /* Completly invalid. */
  TEST_ASSERT_FALSE(MidiIsValidTime(&time));
  time.fps = 0x10;
  TEST_ASSERT_FALSE(MidiIsValidTime(&time));
  time.fps = 0x80;
  TEST_ASSERT_FALSE(MidiIsValidTime(&time));
  time.fps = MIDI_24_FPS;
  TEST_ASSERT_TRUE(MidiIsValidTime(&time));
  time.fps = MIDI_25_FPS;
  TEST_ASSERT_TRUE(MidiIsValidTime(&time));
  time.fps = MIDI_30_FPS_DROP_FRAME;
  TEST_ASSERT_TRUE(MidiIsValidTime(&time));
  time.fps = MIDI_30_FPS_NON_DROP;
  TEST_ASSERT_TRUE(MidiIsValidTime(&time));
}

static void TestMidiTime_Initializer(void) {
  TEST_ASSERT_FALSE(MidiInitializeTime(NULL));
  midi_time_t time;
  time.frame = 0xE5;
  time.seconds = 0x5E;
  time.minutes = 0xE5;
  time.hours = 0x5E;
  time.fps = 0xE5;
  TEST_ASSERT_TRUE(MidiInitializeTime(&time));
  TEST_ASSERT_TRUE(MidiIsValidTime(&time));
  TEST_ASSERT_EQUAL(0x00, time.frame);
  TEST_ASSERT_EQUAL(0x00, time.seconds);
  TEST_ASSERT_EQUAL(0x00, time.minutes);
  TEST_ASSERT_EQUAL(0x00, time.hours);
  TEST_ASSERT_EQUAL(0x00, time.fps);
}

static void TestMidiTime_Update(void) {
  midi_time_code_t time_code;
  midi_time_t time, expected_time;
  MidiInitializeTime(&time);
  MidiInitializeTime(&expected_time);
  TEST_ASSERT_EQUAL_MEMORY(&time, &expected_time, sizeof(midi_time_t));

  TEST_ASSERT_FALSE(MidiUpdateTime(NULL, NULL));
  TEST_ASSERT_FALSE(MidiUpdateTime(&time, NULL));
  TEST_ASSERT_FALSE(MidiUpdateTime(NULL, &time_code));
  time.seconds = 77;
  TEST_ASSERT_FALSE(MidiUpdateTime(&time, &time_code));
  TEST_ASSERT_TRUE(MidiInitializeTime(&time));
  time_code.type = 0x01;  /* Invalid */
  TEST_ASSERT_FALSE(MidiUpdateTime(&time, &time_code));
  TEST_ASSERT_TRUE(MidiInitializeTime(&time));

  /* Set frame = 28 = 0x1C */
  expected_time.frame = 28;
  MidiInitializeTimeCode(&time_code, MIDI_FRAME_COUNT_MSN, 0x1);
  TEST_ASSERT_TRUE(MidiUpdateTime(&time, &time_code));
  TEST_ASSERT_EQUAL(16, time.frame);
  MidiInitializeTimeCode(&time_code, MIDI_FRAME_COUNT_LSN, 0xC);
  TEST_ASSERT_TRUE(MidiUpdateTime(&time, &time_code));
  TEST_ASSERT_EQUAL(28, time.frame);
  TEST_ASSERT_EQUAL_MEMORY(&time, &expected_time, sizeof(midi_time_t));

  /* Try set frame = 30 = 0x1E
   * First by setting MSN first, then by setting LSN. */
  MidiInitializeTime(&time);
  MidiInitializeTimeCode(&time_code, MIDI_FRAME_COUNT_MSN, 0x1);
  TEST_ASSERT_TRUE(MidiUpdateTime(&time, &time_code));
  MidiInitializeTimeCode(&time_code, MIDI_FRAME_COUNT_LSN, 0xE);
  TEST_ASSERT_FALSE(MidiUpdateTime(&time, &time_code));
  TEST_ASSERT_EQUAL(0x10, time.frame);

  MidiInitializeTime(&time);
  MidiInitializeTimeCode(&time_code, MIDI_FRAME_COUNT_LSN, 0xE);
  TEST_ASSERT_TRUE(MidiUpdateTime(&time, &time_code));
  MidiInitializeTimeCode(&time_code, MIDI_FRAME_COUNT_MSN, 0x1);
  TEST_ASSERT_FALSE(MidiUpdateTime(&time, &time_code));
  TEST_ASSERT_EQUAL(0x0E, time.frame);

  /* Try set frame = 32 = 0x20 */
  MidiInitializeTime(&time);
  time_code.type = MIDI_FRAME_COUNT_MSN;
  time_code.value = 0x2;
  TEST_ASSERT_FALSE(MidiUpdateTime(&time, &time_code));

  /* Set seconds = 47 = 0x2F */
  MidiInitializeTime(&time);
  MidiInitializeTime(&expected_time);
  expected_time.seconds = 47;
  MidiInitializeTimeCode(&time_code, MIDI_SECONDS_COUNT_MSN, 0x2);
  TEST_ASSERT_TRUE(MidiUpdateTime(&time, &time_code));
  MidiInitializeTimeCode(&time_code, MIDI_SECONDS_COUNT_LSN, 0xF);
  TEST_ASSERT_TRUE(MidiUpdateTime(&time, &time_code));
  TEST_ASSERT_EQUAL(47, time.seconds);
  TEST_ASSERT_EQUAL_MEMORY(&time, &expected_time, sizeof(midi_time_t));

  /* Try set seconds = 60 = 0x3C
   * First by setting MSN first, then by setting LSN. */
  MidiInitializeTime(&time);
  MidiInitializeTimeCode(&time_code, MIDI_SECONDS_COUNT_MSN, 0x3);
  TEST_ASSERT_TRUE(MidiUpdateTime(&time, &time_code));
  MidiInitializeTimeCode(&time_code, MIDI_SECONDS_COUNT_LSN, 0xC);
  TEST_ASSERT_FALSE(MidiUpdateTime(&time, &time_code));
  TEST_ASSERT_EQUAL(0x30, time.seconds);

  MidiInitializeTime(&time);
  MidiInitializeTimeCode(&time_code, MIDI_SECONDS_COUNT_LSN, 0xC);
  TEST_ASSERT_TRUE(MidiUpdateTime(&time, &time_code));
  MidiInitializeTimeCode(&time_code, MIDI_SECONDS_COUNT_MSN, 0x3);
  TEST_ASSERT_FALSE(MidiUpdateTime(&time, &time_code));
  TEST_ASSERT_EQUAL(0x0C, time.seconds);

  /* Try set seconds = 64 = 0x40 */
  MidiInitializeTime(&time);
  time_code.type = MIDI_SECONDS_COUNT_MSN;
  time_code.value = 0x4;
  TEST_ASSERT_FALSE(MidiUpdateTime(&time, &time_code));

  /* Set minutes = 32 = 0x20 */
  MidiInitializeTime(&time);
  time.minutes = 3;
  MidiInitializeTime(&expected_time);
  expected_time.minutes = 32;
  MidiInitializeTimeCode(&time_code, MIDI_MINUTES_COUNT_MSN, 0x2);
  TEST_ASSERT_TRUE(MidiUpdateTime(&time, &time_code));
  MidiInitializeTimeCode(&time_code, MIDI_MINUTES_COUNT_LSN, 0x0);
  TEST_ASSERT_TRUE(MidiUpdateTime(&time, &time_code));
  TEST_ASSERT_EQUAL(32, time.minutes);
  TEST_ASSERT_EQUAL_MEMORY(&time, &expected_time, sizeof(midi_time_t));

  /* Try set minutes = 60 = 0x3C
   * First by setting MSN first, then by setting LSN. */
  MidiInitializeTime(&time);
  MidiInitializeTimeCode(&time_code, MIDI_MINUTES_COUNT_MSN, 0x3);
  TEST_ASSERT_TRUE(MidiUpdateTime(&time, &time_code));
  MidiInitializeTimeCode(&time_code, MIDI_MINUTES_COUNT_LSN, 0xC);
  TEST_ASSERT_FALSE(MidiUpdateTime(&time, &time_code));
  TEST_ASSERT_EQUAL(0x30, time.minutes);

  MidiInitializeTime(&time);
  MidiInitializeTimeCode(&time_code, MIDI_MINUTES_COUNT_LSN, 0xC);
  TEST_ASSERT_TRUE(MidiUpdateTime(&time, &time_code));
  MidiInitializeTimeCode(&time_code, MIDI_MINUTES_COUNT_MSN, 0x3);
  TEST_ASSERT_FALSE(MidiUpdateTime(&time, &time_code));
  TEST_ASSERT_EQUAL(0x0C, time.minutes);

  /* Try set minutes = 64 = 0x40 */
  MidiInitializeTime(&time);
  time_code.type = MIDI_MINUTES_COUNT_MSN;
  time_code.value = 0x4;
  TEST_ASSERT_FALSE(MidiUpdateTime(&time, &time_code));

  /* Set hours = 11 = 0x0B */
  MidiInitializeTime(&time);
  time.hours = 23;
  MidiInitializeTime(&expected_time);
  expected_time.hours = 11;
  MidiInitializeTimeCode(&time_code, MIDI_HOURS_COUNT_MSN, 0x0);
  TEST_ASSERT_TRUE(MidiUpdateTime(&time, &time_code));
  MidiInitializeTimeCode(&time_code, MIDI_HOURS_COUNT_LSN, 0xB);
  TEST_ASSERT_TRUE(MidiUpdateTime(&time, &time_code));
  TEST_ASSERT_EQUAL(11, time.hours);
  TEST_ASSERT_EQUAL_MEMORY(&time, &expected_time, sizeof(midi_time_t));

  /* Set hours = 17 = 0x11, with 30 FPS drop frame, frame = 0x40 */
  MidiInitializeTime(&time);
  MidiInitializeTime(&expected_time);
  expected_time.hours = 17;
  expected_time.fps = MIDI_30_FPS_DROP_FRAME;
  MidiInitializeTimeCode(&time_code, MIDI_HOURS_COUNT_MSN, 0x1 | 0x4);
  TEST_ASSERT_TRUE(MidiUpdateTime(&time, &time_code));
  MidiInitializeTimeCode(&time_code, MIDI_HOURS_COUNT_LSN, 0x1);
  TEST_ASSERT_TRUE(MidiUpdateTime(&time, &time_code));
  TEST_ASSERT_EQUAL(17, time.hours);
  TEST_ASSERT_EQUAL_MEMORY(&time, &expected_time, sizeof(midi_time_t));

  /* Try set hours = 24 = 0x18, with 30 FPS non drop, frame = 0x60 */
  MidiInitializeTime(&time);
  time.fps = MIDI_30_FPS_DROP_FRAME;
  MidiInitializeTimeCode(
      &time_code, MIDI_HOURS_COUNT_MSN, 0x1 | 0x6 /* 30nd */);
  TEST_ASSERT_TRUE(MidiUpdateTime(&time, &time_code));
  MidiInitializeTimeCode(&time_code, MIDI_HOURS_COUNT_LSN, 0x8);
  TEST_ASSERT_FALSE(MidiUpdateTime(&time, &time_code));
  TEST_ASSERT_EQUAL(0x10, time.hours);
  TEST_ASSERT_EQUAL(MIDI_30_FPS_NON_DROP, time.fps);  /* changed */

  MidiInitializeTime(&time);
  time.fps = MIDI_30_FPS_DROP_FRAME;
  MidiInitializeTimeCode(&time_code, MIDI_HOURS_COUNT_LSN, 0x8);
  TEST_ASSERT_TRUE(MidiUpdateTime(&time, &time_code));
  MidiInitializeTimeCode(
      &time_code, MIDI_HOURS_COUNT_MSN, 0x1 | 0x6 /* 30nd */);
  TEST_ASSERT_FALSE(MidiUpdateTime(&time, &time_code));
  TEST_ASSERT_EQUAL(0x08, time.hours);
  TEST_ASSERT_EQUAL(MIDI_30_FPS_DROP_FRAME, time.fps);  /* unchanged */

  /* Try set reserved */
  MidiInitializeTime(&time);
  time.hours = 13;
  time.fps = MIDI_30_FPS_DROP_FRAME;
  time_code.type = MIDI_HOURS_COUNT_MSN;
  time_code.value = 0x80 | MIDI_30_FPS_NON_DROP;
  TEST_ASSERT_FALSE(MidiUpdateTime(&time, &time_code));
  TEST_ASSERT_EQUAL(13, time.hours);
  TEST_ASSERT_EQUAL(MIDI_30_FPS_DROP_FRAME, time.fps);
}

static void TestMidiTime_Update_FrameReset(void) {
  midi_time_code_t time_code;
  midi_time_t time;
  /* If unchanged, then no effect. */
  MidiInitializeTime(&time);
  time.fps = MIDI_30_FPS_DROP_FRAME;
  time.frame = 27;
  MidiInitializeTimeCode(&time_code, MIDI_HOURS_COUNT_MSN, 0x05);
  TEST_ASSERT_TRUE(MidiUpdateTime(&time, &time_code));
  TEST_ASSERT_EQUAL(MIDI_30_FPS_DROP_FRAME, time.fps);
  TEST_ASSERT_EQUAL(27, time.frame);
  TEST_ASSERT_EQUAL(16, time.hours);

  /* If changed while in bounds, no effect. */
  MidiInitializeTime(&time);
  time.fps = MIDI_30_FPS_DROP_FRAME;
  time.frame = 18;
  MidiInitializeTimeCode(&time_code, MIDI_HOURS_COUNT_MSN, 0x03);
  TEST_ASSERT_TRUE(MidiUpdateTime(&time, &time_code));
  TEST_ASSERT_EQUAL(MIDI_25_FPS, time.fps);
  TEST_ASSERT_EQUAL(18, time.frame);
  TEST_ASSERT_EQUAL(16, time.hours);

  /* Set to 24 FPS with 27 */
  MidiInitializeTime(&time);
  time.fps = MIDI_30_FPS_DROP_FRAME;
  time.frame = 27;
  MidiInitializeTimeCode(&time_code, MIDI_HOURS_COUNT_MSN, 0x00);
  TEST_ASSERT_TRUE(MidiUpdateTime(&time, &time_code));
  TEST_ASSERT_EQUAL(MIDI_24_FPS, time.fps);
  TEST_ASSERT_EQUAL(0, time.frame);

  /* Set to 25, then 24 FPS with 24*/
  MidiInitializeTime(&time);
  time.fps = MIDI_30_FPS_DROP_FRAME;
  time.frame = 24;
  MidiInitializeTimeCode(&time_code, MIDI_HOURS_COUNT_MSN, 0x02);
  TEST_ASSERT_TRUE(MidiUpdateTime(&time, &time_code));
  TEST_ASSERT_EQUAL(MIDI_25_FPS, time.fps);
  TEST_ASSERT_EQUAL(24, time.frame);
  MidiInitializeTimeCode(&time_code, MIDI_HOURS_COUNT_MSN, 0x00);
  TEST_ASSERT_TRUE(MidiUpdateTime(&time, &time_code));
  TEST_ASSERT_EQUAL(MIDI_24_FPS, time.fps);
  TEST_ASSERT_EQUAL(0, time.frame);
}

static void TestMidiTime_ExtractTimeCode(void) {
  midi_time_code_t time_code;
  midi_time_t time;
  MidiInitializeTime(&time);

  /* Invalid input. */
  TEST_ASSERT_FALSE(
      MidiExtractTimeCode(NULL, MIDI_FRAME_COUNT_LSN, &time_code));
  TEST_ASSERT_FALSE(
      MidiExtractTimeCode(&time, 0xFF, &time_code));
  TEST_ASSERT_FALSE(
      MidiExtractTimeCode(&time, MIDI_FRAME_COUNT_LSN, NULL));

  time.frame = /* 22 */ 0x16;
  time.seconds = /* 41 */ 0x2A;
  time.minutes = /* 14 */ 0x0E;
  time.hours = /* 13 */ 0x0D;
  time.fps = MIDI_30_FPS_NON_DROP /* 0x60 */;
  /* Check that time is valid before continuing. */
  TEST_ASSERT_TRUE(MidiIsValidTime(&time));
  /* Frame. */
  TEST_ASSERT_TRUE(MidiExtractTimeCode(
      &time, MIDI_FRAME_COUNT_LSN, &time_code));
  TEST_ASSERT_EQUAL(MIDI_FRAME_COUNT_LSN, time_code.type);
  TEST_ASSERT_EQUAL(0x6, time_code.value);

  TEST_ASSERT_TRUE(MidiExtractTimeCode(
      &time, MIDI_FRAME_COUNT_MSN, &time_code));
  TEST_ASSERT_EQUAL(MIDI_FRAME_COUNT_MSN, time_code.type);
  TEST_ASSERT_EQUAL(0x1, time_code.value);
  /* Seconds. */
  TEST_ASSERT_TRUE(MidiExtractTimeCode(
      &time, MIDI_SECONDS_COUNT_LSN, &time_code));
  TEST_ASSERT_EQUAL(MIDI_SECONDS_COUNT_LSN, time_code.type);
  TEST_ASSERT_EQUAL(0xA, time_code.value);

  TEST_ASSERT_TRUE(MidiExtractTimeCode(
      &time, MIDI_SECONDS_COUNT_MSN, &time_code));
  TEST_ASSERT_EQUAL(MIDI_SECONDS_COUNT_MSN, time_code.type);
  TEST_ASSERT_EQUAL(0x2, time_code.value);
  /* Minutes. */
  TEST_ASSERT_TRUE(MidiExtractTimeCode(
      &time, MIDI_MINUTES_COUNT_LSN, &time_code));
  TEST_ASSERT_EQUAL(MIDI_MINUTES_COUNT_LSN, time_code.type);
  TEST_ASSERT_EQUAL(0xE, time_code.value);

  TEST_ASSERT_TRUE(MidiExtractTimeCode(
      &time, MIDI_MINUTES_COUNT_MSN, &time_code));
  TEST_ASSERT_EQUAL(MIDI_MINUTES_COUNT_MSN, time_code.type);
  TEST_ASSERT_EQUAL(0x0, time_code.value);
  /* Hours. */
  TEST_ASSERT_TRUE(MidiExtractTimeCode(
      &time, MIDI_HOURS_COUNT_LSN, &time_code));
  TEST_ASSERT_EQUAL(MIDI_HOURS_COUNT_LSN, time_code.type);
  TEST_ASSERT_EQUAL(0xD, time_code.value);

  TEST_ASSERT_TRUE(MidiExtractTimeCode(
      &time, MIDI_HOURS_COUNT_MSN, &time_code));
  TEST_ASSERT_EQUAL(MIDI_HOURS_COUNT_MSN, time_code.type);
  TEST_ASSERT_EQUAL(0x6, time_code.value);
}

static void TestMidiTime_Serialize(void) {
  midi_time_t time;
  uint8_t time_data[MIDI_SERIALIZED_TIME_PAYLOAD_SIZE];
  MidiInitializeTime(&time);
  /* Invalid inputs. */
  TEST_ASSERT_EQUAL(0, MidiSerializeTime(
      NULL, MIDI_TIME_FORWARD, time_data, sizeof(time_data)));
  TEST_ASSERT_EQUAL(0, MidiSerializeTime(
      &time, MIDI_TIME_UNKNOWN, time_data, sizeof(time_data)));
  TEST_ASSERT_EQUAL(0, MidiSerializeTime(
      &time, MIDI_TIME_FORWARD, NULL, sizeof(time_data)));
  time.hours = 25;
  TEST_ASSERT_EQUAL(0, MidiSerializeTime(
      &time, MIDI_TIME_FORWARD, time_data, sizeof(time_data)));

  /* Incomplete serialization. */
  MidiInitializeTime(&time);
  TEST_ASSERT_EQUAL(MIDI_SERIALIZED_TIME_PAYLOAD_SIZE, MidiSerializeTime(
      &time, MIDI_TIME_FORWARD, NULL, 0));
  TEST_ASSERT_EQUAL(MIDI_SERIALIZED_TIME_PAYLOAD_SIZE, MidiSerializeTime(
      &time, MIDI_TIME_FORWARD, time_data, 0));
  TEST_ASSERT_EQUAL(MIDI_SERIALIZED_TIME_PAYLOAD_SIZE, MidiSerializeTime(
      &time, MIDI_TIME_FORWARD, time_data,
      MIDI_SERIALIZED_TIME_PAYLOAD_SIZE / 2));

  time.frame = /* 22 */ 0x16;
  time.seconds = /* 41 */ 0x2A;
  time.minutes = /* 14 */ 0x0E;
  time.hours = /* 13 */ 0x0D;
  time.fps = MIDI_30_FPS_NON_DROP /* 0x60 */;
  static uint8_t const kForwardTimeData[] = {
    MIDI_FRAME_COUNT_LSN | 0x6,
    MIDI_FRAME_COUNT_MSN | 0x1,
    MIDI_SECONDS_COUNT_LSN | 0xA,
    MIDI_SECONDS_COUNT_MSN | 0x2,
    MIDI_MINUTES_COUNT_LSN | 0xE,
    MIDI_MINUTES_COUNT_MSN | 0x0,
    MIDI_HOURS_COUNT_LSN | 0xD,
    MIDI_HOURS_COUNT_MSN | 0x0 | 0x6
  };
  static uint8_t const kReverseTimeData[] = {
    MIDI_HOURS_COUNT_MSN | 0x0 | 0x6,
    MIDI_HOURS_COUNT_LSN | 0xD,
    MIDI_MINUTES_COUNT_MSN | 0x0,
    MIDI_MINUTES_COUNT_LSN | 0xE,
    MIDI_SECONDS_COUNT_MSN | 0x2,
    MIDI_SECONDS_COUNT_LSN | 0xA,
    MIDI_FRAME_COUNT_MSN | 0x1,
    MIDI_FRAME_COUNT_LSN | 0x6
  };
  TEST_ASSERT_EQUAL(
      sizeof(kForwardTimeData),
      MidiSerializeTime(
          &time, MIDI_TIME_FORWARD, time_data, sizeof(time_data)));
  TEST_ASSERT_EQUAL_MEMORY(
      kForwardTimeData, time_data, sizeof(kForwardTimeData));

  TEST_ASSERT_EQUAL(
      sizeof(kReverseTimeData),
      MidiSerializeTime(
          &time, MIDI_TIME_REVERSE, time_data, sizeof(time_data)));
  TEST_ASSERT_EQUAL_MEMORY(
      kReverseTimeData, time_data, sizeof(kReverseTimeData));
}

static void TestMidiTime_Increment(void) {
  TEST_ASSERT_FALSE(MidiIncrementTimeFrame(NULL));
  TEST_ASSERT_FALSE(MidiIncrementTimeSeconds(NULL));
  TEST_ASSERT_FALSE(MidiIncrementTimeMinutes(NULL));
  TEST_ASSERT_FALSE(MidiIncrementTimeHours(NULL));

  midi_time_t time;
  MidiInitializeTime(&time);
  time.frame = 40;
  TEST_ASSERT_FALSE(MidiIncrementTimeFrame(&time));
  TEST_ASSERT_FALSE(MidiIncrementTimeSeconds(&time));
  TEST_ASSERT_FALSE(MidiIncrementTimeMinutes(&time));
  TEST_ASSERT_FALSE(MidiIncrementTimeHours(&time));
  TEST_ASSERT_EQUAL(40, time.frame);

  MidiInitializeTime(&time);
  time.frame = 28;
  time.seconds = 58;
  time.minutes = 58;
  time.hours = 22;
  time.fps = MIDI_30_FPS_NON_DROP;
  TEST_ASSERT_TRUE(MidiIncrementTimeFrame(&time));
  TEST_ASSERT_EQUAL(29, time.frame);
  TEST_ASSERT_EQUAL(58, time.seconds);
  TEST_ASSERT_TRUE(MidiIncrementTimeSeconds(&time));
  TEST_ASSERT_EQUAL(59, time.seconds);
  TEST_ASSERT_EQUAL(58, time.minutes);
  TEST_ASSERT_TRUE(MidiIncrementTimeMinutes(&time));
  TEST_ASSERT_EQUAL(59, time.minutes);
  TEST_ASSERT_EQUAL(22, time.hours);
  TEST_ASSERT_TRUE(MidiIncrementTimeHours(&time));
  TEST_ASSERT_EQUAL(23, time.hours);

  TEST_ASSERT_TRUE(MidiIncrementTimeFrame(&time));
  TEST_ASSERT_EQUAL(0, time.frame);
  TEST_ASSERT_EQUAL(0, time.seconds);
  TEST_ASSERT_EQUAL(0, time.minutes);
  TEST_ASSERT_EQUAL(0, time.hours);

  MidiInitializeTime(&time);
  time.fps = MIDI_30_FPS_NON_DROP;
  time.frame = 29;
  TEST_ASSERT_TRUE(MidiIncrementTimeFrame(&time));
  TEST_ASSERT_EQUAL(0, time.frame);

  time.fps = MIDI_30_FPS_DROP_FRAME;
  time.frame = 29;
  TEST_ASSERT_TRUE(MidiIncrementTimeFrame(&time));
  TEST_ASSERT_EQUAL(0, time.frame);

  time.fps = MIDI_25_FPS;
  time.frame = 23;
  TEST_ASSERT_TRUE(MidiIncrementTimeFrame(&time));
  TEST_ASSERT_TRUE(MidiIncrementTimeFrame(&time));
  TEST_ASSERT_EQUAL(0, time.frame);

  time.fps = MIDI_24_FPS;
  time.frame = 22;
  TEST_ASSERT_TRUE(MidiIncrementTimeFrame(&time));
  TEST_ASSERT_TRUE(MidiIncrementTimeFrame(&time));
  TEST_ASSERT_EQUAL(0, time.frame);
}

void MidiTimeTest(void) {
  RUN_TEST(TestMidiTimeCode_Validators);
  RUN_TEST(TestMidiTimeCode_Initializer_Invalid);
  RUN_TEST(TestMidiTimeCode_Initializer_Valid);
  RUN_TEST(TestMidiTimeCode_Serialize_Invalid);
  RUN_TEST(TestMidiTimeCode_Serialize_Valid);
  RUN_TEST(TestMidiTimeCode_Deserialize_Invalid);
  RUN_TEST(TestMidiTimeCode_Deserialize_Valid);
  RUN_TEST(TestMidiTimeCode_Deserialize_IgnoreReservedBits);

  RUN_TEST(TestMidiTime_Validators);
  RUN_TEST(TestMidiTime_Initializer);
  RUN_TEST(TestMidiTime_Update);
  RUN_TEST(TestMidiTime_Update_FrameReset);
  RUN_TEST(TestMidiTime_ExtractTimeCode);
  RUN_TEST(TestMidiTime_Serialize);
  RUN_TEST(TestMidiTime_Increment);
}
