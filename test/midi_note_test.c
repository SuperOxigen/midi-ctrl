/*
 * MIDI Controller - MIDI Note Test
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#include <unity.h>

#include "midi_note.h"

void TestMidiKeyIsValid(void) {
  TEST_ASSERT_FALSE(MidiIsValidNote(NULL));
  midi_note_t note = {};
  TEST_ASSERT_TRUE(MidiIsValidNote(&note));
  note.key = 0xFF;
  TEST_ASSERT_FALSE(MidiIsValidNote(&note));
  note.key = 0x05;
  note.velocity = 0xFF;
  TEST_ASSERT_FALSE(MidiIsValidNote(&note));
  note.velocity = 0x01;
  note.pressure = 0xFF;
  TEST_ASSERT_FALSE(MidiIsValidNote(&note));
  note.pressure = 0x0E;
  TEST_ASSERT_TRUE(MidiIsValidNote(&note));
}

void TestMidiNoteConstructor_Note(void) {
  TEST_ASSERT_FALSE(MidiNote(NULL, 0, 0));
  midi_note_t note = {};
  TEST_ASSERT_TRUE(MidiNote(&note, 0, 0));
  TEST_ASSERT_FALSE(MidiNote(&note, 0xFF, 0));
  TEST_ASSERT_FALSE(MidiNote(&note, 0, 0xFF));
  TEST_ASSERT_TRUE(MidiIsValidNote(&note));

  TEST_ASSERT_TRUE(MidiNote(&note, 0x55, 0x2A));
  TEST_ASSERT_TRUE(MidiIsValidNote(&note));
  TEST_ASSERT_EQUAL(0x55, note.key);
  TEST_ASSERT_EQUAL(0x2A, note.velocity);
  TEST_ASSERT_EQUAL(0x00, note.pressure);
}

void TestMidiNoteConstructor_NotePressure(void) {
  TEST_ASSERT_FALSE(MidiNotePressure(NULL, 0, 0));
  midi_note_t note = {};
  TEST_ASSERT_TRUE(MidiNotePressure(&note, 0, 0));
  TEST_ASSERT_FALSE(MidiNotePressure(&note, 0xFF, 0));
  TEST_ASSERT_FALSE(MidiNotePressure(&note, 0, 0xFF));
  TEST_ASSERT_TRUE(MidiIsValidNote(&note));

  TEST_ASSERT_TRUE(MidiNotePressure(&note, 0x55, 0x2A));
  TEST_ASSERT_TRUE(MidiIsValidNote(&note));
  TEST_ASSERT_EQUAL(0x55, note.key);
  TEST_ASSERT_EQUAL(0x00, note.velocity);
  TEST_ASSERT_EQUAL(0x2A, note.pressure);
}

void TestMidiNoteConstructor_Setters(void) {
  TEST_ASSERT_FALSE(MidiSetNoteKey(NULL, 0));
  TEST_ASSERT_FALSE(MidiSetNoteVelocity(NULL, 0));
  TEST_ASSERT_FALSE(MidiSetNotePressure(NULL, 0));
  midi_note_t note = {};
  TEST_ASSERT_TRUE(MidiSetNoteKey(&note, 0x12));
  TEST_ASSERT_TRUE(MidiSetNoteVelocity(&note, 0x24));
  TEST_ASSERT_TRUE(MidiSetNotePressure(&note, 0x48));
  TEST_ASSERT_TRUE(MidiIsValidNote(&note));
  TEST_ASSERT_EQUAL(0x12, note.key);
  TEST_ASSERT_EQUAL(0x24, note.velocity);
  TEST_ASSERT_EQUAL(0x48, note.pressure);

  TEST_ASSERT_FALSE(MidiSetNoteKey(&note, 0x92));
  TEST_ASSERT_FALSE(MidiSetNoteVelocity(&note, 0xA4));
  TEST_ASSERT_FALSE(MidiSetNotePressure(&note, 0xC8));
  TEST_ASSERT_TRUE(MidiIsValidNote(&note));
  TEST_ASSERT_EQUAL(0x12, note.key);
  TEST_ASSERT_EQUAL(0x24, note.velocity);
  TEST_ASSERT_EQUAL(0x48, note.pressure);
}

void MidiNoteTest(void) {
  RUN_TEST(TestMidiKeyIsValid);
  RUN_TEST(TestMidiNoteConstructor_Note);
  RUN_TEST(TestMidiNoteConstructor_NotePressure);
  RUN_TEST(TestMidiNoteConstructor_Setters);
}
