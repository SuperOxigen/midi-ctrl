/*
 * MIDI Controller - MIDI Manufacturer ID Test
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#include <string.h>
#include <unity.h>

#include "midi_defs.h"
#include "midi_man_id.h"

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
  TEST_ASSERT_FALSE(MidiClearManufacturerId(NULL));
  midi_manufacturer_id_t man_id;
  TEST_ASSERT_TRUE(MidiClearManufacturerId(man_id));
  TEST_ASSERT_TRUE(MidiIsBlankManufacturerId(man_id));
  man_id[0] = MIDI_REAL_TIME_ID;
  TEST_ASSERT_TRUE(MidiClearManufacturerId(man_id));
  TEST_ASSERT_TRUE(MidiIsBlankManufacturerId(man_id));
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

static void TestMidiManufacturerId_ToString(void) {
  uint8_t const kShort[] = {0x40, 0x00, 0x00};
  uint8_t const kLong[] = {0x00, 0x3f, 0x32};
  uint8_t const kBadShort[] = {0x40, 0x00, 0x10};
  uint8_t const kBadLong[] = {0x00, 0x4F, 0x83};
  char id_repr[16];
  /* Invalid input. */
  TEST_ASSERT_EQUAL(
      0, MidiManufacturerIdToString(NULL, NULL, sizeof(id_repr)));
  TEST_ASSERT_EQUAL(
      0, MidiManufacturerIdToString(kShort, NULL, sizeof(id_repr)));
  TEST_ASSERT_EQUAL(
      0, MidiManufacturerIdToString(NULL, id_repr, sizeof(id_repr)));
  TEST_ASSERT_EQUAL(
      0, MidiManufacturerIdToString(kBadShort, id_repr, sizeof(id_repr)));
  TEST_ASSERT_EQUAL(
      0, MidiManufacturerIdToString(kBadLong, id_repr, sizeof(id_repr)));

  /* 40 */
  TEST_ASSERT_EQUAL(
      2, MidiManufacturerIdToString(kShort, id_repr, sizeof(id_repr)));
  TEST_ASSERT_EQUAL_STRING("40", id_repr);

  /* 3F-32 */
  TEST_ASSERT_EQUAL(
      5, MidiManufacturerIdToString(kLong, id_repr, sizeof(id_repr)));
  TEST_ASSERT_EQUAL_STRING("3F-32", id_repr);

  /* Truncated */
  TEST_ASSERT_EQUAL(
      2, MidiManufacturerIdToString(kShort, id_repr, 1));
  TEST_ASSERT_EQUAL_STRING("", id_repr);
  TEST_ASSERT_EQUAL(
      2, MidiManufacturerIdToString(kShort, id_repr, 2));
  TEST_ASSERT_EQUAL_STRING("4", id_repr);

  TEST_ASSERT_EQUAL(
      5, MidiManufacturerIdToString(kLong, id_repr, 3));
  TEST_ASSERT_EQUAL_STRING("3F", id_repr);
  TEST_ASSERT_EQUAL(
      5, MidiManufacturerIdToString(kLong, id_repr, 4));
  TEST_ASSERT_EQUAL_STRING("3F-", id_repr);
  TEST_ASSERT_EQUAL(
      5, MidiManufacturerIdToString(kLong, id_repr, 6));
  TEST_ASSERT_EQUAL_STRING("3F-32", id_repr);
}

static void TestMidiManufacturerId_Serial(void) {
  static uint8_t const kShort[] = {0x40, 0x00, 0x00};
  static uint8_t const kLong[] = {0x00, 0x3F, 0x32};
  static uint8_t const kBadShort[] = {0x40, 0x00, 0x10};
  static uint8_t const kBadLong[] = {0x00, 0x4F, 0x83};
  uint8_t id_data[4];
  /* Invalid input. */
  TEST_ASSERT_EQUAL(
      0, MidiSerializeManufacturerId(NULL, NULL, sizeof(id_data)));
  TEST_ASSERT_EQUAL(
      0, MidiSerializeManufacturerId(kShort, NULL, sizeof(id_data)));
  TEST_ASSERT_EQUAL(
      0, MidiSerializeManufacturerId(NULL, id_data, sizeof(id_data)));
  TEST_ASSERT_EQUAL(
      0, MidiSerializeManufacturerId(kBadShort, id_data, sizeof(id_data)));
  TEST_ASSERT_EQUAL(
      0, MidiSerializeManufacturerId(kBadLong, id_data, sizeof(id_data)));

  TEST_ASSERT_EQUAL(1,
      MidiSerializeManufacturerId(kShort, id_data, sizeof(id_data)));
  TEST_ASSERT_EQUAL_MEMORY(kShort, id_data, 1);

  TEST_ASSERT_EQUAL(3,
      MidiSerializeManufacturerId(kLong, id_data, sizeof(id_data)));
  TEST_ASSERT_EQUAL_MEMORY(kLong, id_data, 3);

  /* Truncated */
  TEST_ASSERT_EQUAL(3,
      MidiSerializeManufacturerId(kLong, id_data, 0));
  id_data[2] = 0xFF;
  TEST_ASSERT_EQUAL(3,
      MidiSerializeManufacturerId(kLong, id_data, 2));
  TEST_ASSERT_EQUAL(0xFF, id_data[2]);
  TEST_ASSERT_EQUAL_MEMORY(kLong, id_data, 2);
}

static void TestMidiManufacturerId_Deserial(void) {
  static uint8_t const kShort[] = {0x40};
  static uint8_t const kLong[] = {0x00, 0x3F, 0x32};
  static uint8_t const kBadShort[] = {0xA0};
  static uint8_t const kBadLong[] = {0x00, 0x4F, 0x83};
  midi_manufacturer_id_t man_id;

  /* Invalid input. */
  TEST_ASSERT_EQUAL(
      0, MidiDeserializeManufacturerId(NULL, sizeof(kShort), NULL));
  TEST_ASSERT_EQUAL(
      0, MidiDeserializeManufacturerId(kShort, sizeof(kShort), NULL));
  TEST_ASSERT_EQUAL(
      0, MidiDeserializeManufacturerId(NULL, sizeof(kShort), man_id));
  TEST_ASSERT_EQUAL(
      0, MidiDeserializeManufacturerId(kShort, 0, man_id));
  TEST_ASSERT_EQUAL(
      0, MidiDeserializeManufacturerId(kLong, 2, man_id));
  TEST_ASSERT_EQUAL(
      0, MidiDeserializeManufacturerId(kBadShort, sizeof(kBadShort), man_id));
  TEST_ASSERT_EQUAL(
      0, MidiDeserializeManufacturerId(kBadLong, sizeof(kBadLong), man_id));

  TEST_ASSERT_EQUAL(
      1, MidiDeserializeManufacturerId(kShort, sizeof(kShort), man_id));
  TEST_ASSERT_EQUAL(
      3, MidiDeserializeManufacturerId(kLong, sizeof(kLong), man_id));
}

void MidiManufacturerIdTest(void) {
  RUN_TEST(TestMidiManufacturerId_Validators);
  RUN_TEST(TestMidiManufacturerId_Initializer);
  RUN_TEST(TestMidiManufacturerId_UniversalCheck);
  RUN_TEST(TestMidiManufacturerId_Region);
  RUN_TEST(TestMidiManufacturerId_ToString);
  RUN_TEST(TestMidiManufacturerId_Serial);
  RUN_TEST(TestMidiManufacturerId_Deserial);
}
