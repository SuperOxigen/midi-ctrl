/*
 * MIDI Controller - MIDI System Information Test
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#include <unity.h>

#include "midi_sys_info.h"

static midi_sys_info_t const kValidSysInfo = {
  .id = {0x00, 0x12, 0x34},
  .device_family_code = 0x1234,
  .device_family_member_code = 0x2468,
  .software_revision_level = {'M', 'I', 'D', 'I'}
};

static uint8_t const kManufacturerId[3] = {0x00, 0x12, 0x34};
static uint8_t const kBadManufacturerId[3] = {0x00, 0x45, 0x84};

static uint8_t const kSoftwareRevisionLevel[MIDI_SOFTWARE_REVISION_SIZE] = {
  'M', 'I', 'D', 'I'
};
static uint8_t const kBadSoftwareRevisionLevel[MIDI_SOFTWARE_REVISION_SIZE] = {
  'M', 'I', 'D' | 0x80, 'I'
};

static void TestMidiSystemInfo_Validator(void) {
  midi_sys_info_t info = kValidSysInfo;
  TEST_ASSERT_FALSE(MidiIsValidSystemInfo(NULL));
  info.id[2] |= 0x80;
  TEST_ASSERT_FALSE(MidiIsValidSystemInfo(&info));
  info.id[2] &= 0x7F;
  info.device_family_code = 0x4000;
  TEST_ASSERT_FALSE(MidiIsValidSystemInfo(&info));
  info.device_family_code = 0x1234;
  info.device_family_member_code |= 0x8000;
  TEST_ASSERT_FALSE(MidiIsValidSystemInfo(&info));
  info.device_family_member_code &= 0x3FFF;
  info.software_revision_level[2] = 'D' | 0x80;
  TEST_ASSERT_FALSE(MidiIsValidSystemInfo(&info));
  info.software_revision_level[2] = 'D';
  TEST_ASSERT_TRUE(MidiIsValidSystemInfo(&info));
}

static void TestMidiSystemInfo_Initializer(void) {
  midi_sys_info_t info;
  TEST_ASSERT_FALSE(MidiInitializeSystemInfo(
      NULL, kManufacturerId, 0x1234, 0x2468, kSoftwareRevisionLevel));
  TEST_ASSERT_FALSE(MidiInitializeSystemInfo(
      &info, NULL, 0x1234, 0x2468, kSoftwareRevisionLevel));
  TEST_ASSERT_FALSE(MidiInitializeSystemInfo(
      &info, kBadManufacturerId, 0x1234, 0x2468, kSoftwareRevisionLevel));
  TEST_ASSERT_FALSE(MidiInitializeSystemInfo(
      &info, kManufacturerId, 0x4234, 0x2468, kSoftwareRevisionLevel));
  TEST_ASSERT_FALSE(MidiInitializeSystemInfo(
      &info, kManufacturerId, 0x1234, 0x8468, kSoftwareRevisionLevel));
  TEST_ASSERT_FALSE(MidiInitializeSystemInfo(
      &info, kManufacturerId, 0x1234, 0x2468, NULL));
  TEST_ASSERT_FALSE(MidiInitializeSystemInfo(
      &info, kManufacturerId, 0x1234, 0x2468, kBadSoftwareRevisionLevel));

  TEST_ASSERT_TRUE(MidiInitializeSystemInfo(
      &info, kManufacturerId, 0x1234, 0x2468, kSoftwareRevisionLevel));
  TEST_ASSERT_EQUAL_MEMORY(kManufacturerId, info.id, 3);
  TEST_ASSERT_EQUAL(0x1234, info.device_family_code);
  TEST_ASSERT_EQUAL(0x2468, info.device_family_member_code);
  TEST_ASSERT_EQUAL_MEMORY(
      kSoftwareRevisionLevel, info.software_revision_level,
      MIDI_SOFTWARE_REVISION_SIZE);
}

void MidiSystemInfoTest(void) {
  RUN_TEST(TestMidiSystemInfo_Validator);
  RUN_TEST(TestMidiSystemInfo_Initializer);
}
