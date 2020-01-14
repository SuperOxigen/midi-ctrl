/*
 * MIDI Controller - MIDI Control Change Test
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#include <unity.h>

#include "midi_control.h"
#include "midi_defs.h"

static void TestMidiControlIsValid(void) {
  TEST_ASSERT_FALSE(MidiIsValidControlChange(NULL));
  midi_control_change_t cc = {};
  TEST_ASSERT_TRUE(MidiIsValidControlChange(&cc));
  cc.number = 0x80;
  TEST_ASSERT_FALSE(MidiIsValidControlChange(&cc));
  cc.value = 0xFF;
  TEST_ASSERT_FALSE(MidiIsValidControlChange(&cc));
  cc.number = 0x7F;
  TEST_ASSERT_FALSE(MidiIsValidControlChange(&cc));
  cc.value = 0x10;
  TEST_ASSERT_TRUE(MidiIsValidControlChange(&cc));
}

static void TestMidiControlChangeConstructor(void) {
  TEST_ASSERT_FALSE(MidiControlChange(NULL, MIDI_BANK_SELECT_LSB, 0x02));
  midi_control_change_t cc = {
    .number = MIDI_MAIN_VOLUME_LSB,
    .value = 0x10
  };
  TEST_ASSERT_TRUE(MidiIsValidControlChange(&cc));
  TEST_ASSERT_FALSE(MidiControlChange(&cc, MIDI_BANK_SELECT_LSB | 0x80, 0x02));
  TEST_ASSERT_FALSE(MidiControlChange(&cc, MIDI_BANK_SELECT_LSB, 0x02 | 0x80));
  /* Remain unchanged. */
  TEST_ASSERT_TRUE(MidiIsValidControlChange(&cc));
  TEST_ASSERT_EQUAL(MIDI_MAIN_VOLUME_LSB, cc.number);
  TEST_ASSERT_EQUAL(0x10, cc.value);

  TEST_ASSERT_TRUE(MidiControlChange(&cc, MIDI_BANK_SELECT_LSB, 0x02));
  TEST_ASSERT_TRUE(MidiIsValidControlChange(&cc));
  TEST_ASSERT_EQUAL(MIDI_BANK_SELECT_LSB, cc.number);
  TEST_ASSERT_EQUAL(0x02, cc.value);
}

void MidiControlTest(void) {
  RUN_TEST(TestMidiControlIsValid);
  RUN_TEST(TestMidiControlChangeConstructor);
}
