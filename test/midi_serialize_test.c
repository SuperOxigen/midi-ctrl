/*
 * MIDI Controller - MIDI Serialize / Deserialize Messages Test.
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#include <string.h>
#include <unity.h>

#include "midi_defs.h"
#include "midi_message.h"
#include "midi_serialize.h"

static uint8_t const kFillerByte = 0x42;
static uint16_t const kFillerWord = 0x2424;
static uint8_t const kFillerWordLsb = 0x24;
static uint8_t const kFillerWordMsb = 0x48;

static uint8_t const kNoteOnPacket[] = {
  MIDI_NOTE_ON | MIDI_CHANNEL_4, MIDI_MIDDLE_C,  MIDI_NOTE_ON_VELOCITY
};
static midi_message_t const kNoteOnMessage = {
  .type = MIDI_NOTE_ON,
  .channel = MIDI_CHANNEL_4,
  .note = {
    .key = MIDI_MIDDLE_C,
    .velocity = MIDI_NOTE_ON_VELOCITY
  }
};

static uint8_t const kNoteOffPacket[] = {
  MIDI_NOTE_OFF | MIDI_CHANNEL_5, MIDI_MIDDLE_C,  MIDI_NOTE_ON_VELOCITY
};
static midi_message_t const kNoteOffMessage = {
  .type = MIDI_NOTE_OFF,
  .channel = MIDI_CHANNEL_5,
  .note = {
    .key = MIDI_MIDDLE_C,
    .velocity = MIDI_NOTE_ON_VELOCITY
  }
};

static uint8_t const kKeyPressurePacket[] = {
  MIDI_KEY_PRESSURE | MIDI_CHANNEL_7, MIDI_MIDDLE_C, kFillerByte
};
static midi_message_t const kKeyPressureMessage = {
  .type = MIDI_KEY_PRESSURE,
  .channel = MIDI_CHANNEL_7,
  .note = {
    .key = MIDI_MIDDLE_C,
    .pressure = kFillerByte
  }
};

static uint8_t const kControlChangePacket[] = {
  MIDI_CONTROL_CHANGE | MIDI_CHANNEL_2, MIDI_DECAY_TIME, kFillerByte
};
static midi_message_t const kControlChangeMessage = {
  .type = MIDI_CONTROL_CHANGE,
  .channel = MIDI_CHANNEL_2,
  .control = {
    .number = MIDI_DECAY_TIME,
    .value = kFillerByte
  }
};

static uint8_t const kProgramChangePacket[] = {
  MIDI_PROGRAM_CHANGE | MIDI_CHANNEL_1, MIDI_FLUTE
};
static midi_message_t const kProgramChangeMessage = {
  .type = MIDI_PROGRAM_CHANGE,
  .channel = MIDI_CHANNEL_1,
  .program = MIDI_FLUTE
};

static uint8_t const kChannelPressurePacket[] = {
  MIDI_CHANNEL_PRESSURE | MIDI_CHANNEL_12, kFillerByte,
};
static midi_message_t const kChannelPressureMessage = {
  .type = MIDI_CHANNEL_PRESSURE,
  .channel = MIDI_CHANNEL_12,
  .pressure = kFillerByte
};

static uint8_t const kPitchWheelPacket[] = {
  MIDI_PITCH_WHEEL | MIDI_CHANNEL_16, kFillerWordLsb, kFillerWordMsb
};
static midi_message_t const kPitchWheelMessage = {
  .type = MIDI_PITCH_WHEEL,
  .channel = MIDI_CHANNEL_16,
  .pitch = kFillerWord
};

static uint8_t const kTimeCodePacket[] = { MIDI_TIME_CODE,
    MIDI_HOURS_COUNT_LSN | 0x05 };
static midi_message_t const kTimeCodeMessage = {
  .type = MIDI_TIME_CODE,
  .time_code = {
    .type = MIDI_HOURS_COUNT_LSN,
    .value = 0x05
  }
};

static uint8_t const kSongPositionPointerPacket[] = {
  MIDI_SONG_POSITION_POINTER, 0x70, 0x19
};
static midi_message_t const kSongPositionPointerMessage = {
  .type = MIDI_SONG_POSITION_POINTER,
  .song_position = 0x0CF0  /* Divisible by 6. */
};

static uint8_t const kSongSelectPacket[] = { MIDI_SONG_SELECT, kFillerByte };
static midi_message_t const kSongSelectMessage = {
  .type = MIDI_SONG_SELECT,
  .song_number = kFillerByte
};

static uint8_t const kTuneRequestPacket[] = { MIDI_TUNE_REQUEST };
static midi_message_t const kTuneRequestMessage = {
  .type = MIDI_TUNE_REQUEST,
};

static uint8_t const kTimingClockPacket[] = { MIDI_TIMING_CLOCK };
static midi_message_t const kTimingClockMessage = {
  .type = MIDI_TIMING_CLOCK
};
static uint8_t const kStartPacket[] = { MIDI_START };
static midi_message_t const kStartMessage = { .type = MIDI_START };
static uint8_t const kContinuePacket[] = { MIDI_CONTINUE };
static midi_message_t const kContinueMessage = { .type = MIDI_CONTINUE };
static uint8_t const kStopPacket[] = { MIDI_STOP };
static midi_message_t const kStopMessage = { .type = MIDI_STOP };
static uint8_t const kActiveSensingPacket[] = { MIDI_ACTIVE_SENSING };
static midi_message_t const kActiveSensingMessage = {
  .type = MIDI_ACTIVE_SENSING,
};
static uint8_t const kSystemResetPacket[] = { MIDI_SYSTEM_RESET };
static midi_message_t const kSystemResetMessage = {
  .type = MIDI_SYSTEM_RESET,
};

static midi_message_t const kNoneMessage = {
  .type = MIDI_NONE
};
static uint8_t const kUnknownPacket[] = { 0xF4 };

/* System Exclusive Message. */

static uint8_t const kDataPacketSysExPacket[] = {
  MIDI_SYSTEM_EXCLUSIVE,
  MIDI_NON_REAL_TIME_ID, kFillerByte, MIDI_DATA_PACKET,
  kFillerByte,
  /* Random bytes, checksum = device_id =  number = 0x42 */
  0x30, 0x0B, 0x4A, 0x08, 0x25, 0x22, 0x0F, 0x70,
  0x04, 0x7C, 0x0D, 0x53, 0x77, 0x27, 0x44, 0x68,
  0x40, 0x5E, 0x55, 0x13, 0x52, 0x3D, 0x13, 0x3C,
  0x1C, 0x50, 0x7A, 0x4D, 0x37, 0x30, 0x62, 0x3A,
  0x70, 0x62, 0x2D, 0x5B, 0x25, 0x5F, 0x46, 0x06,
  0x2D, 0x44, 0x2C, 0x3C, 0x28, 0x62, 0x7C, 0x1C,
  0x20, 0x32, 0x39, 0x49, 0x05, 0x13, 0x1A, 0x1B,
  0x18, 0x1D, 0x32, 0x3E, 0x21, 0x16, 0x63, 0x2F,
  0x4B, 0x23, 0x06, 0x78, 0x34, 0x1E, 0x26, 0x5A,
  0x4C, 0x20, 0x44, 0x6E, 0x00, 0x45, 0x08, 0x5D,
  0x7D, 0x2C, 0x33, 0x54, 0x79, 0x75, 0x0D, 0x57,
  0x3F, 0x6A, 0x2C, 0x29, 0x41, 0x44, 0x16, 0x69,
  0x1E, 0x68, 0x3B, 0x51, 0x0E, 0x1C, 0x07, 0x4D,
  0x11, 0x4B, 0x33, 0x36, 0x22, 0x01, 0x6F, 0x47,
  0x6C, 0x5D, 0x25, 0x70, 0x60, 0x7E, 0x2B, 0x4E,
  kFillerByte,
  MIDI_END_SYSTEM_EXCLUSIVE
};
static uint8_t gDataPacketSysEx[MIDI_DATA_PACKET_DATA_LENGTH] = {
  /* Random bytes, checksum = device_id =  number = 0x42 */
  0x30, 0x0B, 0x4A, 0x08, 0x25, 0x22, 0x0F, 0x70,
  0x04, 0x7C, 0x0D, 0x53, 0x77, 0x27, 0x44, 0x68,
  0x40, 0x5E, 0x55, 0x13, 0x52, 0x3D, 0x13, 0x3C,
  0x1C, 0x50, 0x7A, 0x4D, 0x37, 0x30, 0x62, 0x3A,
  0x70, 0x62, 0x2D, 0x5B, 0x25, 0x5F, 0x46, 0x06,
  0x2D, 0x44, 0x2C, 0x3C, 0x28, 0x62, 0x7C, 0x1C,
  0x20, 0x32, 0x39, 0x49, 0x05, 0x13, 0x1A, 0x1B,
  0x18, 0x1D, 0x32, 0x3E, 0x21, 0x16, 0x63, 0x2F,
  0x4B, 0x23, 0x06, 0x78, 0x34, 0x1E, 0x26, 0x5A,
  0x4C, 0x20, 0x44, 0x6E, 0x00, 0x45, 0x08, 0x5D,
  0x7D, 0x2C, 0x33, 0x54, 0x79, 0x75, 0x0D, 0x57,
  0x3F, 0x6A, 0x2C, 0x29, 0x41, 0x44, 0x16, 0x69,
  0x1E, 0x68, 0x3B, 0x51, 0x0E, 0x1C, 0x07, 0x4D,
  0x11, 0x4B, 0x33, 0x36, 0x22, 0x01, 0x6F, 0x47,
  0x6C, 0x5D, 0x25, 0x70, 0x60, 0x7E, 0x2B, 0x4E
};
static midi_message_t const kDataPacketSysExMessage = {
  .type = MIDI_SYSTEM_EXCLUSIVE,
  .sys_ex = {
    .id = {MIDI_NON_REAL_TIME_ID, 0x00, 0x00},
    .device_id = kFillerByte,
    .sub_id = MIDI_DATA_PACKET,
    .data_packet = {
      .number = kFillerByte,
      .data = gDataPacketSysEx,
      .length = MIDI_DATA_PACKET_DATA_LENGTH,
      .checksum = kFillerByte
    }
  }
};

static uint8_t const kDeviceControlBalanceSysExPacket[] = {
  MIDI_SYSTEM_EXCLUSIVE,
  MIDI_REAL_TIME_ID, kFillerByte, MIDI_DEVICE_CONTROL,
  MIDI_MASTER_BALANCE, kFillerWordLsb, kFillerWordMsb,
  MIDI_END_SYSTEM_EXCLUSIVE
};
static midi_message_t const kDeviceControlBalanceSysExMessage = {
  .type = MIDI_SYSTEM_EXCLUSIVE,
  .sys_ex = {
    .id = {MIDI_REAL_TIME_ID, 0x00, 0x00},
    .device_id = kFillerByte,
    .sub_id = MIDI_DEVICE_CONTROL,
    .device_control = {
      .sub_id = MIDI_MASTER_BALANCE,
      .balance = kFillerWord
    }
  }
};

static uint8_t const kDeviceInquirySysExPacket[] = {
  MIDI_SYSTEM_EXCLUSIVE, MIDI_NON_REAL_TIME_ID, MIDI_ALL_CALL,
  MIDI_GENERAL_INFO, MIDI_DEVICE_INQUIRY_RESPONSE,
  0x00, 0x12, 0x34,
  0x7C, 0x1F,
  0x33, 0x66,
  'M', 'I', 'D', 'I',
  MIDI_END_SYSTEM_EXCLUSIVE
};
static midi_message_t const kDeviceInquirySysExMessage = {
  .type = MIDI_SYSTEM_EXCLUSIVE,
  .sys_ex = {
    .id = {MIDI_NON_REAL_TIME_ID, 0x00, 0x00},
    .device_id = MIDI_ALL_CALL,
    .sub_id = MIDI_GENERAL_INFO,
    .device_inquiry = {
      .sub_id = MIDI_DEVICE_INQUIRY_RESPONSE,
      .id = {0x00, 0x12, 0x34},
      .device_family_code = 0x0FFC,
      .device_family_member_code = 0x3333,
      .software_revision_level = {'M', 'I', 'D', 'I'}
    }
  }
};

static uint8_t const kAckSysExPacket[] = {
  MIDI_SYSTEM_EXCLUSIVE,
  MIDI_NON_REAL_TIME_ID, kFillerByte, MIDI_ACK, kFillerByte,
  MIDI_END_SYSTEM_EXCLUSIVE
};
static midi_message_t const kAckSysExMessage = {
  .type = MIDI_SYSTEM_EXCLUSIVE,
  .sys_ex = {
    .id = {MIDI_NON_REAL_TIME_ID, 0x00, 0x00},
    .device_id = kFillerByte,
    .sub_id = MIDI_ACK,
    .packet_number = kFillerByte
  }
};

static uint8_t const kProprietarySysExPacket[] = {
  MIDI_SYSTEM_EXCLUSIVE,
  0x00, kFillerByte, kFillerByte,
  kFillerByte, kFillerByte,
  0x49, 0x54, 0x6D, 0x51, 0x16, 0x45, 0x01, 0x60,
  0x10, 0x1E, 0x21, 0x5E, 0x5E, 0x08, 0x23, 0x64,
  0x6C, 0x02, 0x40, 0x0D, 0x30, 0x7E, 0x7A, 0x16,
  0x20, 0x6D, 0x3D, 0x38, 0x51, 0x5C, 0x75, 0x6F,
  0x2F, 0x5A, 0x7B, 0x7B, 0x5E, 0x22, 0x72, 0x69,
  0x06, 0x1E, 0x0A, 0x0B, 0x69, 0x0E, 0x24, 0x11,
  0x2A, 0x10, 0x64, 0x78, 0x35, 0x78, 0x2D, 0x0B,
  0x66, 0x10, 0x06, 0x2F, 0x24, 0x0E, 0x10, 0x4D,
  0x56, 0x09, 0x05, 0x5A, 0x03, 0x0E, 0x55, 0x39,
  0x7B, 0x5B, 0x3C, 0x43, 0x37, 0x6F, 0x16, 0x45,
  0x3C, 0x7F, 0x39, 0x10, 0x1D, 0x0F, 0x67, 0x5E,
  0x5B, 0x1F, 0x79, 0x3D, 0x32, 0x35, 0x0B, 0x2F,
  0x1C, 0x0D, 0x72, 0x43, 0x4E, 0x32, 0x2E, 0x55,
  0x1E, 0x06, 0x68, 0x69, 0x5E, 0x09, 0x4C, 0x21,
  0x0B, 0x11, 0x08, 0x24, 0x73, 0x44, 0x46, 0x5D,
  0x27, 0x5C, 0x26, 0x13, 0x35, 0x40, 0x19, 0x7F,
  0x6A, 0x1D, 0x0B, 0x6F, 0x79, 0x39, 0x21, 0x59,
  0x4B, 0x44, 0x19, 0x70, 0x1C, 0x00, 0x09, 0x21,
  0x57, 0x49, 0x13, 0x17, 0x42, 0x36, 0x16, 0x28,
  0x1A, 0x48, 0x0D, 0x75, 0x32, 0x0B, 0x69, 0x68,
  0x04, 0x6E, 0x19, 0x0B, 0x44, 0x6D, 0x2A, 0x48,
  0x76, 0x79, 0x3C, 0x38, 0x52, 0x73, 0x66, 0x6E,
  0x05, 0x0C, 0x72, 0x6E, 0x10, 0x22, 0x6B, 0x2F,
  0x61, 0x2B, 0x34, 0x7F, 0x20, 0x3E, 0x0F, 0x2F,
  0x7D, 0x07, 0x6A, 0x1C, 0x1B, 0x41, 0x1A, 0x11,
  0x35, 0x2A, 0x36, 0x65, 0x7A, 0x38, 0x58, 0x4E,
  0x7F, 0x0D, 0x54, 0x11, 0x16, 0x65, 0x4C, 0x14,
  0x0D, 0x47, 0x41, 0x36, 0x46, 0x30, 0x60, 0x4F,
  0x58, 0x21, 0x49, 0x3E, 0x0B, 0x47, 0x03, 0x08,
  0x70, 0x15, 0x7D, 0x18, 0x21, 0x3F, 0x42, 0x5B,
  MIDI_END_SYSTEM_EXCLUSIVE
};
static midi_message_t const kProprietarySysExMessage = {
  .type = MIDI_SYSTEM_EXCLUSIVE,
  .sys_ex = {
    .id = {0x00, kFillerByte, kFillerByte},
    .device_id = kFillerByte,
    .sub_id = kFillerByte
  }
};

static uint8_t const kInvalidSysExPacket[] = {
  MIDI_SYSTEM_EXCLUSIVE,
  MIDI_NON_REAL_TIME_ID, kFillerByte, MIDI_DUMP_REQUEST,
  0x44, 0x83,
  MIDI_END_SYSTEM_EXCLUSIVE
};
static midi_message_t const kInvalidSysExMessage = {
  .type = MIDI_SYSTEM_EXCLUSIVE,
  .sys_ex = {
    .id = {MIDI_NON_REAL_TIME_ID, 0x00, 0x00},
    .device_id = kFillerByte,
    .sub_id = MIDI_DUMP_REQUEST,
    .dump_request = {
      .sample_number = 0x41C3
    }
  }
};

static void TestMidiSerialize_Invalid(void) {
  uint8_t buffer[4];
  TEST_ASSERT_EQUAL(0, MidiSerializeMessage(
      NULL, false, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL(0, MidiSerializeMessage(
      &kNoteOnMessage, false, NULL, sizeof(buffer)));
  TEST_ASSERT_EQUAL(0, MidiSerializeMessage(
      &kNoneMessage, false, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL(0, MidiSerializeMessage(
      &kInvalidSysExMessage, false, buffer, sizeof(buffer)));
}

static void TestMidiSerialize_KnownMessage(void) {
  uint8_t buffer[4];
  /* Partial serialize. */
  TEST_ASSERT_EQUAL(
      sizeof(kNoteOffPacket),
      MidiSerializeMessage(&kNoteOffMessage, false, NULL, 0));
  TEST_ASSERT_EQUAL(
      sizeof(kNoteOffPacket),
      MidiSerializeMessage(&kNoteOffMessage, false, buffer, 1));
  TEST_ASSERT_EQUAL(
      sizeof(kProgramChangePacket),
      MidiSerializeMessage(&kProgramChangeMessage, false, buffer, 1));

  /* Successful serial. */
  TEST_ASSERT_EQUAL(
      sizeof(kNoteOffPacket),
      MidiSerializeMessage(&kNoteOffMessage, false, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_MEMORY(kNoteOffPacket, buffer, sizeof(kNoteOffPacket));

  TEST_ASSERT_EQUAL(
      sizeof(kNoteOnPacket),
      MidiSerializeMessage(&kNoteOnMessage, false, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_MEMORY(kNoteOnPacket, buffer, sizeof(kNoteOnPacket));

  TEST_ASSERT_EQUAL(
      sizeof(kKeyPressurePacket),
      MidiSerializeMessage(
          &kKeyPressureMessage, false, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_MEMORY(kKeyPressurePacket, buffer, sizeof(kKeyPressurePacket));

  TEST_ASSERT_EQUAL(
      sizeof(kControlChangePacket),
      MidiSerializeMessage(
          &kControlChangeMessage, false, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_MEMORY(kControlChangePacket, buffer, sizeof(kControlChangePacket));

  TEST_ASSERT_EQUAL(
      sizeof(kProgramChangePacket),
      MidiSerializeMessage(
          &kProgramChangeMessage, false, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_MEMORY(
      kProgramChangePacket, buffer, sizeof(kProgramChangePacket));

  TEST_ASSERT_EQUAL(
      sizeof(kChannelPressurePacket),
      MidiSerializeMessage(
          &kChannelPressureMessage, false, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_MEMORY(
      kChannelPressurePacket, buffer, sizeof(kChannelPressurePacket));

  TEST_ASSERT_EQUAL(
      sizeof(kPitchWheelPacket),
      MidiSerializeMessage(
          &kPitchWheelMessage, false, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_MEMORY(
      kPitchWheelPacket, buffer, sizeof(kPitchWheelPacket));

  /* System exclusive has its own serialization test. */
  TEST_ASSERT_EQUAL(
      sizeof(kTimeCodePacket),
      MidiSerializeMessage(&kTimeCodeMessage, false, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_MEMORY(kTimeCodePacket, buffer, sizeof(kTimeCodePacket));

  TEST_ASSERT_EQUAL(
      sizeof(kSongPositionPointerPacket),
      MidiSerializeMessage(
          &kSongPositionPointerMessage, false, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_MEMORY(
      kSongPositionPointerPacket, buffer, sizeof(kSongPositionPointerPacket));

  TEST_ASSERT_EQUAL(
      sizeof(kSongSelectPacket),
      MidiSerializeMessage(
          &kSongSelectMessage, false, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_MEMORY(
      kSongSelectPacket, buffer, sizeof(kSongSelectPacket));

  TEST_ASSERT_EQUAL(
      sizeof(kTuneRequestPacket),
      MidiSerializeMessage(
          &kTuneRequestMessage, false, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_MEMORY(
      kTuneRequestPacket, buffer, sizeof(kTuneRequestPacket));

  TEST_ASSERT_EQUAL(
      sizeof(kTimingClockPacket),
      MidiSerializeMessage(
          &kTimingClockMessage, false, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_MEMORY(
      kTimingClockPacket, buffer, sizeof(kTimingClockPacket));

  TEST_ASSERT_EQUAL(
      sizeof(kStartPacket),
      MidiSerializeMessage(&kStartMessage, false, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_MEMORY(kStartPacket, buffer, sizeof(kStartPacket));

  TEST_ASSERT_EQUAL(
      sizeof(kContinuePacket),
      MidiSerializeMessage(&kContinueMessage, false, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_MEMORY(kContinuePacket, buffer, sizeof(kContinuePacket));

  TEST_ASSERT_EQUAL(
      sizeof(kStopPacket),
      MidiSerializeMessage(&kStopMessage, false, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_MEMORY(kStopPacket, buffer, sizeof(kStopPacket));

  TEST_ASSERT_EQUAL(
      sizeof(kActiveSensingPacket),
      MidiSerializeMessage(
          &kActiveSensingMessage, false, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_MEMORY(
      kActiveSensingPacket, buffer, sizeof(kActiveSensingPacket));

  TEST_ASSERT_EQUAL(
      sizeof(kSystemResetPacket),
      MidiSerializeMessage(
          &kSystemResetMessage, false, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_MEMORY(
      kSystemResetPacket, buffer, sizeof(kSystemResetPacket));
}

static void TestMidiSerialize_SysEx(void) {
  uint8_t buffer[128];
  /* Partial serialization. */
  TEST_ASSERT_EQUAL(
      sizeof(kDataPacketSysExPacket),
      MidiSerializeMessage(
          &kDataPacketSysExMessage, false, NULL, 0));
  TEST_ASSERT_EQUAL(
      sizeof(kDataPacketSysExPacket),
      MidiSerializeMessage(
          &kDataPacketSysExMessage, false, buffer, sizeof(buffer) / 2));
  TEST_ASSERT_EQUAL(
      sizeof(kDeviceControlBalanceSysExPacket),
      MidiSerializeMessage(
          &kDeviceControlBalanceSysExMessage, false, NULL, 0));

  /* Successful serialization. */
  TEST_ASSERT_EQUAL(
      sizeof(kDataPacketSysExPacket),
      MidiSerializeMessage(
          &kDataPacketSysExMessage, false, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_MEMORY(
      kDataPacketSysExPacket, buffer, sizeof(kDataPacketSysExPacket));

  TEST_ASSERT_EQUAL(
      sizeof(kDeviceControlBalanceSysExPacket),
      MidiSerializeMessage(
          &kDeviceControlBalanceSysExMessage, false, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_MEMORY(
      kDeviceControlBalanceSysExPacket, buffer,
      sizeof(kDeviceControlBalanceSysExPacket));

  TEST_ASSERT_EQUAL(sizeof(kDeviceInquirySysExPacket), MidiSerializeMessage(
      &kDeviceInquirySysExMessage, false, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_MEMORY(
      kDeviceInquirySysExPacket, buffer, sizeof(kDeviceInquirySysExPacket));

  TEST_ASSERT_EQUAL(
      sizeof(kAckSysExPacket),
      MidiSerializeMessage(&kAckSysExMessage, false, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_MEMORY(kAckSysExPacket, buffer, sizeof(kAckSysExPacket));

  /* Unsupported proprietary. */
  TEST_ASSERT_EQUAL(0, MidiSerializeMessage(
      &kProprietarySysExMessage, false, buffer, sizeof(buffer)));
}

static void TestMidiSerialize_TimePacket(void) {
  midi_time_t time;
  uint8_t time_packet[12];
  MidiInitializeTime(&time);
  TEST_ASSERT_EQUAL(0, MidiSerializeTimeAsPacket(
      NULL, true, time_packet,  sizeof(time_packet)));
  TEST_ASSERT_EQUAL(0, MidiSerializeTimeAsPacket(
      &time, false, NULL,  sizeof(time_packet)));
  time.hours = 24;
  TEST_ASSERT_EQUAL(0, MidiSerializeTimeAsPacket(
      &time, true, time_packet,  sizeof(time_packet)));

  MidiInitializeTime(&time);
  time.frame = /* 22 */ 0x16;
  time.seconds = /* 41 */ 0x2A;
  time.minutes = /* 14 */ 0x0E;
  time.hours = /* 13 */ 0x0D;
  time.fps = MIDI_30_FPS_NON_DROP /* 0x60 */;
  static uint8_t const kForwardTimePacket[] = {
    MIDI_TIME_CODE,
    MIDI_FRAME_COUNT_LSN | 0x6,
    MIDI_FRAME_COUNT_MSN | 0x1,
    MIDI_SECONDS_COUNT_LSN | 0xA,
    MIDI_SECONDS_COUNT_MSN | 0x2,
    MIDI_MINUTES_COUNT_LSN | 0xE,
    MIDI_MINUTES_COUNT_MSN | 0x0,
    MIDI_HOURS_COUNT_LSN | 0xD,
    MIDI_HOURS_COUNT_MSN | 0x0 | 0x6
  };
  static uint8_t const kBackwardTimeData[] = {
    MIDI_TIME_CODE,
    MIDI_HOURS_COUNT_MSN | 0x0 | 0x6,
    MIDI_HOURS_COUNT_LSN | 0xD,
    MIDI_MINUTES_COUNT_MSN | 0x0,
    MIDI_MINUTES_COUNT_LSN | 0xE,
    MIDI_SECONDS_COUNT_MSN | 0x2,
    MIDI_SECONDS_COUNT_LSN | 0xA,
    MIDI_FRAME_COUNT_MSN | 0x1,
    MIDI_FRAME_COUNT_LSN | 0x6
  };
  TEST_ASSERT_EQUAL(
      sizeof(kForwardTimePacket),
      MidiSerializeTimeAsPacket(
          &time, false, time_packet,  sizeof(time_packet)));
  TEST_ASSERT_EQUAL_MEMORY(
      kForwardTimePacket, time_packet, sizeof(kForwardTimePacket));

  TEST_ASSERT_EQUAL(
      sizeof(kBackwardTimeData),
      MidiSerializeTimeAsPacket(
          &time, true, time_packet,  sizeof(time_packet)));
  TEST_ASSERT_EQUAL_MEMORY(
      kBackwardTimeData, time_packet, sizeof(kBackwardTimeData));
}

static void TestMidiDeserialize_Invalid(void) {
  midi_message_t message;
  TEST_ASSERT_EQUAL(0, MidiDeserializeMessage(
      NULL, 3, MIDI_NONE, &message));
  TEST_ASSERT_EQUAL(0, MidiDeserializeMessage(
      kNoteOffPacket, 0, MIDI_NONE, NULL));
  TEST_ASSERT_EQUAL(0, MidiDeserializeMessage(
      kNoteOffPacket, sizeof(kNoteOffPacket), MIDI_NONE, NULL));
  TEST_ASSERT_EQUAL(0, MidiDeserializeMessage(
      kUnknownPacket, sizeof(kUnknownPacket), MIDI_NONE, &message));
  TEST_ASSERT_EQUAL(0, MidiDeserializeMessage(
      kInvalidSysExPacket, sizeof(kInvalidSysExPacket), MIDI_NONE, &message));
}

static void TestMidiDeserialize_KnownMessage(void) {
  midi_message_t message;
  TEST_ASSERT_EQUAL(
      sizeof(kNoteOffPacket),
      MidiDeserializeMessage(
          kNoteOffPacket, sizeof(kNoteOffPacket), MIDI_NONE, &message));
  TEST_ASSERT_EQUAL(kNoteOffMessage.type, message.type);
  TEST_ASSERT_EQUAL(kNoteOffMessage.channel, message.channel);
  TEST_ASSERT_EQUAL(kNoteOffMessage.note.key, message.note.key);
  TEST_ASSERT_EQUAL(kNoteOffMessage.note.velocity, message.note.velocity);

  TEST_ASSERT_EQUAL(
      sizeof(kNoteOnPacket),
      MidiDeserializeMessage(
          kNoteOnPacket, sizeof(kNoteOnPacket), MIDI_NONE, &message));
  TEST_ASSERT_EQUAL(kNoteOnMessage.type, message.type);
  TEST_ASSERT_EQUAL(kNoteOnMessage.channel, message.channel);
  TEST_ASSERT_EQUAL(kNoteOnMessage.note.key, message.note.key);
  TEST_ASSERT_EQUAL(kNoteOnMessage.note.velocity, message.note.velocity);

  TEST_ASSERT_EQUAL(
      sizeof(kKeyPressurePacket),
      MidiDeserializeMessage(
          kKeyPressurePacket, sizeof(kKeyPressurePacket), MIDI_NONE, &message));
  TEST_ASSERT_EQUAL(kKeyPressureMessage.type, message.type);
  TEST_ASSERT_EQUAL(kKeyPressureMessage.channel, message.channel);
  TEST_ASSERT_EQUAL(kKeyPressureMessage.note.key, message.note.key);
  TEST_ASSERT_EQUAL(kKeyPressureMessage.note.pressure, message.note.pressure);

  TEST_ASSERT_EQUAL(
      sizeof(kControlChangePacket),
      MidiDeserializeMessage(
          kControlChangePacket, sizeof(kControlChangePacket),
          MIDI_NONE, &message));
  TEST_ASSERT_EQUAL(kControlChangeMessage.type, message.type);
  TEST_ASSERT_EQUAL(kControlChangeMessage.channel, message.channel);
  TEST_ASSERT_EQUAL(
      kControlChangeMessage.control.number, message.control.number);
  TEST_ASSERT_EQUAL(
      kControlChangeMessage.control.value, message.control.value);

  TEST_ASSERT_EQUAL(
      sizeof(kProgramChangePacket),
      MidiDeserializeMessage(
          kProgramChangePacket, sizeof(kProgramChangePacket),
          MIDI_NONE, &message));
  TEST_ASSERT_EQUAL(kProgramChangeMessage.type, message.type);
  TEST_ASSERT_EQUAL(kProgramChangeMessage.channel, message.channel);
  TEST_ASSERT_EQUAL(kProgramChangeMessage.program, message.program);

  TEST_ASSERT_EQUAL(
      sizeof(kChannelPressurePacket),
      MidiDeserializeMessage(
          kChannelPressurePacket, sizeof(kChannelPressurePacket),
          MIDI_NONE, &message));
  TEST_ASSERT_EQUAL(kChannelPressureMessage.type, message.type);
  TEST_ASSERT_EQUAL(kChannelPressureMessage.channel, message.channel);
  TEST_ASSERT_EQUAL(kChannelPressureMessage.pressure, message.pressure);

  TEST_ASSERT_EQUAL(
      sizeof(kPitchWheelPacket),
      MidiDeserializeMessage(
          kPitchWheelPacket, sizeof(kPitchWheelPacket),
          MIDI_NONE, &message));
  TEST_ASSERT_EQUAL(kPitchWheelMessage.type, message.type);
  TEST_ASSERT_EQUAL(kPitchWheelMessage.channel, message.channel);
  TEST_ASSERT_EQUAL(kPitchWheelMessage.pitch, message.pitch);

  /* System exclusive has its own deserialization test. */

  TEST_ASSERT_EQUAL(
      sizeof(kTimeCodePacket),
      MidiDeserializeMessage(
          kTimeCodePacket, sizeof(kTimeCodePacket),
          MIDI_NONE, &message));
  TEST_ASSERT_EQUAL(kTimeCodeMessage.type, message.type);
  TEST_ASSERT_EQUAL_MEMORY(
      &kTimeCodeMessage.time_code, &message.time_code,
      sizeof(midi_time_code_t));

  TEST_ASSERT_EQUAL(
      sizeof(kSongPositionPointerPacket),
      MidiDeserializeMessage(
          kSongPositionPointerPacket, sizeof(kSongPositionPointerPacket),
          MIDI_NONE, &message));
  TEST_ASSERT_EQUAL(kSongPositionPointerMessage.type, message.type);
  TEST_ASSERT_EQUAL(
      kSongPositionPointerMessage.song_position, message.song_position);

  TEST_ASSERT_EQUAL(
      sizeof(kSongSelectPacket),
      MidiDeserializeMessage(
          kSongSelectPacket, sizeof(kSongSelectPacket),
          MIDI_NONE, &message));
  TEST_ASSERT_EQUAL(kSongSelectMessage.type, message.type);
  TEST_ASSERT_EQUAL(kSongSelectMessage.song_number, message.song_number);

  TEST_ASSERT_EQUAL(
      sizeof(kTuneRequestPacket),
      MidiDeserializeMessage(
          kTuneRequestPacket, sizeof(kTuneRequestPacket),
          MIDI_NONE, &message));
  TEST_ASSERT_EQUAL(kTuneRequestMessage.type, message.type);

  TEST_ASSERT_EQUAL(
      sizeof(kTimingClockPacket),
      MidiDeserializeMessage(
          kTimingClockPacket, sizeof(kTimingClockPacket),
          MIDI_NONE, &message));
  TEST_ASSERT_EQUAL(kTimingClockMessage.type, message.type);

  TEST_ASSERT_EQUAL(
      sizeof(kStartPacket),
      MidiDeserializeMessage(
          kStartPacket, sizeof(kStartPacket),
          MIDI_NONE, &message));
  TEST_ASSERT_EQUAL(kStartMessage.type, message.type);

  TEST_ASSERT_EQUAL(
      sizeof(kContinuePacket),
      MidiDeserializeMessage(
          kContinuePacket, sizeof(kContinuePacket),
          MIDI_NONE, &message));
  TEST_ASSERT_EQUAL(kContinueMessage.type, message.type);

  TEST_ASSERT_EQUAL(
      sizeof(kStopPacket),
      MidiDeserializeMessage(
          kStopPacket, sizeof(kStopPacket),
          MIDI_NONE, &message));
  TEST_ASSERT_EQUAL(kStopMessage.type, message.type);

  TEST_ASSERT_EQUAL(
      sizeof(kActiveSensingPacket),
      MidiDeserializeMessage(
          kActiveSensingPacket, sizeof(kActiveSensingPacket),
          MIDI_NONE, &message));
  TEST_ASSERT_EQUAL(kActiveSensingMessage.type, message.type);

  TEST_ASSERT_EQUAL(
      sizeof(kSystemResetPacket),
      MidiDeserializeMessage(
          kSystemResetPacket, sizeof(kSystemResetPacket),
          MIDI_NONE, &message));
  TEST_ASSERT_EQUAL(kSystemResetMessage.type, message.type);
}  /* TestMidiDeserialize_KnownMessage */

static void TestMidiDeserialize_MultiByteMessage_WithOverride(void) {
  midi_message_t message;
  TEST_ASSERT_EQUAL(sizeof(kNoteOffPacket) - 1, MidiDeserializeMessage(
      &kNoteOffPacket[1], sizeof(kNoteOffPacket) - 1,
      kNoteOffMessage.type | kNoteOffMessage.channel, &message));
  TEST_ASSERT_EQUAL(kNoteOffMessage.type, message.type);
  TEST_ASSERT_EQUAL(kNoteOffMessage.channel, message.channel);
  TEST_ASSERT_EQUAL(kNoteOffMessage.note.key, message.note.key);
  TEST_ASSERT_EQUAL(kNoteOffMessage.note.velocity, message.note.velocity);

  TEST_ASSERT_EQUAL(sizeof(kNoteOnPacket) - 1, MidiDeserializeMessage(
      &kNoteOnPacket[1], sizeof(kNoteOnPacket) - 1,
      kNoteOnMessage.type | kNoteOnMessage.channel, &message));
  TEST_ASSERT_EQUAL(kNoteOnMessage.type, message.type);
  TEST_ASSERT_EQUAL(kNoteOnMessage.channel, message.channel);
  TEST_ASSERT_EQUAL(kNoteOnMessage.note.key, message.note.key);
  TEST_ASSERT_EQUAL(kNoteOnMessage.note.velocity, message.note.velocity);

  TEST_ASSERT_EQUAL(sizeof(kKeyPressurePacket) - 1, MidiDeserializeMessage(
      &kKeyPressurePacket[1], sizeof(kKeyPressurePacket) - 1,
      kKeyPressureMessage.type | kKeyPressureMessage.channel, &message));
  TEST_ASSERT_EQUAL(kKeyPressureMessage.type, message.type);
  TEST_ASSERT_EQUAL(kKeyPressureMessage.channel, message.channel);
  TEST_ASSERT_EQUAL(kKeyPressureMessage.note.key, message.note.key);
  TEST_ASSERT_EQUAL(kKeyPressureMessage.note.pressure, message.note.pressure);

  TEST_ASSERT_EQUAL(sizeof(kControlChangePacket) - 1, MidiDeserializeMessage(
      &kControlChangePacket[1], sizeof(kControlChangePacket) - 1,
      kControlChangeMessage.type | kControlChangeMessage.channel, &message));
  TEST_ASSERT_EQUAL(kControlChangeMessage.type, message.type);
  TEST_ASSERT_EQUAL(kControlChangeMessage.channel, message.channel);
  TEST_ASSERT_EQUAL(
      kControlChangeMessage.control.number, message.control.number);
  TEST_ASSERT_EQUAL(
      kControlChangeMessage.control.value, message.control.value);

  TEST_ASSERT_EQUAL(sizeof(kProgramChangePacket) - 1, MidiDeserializeMessage(
      &kProgramChangePacket[1], sizeof(kProgramChangePacket) - 1,
      kProgramChangeMessage.type | kProgramChangeMessage.channel, &message));
  TEST_ASSERT_EQUAL(kProgramChangeMessage.type, message.type);
  TEST_ASSERT_EQUAL(kProgramChangeMessage.channel, message.channel);
  TEST_ASSERT_EQUAL(kProgramChangeMessage.program, message.program);

  TEST_ASSERT_EQUAL(sizeof(kChannelPressurePacket) - 1, MidiDeserializeMessage(
      &kChannelPressurePacket[1], sizeof(kChannelPressurePacket) - 1,
      kChannelPressureMessage.type | kChannelPressureMessage.channel,
      &message));
  TEST_ASSERT_EQUAL(kChannelPressureMessage.type, message.type);
  TEST_ASSERT_EQUAL(kChannelPressureMessage.channel, message.channel);
  TEST_ASSERT_EQUAL(kChannelPressureMessage.pressure, message.pressure);

  TEST_ASSERT_EQUAL(sizeof(kPitchWheelPacket) - 1, MidiDeserializeMessage(
      &kPitchWheelPacket[1], sizeof(kPitchWheelPacket) - 1,
      kPitchWheelMessage.type | kPitchWheelMessage.channel, &message));
  TEST_ASSERT_EQUAL(kPitchWheelMessage.type, message.type);
  TEST_ASSERT_EQUAL(kPitchWheelMessage.channel, message.channel);
  TEST_ASSERT_EQUAL(kPitchWheelMessage.pitch, message.pitch);

  /* System exclusive has its own deserialization test. */

  TEST_ASSERT_EQUAL(sizeof(kTimeCodePacket) - 1, MidiDeserializeMessage(
      &kTimeCodePacket[1], sizeof(kTimeCodePacket) - 1,
      kTimeCodeMessage.type, &message));
  TEST_ASSERT_EQUAL(kTimeCodeMessage.type, message.type);
  TEST_ASSERT_EQUAL_MEMORY(
      &kTimeCodeMessage.time_code, &message.time_code,
      sizeof(midi_time_code_t));

  TEST_ASSERT_EQUAL(
      sizeof(kSongPositionPointerPacket) - 1,
      MidiDeserializeMessage(
          &kSongPositionPointerPacket[1],
          sizeof(kSongPositionPointerPacket) - 1,
          kSongPositionPointerMessage.type, &message));
  TEST_ASSERT_EQUAL(kSongPositionPointerMessage.type, message.type);
  TEST_ASSERT_EQUAL(
      kSongPositionPointerMessage.song_position, message.song_position);

  TEST_ASSERT_EQUAL(sizeof(kSongSelectPacket) - 1, MidiDeserializeMessage(
      &kSongSelectPacket[1], sizeof(kSongSelectPacket) - 1,
      kSongSelectMessage.type, &message));
  TEST_ASSERT_EQUAL(kSongSelectMessage.type, message.type);
  TEST_ASSERT_EQUAL(kSongSelectMessage.song_number, message.song_number);
}  /* TestMidiDeserialize_KnownMessage_WithOverride */

static void TestMidiDeserialize_SingleByteMessage_WithOverride(void) {
  midi_message_t message;
  TEST_ASSERT_EQUAL(0, MidiDeserializeMessage(
      NULL, 0, MIDI_TUNE_REQUEST, &message));
  TEST_ASSERT_EQUAL(MIDI_TUNE_REQUEST, message.type);

  TEST_ASSERT_EQUAL(0, MidiDeserializeMessage(
      NULL, 0, MIDI_END_SYSTEM_EXCLUSIVE, &message));
  TEST_ASSERT_EQUAL(MIDI_END_SYSTEM_EXCLUSIVE, message.type);

  TEST_ASSERT_EQUAL(0, MidiDeserializeMessage(
      NULL, 0, MIDI_TIMING_CLOCK, &message));
  TEST_ASSERT_EQUAL(MIDI_TIMING_CLOCK, message.type);

  TEST_ASSERT_EQUAL(0, MidiDeserializeMessage(
      NULL, 0, MIDI_START, &message));
  TEST_ASSERT_EQUAL(MIDI_START, message.type);

  TEST_ASSERT_EQUAL(0, MidiDeserializeMessage(
      NULL, 0, MIDI_CONTINUE, &message));
  TEST_ASSERT_EQUAL(MIDI_CONTINUE, message.type);

  TEST_ASSERT_EQUAL(0, MidiDeserializeMessage(
      NULL, 0, MIDI_STOP, &message));
  TEST_ASSERT_EQUAL(MIDI_STOP, message.type);

  TEST_ASSERT_EQUAL(0, MidiDeserializeMessage(
      NULL, 0, MIDI_ACTIVE_SENSING, &message));
  TEST_ASSERT_EQUAL(MIDI_ACTIVE_SENSING, message.type);

  TEST_ASSERT_EQUAL(0, MidiDeserializeMessage(
      NULL, 0, MIDI_SYSTEM_RESET, &message));
  TEST_ASSERT_EQUAL(MIDI_SYSTEM_RESET, message.type);
}

static void TestMidiDeserialize_SysEx(void) {
  midi_message_t message;
  /* Partial serialize. */
  TEST_ASSERT_EQUAL(
      sizeof(kDataPacketSysExPacket),
      MidiDeserializeMessage(
          kDataPacketSysExPacket, sizeof(kDataPacketSysExPacket) / 2,
          MIDI_NONE, &message));
  TEST_ASSERT_EQUAL(
      sizeof(kDeviceControlBalanceSysExPacket),
      MidiDeserializeMessage(
          kDeviceControlBalanceSysExPacket,
          sizeof(kDeviceControlBalanceSysExPacket) - 2, MIDI_NONE,
          &message));

  /* Successful serialization. */
  midi_data_packet_buffer_t buffer;
  TEST_ASSERT_TRUE(MidiPushGlobalDataPacketBuffer(buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL(
      sizeof(kDataPacketSysExPacket),
      MidiDeserializeMessage(
          kDataPacketSysExPacket, sizeof(kDataPacketSysExPacket), MIDI_NONE,
          &message));
  TEST_ASSERT_EQUAL(kDataPacketSysExMessage.type, message.type);
  TEST_ASSERT_EQUAL_MEMORY(
      kDataPacketSysExMessage.sys_ex.id, message.sys_ex.id,
      sizeof(midi_manufacturer_id_t));
  TEST_ASSERT_EQUAL(
      kDataPacketSysExMessage.sys_ex.device_id, message.sys_ex.device_id);
  TEST_ASSERT_EQUAL(kDataPacketSysExMessage.sys_ex.sub_id, message.sys_ex.sub_id);
  TEST_ASSERT_EQUAL(
      kDataPacketSysExMessage.sys_ex.data_packet.number,
      message.sys_ex.data_packet.number);
  TEST_ASSERT_EQUAL(((uint8_t*) buffer), message.sys_ex.data_packet.data);
  TEST_ASSERT_EQUAL_MEMORY(
      kDataPacketSysExMessage.sys_ex.data_packet.data,
      message.sys_ex.data_packet.data, MIDI_DATA_PACKET_DATA_LENGTH);
  TEST_ASSERT_EQUAL(
      kDataPacketSysExMessage.sys_ex.data_packet.length,
      message.sys_ex.data_packet.length);
  TEST_ASSERT_EQUAL(
      kDataPacketSysExMessage.sys_ex.data_packet.checksum,
      message.sys_ex.data_packet.checksum);
  TEST_ASSERT_TRUE(MidiVerifyDataPacketChecksum(
      &message.sys_ex.data_packet, message.sys_ex.device_id));
  MidiPushGlobalDataPacketBuffer(NULL, 0);

  memset(&message, 0, sizeof(message));
  TEST_ASSERT_EQUAL(
      sizeof(kDeviceControlBalanceSysExPacket),
      MidiDeserializeMessage(
          kDeviceControlBalanceSysExPacket,
          sizeof(kDeviceControlBalanceSysExPacket), MIDI_NONE, &message));
  TEST_ASSERT_EQUAL(kDeviceControlBalanceSysExMessage.type, message.type);
  TEST_ASSERT_EQUAL_MEMORY(
      kDeviceControlBalanceSysExMessage.sys_ex.id, message.sys_ex.id,
      sizeof(midi_manufacturer_id_t));
  TEST_ASSERT_EQUAL(
      kDeviceControlBalanceSysExMessage.sys_ex.device_id,
      message.sys_ex.device_id);
  TEST_ASSERT_EQUAL(
      kDeviceControlBalanceSysExMessage.sys_ex.sub_id, message.sys_ex.sub_id);
  TEST_ASSERT_EQUAL(
      kDeviceControlBalanceSysExMessage.sys_ex.device_control.sub_id,
      message.sys_ex.device_control.sub_id);
  TEST_ASSERT_EQUAL(
      kDeviceControlBalanceSysExMessage.sys_ex.device_control.balance,
      message.sys_ex.device_control.balance);

  TEST_ASSERT_EQUAL(
      sizeof(kDeviceInquirySysExPacket),
      MidiDeserializeMessage(
          kDeviceInquirySysExPacket,
          sizeof(kDeviceInquirySysExPacket), MIDI_NONE, &message));
  TEST_ASSERT_EQUAL(kDeviceInquirySysExMessage.type, message.type);
  TEST_ASSERT_EQUAL_MEMORY(
      kDeviceInquirySysExMessage.sys_ex.id, message.sys_ex.id,
      sizeof(midi_manufacturer_id_t));
  TEST_ASSERT_EQUAL(
      kDeviceInquirySysExMessage.sys_ex.device_id, message.sys_ex.device_id);
  TEST_ASSERT_EQUAL(
      kDeviceInquirySysExMessage.sys_ex.sub_id, message.sys_ex.sub_id);
  TEST_ASSERT_EQUAL(
      kDeviceInquirySysExMessage.sys_ex.device_inquiry.sub_id,
      message.sys_ex.device_inquiry.sub_id);
  TEST_ASSERT_EQUAL_MEMORY(
      kDeviceInquirySysExMessage.sys_ex.device_inquiry.id,
      message.sys_ex.device_inquiry.id, sizeof(midi_manufacturer_id_t));
  TEST_ASSERT_EQUAL(
      kDeviceInquirySysExMessage.sys_ex.device_inquiry.device_family_code,
      message.sys_ex.device_inquiry.device_family_code);
  TEST_ASSERT_EQUAL(
      kDeviceInquirySysExMessage.sys_ex.device_inquiry.device_family_member_code,
      message.sys_ex.device_inquiry.device_family_member_code);
  TEST_ASSERT_EQUAL_MEMORY(
      kDeviceInquirySysExMessage.sys_ex.device_inquiry.software_revision_level,
      message.sys_ex.device_inquiry.software_revision_level,
      MIDI_SOFTWARE_REVISION_SIZE);

  memset(&message, 0, sizeof(message));
  TEST_ASSERT_EQUAL(
      sizeof(kAckSysExPacket),
      MidiDeserializeMessage(
          kAckSysExPacket,
          sizeof(kAckSysExPacket), MIDI_NONE, &message));
  TEST_ASSERT_EQUAL(kAckSysExMessage.type, message.type);
  TEST_ASSERT_EQUAL_MEMORY(
      kAckSysExMessage.sys_ex.id, message.sys_ex.id,
      sizeof(midi_manufacturer_id_t));
  TEST_ASSERT_EQUAL(
      kAckSysExMessage.sys_ex.device_id, message.sys_ex.device_id);
  TEST_ASSERT_EQUAL(
      kAckSysExMessage.sys_ex.sub_id, message.sys_ex.sub_id);
  TEST_ASSERT_EQUAL(
      kAckSysExMessage.sys_ex.packet_number, message.sys_ex.packet_number);

  /* Deserialize proprietary */
  memset(&message, 0, sizeof(message));
  TEST_ASSERT_EQUAL(sizeof(kProprietarySysExPacket), MidiDeserializeMessage(
      kProprietarySysExPacket, sizeof(kProprietarySysExPacket), MIDI_NONE,
      &message));
  TEST_ASSERT_EQUAL(kProprietarySysExMessage.type, message.type);
  TEST_ASSERT_EQUAL_MEMORY(
      kProprietarySysExMessage.sys_ex.id, message.sys_ex.id,
      sizeof(midi_manufacturer_id_t));
  TEST_ASSERT_EQUAL(
      kProprietarySysExMessage.sys_ex.device_id,
      message.sys_ex.device_id);
  TEST_ASSERT_EQUAL(
      kProprietarySysExMessage.sys_ex.sub_id, message.sys_ex.sub_id);
}  /* TestMidiDeserialize_SysEx */

void MidiSerializeTest(void) {
  RUN_TEST(TestMidiSerialize_Invalid);
  RUN_TEST(TestMidiSerialize_KnownMessage);
  RUN_TEST(TestMidiSerialize_SysEx);
  RUN_TEST(TestMidiSerialize_TimePacket);
  RUN_TEST(TestMidiDeserialize_Invalid);
  RUN_TEST(TestMidiDeserialize_KnownMessage);
  RUN_TEST(TestMidiDeserialize_MultiByteMessage_WithOverride);
  RUN_TEST(TestMidiDeserialize_SingleByteMessage_WithOverride);
  RUN_TEST(TestMidiDeserialize_SysEx);
}
