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

static void TestMidiManufacturerId_Validators(void) {
  TEST_ASSERT_FALSE(MidiIsValidManufacturerId(NULL));
  TEST_ASSERT_FALSE(MidiIsBlankManufacturerId(NULL));
  midi_manufacturer_id_t man_id = {
    0x2D, 0x00, 0x01
  };
  TEST_ASSERT_FALSE(MidiIsValidManufacturerId(man_id));
  TEST_ASSERT_FALSE(MidiIsBlankManufacturerId(man_id));
  man_id[2] = 0x00;
  TEST_ASSERT_TRUE(MidiIsValidManufacturerId(man_id));
  man_id[0] = 0x00;
  TEST_ASSERT_FALSE(MidiIsValidManufacturerId(man_id));
  TEST_ASSERT_TRUE(MidiIsBlankManufacturerId(man_id));
  man_id[2] = 0x02;
  TEST_ASSERT_TRUE(MidiIsValidManufacturerId(man_id));
}

static void TestMidiManufacturerId_Initializer(void) {
  TEST_ASSERT_FALSE(MidiInitializeManufacturerId(NULL));
  midi_manufacturer_id_t man_id;
  TEST_ASSERT_TRUE(MidiInitializeManufacturerId(man_id));
  TEST_ASSERT_TRUE(MidiIsBlankManufacturerId(man_id));
  man_id[0] = MIDI_REAL_TIME_ID;
  TEST_ASSERT_TRUE(MidiInitializeManufacturerId(man_id));
  TEST_ASSERT_TRUE(MidiIsBlankManufacturerId(man_id));
}

static uint8_t const kAmericanOne[] = {0x01, 0x00, 0x00};
static uint8_t const kAmericanTwo[] = {0x1F, 0x00, 0x00};
static uint8_t const kAmericanThree[] = {0x00, 0x00, 0x01};
static uint8_t const kAmericanFour[] = {0x00, 0x1F, 0x7F};
static uint8_t const kEuropeanOne[] = {0x20, 0x00, 0x00};
static uint8_t const kEuropeanTwo[] = {0x3F, 0x00, 0x00};
static uint8_t const kEuropeanThree[] = {0x00, 0x20, 0x00};
static uint8_t const kEuropeanFour[] = {0x00, 0x3F, 0x7F};
static uint8_t const kJapaneseOne[] = {0x40, 0x00, 0x00};
static uint8_t const kJapaneseTwo[] = {0x5F, 0x00, 0x00};
static uint8_t const kJapaneseThree[] = {0x00, 0x40, 0x00};
static uint8_t const kJapaneseFour[] = {0x00, 0x5F, 0x7F};
static uint8_t const kOtherOne[] = {0x60, 0x00, 0x00};
static uint8_t const kOtherTwo[] = {0x7C, 0x00, 0x00};
static uint8_t const kOtherThree[] = {0x00, 0x60, 0x00};
static uint8_t const kOtherFour[] = {0x00, 0x7F, 0x7F};
static uint8_t const kSpecialOne[] = {0x7D, 0x00, 0x00};
static uint8_t const kSpecialTwo[] = {MIDI_NON_REAL_TIME_ID, 0x00, 0x00};
static uint8_t const kSpecialThree[] = {MIDI_REAL_TIME_ID, 0x00, 0x00};
#define kUniversalOne kSpecialTwo
#define kUniversalTwo kSpecialThree

static void TestMidiManufacturerId_Region(void) {
  TEST_ASSERT_EQUAL(MIDI_REGION_AMERICA, MidiManufacturerRegion(kAmericanOne));
  TEST_ASSERT_EQUAL(MIDI_REGION_AMERICA, MidiManufacturerRegion(kAmericanTwo));
  TEST_ASSERT_EQUAL(MIDI_REGION_AMERICA, MidiManufacturerRegion(kAmericanThree));
  TEST_ASSERT_EQUAL(MIDI_REGION_AMERICA, MidiManufacturerRegion(kAmericanFour));
  TEST_ASSERT_EQUAL(MIDI_REGION_EUROPEAN, MidiManufacturerRegion(kEuropeanOne));
  TEST_ASSERT_EQUAL(MIDI_REGION_EUROPEAN, MidiManufacturerRegion(kEuropeanTwo));
  TEST_ASSERT_EQUAL(MIDI_REGION_EUROPEAN, MidiManufacturerRegion(kEuropeanThree));
  TEST_ASSERT_EQUAL(MIDI_REGION_EUROPEAN, MidiManufacturerRegion(kEuropeanFour));
  TEST_ASSERT_EQUAL(MIDI_REGION_JAPANESE, MidiManufacturerRegion(kJapaneseOne));
  TEST_ASSERT_EQUAL(MIDI_REGION_JAPANESE, MidiManufacturerRegion(kJapaneseTwo));
  TEST_ASSERT_EQUAL(MIDI_REGION_JAPANESE, MidiManufacturerRegion(kJapaneseThree));
  TEST_ASSERT_EQUAL(MIDI_REGION_JAPANESE, MidiManufacturerRegion(kJapaneseFour));
  TEST_ASSERT_EQUAL(MIDI_REGION_OTHER, MidiManufacturerRegion(kOtherOne));
  TEST_ASSERT_EQUAL(MIDI_REGION_OTHER, MidiManufacturerRegion(kOtherTwo));
  TEST_ASSERT_EQUAL(MIDI_REGION_OTHER, MidiManufacturerRegion(kOtherThree));
  TEST_ASSERT_EQUAL(MIDI_REGION_OTHER, MidiManufacturerRegion(kOtherFour));
  TEST_ASSERT_EQUAL(MIDI_REGION_SPECIAL, MidiManufacturerRegion(kSpecialOne));
  TEST_ASSERT_EQUAL(MIDI_REGION_SPECIAL, MidiManufacturerRegion(kSpecialTwo));
  TEST_ASSERT_EQUAL(MIDI_REGION_SPECIAL, MidiManufacturerRegion(kSpecialThree));
}

static void TestMidiManufacturerId_UniversalCheck(void) {
  TEST_ASSERT_FALSE(MidiManufacturerIdIsUniversal(kAmericanOne));
  TEST_ASSERT_FALSE(MidiManufacturerIdIsUniversal(kAmericanTwo));
  TEST_ASSERT_FALSE(MidiManufacturerIdIsUniversal(kAmericanThree));
  TEST_ASSERT_FALSE(MidiManufacturerIdIsUniversal(kAmericanFour));
  TEST_ASSERT_FALSE(MidiManufacturerIdIsUniversal(kEuropeanOne));
  TEST_ASSERT_FALSE(MidiManufacturerIdIsUniversal(kEuropeanTwo));
  TEST_ASSERT_FALSE(MidiManufacturerIdIsUniversal(kEuropeanThree));
  TEST_ASSERT_FALSE(MidiManufacturerIdIsUniversal(kEuropeanFour));
  TEST_ASSERT_FALSE(MidiManufacturerIdIsUniversal(kJapaneseOne));
  TEST_ASSERT_FALSE(MidiManufacturerIdIsUniversal(kJapaneseTwo));
  TEST_ASSERT_FALSE(MidiManufacturerIdIsUniversal(kJapaneseThree));
  TEST_ASSERT_FALSE(MidiManufacturerIdIsUniversal(kJapaneseFour));
  TEST_ASSERT_FALSE(MidiManufacturerIdIsUniversal(kOtherOne));
  TEST_ASSERT_FALSE(MidiManufacturerIdIsUniversal(kOtherTwo));
  TEST_ASSERT_FALSE(MidiManufacturerIdIsUniversal(kOtherThree));
  TEST_ASSERT_FALSE(MidiManufacturerIdIsUniversal(kOtherFour));
  TEST_ASSERT_FALSE(MidiManufacturerIdIsUniversal(kSpecialOne));
  TEST_ASSERT_TRUE(MidiManufacturerIdIsUniversal(kUniversalOne));
  TEST_ASSERT_TRUE(MidiManufacturerIdIsUniversal(kUniversalTwo));
}

static void  TestMidiSysEx_Validator(void) {
  TEST_ASSERT_FALSE(MidiIsValidSysEx(NULL));
  midi_sys_ex_t sys_ex = {};
  TEST_ASSERT_FALSE(MidiIsValidSysEx(&sys_ex));
  memcpy(sys_ex.id, kJapaneseOne, sizeof(sys_ex.id));
  TEST_ASSERT_TRUE(MidiIsValidSysEx(&sys_ex));
}

static void  TestMidiSysEx_Initializer(void) {
  midi_sys_ex_t sys_ex;
  TEST_ASSERT_FALSE(MidiInitializeSysEx(NULL, NULL));
  TEST_ASSERT_FALSE(MidiInitializeSysEx(NULL, kAmericanTwo));
  sys_ex.id[0] = MIDI_NON_REAL_TIME_ID;
  /* Initialize as blank. */
  TEST_ASSERT_TRUE(MidiInitializeSysEx(&sys_ex, NULL));
  TEST_ASSERT_TRUE(MidiIsBlankManufacturerId(sys_ex.id));

  TEST_ASSERT_TRUE(MidiInitializeSysEx(&sys_ex, kAmericanTwo));
  TEST_ASSERT_FALSE(MidiIsBlankManufacturerId(sys_ex.id));
  TEST_ASSERT_TRUE(MidiIsValidSysEx(&sys_ex));
  TEST_ASSERT_EQUAL_MEMORY(kAmericanTwo, sys_ex.id, sizeof(sys_ex.id));
}

static void  TestMidiSysEx_ValidatorForUniversal(void) {
  midi_sys_ex_t sys_ex;
  TEST_ASSERT_TRUE(MidiInitializeSysEx(&sys_ex, kUniversalOne));
  TEST_ASSERT_FALSE(MidiIsValidSysEx(&sys_ex));
  sys_ex.sub_id_1 = MIDI_DATA_PACKET;
  TEST_ASSERT_TRUE(MidiIsValidSysEx(&sys_ex));
}

void MidiSystemExclusiveTest(void) {
  RUN_TEST(TestMidiManufacturerId_Validators);
  RUN_TEST(TestMidiManufacturerId_Initializer);
  RUN_TEST(TestMidiManufacturerId_Region);
  RUN_TEST(TestMidiManufacturerId_UniversalCheck);
  RUN_TEST(TestMidiSysEx_Validator);
  RUN_TEST(TestMidiSysEx_Initializer);
  RUN_TEST(TestMidiSysEx_ValidatorForUniversal);
}
