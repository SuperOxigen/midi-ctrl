/*
 * MIDI Controller - MIDI Program Number Test.
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#include <unity.h>

#include "midi_defs.h"
#include "midi_program.h"

static void MidiProgramNumberToFamilyTest(void) {
  TEST_ASSERT_EQUAL(
      MIDI_FAMILY_PIANO,
      MidiProgramNumberToFamily(MIDI_ACOUSTIC_GRAND_PIANO));
  TEST_ASSERT_EQUAL(
      MIDI_FAMILY_PIANO,
      MidiProgramNumberToFamily(MIDI_CLAVI));
  TEST_ASSERT_EQUAL(
      MIDI_FAMILY_CHROMATIC_PERCUSSION,
      MidiProgramNumberToFamily(MIDI_CELESTA));
  TEST_ASSERT_EQUAL(
      MIDI_FAMILY_CHROMATIC_PERCUSSION,
      MidiProgramNumberToFamily(MIDI_DUCLIMER));
  TEST_ASSERT_EQUAL(
      MIDI_FAMILY_ORGAN,
      MidiProgramNumberToFamily(MIDI_DRAWBAR_ORGAN));
  TEST_ASSERT_EQUAL(
      MIDI_FAMILY_ORGAN,
      MidiProgramNumberToFamily(MIDI_TANGO_ACCORDION));
  TEST_ASSERT_EQUAL(
      MIDI_FAMILY_GUITAR,
      MidiProgramNumberToFamily(MIDI_ACOUSTIC_GUITAR_NYLON));
  TEST_ASSERT_EQUAL(
      MIDI_FAMILY_GUITAR,
      MidiProgramNumberToFamily(MIDI_GUITAR_HARMONICS));
  TEST_ASSERT_EQUAL(
      MIDI_FAMILY_BASE,
      MidiProgramNumberToFamily(MIDI_ACOUSTIC_BASS));
  TEST_ASSERT_EQUAL(
      MIDI_FAMILY_BASE,
      MidiProgramNumberToFamily(MIDI_SYNTH_BASS_2));
  TEST_ASSERT_EQUAL(
      MIDI_FAMILY_STRINGS,
      MidiProgramNumberToFamily(MIDI_VIOLIN));
  TEST_ASSERT_EQUAL(
      MIDI_FAMILY_STRINGS,
      MidiProgramNumberToFamily(MIDI_TIMPANI));
  TEST_ASSERT_EQUAL(
      MIDI_FAMILY_ENSEMBLE,
      MidiProgramNumberToFamily(MIDI_STRING_ENSEMBLE_1));
  TEST_ASSERT_EQUAL(
      MIDI_FAMILY_ENSEMBLE,
      MidiProgramNumberToFamily(MIDI_ORCHESTRA_HIT));
  TEST_ASSERT_EQUAL(
      MIDI_FAMILY_BRASS,
      MidiProgramNumberToFamily(MIDI_TRUMPET));
  TEST_ASSERT_EQUAL(
      MIDI_FAMILY_BRASS,
      MidiProgramNumberToFamily(MIDI_SYNTH_BRASS_2));
  TEST_ASSERT_EQUAL(
      MIDI_FAMILY_REED,
      MidiProgramNumberToFamily(MIDI_SOPRANO_SAX));
  TEST_ASSERT_EQUAL(
      MIDI_FAMILY_REED,
      MidiProgramNumberToFamily(MIDI_CLARINET));
  TEST_ASSERT_EQUAL(
      MIDI_FAMILY_PIPE,
      MidiProgramNumberToFamily(MIDI_PICCOLO));
  TEST_ASSERT_EQUAL(
      MIDI_FAMILY_PIPE,
      MidiProgramNumberToFamily(MIDI_OCARINA));
  TEST_ASSERT_EQUAL(
      MIDI_FAMILY_SYNTH_LEAD,
      MidiProgramNumberToFamily(MIDI_LEAD_1_SQUARE));
  TEST_ASSERT_EQUAL(
      MIDI_FAMILY_SYNTH_LEAD,
      MidiProgramNumberToFamily(MIDI_LEAD_8_BASS_LEAD));
  TEST_ASSERT_EQUAL(
      MIDI_FAMILY_SYNTH_PAD,
      MidiProgramNumberToFamily(MIDI_PAD_1_NEW_AGE));
  TEST_ASSERT_EQUAL(
      MIDI_FAMILY_SYNTH_PAD,
      MidiProgramNumberToFamily(MIDI_PAD_8_SWEEP));
  TEST_ASSERT_EQUAL(
      MIDI_FAMILY_SYNTH_EFFECTS,
      MidiProgramNumberToFamily(MIDI_FX_1_RAIN));
  TEST_ASSERT_EQUAL(
      MIDI_FAMILY_SYNTH_EFFECTS,
      MidiProgramNumberToFamily(MIDI_FX_8_SCI_FI));
  TEST_ASSERT_EQUAL(
      MIDI_FAMILY_ETHNIC,
      MidiProgramNumberToFamily(MIDI_SITAR));
  TEST_ASSERT_EQUAL(
      MIDI_FAMILY_ETHNIC,
      MidiProgramNumberToFamily(MIDI_SHANAI));
  TEST_ASSERT_EQUAL(
      MIDI_FAMILY_PERCUSSIVE,
      MidiProgramNumberToFamily(MIDI_TINKLE_BELL));
  TEST_ASSERT_EQUAL(
      MIDI_FAMILY_PERCUSSIVE,
      MidiProgramNumberToFamily(MIDI_REVERSE_CYMBAL));
  TEST_ASSERT_EQUAL(
      MIDI_FAMILY_SOUND_EFFECTS,
      MidiProgramNumberToFamily(MIDI_GUITAR_FRET_NOISE));
  TEST_ASSERT_EQUAL(
      MIDI_FAMILY_SOUND_EFFECTS,
      MidiProgramNumberToFamily(MIDI_GUNSHOT));
}

void MidiProgramTest(void) {
  RUN_TEST(MidiProgramNumberToFamilyTest);
}
