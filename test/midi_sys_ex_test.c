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

static uint8_t const kAmericanManId[] = {0x01, 0x00, 0x00};
static uint8_t const kInvalidManId[] = {0x00, 0x80, 0x00};
static uint8_t const kSpecialManId[] = {MIDI_SPECIAL_ID, 0x00, 0x00};
static uint8_t const kNonRealTimeManId[] = {MIDI_NON_REAL_TIME_ID, 0x00, 0x00};
static uint8_t const kRealTimeManId[] = {MIDI_REAL_TIME_ID, 0x00, 0x00};

static void TestMidiSysEx_SpecialSysExId(void) {
  TEST_ASSERT_FALSE(MidiIsSpecialSysExId(NULL));
  TEST_ASSERT_FALSE(MidiIsSpecialSysExId(kInvalidManId));
  TEST_ASSERT_FALSE(MidiIsSpecialSysExId(kAmericanManId));
  TEST_ASSERT_FALSE(MidiIsSpecialSysExId(kSpecialManId));

  TEST_ASSERT_TRUE(MidiIsSpecialSysExId(kNonRealTimeManId));
  TEST_ASSERT_TRUE(MidiIsSpecialSysExId(kRealTimeManId));
}

/* Sample SysEx messages. */

static midi_sys_ex_t const kCancelSysEx = {
  .id = { MIDI_NON_REAL_TIME_ID, 0x00, 0x00 },
  .device_id = MIDI_ALL_CALL,
  .sub_id = MIDI_CANCEL,
  .packet_number = 0x33
};
static uint8_t const kCancelSysExData[] = {
  MIDI_NON_REAL_TIME_ID, MIDI_ALL_CALL, MIDI_CANCEL, 0x33
};

static midi_sys_ex_t const kAckSysEx = {
  .id = { MIDI_NON_REAL_TIME_ID, 0x00, 0x00 },
  .device_id = 0x4A,
  .sub_id = MIDI_ACK,
  .packet_number = 0x38
};
static uint8_t const kAckSysExData[] = {
  MIDI_NON_REAL_TIME_ID, 0x4A, MIDI_ACK, 0x38
};

static midi_sys_ex_t const kDumpRequestSysEx = {
  .id = { MIDI_NON_REAL_TIME_ID, 0x00, 0x00 },
  .device_id = 0x21,
  .sub_id = MIDI_DUMP_REQUEST,
  .dump_request = {
    .sample_number = 0x0404
  }
};
static uint8_t const kDumpRequestSysExData[] = {
  MIDI_NON_REAL_TIME_ID, 0x21, MIDI_DUMP_REQUEST, 0x04, 0x08
};

static midi_sys_ex_t const kSampleDumpResponseSysEx = {
  .id = { MIDI_NON_REAL_TIME_ID, 0x00, 0x00 },
  .device_id = 0x2F,
  .sub_id = MIDI_SAMPLE_DUMP,
  .sample_dump = {
    .sub_id = MIDI_SAMPLE_LOOP_RESPONSE,
    .sample_number = 0x040A,
    .loop_number = 0x01FF,
    .loop_type = MIDI_LOOP_OFF,
    .loop_start_address = 0x00010000,
    .loop_end_address = 0x0003FFFF
  }
};
static uint8_t const kSampleDumpResponseSysExData[] = {
  MIDI_NON_REAL_TIME_ID, 0x2F, MIDI_SAMPLE_DUMP,
  MIDI_SAMPLE_LOOP_RESPONSE,
  0x0A, 0x08,
  0x7F, 0x03,
  MIDI_LOOP_OFF,
  0x00, 0x00, 0x04,
  0x7F, 0x7F, 0x0F
};

static midi_sys_ex_t const kDeviceInquiryResponseSysEx = {
  .id = { MIDI_NON_REAL_TIME_ID, 0x00, 0x00 },
  .device_id = 0x17,
  .sub_id = MIDI_GENERAL_INFO,
  .device_inquiry = {
    .sub_id = MIDI_SAMPLE_LOOP_RESPONSE,
    .id = { 0x00, 0x40, 0x60 },
    .device_family_code = 0x1133,
    .device_family_member_code = 0x3311,
    .software_revision_level = { 0x4D, 0x49, 0x44, 0x49 }
  }
};
static uint8_t const kDeviceInquiryResponseSysExData[] = {
  MIDI_NON_REAL_TIME_ID, 0x17, MIDI_GENERAL_INFO,
  MIDI_SAMPLE_LOOP_RESPONSE,
  0x00, 0x40, 0x60,
  0x33, 0x22,
  0x11, 0x66,
  0x4D, 0x49, 0x44, 0x49
};

static midi_sys_ex_t const kGeneralMidiModeOnSysEx = {
  .id = { MIDI_NON_REAL_TIME_ID, 0x00, 0x00 },
  .device_id = 0x43,
  .sub_id = MIDI_GENERAL_MIDI,
  .gm_mode = MIDI_GENERAL_MIDI_ON
};
static uint8_t const kGeneralMidiModeOnSysExData[] = {
  MIDI_NON_REAL_TIME_ID, 0x43, MIDI_GENERAL_MIDI, MIDI_GENERAL_MIDI_ON
};

static midi_sys_ex_t const kDeviceControlBalanceSysEx = {
  .id = { MIDI_REAL_TIME_ID, 0x00, 0x00 },
  .device_id = 0x1A,
  .sub_id = MIDI_DEVICE_CONTROL,
  .device_control = {
    .sub_id = MIDI_MASTER_BALANCE,
    .balance = 0x2222,
  }
};
static uint8_t const kDeviceControlBalanceSysExData[] = {
  MIDI_REAL_TIME_ID, 0x1A, MIDI_DEVICE_CONTROL,
  MIDI_MASTER_BALANCE, 0x22, 0x44
};

static midi_sys_ex_t const kInvalidSysEx = {
  .id = { MIDI_NON_REAL_TIME_ID, 0x00, 0x00 },
  .device_id = 0x1F,
  .sub_id = MIDI_GENERAL_INFO,
  .device_inquiry = {
    .sub_id = 0x0A
  }
};
static uint8_t const kInvalidSysExData[] = {
  MIDI_NON_REAL_TIME_ID, 0x1F, MIDI_GENERAL_INFO, 0x0A
};

static midi_sys_ex_t const kProprietarySysExOne = {
  .id = {0x50, 0x00, 0x00},
  .device_id = 0x77,
  .sub_id = 0x44
};
static uint8_t const kProprietarySysExOneData[] = {
  0x50, 0x77, 0x44,
  /* Random garbage. */
  0x5D, 0x3B, 0x70, 0x72, 0x25, 0x4C, 0x22, 0x2B,
  0x1A, 0x6C, 0x0B, 0x32, 0x0D, 0x08, 0x30, 0x4B,
  0x63, 0x13, 0x06, 0x5E, 0x41, 0x4C, 0x7A, 0x42,
  0x5C, 0x7E, 0x0F, 0x19, 0x0E, 0x23, 0x0A, 0x3D,
  0x5C, 0x28, 0x5F, 0x27, 0x40, 0x7D, 0x4A, 0x4C,
  /* End. */
  MIDI_END_SYSTEM_EXCLUSIVE
};

static midi_sys_ex_t const kProprietarySysExTwo = {
  .id = {0x00, 0x12, 0x34},
  .device_id = 0x56,
  .sub_id = 0x78
};
static uint8_t const kProprietarySysExTwoData[] = {
  0x00, 0x12, 0x34, 0x56, 0x78, MIDI_END_SYSTEM_EXCLUSIVE
};

static midi_sys_ex_t const kInvalidProprietarySysEx = {
  .id = {0x00, 0x12, 0x34},
  .device_id = 0x09,
  .sub_id = 0x98
};
static uint8_t const kInvalidProprietarySysExData[] = {
  0x50, 0x77, 0x44,
  /* Random garbage. */
  0x54, 0x18, 0x09, 0x70, 0x1F, 0x2D, 0x6B, 0x6F,
  0x41, 0x6B, 0x7C, 0x7D, 0x55, 0x5B, 0x2D, 0x55,
  0x07, 0x0A, 0x06, 0x12, 0x45, 0x8F, 0x14, 0x35,  /* 0x8F is bad */
  0x35, 0x2B, 0x61, 0x1F, 0x44, 0x11, 0x63, 0x74,
  /* End. */
  MIDI_END_SYSTEM_EXCLUSIVE
};

static void TestMidiSysEx_Validator(void) {
  TEST_ASSERT_FALSE(MidiIsValidSysEx(NULL));
  midi_sys_ex_t sys_ex = {};
  TEST_ASSERT_FALSE(MidiIsValidSysEx(&sys_ex));
  memcpy(sys_ex.id, kAmericanManId, sizeof(sys_ex.id));
  TEST_ASSERT_TRUE(MidiIsValidSysEx(&sys_ex));

  TEST_ASSERT_TRUE(MidiIsValidSysEx(&kCancelSysEx));
  TEST_ASSERT_TRUE(MidiIsValidSysEx(&kAckSysEx));
  TEST_ASSERT_TRUE(MidiIsValidSysEx(&kDumpRequestSysEx));
  TEST_ASSERT_TRUE(MidiIsValidSysEx(&kSampleDumpResponseSysEx));
  TEST_ASSERT_TRUE(MidiIsValidSysEx(&kDeviceInquiryResponseSysEx));
  TEST_ASSERT_TRUE(MidiIsValidSysEx(&kGeneralMidiModeOnSysEx));
  TEST_ASSERT_TRUE(MidiIsValidSysEx(&kDeviceControlBalanceSysEx));
  TEST_ASSERT_TRUE(MidiIsValidSysEx(&kProprietarySysExOne));
  TEST_ASSERT_TRUE(MidiIsValidSysEx(&kProprietarySysExTwo));

  TEST_ASSERT_FALSE(MidiIsValidSysEx(&kInvalidSysEx));
  TEST_ASSERT_FALSE(MidiIsValidSysEx(&kInvalidProprietarySysEx));
  sys_ex = kDeviceInquiryResponseSysEx;
  sys_ex.device_inquiry.id[1] = 0x90;
  TEST_ASSERT_FALSE(MidiIsValidSysEx(&sys_ex));
}

static void TestMidiSysEx_Initializer(void) {
  midi_sys_ex_t sys_ex;
  TEST_ASSERT_FALSE(MidiInitializeSysEx(NULL, NULL));
  TEST_ASSERT_FALSE(MidiInitializeSysEx(NULL, kAmericanManId));
  TEST_ASSERT_FALSE(MidiInitializeSysEx(&sys_ex, kInvalidManId));
  sys_ex.id[0] = MIDI_NON_REAL_TIME_ID;
  /* Initialize as blank. */
  TEST_ASSERT_TRUE(MidiInitializeSysEx(&sys_ex, NULL));
  TEST_ASSERT_TRUE(MidiIsBlankManufacturerId(sys_ex.id));

  TEST_ASSERT_TRUE(MidiInitializeSysEx(&sys_ex, kAmericanManId));
  TEST_ASSERT_FALSE(MidiIsBlankManufacturerId(sys_ex.id));
  TEST_ASSERT_TRUE(MidiIsValidSysEx(&sys_ex));
  TEST_ASSERT_EQUAL_MEMORY(kAmericanManId, sys_ex.id, sizeof(sys_ex.id));
}

static void TestMidiHandShakeSysEx_Checker(void) {
  TEST_ASSERT_FALSE(MidiIsHandShakeSysEx(NULL));

  TEST_ASSERT_TRUE(MidiIsHandShakeSysEx(&kAckSysEx));
  TEST_ASSERT_TRUE(MidiIsHandShakeSysEx(&kCancelSysEx));

  TEST_ASSERT_FALSE(MidiIsHandShakeSysEx(&kDumpRequestSysEx));
  TEST_ASSERT_FALSE(MidiIsHandShakeSysEx(&kSampleDumpResponseSysEx));
  TEST_ASSERT_FALSE(MidiIsHandShakeSysEx(&kDeviceInquiryResponseSysEx));

  // midi_sys_ex_t sys_ex = kAckSysEx;
  // sys_ex.packet_number = 0x80;
  // TEST_ASSERT_FALSE(MidiIsHandShakeSysEx(&sys_ex));
}

static void TestMidiHandShakeSysEx_Initializer(void) {
  midi_sys_ex_t sys_ex;
  TEST_ASSERT_FALSE(MidiHandShakeSysEx(
      NULL, 0x40, MIDI_CANCEL, 0x30));
  TEST_ASSERT_FALSE(MidiHandShakeSysEx(
      &sys_ex, 0x80, MIDI_CANCEL, 0x30));
  TEST_ASSERT_FALSE(MidiHandShakeSysEx(
      &sys_ex, 0x40, MIDI_SAMPLE_DUMP, 0x30));
  TEST_ASSERT_FALSE(MidiHandShakeSysEx(
      &sys_ex, 0x40, MIDI_CANCEL, 0x8F));

  TEST_ASSERT_TRUE(MidiHandShakeSysEx(&sys_ex, 0x40, MIDI_WAIT, 0x6F));
  TEST_ASSERT_EQUAL_MEMORY(
      kNonRealTimeManId, sys_ex.id, sizeof(kNonRealTimeManId));
  TEST_ASSERT_EQUAL(0x40, sys_ex.device_id);
  TEST_ASSERT_EQUAL(MIDI_WAIT, sys_ex.sub_id);
  TEST_ASSERT_EQUAL(0x6F, sys_ex.packet_number);
}

static void TestMidiGeneralMidiModeSysEx_Initializer(void) {
  midi_sys_ex_t sys_ex;
  TEST_ASSERT_FALSE(MidiGeneralMidiModeSysEx(NULL, 0x40, false));
  TEST_ASSERT_FALSE(MidiGeneralMidiModeSysEx(&sys_ex, 0xC0, false));

  TEST_ASSERT_TRUE(MidiGeneralMidiModeOnSysEx(&sys_ex, 0x40));
  TEST_ASSERT_EQUAL_MEMORY(
      kNonRealTimeManId, sys_ex.id, sizeof(kNonRealTimeManId));
  TEST_ASSERT_EQUAL(0x40, sys_ex.device_id);
  TEST_ASSERT_EQUAL(MIDI_GENERAL_MIDI, sys_ex.sub_id);
  TEST_ASSERT_EQUAL(MIDI_GENERAL_MIDI_ON, sys_ex.gm_mode);

  TEST_ASSERT_TRUE(MidiGeneralMidiModeOffSysEx(&sys_ex, 0x43));
  TEST_ASSERT_EQUAL_MEMORY(
      kNonRealTimeManId, sys_ex.id, sizeof(kNonRealTimeManId));
  TEST_ASSERT_EQUAL(0x43, sys_ex.device_id);
  TEST_ASSERT_EQUAL(MIDI_GENERAL_MIDI, sys_ex.sub_id);
  TEST_ASSERT_EQUAL(MIDI_GENERAL_MIDI_OFF, sys_ex.gm_mode);
}

static void TestMidiStandardSysEx_Initializer(void) {
  midi_sys_ex_t sys_ex;
  TEST_ASSERT_FALSE(MidiInitializeSysUni(
      NULL, false, 0x40, MIDI_GENERAL_INFO));
  TEST_ASSERT_FALSE(MidiInitializeSysUni(
      &sys_ex, false, 0x80, MIDI_GENERAL_INFO));
  TEST_ASSERT_FALSE(MidiInitializeSysUni(
      &sys_ex, false, 0x40, 0x50));
  TEST_ASSERT_FALSE(MidiInitializeSysUni(
      &sys_ex, false, 0x40, 0x8F));

  /* Non-realtime. */
  TEST_ASSERT_TRUE(MidiInitializeSysUni(
      &sys_ex, false, 0x60, MIDI_SAMPLE_DUMP));
  TEST_ASSERT_EQUAL_MEMORY(
      kNonRealTimeManId, sys_ex.id, sizeof(kNonRealTimeManId));
  TEST_ASSERT_EQUAL(0x60, sys_ex.device_id);
  TEST_ASSERT_EQUAL(MIDI_SAMPLE_DUMP, sys_ex.sub_id);

  /* TODO: Test realtime. */
}

static void TestMidiSysEx_Serialize(void) {
  uint8_t sys_ex_data[128];
  /* Invalid input. */
  TEST_ASSERT_EQUAL(0, MidiSerializeSysEx(
      NULL, sys_ex_data, sizeof(sys_ex_data)));
  TEST_ASSERT_EQUAL(0, MidiSerializeSysEx(
      &kDumpRequestSysEx, NULL, sizeof(sys_ex_data)));
  TEST_ASSERT_EQUAL(0, MidiSerializeSysEx(
      &kInvalidSysEx, sys_ex_data, sizeof(sys_ex_data)));

  /* Unsupported messages. */
  TEST_ASSERT_EQUAL(0, MidiSerializeSysEx(
      &kProprietarySysExOne, sys_ex_data, sizeof(sys_ex_data)));
  TEST_ASSERT_EQUAL(0, MidiSerializeSysEx(
      &kProprietarySysExTwo, sys_ex_data, sizeof(sys_ex_data)));

  /* Partial serialization. */
  TEST_ASSERT_EQUAL(sizeof(kCancelSysExData), MidiSerializeSysEx(
      &kCancelSysEx, NULL, 0u));
  TEST_ASSERT_EQUAL(sizeof(kCancelSysExData), MidiSerializeSysEx(
      &kCancelSysEx, sys_ex_data, 0u));
  TEST_ASSERT_EQUAL(sizeof(kCancelSysExData), MidiSerializeSysEx(
      &kCancelSysEx, sys_ex_data, 2));
  TEST_ASSERT_EQUAL(sizeof(kDumpRequestSysExData), MidiSerializeSysEx(
      &kDumpRequestSysEx, NULL, 0u));
  TEST_ASSERT_EQUAL(sizeof(kDumpRequestSysExData), MidiSerializeSysEx(
      &kDumpRequestSysEx, sys_ex_data, sizeof(kDumpRequestSysExData) / 2));

  /* Successful serialization. */
  TEST_ASSERT_EQUAL(sizeof(kCancelSysExData), MidiSerializeSysEx(
      &kCancelSysEx, sys_ex_data, sizeof(sys_ex_data)));
  TEST_ASSERT_EQUAL_MEMORY(
      kCancelSysExData, sys_ex_data, sizeof(kCancelSysExData));

  TEST_ASSERT_EQUAL(sizeof(kAckSysExData), MidiSerializeSysEx(
      &kAckSysEx, sys_ex_data, sizeof(sys_ex_data)));
  TEST_ASSERT_EQUAL_MEMORY(
      kAckSysExData, sys_ex_data, sizeof(kAckSysExData));

  TEST_ASSERT_EQUAL(sizeof(kDumpRequestSysExData), MidiSerializeSysEx(
      &kDumpRequestSysEx, sys_ex_data, sizeof(sys_ex_data)));
  TEST_ASSERT_EQUAL_MEMORY(
      kDumpRequestSysExData, sys_ex_data, sizeof(kDumpRequestSysExData));

  TEST_ASSERT_EQUAL(sizeof(kSampleDumpResponseSysExData), MidiSerializeSysEx(
      &kSampleDumpResponseSysEx, sys_ex_data, sizeof(sys_ex_data)));
  TEST_ASSERT_EQUAL_MEMORY(
      kSampleDumpResponseSysExData, sys_ex_data,
      sizeof(kSampleDumpResponseSysExData));

  TEST_ASSERT_EQUAL(sizeof(kDeviceInquiryResponseSysExData), MidiSerializeSysEx(
      &kDeviceInquiryResponseSysEx, sys_ex_data, sizeof(sys_ex_data)));
  TEST_ASSERT_EQUAL_MEMORY(
      kDeviceInquiryResponseSysExData, sys_ex_data,
      sizeof(kDeviceInquiryResponseSysExData));

  TEST_ASSERT_EQUAL(sizeof(kGeneralMidiModeOnSysExData), MidiSerializeSysEx(
      &kGeneralMidiModeOnSysEx, sys_ex_data, sizeof(sys_ex_data)));
  TEST_ASSERT_EQUAL_MEMORY(
      kGeneralMidiModeOnSysExData, sys_ex_data,
      sizeof(kGeneralMidiModeOnSysExData));

  TEST_ASSERT_EQUAL(sizeof(kDeviceControlBalanceSysExData), MidiSerializeSysEx(
      &kDeviceControlBalanceSysEx, sys_ex_data, sizeof(sys_ex_data)));
  TEST_ASSERT_EQUAL_MEMORY(
      kDeviceControlBalanceSysExData, sys_ex_data,
      sizeof(kDeviceControlBalanceSysExData));
}

static void TestMidiSysEx_Deserialize(void) {
  midi_sys_ex_t sys_ex;
  /* Invalid input. */
  TEST_ASSERT_EQUAL(0, MidiDeserializeSysEx(
      NULL, sizeof(kAckSysExData), &sys_ex));
  TEST_ASSERT_EQUAL(0, MidiDeserializeSysEx(
      kAckSysExData, sizeof(kAckSysExData), NULL));
  TEST_ASSERT_EQUAL(0, MidiDeserializeSysEx(
      kInvalidSysExData, sizeof(kInvalidSysExData), &sys_ex));
  TEST_ASSERT_EQUAL(0, MidiDeserializeSysEx(
      kInvalidProprietarySysExData, sizeof(kInvalidProprietarySysExData),
      &sys_ex));

  /* Partial deserialization. */
  TEST_ASSERT_EQUAL(1, MidiDeserializeSysEx(NULL, 0, &sys_ex));
  TEST_ASSERT_EQUAL(3, MidiDeserializeSysEx(kSampleDumpResponseSysExData, 1, &sys_ex));
  TEST_ASSERT_EQUAL(sizeof(kSampleDumpResponseSysExData),
      MidiDeserializeSysEx(kSampleDumpResponseSysExData,
      3 + MIDI_SAMPLE_LOOP_RESPONSE_PAYLOAD_SIZE, &sys_ex));

  TEST_ASSERT_EQUAL(1, MidiDeserializeSysEx(kAckSysExData, 0, &sys_ex));
  TEST_ASSERT_EQUAL(3, MidiDeserializeSysEx(kAckSysExData, 1, &sys_ex));
  TEST_ASSERT_EQUAL(sizeof(kAckSysExData), MidiDeserializeSysEx(
      kAckSysExData, 3, &sys_ex));

  /* Successful deserialization. */
  TEST_ASSERT_EQUAL(sizeof(kAckSysExData), MidiDeserializeSysEx(
      kAckSysExData, sizeof(kAckSysExData), &sys_ex));
  TEST_ASSERT_EQUAL_MEMORY(
      kAckSysEx.id, sys_ex.id, sizeof(midi_manufacturer_id_t));
  TEST_ASSERT_EQUAL(kAckSysEx.device_id, sys_ex.device_id);
  TEST_ASSERT_EQUAL(kAckSysEx.sub_id, sys_ex.sub_id);
  TEST_ASSERT_EQUAL(kAckSysEx.packet_number, sys_ex.packet_number);

  TEST_ASSERT_EQUAL(sizeof(kCancelSysExData), MidiDeserializeSysEx(
      kCancelSysExData, sizeof(kCancelSysExData), &sys_ex));
  TEST_ASSERT_EQUAL_MEMORY(
      kCancelSysEx.id, sys_ex.id, sizeof(midi_manufacturer_id_t));
  TEST_ASSERT_EQUAL(kCancelSysEx.device_id, sys_ex.device_id);
  TEST_ASSERT_EQUAL(kCancelSysEx.sub_id, sys_ex.sub_id);
  TEST_ASSERT_EQUAL(kCancelSysEx.packet_number, sys_ex.packet_number);

  TEST_ASSERT_EQUAL(sizeof(kDumpRequestSysExData), MidiDeserializeSysEx(
      kDumpRequestSysExData, sizeof(kDumpRequestSysExData), &sys_ex));
  TEST_ASSERT_EQUAL_MEMORY(
      kDumpRequestSysEx.id, sys_ex.id, sizeof(midi_manufacturer_id_t));
  TEST_ASSERT_EQUAL(kDumpRequestSysEx.device_id, sys_ex.device_id);
  TEST_ASSERT_EQUAL(kDumpRequestSysEx.sub_id, sys_ex.sub_id);
  TEST_ASSERT_EQUAL(
      kDumpRequestSysEx.dump_request.sample_number,
      sys_ex.dump_request.sample_number);

  TEST_ASSERT_EQUAL(sizeof(kSampleDumpResponseSysExData), MidiDeserializeSysEx(
      kSampleDumpResponseSysExData, sizeof(kSampleDumpResponseSysExData), &sys_ex));
  TEST_ASSERT_EQUAL_MEMORY(
      kSampleDumpResponseSysEx.id, sys_ex.id,
      sizeof(midi_manufacturer_id_t));
  TEST_ASSERT_EQUAL(kSampleDumpResponseSysEx.device_id, sys_ex.device_id);
  TEST_ASSERT_EQUAL(kSampleDumpResponseSysEx.sub_id, sys_ex.sub_id);
  TEST_ASSERT_EQUAL(
      kSampleDumpResponseSysEx.sample_dump.sub_id,
      sys_ex.sample_dump.sub_id);
  TEST_ASSERT_EQUAL(
      kSampleDumpResponseSysEx.sample_dump.sample_number,
      sys_ex.sample_dump.sample_number);
  TEST_ASSERT_EQUAL(
      kSampleDumpResponseSysEx.sample_dump.loop_number,
      sys_ex.sample_dump.loop_number);
  TEST_ASSERT_EQUAL(
      kSampleDumpResponseSysEx.sample_dump.loop_type,
      sys_ex.sample_dump.loop_type);
  TEST_ASSERT_EQUAL(
      kSampleDumpResponseSysEx.sample_dump.loop_start_address,
      sys_ex.sample_dump.loop_start_address);
  TEST_ASSERT_EQUAL(
      kSampleDumpResponseSysEx.sample_dump.loop_end_address,
      sys_ex.sample_dump.loop_end_address);

  TEST_ASSERT_EQUAL(sizeof(kDeviceInquiryResponseSysExData), MidiDeserializeSysEx(
      kDeviceInquiryResponseSysExData, sizeof(kDeviceInquiryResponseSysExData),
      &sys_ex));
  TEST_ASSERT_EQUAL_MEMORY(
      kDeviceInquiryResponseSysEx.id, sys_ex.id,
      sizeof(midi_manufacturer_id_t));
  TEST_ASSERT_EQUAL(kDeviceInquiryResponseSysEx.device_id, sys_ex.device_id);
  TEST_ASSERT_EQUAL(kDeviceInquiryResponseSysEx.sub_id, sys_ex.sub_id);
  TEST_ASSERT_EQUAL(
      kDeviceInquiryResponseSysEx.device_inquiry.sub_id,
      sys_ex.device_inquiry.sub_id);
  TEST_ASSERT_EQUAL_MEMORY(
      kDeviceInquiryResponseSysEx.device_inquiry.id,
      sys_ex.device_inquiry.id, sizeof(midi_manufacturer_id_t));
  TEST_ASSERT_EQUAL(
      kDeviceInquiryResponseSysEx.device_inquiry.device_family_code,
      sys_ex.device_inquiry.device_family_code);
  TEST_ASSERT_EQUAL(
      kDeviceInquiryResponseSysEx.device_inquiry.device_family_member_code,
      sys_ex.device_inquiry.device_family_member_code);
  TEST_ASSERT_EQUAL_MEMORY(
      kDeviceInquiryResponseSysEx.device_inquiry.software_revision_level,
      sys_ex.device_inquiry.software_revision_level,
      MIDI_SOFTWARE_REVISION_SIZE);

  TEST_ASSERT_EQUAL(sizeof(kGeneralMidiModeOnSysExData), MidiDeserializeSysEx(
      kGeneralMidiModeOnSysExData, sizeof(kGeneralMidiModeOnSysExData),
      &sys_ex));
  TEST_ASSERT_EQUAL_MEMORY(
      kGeneralMidiModeOnSysEx.id, sys_ex.id, sizeof(midi_manufacturer_id_t));
  TEST_ASSERT_EQUAL(kGeneralMidiModeOnSysEx.device_id, sys_ex.device_id);
  TEST_ASSERT_EQUAL(kGeneralMidiModeOnSysEx.sub_id, sys_ex.sub_id);
  TEST_ASSERT_EQUAL(kGeneralMidiModeOnSysEx.gm_mode, sys_ex.gm_mode);

  TEST_ASSERT_EQUAL(sizeof(kDeviceControlBalanceSysExData), MidiDeserializeSysEx(
      kDeviceControlBalanceSysExData, sizeof(kDeviceControlBalanceSysExData),
      &sys_ex));
  TEST_ASSERT_EQUAL_MEMORY(
      kDeviceControlBalanceSysEx.id, sys_ex.id, sizeof(midi_manufacturer_id_t));
  TEST_ASSERT_EQUAL(kDeviceControlBalanceSysEx.device_id, sys_ex.device_id);
  TEST_ASSERT_EQUAL(kDeviceControlBalanceSysEx.sub_id, sys_ex.sub_id);
  TEST_ASSERT_EQUAL(kDeviceControlBalanceSysEx.device_control.sub_id,
      sys_ex.device_control.sub_id);
  TEST_ASSERT_EQUAL(kDeviceControlBalanceSysEx.device_control.balance,
      sys_ex.device_control.balance);
}

static void TestMidiSysEx_Proprietary(void) {
  midi_sys_ex_t sys_ex;
  uint8_t sys_ex_data[128];

  TEST_ASSERT_TRUE(MidiIsValidSysEx(&kProprietarySysExOne));
  TEST_ASSERT_TRUE(MidiIsValidSysEx(&kProprietarySysExTwo));
  TEST_ASSERT_FALSE(MidiIsValidSysEx(&kInvalidProprietarySysEx));

  /* Do not support serialize proprietary messages. */
  TEST_ASSERT_EQUAL(0, MidiSerializeSysEx(
      &kProprietarySysExOne, sys_ex_data, sizeof(sys_ex_data)));
  TEST_ASSERT_EQUAL(0, MidiSerializeSysEx(
      &kProprietarySysExTwo, sys_ex_data, sizeof(sys_ex_data)));
  TEST_ASSERT_EQUAL(0, MidiSerializeSysEx(
      &kInvalidProprietarySysEx, sys_ex_data, sizeof(sys_ex_data)));

  TEST_ASSERT_EQUAL(0, MidiDeserializeSysEx(
      kInvalidProprietarySysExData, sizeof(kInvalidProprietarySysExData),
      &sys_ex));
  /* Deserializing a proprietary message requires the EndSysEx in
   * the message. */
  for (size_t i = 3; i < (sizeof(kProprietarySysExOneData) - 1); ++i) {
    TEST_ASSERT_EQUAL(i + 1, MidiDeserializeSysEx(
        kProprietarySysExOneData, i, &sys_ex));
  }
  TEST_ASSERT_EQUAL(
      sizeof(kProprietarySysExOneData) - 1,
      MidiDeserializeSysEx(
          kProprietarySysExOneData, sizeof(kProprietarySysExOneData),
          &sys_ex));
  TEST_ASSERT_TRUE(MidiIsValidSysEx(&sys_ex));
  TEST_ASSERT_EQUAL(kProprietarySysExOne.device_id, sys_ex.device_id);
  TEST_ASSERT_EQUAL(kProprietarySysExOne.sub_id, sys_ex.sub_id);
  TEST_ASSERT_EQUAL(kProprietarySysExOne.packet_number, sys_ex.packet_number);

  TEST_ASSERT_EQUAL(
      sizeof(kProprietarySysExTwoData) - 1,
      MidiDeserializeSysEx(
          kProprietarySysExTwoData, sizeof(kProprietarySysExTwoData),
          &sys_ex));
  TEST_ASSERT_TRUE(MidiIsValidSysEx(&sys_ex));
  TEST_ASSERT_EQUAL(kProprietarySysExTwo.device_id, sys_ex.device_id);
  TEST_ASSERT_EQUAL(kProprietarySysExTwo.sub_id, sys_ex.sub_id);
  TEST_ASSERT_EQUAL(kProprietarySysExTwo.packet_number, sys_ex.packet_number);
}

void MidiSystemExclusiveTest(void) {
  RUN_TEST(TestMidiSysEx_SpecialSysExId);

  RUN_TEST(TestMidiSysEx_Validator);
  RUN_TEST(TestMidiSysEx_Initializer);
  RUN_TEST(TestMidiHandShakeSysEx_Checker);
  RUN_TEST(TestMidiHandShakeSysEx_Initializer);
  RUN_TEST(TestMidiGeneralMidiModeSysEx_Initializer);

  RUN_TEST(TestMidiStandardSysEx_Initializer);

  RUN_TEST(TestMidiSysEx_Serialize);
  RUN_TEST(TestMidiSysEx_Deserialize);

  RUN_TEST(TestMidiSysEx_Proprietary);
}
