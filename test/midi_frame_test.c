/*
 * MIDI Controller - MIDI Frame Buffer
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#include <string.h>
#include <unity.h>

#include "midi_frame.h"
#include "smart_string.h"

#if defined(_PLATFORM_NATIVE) && defined(_DEBUG)
#include <stdio.h>
static void print_frame(midi_frame_buffer_t const *frame) {
  if (frame == NULL) {
    printf("frame = NULL\n");
    return;
  }
  printf("frame = {\n");
  char buffer_hex[MIDI_FRAME_BUFFER_SIZE * 2 + 1];
  SmartStringHexEncode(
      frame->buffer, MIDI_FRAME_BUFFER_SIZE, buffer_hex, sizeof(buffer_hex));
  printf("  .buffer = %s,\n", buffer_hex);
  SmartStringHexFormat(
      frame->front, SS_SINGLE | SS_ZERO_X, buffer_hex, sizeof(buffer_hex));
  printf("  .front = %s,\n", buffer_hex);
  SmartStringHexFormat(
      frame->size, SS_SINGLE | SS_ZERO_X, buffer_hex, sizeof(buffer_hex));
  printf("  .size = %s\n", buffer_hex);
  printf("}\n");
}
#else
#define print_frame(frame)
#endif /* _PLATFORM_NATIVE */

static uint8_t gTestData[MIDI_FRAME_BUFFER_SIZE * 8];

static void IntializeMidiFrameBufferTest(void) {
  for (size_t i = 0; i < sizeof(gTestData); ++i) {
    uint8_t const byte = (i + 1) & 0xFF;
    gTestData[i] = byte;
  }
}

static void TestMidiFrame_Info(void) {
  TEST_ASSERT_FALSE(MidiInitializeFrameBuffer(NULL));
  TEST_ASSERT_FALSE(MidiFrameBufferFull(NULL));
  TEST_ASSERT_FALSE(MidiFrameBufferEmpty(NULL));
  midi_frame_buffer_t frame;
  TEST_ASSERT_TRUE(MidiInitializeFrameBuffer(&frame));
  TEST_ASSERT_FALSE(MidiFrameBufferFull(&frame));
  TEST_ASSERT_TRUE(MidiFrameBufferEmpty(&frame));
}

static void TestMidiFrame_Invalid(void) {
  uint8_t buffer[MIDI_FRAME_BUFFER_SIZE];
  midi_frame_buffer_t frame;
  frame.size = MIDI_FRAME_BUFFER_SIZE;  /* Set full */

  TEST_ASSERT_EQUAL(0, MidiPutFrameBufferData(NULL, 1, &frame));
  TEST_ASSERT_EQUAL(0, MidiPutFrameBufferData(gTestData, 1, NULL));

  TEST_ASSERT_EQUAL(0, MidiPutFrameBufferByte(0x01, NULL));

  TEST_ASSERT_EQUAL(0, MidiPeakFrameBufferData(&frame, NULL, 2));
  TEST_ASSERT_EQUAL(0, MidiPeakFrameBufferData(NULL, buffer, 2));

  TEST_ASSERT_EQUAL(0, MidiTakeFrameBufferData(&frame, NULL, 2));
  TEST_ASSERT_EQUAL(0, MidiTakeFrameBufferData(NULL, buffer, 2));

  TEST_ASSERT_EQUAL(0, MidiClearFrameBufferData(NULL, 2));
  TEST_ASSERT_EQUAL(0, MidiClearAllFrameBufferData(NULL));
}

static void TestMidiFrame_SmallInput(void) {
  midi_frame_buffer_t frame;
  uint8_t buffer[MIDI_FRAME_BUFFER_SIZE];
  TEST_ASSERT_TRUE(MidiInitializeFrameBuffer(&frame));

  TEST_ASSERT_EQUAL(2, MidiPutFrameBufferData(gTestData, 2, &frame));
  TEST_ASSERT_FALSE(MidiFrameBufferEmpty(&frame));

  TEST_ASSERT_EQUAL(2, MidiPeakFrameBufferData(&frame, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_MEMORY(gTestData, buffer, 2);
  TEST_ASSERT_EQUAL(2, MidiTakeFrameBufferData(&frame, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_MEMORY(gTestData, buffer, 2);
  TEST_ASSERT_TRUE(MidiFrameBufferEmpty(&frame));
  TEST_ASSERT_EQUAL(0, MidiPeakFrameBufferData(&frame, buffer, sizeof(buffer)));

  TEST_ASSERT_EQUAL(
      MIDI_FRAME_BUFFER_SIZE,
      MidiPutFrameBufferData(gTestData, MIDI_FRAME_BUFFER_SIZE, &frame));
  TEST_ASSERT_FALSE(MidiFrameBufferEmpty(&frame));
  TEST_ASSERT_TRUE(MidiFrameBufferFull(&frame));
  TEST_ASSERT_EQUAL(
      MIDI_FRAME_BUFFER_SIZE,
      MidiPeakFrameBufferData(&frame, buffer, sizeof(buffer)));
  TEST_ASSERT_TRUE(MidiFrameBufferFull(&frame));
  TEST_ASSERT_EQUAL_MEMORY(gTestData, buffer, MIDI_FRAME_BUFFER_SIZE);
  TEST_ASSERT_EQUAL(
      MIDI_FRAME_BUFFER_SIZE,
      MidiTakeFrameBufferData(&frame, buffer, sizeof(buffer)));
  TEST_ASSERT_FALSE(MidiFrameBufferFull(&frame));
  TEST_ASSERT_TRUE(MidiFrameBufferEmpty(&frame));
  TEST_ASSERT_EQUAL_MEMORY(gTestData, buffer, MIDI_FRAME_BUFFER_SIZE);
}

static void TestMidiFrame_LargeInput(void) {
  midi_frame_buffer_t frame;
  uint8_t buffer[MIDI_FRAME_BUFFER_SIZE * 2];
  memset(buffer, 0, sizeof(buffer));
  TEST_ASSERT_TRUE(MidiInitializeFrameBuffer(&frame));

  TEST_ASSERT_EQUAL(
    MIDI_FRAME_BUFFER_SIZE * 2,
    MidiPutFrameBufferData(gTestData, MIDI_FRAME_BUFFER_SIZE * 2, &frame));
  TEST_ASSERT_FALSE(MidiFrameBufferEmpty(&frame));
  TEST_ASSERT_TRUE(MidiFrameBufferFull(&frame));

  TEST_ASSERT_EQUAL(
      MIDI_FRAME_BUFFER_SIZE,
      MidiPeakFrameBufferData(&frame, buffer, sizeof(buffer)));
  TEST_ASSERT_FALSE(MidiFrameBufferEmpty(&frame));
  TEST_ASSERT_TRUE(MidiFrameBufferFull(&frame));

  TEST_ASSERT_EQUAL_MEMORY(
      &gTestData[MIDI_FRAME_BUFFER_SIZE], buffer, MIDI_FRAME_BUFFER_SIZE);
  memset(buffer, 0, sizeof(buffer));

  TEST_ASSERT_EQUAL(
      MIDI_FRAME_BUFFER_SIZE,
      MidiTakeFrameBufferData(&frame, buffer, sizeof(buffer)));
  TEST_ASSERT_TRUE(MidiFrameBufferEmpty(&frame));
  TEST_ASSERT_FALSE(MidiFrameBufferFull(&frame));

  TEST_ASSERT_EQUAL_MEMORY(
      &gTestData[MIDI_FRAME_BUFFER_SIZE], buffer, MIDI_FRAME_BUFFER_SIZE);
  memset(buffer, 0, sizeof(buffer));

  TEST_ASSERT_EQUAL(0, MidiTakeFrameBufferData(&frame, buffer, sizeof(buffer)));
  TEST_ASSERT_TRUE(MidiFrameBufferEmpty(&frame));
  TEST_ASSERT_FALSE(MidiFrameBufferFull(&frame));
}

static void TestMidiFrame_PartialInput(void) {
  midi_frame_buffer_t frame;
  uint8_t buffer[MIDI_FRAME_BUFFER_SIZE];
  memset(buffer, 0, sizeof(buffer));
  TEST_ASSERT_TRUE(MidiInitializeFrameBuffer(&frame));

  TEST_ASSERT_EQUAL(2, MidiPutFrameBufferData(gTestData, 2, &frame));
  TEST_ASSERT_EQUAL(
      4, MidiPutFrameBufferData(&gTestData[2], 4, &frame));

  TEST_ASSERT_EQUAL(
      4, MidiTakeFrameBufferData(&frame, buffer, 4));
  TEST_ASSERT_EQUAL_MEMORY(gTestData, buffer, 4);

  TEST_ASSERT_EQUAL(
      4, MidiPutFrameBufferData(&gTestData[6], 4, &frame));
  TEST_ASSERT_EQUAL(
      6, MidiTakeFrameBufferData(&frame, buffer, 6));
  TEST_ASSERT_EQUAL_MEMORY(&gTestData[4], buffer, 6);

  TEST_ASSERT_EQUAL(
      MIDI_FRAME_BUFFER_SIZE,
      MidiPutFrameBufferData(&gTestData[10], MIDI_FRAME_BUFFER_SIZE, &frame));
  TEST_ASSERT_EQUAL(
      MIDI_FRAME_BUFFER_SIZE,
      MidiTakeFrameBufferData(&frame, buffer, MIDI_FRAME_BUFFER_SIZE));
  TEST_ASSERT_EQUAL_MEMORY(&gTestData[10], buffer, MIDI_FRAME_BUFFER_SIZE);
}

static void TestMidiFrame_Clearing(void) {
  static size_t const kHalfBuffer = MIDI_FRAME_BUFFER_SIZE / 2;
  midi_frame_buffer_t frame;
  uint8_t buffer[MIDI_FRAME_BUFFER_SIZE];
  memset(buffer, 0, sizeof(buffer));
  TEST_ASSERT_TRUE(MidiInitializeFrameBuffer(&frame));
  TEST_ASSERT_EQUAL(0, MidiClearFrameBufferData(&frame, 0));
  TEST_ASSERT_EQUAL(0, MidiClearFrameBufferData(&frame, 3));
  TEST_ASSERT_EQUAL(0, MidiClearAllFrameBufferData(&frame));

  TEST_ASSERT_EQUAL(
      MIDI_FRAME_BUFFER_SIZE,
      MidiPutFrameBufferData(gTestData, MIDI_FRAME_BUFFER_SIZE, &frame));
  TEST_ASSERT_FALSE(MidiFrameBufferEmpty(&frame));
  TEST_ASSERT_TRUE(MidiFrameBufferFull(&frame));

  TEST_ASSERT_EQUAL(
      kHalfBuffer, MidiClearFrameBufferData(&frame, kHalfBuffer));
  TEST_ASSERT_EQUAL(
      kHalfBuffer,
      MidiPeakFrameBufferData(&frame, buffer, MIDI_FRAME_BUFFER_SIZE));
  TEST_ASSERT_EQUAL_MEMORY(&gTestData[kHalfBuffer], buffer, kHalfBuffer);

  TEST_ASSERT_EQUAL(
      kHalfBuffer,
      MidiPutFrameBufferData(
          &gTestData[MIDI_FRAME_BUFFER_SIZE], kHalfBuffer, &frame));
  TEST_ASSERT_EQUAL(
      MIDI_FRAME_BUFFER_SIZE,
      MidiPeakFrameBufferData(&frame, buffer, MIDI_FRAME_BUFFER_SIZE));
  TEST_ASSERT_EQUAL_MEMORY(
      &gTestData[kHalfBuffer], buffer, MIDI_FRAME_BUFFER_SIZE);

  TEST_ASSERT_EQUAL(
      MIDI_FRAME_BUFFER_SIZE, MidiClearAllFrameBufferData(&frame));
  TEST_ASSERT_TRUE(MidiFrameBufferEmpty(&frame));
}

void MidiFrameTest(void) {
  IntializeMidiFrameBufferTest();
  RUN_TEST(TestMidiFrame_Info);
  RUN_TEST(TestMidiFrame_Invalid);
  RUN_TEST(TestMidiFrame_SmallInput);
  RUN_TEST(TestMidiFrame_LargeInput);
  RUN_TEST(TestMidiFrame_PartialInput);
  RUN_TEST(TestMidiFrame_Clearing);
}
