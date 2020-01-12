/*
 * MIDI Controller - MIDI Bytes Info Test
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#include <unity.h>

#include "midi_bytes.h"

void TestMidiStatusByte_Validators(void) {
  TEST_ASSERT_TRUE(MidiIsStatusByte(0xFF));
  TEST_ASSERT_TRUE(MidiIsStatusByte(0x80));
  TEST_ASSERT_FALSE(MidiIsStatusByte(0x00));
  TEST_ASSERT_FALSE(MidiIsStatusByte(0x7F));
}

void TestMidiDataByte_Validators(void) {
  TEST_ASSERT_FALSE(MidiIsDataByte(0xFF));
  TEST_ASSERT_FALSE(MidiIsDataByte(0x80));
  TEST_ASSERT_TRUE(MidiIsDataByte(0x00));
  TEST_ASSERT_TRUE(MidiIsDataByte(0x7F));
}

void TestMidiDataWord_Validators(void) {
  TEST_ASSERT_FALSE(MidiIsDataWord(0xFFFF));
  TEST_ASSERT_FALSE(MidiIsDataWord(0xFF3F));
  TEST_ASSERT_TRUE(MidiIsDataWord(0x0000));
  TEST_ASSERT_TRUE(MidiIsDataWord(0x3FFF));
}

void TestMidiDataWord_Getters(void) {
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

void TestMidiDataWord_Setters(void) {
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

void MidiBytesTest(void) {
  RUN_TEST(TestMidiStatusByte_Validators);
  RUN_TEST(TestMidiDataByte_Validators);
  RUN_TEST(TestMidiDataWord_Validators);
  RUN_TEST(TestMidiDataWord_Getters);
  RUN_TEST(TestMidiDataWord_Setters);
}
