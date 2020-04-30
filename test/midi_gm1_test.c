/*
 * MIDI Controller - MIDI General MIDI 1 Test
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#include <string.h>
#include <unity.h>

#include "midi_defs.h"
#include "midi_gm1.h"

static void TestGeneralMidiOne_ProgramName_InvalidInput(void) {
  char buffer[64];
  memset(buffer, 0, sizeof(buffer));

  TEST_ASSERT_EQUAL(0, MidiGetGeneralMidiProgramName(
      222, MIDI_NONE, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL(0, MidiGetGeneralMidiProgramName(
      MIDI_ELECTRIC_GRAND_PIANO, 0xFF, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL(0, MidiGetGeneralMidiProgramName(
      MIDI_ELECTRIC_GRAND_PIANO, MIDI_NONE, NULL, sizeof(buffer)));
}

static void TestGeneralMidiOne_ProgramName(void) {
  char buffer[64];
  memset(buffer, 0, sizeof(buffer));

  TEST_ASSERT_EQUAL(5, MidiGetGeneralMidiProgramName(
      MIDI_CLAVI, MIDI_NONE, NULL, 0));

  TEST_ASSERT_EQUAL(5, MidiGetGeneralMidiProgramName(
      MIDI_CLAVI, MIDI_NONE, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("CLAVI", buffer);

  TEST_ASSERT_EQUAL(5, MidiGetGeneralMidiProgramName(
      MIDI_CLAVI, MIDI_GM1_UPPER_CASE, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("CLAVI", buffer);

  TEST_ASSERT_EQUAL(5, MidiGetGeneralMidiProgramName(
      MIDI_CLAVI, MIDI_GM1_LOWER_CASE, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("clavi", buffer);

  TEST_ASSERT_EQUAL(5, MidiGetGeneralMidiProgramName(
      MIDI_CLAVI, MIDI_GM1_CAPITALIZATION, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("Clavi", buffer);

  TEST_ASSERT_EQUAL(7, MidiGetGeneralMidiProgramName(
      MIDI_CLAVI, MIDI_GM1_WITH_NUMBER, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("8 CLAVI", buffer);


  TEST_ASSERT_EQUAL(13, MidiGetGeneralMidiProgramName(
      MIDI_ACOUSTIC_BASS, MIDI_NONE, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("ACOUSTIC_BASS", buffer);

  TEST_ASSERT_EQUAL(13, MidiGetGeneralMidiProgramName(
      MIDI_ACOUSTIC_BASS, MIDI_GM1_UPPER_CASE, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("ACOUSTIC BASS", buffer);

  TEST_ASSERT_EQUAL(13, MidiGetGeneralMidiProgramName(
      MIDI_ACOUSTIC_BASS, MIDI_GM1_LOWER_CASE, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("acoustic bass", buffer);

  TEST_ASSERT_EQUAL(13, MidiGetGeneralMidiProgramName(
      MIDI_ACOUSTIC_BASS, MIDI_GM1_CAPITALIZATION, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("Acoustic Bass", buffer);

  TEST_ASSERT_EQUAL(16, MidiGetGeneralMidiProgramName(
      MIDI_ACOUSTIC_BASS, MIDI_GM1_CAPITALIZATION | MIDI_GM1_WITH_NUMBER,
      buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("33 Acoustic Bass", buffer);
}

static void TestGeneralMidiOne_PercussionName_InvalidInput(void) {
  char buffer[64];
  memset(buffer, 0, sizeof(buffer));

  TEST_ASSERT_EQUAL(0, MidiGetGeneralMidiPercussionName(
      MIDI_ACOUSTIC_BASS_DRUM - 1, MIDI_NONE, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL(0, MidiGetGeneralMidiPercussionName(
      MIDI_OPEN_TRIANGLE + 1, MIDI_NONE, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL(0, MidiGetGeneralMidiPercussionName(
      MIDI_COWBELL, 0xFF, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL(0, MidiGetGeneralMidiPercussionName(
      MIDI_COWBELL, MIDI_NONE, NULL, sizeof(buffer)));
}

static void TestGeneralMidiOne_PercussionName(void) {
  char buffer[64];
  memset(buffer, 0, sizeof(buffer));

  TEST_ASSERT_EQUAL(18, MidiGetGeneralMidiPercussionName(
      MIDI_ACOUSTIC_BASS_DRUM, MIDI_NONE, NULL, 0));

  TEST_ASSERT_EQUAL(18, MidiGetGeneralMidiPercussionName(
      MIDI_ACOUSTIC_BASS_DRUM, MIDI_NONE, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("ACOUSTIC_BASS_DRUM", buffer);

  TEST_ASSERT_EQUAL(18, MidiGetGeneralMidiPercussionName(
      MIDI_ACOUSTIC_BASS_DRUM, MIDI_GM1_UPPER_CASE, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("ACOUSTIC BASS DRUM", buffer);

  TEST_ASSERT_EQUAL(18, MidiGetGeneralMidiPercussionName(
      MIDI_ACOUSTIC_BASS_DRUM, MIDI_GM1_LOWER_CASE, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("acoustic bass drum", buffer);

  TEST_ASSERT_EQUAL(18, MidiGetGeneralMidiPercussionName(
      MIDI_ACOUSTIC_BASS_DRUM, MIDI_GM1_CAPITALIZATION, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("Acoustic Bass Drum", buffer);

  TEST_ASSERT_EQUAL(21, MidiGetGeneralMidiPercussionName(
      MIDI_ACOUSTIC_BASS_DRUM, MIDI_GM1_WITH_NUMBER, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("35 ACOUSTIC_BASS_DRUM", buffer);


  TEST_ASSERT_EQUAL(13, MidiGetGeneralMidiPercussionName(
      MIDI_OPEN_TRIANGLE, MIDI_NONE, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("OPEN_TRIANGLE", buffer);

  TEST_ASSERT_EQUAL(13, MidiGetGeneralMidiPercussionName(
      MIDI_OPEN_TRIANGLE, MIDI_GM1_UPPER_CASE, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("OPEN TRIANGLE", buffer);

  TEST_ASSERT_EQUAL(13, MidiGetGeneralMidiPercussionName(
      MIDI_OPEN_TRIANGLE, MIDI_GM1_LOWER_CASE, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("open triangle", buffer);

  TEST_ASSERT_EQUAL(13, MidiGetGeneralMidiPercussionName(
      MIDI_OPEN_TRIANGLE, MIDI_GM1_CAPITALIZATION, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("Open Triangle", buffer);

  TEST_ASSERT_EQUAL(16, MidiGetGeneralMidiPercussionName(
      MIDI_OPEN_TRIANGLE, MIDI_GM1_WITH_NUMBER, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("81 OPEN_TRIANGLE", buffer);
}

void MidiGeneralMidiOneTest(void) {
  RUN_TEST(TestGeneralMidiOne_ProgramName_InvalidInput);
  RUN_TEST(TestGeneralMidiOne_ProgramName);

  RUN_TEST(TestGeneralMidiOne_PercussionName_InvalidInput);
  RUN_TEST(TestGeneralMidiOne_PercussionName);
}
