/*
 * MIDI Controller - MIDI Bytes Info Test
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#include <unity.h>

#include "midi_bytes.h"

static void TestMidiStatusByte_Validators(void) {
  TEST_ASSERT_TRUE(MidiIsStatusByte(0xFF));
  TEST_ASSERT_TRUE(MidiIsStatusByte(0x80));
  TEST_ASSERT_FALSE(MidiIsStatusByte(0x00));
  TEST_ASSERT_FALSE(MidiIsStatusByte(0x7F));
}

static void TestMidiDataByte_Validators(void) {
  TEST_ASSERT_FALSE(MidiIsDataByte(0xFF));
  TEST_ASSERT_FALSE(MidiIsDataByte(0x80));
  TEST_ASSERT_TRUE(MidiIsDataByte(0x00));
  TEST_ASSERT_TRUE(MidiIsDataByte(0x7F));
}

static void TestMidiDataWord_Validators(void) {
  TEST_ASSERT_FALSE(MidiIsDataWord(0xFFFF));
  TEST_ASSERT_FALSE(MidiIsDataWord(0xFF3F));
  TEST_ASSERT_TRUE(MidiIsDataWord(0x0000));
  TEST_ASSERT_TRUE(MidiIsDataWord(0x3FFF));
}

static void TestMidiTriByte_Validators(void) {
  TEST_ASSERT_FALSE(MidiIsDataTriByte(0xFFFFFFFF));
  TEST_ASSERT_FALSE(MidiIsDataTriByte(0xFFE00000));
  TEST_ASSERT_FALSE(MidiIsDataTriByte(0x00200000));
  TEST_ASSERT_TRUE(MidiIsDataTriByte(0x00000000));
  TEST_ASSERT_TRUE(MidiIsDataTriByte(0x001FFFFF));
}

static void TestMidiQuadByte_Validators(void) {
  TEST_ASSERT_FALSE(MidiIsDataQuadByte(0xFFFFFFFF));
  TEST_ASSERT_FALSE(MidiIsDataQuadByte(0xF0000000));
  TEST_ASSERT_FALSE(MidiIsDataQuadByte(0x10000000));
  TEST_ASSERT_TRUE(MidiIsDataQuadByte(0x00000000));
  TEST_ASSERT_TRUE(MidiIsDataQuadByte(0x0FFFFFFF));
}

static void  TestMidiDataArray_Validators(void) {
  static char const kShortData[] = {0x63};
  static char const kLongData[] = {0x63, 0x77, 0x00};
  static char const kBadData[] = {0x63, 0x77, 0x80};
  TEST_ASSERT_FALSE(MidiIsDataArray(NULL, 1u));
  TEST_ASSERT_FALSE(MidiIsDataArray(kShortData, 0));
  TEST_ASSERT_FALSE(MidiIsDataArray(kBadData, sizeof(kBadData)));

  TEST_ASSERT_TRUE(MidiIsDataArray(kShortData, sizeof(kShortData)));
  TEST_ASSERT_TRUE(MidiIsDataArray(kLongData, sizeof(kLongData)));
}

static void TestMidiDataWord_Getters(void) {
  uint16_t word = 0;
  TEST_ASSERT_EQUAL(0, MidiGetDataWordMsb(word));
  TEST_ASSERT_EQUAL(0, MidiGetDataWordLsb(word));
  word = 0x3FFF;
  TEST_ASSERT_EQUAL(0x7F, MidiGetDataWordMsb(word));
  TEST_ASSERT_EQUAL(0x7F, MidiGetDataWordLsb(word));
  word = 0x1248;
  TEST_ASSERT_EQUAL(0x24, MidiGetDataWordMsb(word));
  TEST_ASSERT_EQUAL(0x48, MidiGetDataWordLsb(word));
  word = 0x3F80;
  TEST_ASSERT_EQUAL(0x7F, MidiGetDataWordMsb(word));
  TEST_ASSERT_EQUAL(0x00, MidiGetDataWordLsb(word));
  word = 0x0080;
  TEST_ASSERT_EQUAL(0x01, MidiGetDataWordMsb(word));
  TEST_ASSERT_EQUAL(0x00, MidiGetDataWordLsb(word));
}

static void TestMidiDataWord_Setters(void) {
  uint16_t word = 0;
  TEST_ASSERT_TRUE(MidiSetDataWordLsb(&word,  0x7F));
  TEST_ASSERT_EQUAL(0x007F, word);
  TEST_ASSERT_TRUE(MidiSetDataWordMsb(&word,  0x7F));
  TEST_ASSERT_EQUAL(0x3FFF, word);

  TEST_ASSERT_FALSE(MidiSetDataWordLsb(&word,  0x80));
  TEST_ASSERT_FALSE(MidiSetDataWordMsb(&word,  0xFF));
  TEST_ASSERT_EQUAL(0x3FFF, word);

  TEST_ASSERT_TRUE(MidiSetDataWordLsb(&word,  0x00));
  TEST_ASSERT_EQUAL(0x3F80, word);

  TEST_ASSERT_TRUE(MidiSetDataWordMsb(&word,  0x01));
  TEST_ASSERT_EQUAL(0x0080, word);
}

static void TestMidiTriByte_Getters(void) {
  TEST_ASSERT_EQUAL(0x40, MidiGetDataTriByteMsb(0x00101010));
  TEST_ASSERT_EQUAL(0x20, MidiGetDataTriByteMid(0x00101010));
  TEST_ASSERT_EQUAL(0x10, MidiGetDataTriByteLsb(0x00101010));
}

static void TestMidiQuadByte_Getters(void) {
  TEST_ASSERT_EQUAL(0x08, MidiGetDataQuadByteMsb(0x01010101));
  TEST_ASSERT_EQUAL(0x04, MidiGetDataQuadByteMMid(0x01010101));
  TEST_ASSERT_EQUAL(0x02, MidiGetDataQuadByteLMid(0x01010101));
  TEST_ASSERT_EQUAL(0x01, MidiGetDataQuadByteLsb(0x01010101));
}

static void TestMidiData_SerializeAndDeserialize(void) {
  uint32_t value = 0;
  uint8_t value_data[4] = {};

  TEST_ASSERT_FALSE(MidiSerializeTriByte(0x00, NULL));
  TEST_ASSERT_FALSE(MidiSerializeTriByte(0x10000000, value_data));
  TEST_ASSERT_FALSE(MidiSerializeQuadByte(0x00, NULL));
  TEST_ASSERT_FALSE(MidiSerializeQuadByte(0x10000000, value_data));

  TEST_ASSERT_FALSE(MidiDeserializeTriByte(NULL, NULL));
  TEST_ASSERT_FALSE(MidiDeserializeTriByte(NULL, &value));
  TEST_ASSERT_FALSE(MidiDeserializeTriByte(value_data, NULL));
  TEST_ASSERT_FALSE(MidiDeserializeQuadByte(NULL, NULL));
  TEST_ASSERT_FALSE(MidiDeserializeQuadByte(NULL, &value));
  TEST_ASSERT_FALSE(MidiDeserializeQuadByte(value_data, NULL));

  static uint8_t const kTriValue[] = {0x04, 0x04, 0x04};
  TEST_ASSERT_TRUE(MidiSerializeTriByte(0x00010204, value_data));
  TEST_ASSERT_EQUAL_MEMORY(kTriValue, value_data, 3);
  TEST_ASSERT_TRUE(MidiDeserializeTriByte(kTriValue, &value));
  TEST_ASSERT_EQUAL(0x00010204, value);

  static uint8_t const kQuadValue[] = {0x08, 0x08, 0x08, 0x08};
  TEST_ASSERT_TRUE(MidiSerializeQuadByte(0x01020408, value_data));
  TEST_ASSERT_EQUAL_MEMORY(kQuadValue, value_data, 4);
  TEST_ASSERT_TRUE(MidiDeserializeQuadByte(kQuadValue, &value));
  TEST_ASSERT_EQUAL(0x01020408, value);
}

static void TestMidiData_Creators(void) {
  /* Words */
  TEST_ASSERT_EQUAL(0x0000, MidiDataWordFromBytes(0x80, 0x03));
  TEST_ASSERT_EQUAL(0x3FFF, MidiDataWordFromBytes(0x7F, 0x7F));
  TEST_ASSERT_EQUAL(0x2020, MidiDataWordFromBytes(0x40, 0x20));
  /* Tri Bytes */
  TEST_ASSERT_EQUAL(0x00000000, MidiDataTriByteFromBytes(0x80, 0x03, 0x03));
  TEST_ASSERT_EQUAL(0x00000000, MidiDataTriByteFromBytes(0x03, 0x83, 0x03));
  TEST_ASSERT_EQUAL(0x00000000, MidiDataTriByteFromBytes(0x03, 0x03, 0x83));
  TEST_ASSERT_EQUAL(0x001FFFFF, MidiDataTriByteFromBytes(0x7F, 0x7F, 0x7F));
  TEST_ASSERT_EQUAL(0x00101010, MidiDataTriByteFromBytes(0x40, 0x20, 0x10));
  /* Quad Bytes */
  TEST_ASSERT_EQUAL(
      0x00000000, MidiDataQuadByteFromBytes(0x83, 0x03, 0x03, 0x03));
  TEST_ASSERT_EQUAL(
      0x00000000, MidiDataQuadByteFromBytes(0x03, 0x83, 0x03, 0x03));
  TEST_ASSERT_EQUAL(
      0x00000000, MidiDataQuadByteFromBytes(0x03, 0x03, 0x83, 0x03));
  TEST_ASSERT_EQUAL(
      0x00000000, MidiDataQuadByteFromBytes(0x03, 0x03, 0x03, 0x83));
  TEST_ASSERT_EQUAL(
      0x0FFFFFFF, MidiDataQuadByteFromBytes(0x7F, 0x7F, 0x7F, 0x7F));
  TEST_ASSERT_EQUAL(
      0x01010101, MidiDataQuadByteFromBytes(0x08, 0x04, 0x02, 0x01));
}

void MidiBytesTest(void) {
  RUN_TEST(TestMidiStatusByte_Validators);
  RUN_TEST(TestMidiDataByte_Validators);
  RUN_TEST(TestMidiDataWord_Validators);
  RUN_TEST(TestMidiTriByte_Validators);
  RUN_TEST(TestMidiQuadByte_Validators);
  RUN_TEST(TestMidiDataArray_Validators);
  RUN_TEST(TestMidiDataWord_Getters);
  RUN_TEST(TestMidiDataWord_Setters);
  RUN_TEST(TestMidiTriByte_Getters);
  RUN_TEST(TestMidiQuadByte_Getters);
  RUN_TEST(TestMidiData_SerializeAndDeserialize);
  RUN_TEST(TestMidiData_Creators);
}
