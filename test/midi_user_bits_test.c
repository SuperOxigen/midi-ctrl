/*
 * MIDI Controller - MIDI User Bits Test.
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#include <string.h>
#include <unity.h>

#include "midi_user_bits.h"

static uint8_t const kUserData[MIDI_USER_BITS_DATA_SIZE] = {
  0x89, 0xAB, 0xCD, 0xEF
};

static uint8_t const kValidUserBitsData[] = {
  0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A, 0x09, 0x08, 0x01
};
static midi_user_bits_t const kValidUserBits = {
  .data = {0x89, 0xAB, 0xCD, 0xEF},
  .bg_flags = 0x01
};

static uint8_t const kInvalidUserBitsData[] = {
  0x0F, 0x0E, 0x0D, 0x0C, 0x1B, 0x0A, 0x09, 0x08, 0x01
};
static midi_user_bits_t const kInvalidUserBits = {
  .data = {0x89, 0xAB, 0xCD, 0xEF},
  .bg_flags = 0x04
};

static void TestMidiUserBits_Validator(void) {
  midi_user_bits_t user_bits;
  TEST_ASSERT_FALSE(MidiIsValidUserBits(NULL));
  user_bits.bg_flags = 0x10;
  TEST_ASSERT_FALSE(MidiIsValidUserBits(&user_bits));
  user_bits.bg_flags = 0x04;
  TEST_ASSERT_FALSE(MidiIsValidUserBits(&user_bits));
  user_bits.bg_flags = 0x00;
  TEST_ASSERT_TRUE(MidiIsValidUserBits(&user_bits));
  user_bits.bg_flags = 0x03;
  TEST_ASSERT_TRUE(MidiIsValidUserBits(&user_bits));
}

static void TestMidiUserBits_Initializer(void) {
  midi_user_bits_t user_bits;
  TEST_ASSERT_FALSE(MidiInitializeUserBits(
      NULL, kUserData, MIDI_USER_BITS_DATA_SIZE));
  TEST_ASSERT_FALSE(MidiInitializeUserBits(
      &user_bits, NULL, MIDI_USER_BITS_DATA_SIZE));
  TEST_ASSERT_FALSE(MidiInitializeUserBits(
      &user_bits, kUserData, MIDI_USER_BITS_DATA_SIZE - 1));
  TEST_ASSERT_FALSE(MidiInitializeUserBits(
      &user_bits, kUserData, MIDI_USER_BITS_DATA_SIZE + 1));

  memset(&user_bits, 0xFF, sizeof(midi_user_bits_t));
  TEST_ASSERT_TRUE(MidiInitializeUserBits(
      &user_bits, kUserData, MIDI_USER_BITS_DATA_SIZE));
  TEST_ASSERT_EQUAL_MEMORY(kUserData, user_bits.data, MIDI_USER_BITS_DATA_SIZE);
  TEST_ASSERT_EQUAL(0x00, (user_bits.bg_flags & 0xFC));
}

static void TestMidiUserBits_Serialize(void) {
  uint8_t buffer[16];
  /* Invalid input */
  TEST_ASSERT_EQUAL(0, MidiSerializeUserBits(
      NULL, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL(0, MidiSerializeUserBits(
      &kValidUserBits, NULL, sizeof(buffer)));
  TEST_ASSERT_EQUAL(0, MidiSerializeUserBits(
      &kInvalidUserBits, buffer, sizeof(buffer)));

  /* Partial serialization */
  TEST_ASSERT_EQUAL(9, MidiSerializeUserBits(
      &kValidUserBits, NULL, 0));
  TEST_ASSERT_EQUAL(9, MidiSerializeUserBits(
      &kValidUserBits, buffer, 7));

  /* Successful serialization. */
  TEST_ASSERT_EQUAL(9, MidiSerializeUserBits(
      &kValidUserBits, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_MEMORY(
      kValidUserBitsData, buffer, sizeof(kValidUserBitsData));
}

static void TestMidiUserBits_Deserialize(void) {
  midi_user_bits_t user_bits;
  /* Invalid input */
  TEST_ASSERT_EQUAL(0, MidiDeserializeUserBits(
      NULL, sizeof(kValidUserBitsData), &user_bits));
  TEST_ASSERT_EQUAL(0, MidiDeserializeUserBits(
      kValidUserBitsData, sizeof(kValidUserBitsData), NULL));
  TEST_ASSERT_EQUAL(0, MidiDeserializeUserBits(
      kInvalidUserBitsData, sizeof(kInvalidUserBitsData), &user_bits));

  /* Partial deserialization. */
  TEST_ASSERT_EQUAL(9, MidiDeserializeUserBits(
      NULL, 0, &user_bits));
  TEST_ASSERT_EQUAL(9, MidiDeserializeUserBits(
      kValidUserBitsData, 6, &user_bits));

  /* Successful deserialization */
  TEST_ASSERT_EQUAL(9, MidiDeserializeUserBits(
      kValidUserBitsData, sizeof(kValidUserBitsData), &user_bits));
  TEST_ASSERT_EQUAL_MEMORY(
      kValidUserBits.data, user_bits.data, MIDI_USER_BITS_DATA_SIZE);
  TEST_ASSERT_EQUAL(kValidUserBits.bg_flags, user_bits.bg_flags);
}

void MidiUserBitsTest(void) {
  RUN_TEST(TestMidiUserBits_Validator);
  RUN_TEST(TestMidiUserBits_Initializer);
  RUN_TEST(TestMidiUserBits_Serialize);
  RUN_TEST(TestMidiUserBits_Deserialize);
}
