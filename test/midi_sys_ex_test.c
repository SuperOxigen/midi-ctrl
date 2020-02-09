/*
 * MIDI Controller - MIDI System Exclusive Test
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#include <string.h>
#include <unity.h>

#include "midi_defs.h"
#include "midi_sys_ex.h"

static uint8_t const kAmericanOne[] = {0x01, 0x00, 0x00};
static uint8_t const kUniversalOne[] = {MIDI_NON_REAL_TIME_ID, 0x00, 0x00};

static void TestMidiSysEx_Validator(void) {
  TEST_ASSERT_FALSE(MidiIsValidSysEx(NULL));
  midi_sys_ex_t sys_ex = {};
  TEST_ASSERT_FALSE(MidiIsValidSysEx(&sys_ex));
  memcpy(sys_ex.id, kAmericanOne, sizeof(sys_ex.id));
  TEST_ASSERT_TRUE(MidiIsValidSysEx(&sys_ex));
}

static void TestMidiSysEx_Initializer(void) {
  midi_sys_ex_t sys_ex;
  TEST_ASSERT_FALSE(MidiInitializeSysEx(NULL, NULL));
  TEST_ASSERT_FALSE(MidiInitializeSysEx(NULL, kAmericanOne));
  sys_ex.id[0] = MIDI_NON_REAL_TIME_ID;
  /* Initialize as blank. */
  TEST_ASSERT_TRUE(MidiInitializeSysEx(&sys_ex, NULL));
  TEST_ASSERT_TRUE(MidiIsBlankManufacturerId(sys_ex.id));

  TEST_ASSERT_TRUE(MidiInitializeSysEx(&sys_ex, kAmericanOne));
  TEST_ASSERT_FALSE(MidiIsBlankManufacturerId(sys_ex.id));
  TEST_ASSERT_TRUE(MidiIsValidSysEx(&sys_ex));
  TEST_ASSERT_EQUAL_MEMORY(kAmericanOne, sys_ex.id, sizeof(sys_ex.id));
}

static void TestMidiSysEx_ValidatorForUniversal(void) {
  midi_sys_ex_t sys_ex;
  TEST_ASSERT_TRUE(MidiInitializeSysEx(&sys_ex, kUniversalOne));
  TEST_ASSERT_FALSE(MidiIsValidSysEx(&sys_ex));
  sys_ex.sub_id = MIDI_DATA_PACKET;
  TEST_ASSERT_TRUE(MidiIsValidSysEx(&sys_ex));
}

void MidiSystemExclusiveTest(void) {
  RUN_TEST(TestMidiSysEx_Validator);
  RUN_TEST(TestMidiSysEx_Initializer);
  RUN_TEST(TestMidiSysEx_ValidatorForUniversal);
}
