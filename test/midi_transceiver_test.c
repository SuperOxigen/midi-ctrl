/*
 * MIDI Controller - MIDI Tx and Rx Context Test.
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#include <string.h>
#include <unity.h>

#include "midi_defs.h"
#include "midi_transceiver.h"

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

static uint8_t const kContinuePacket[] = {
  MIDI_CONTINUE
};

static uint8_t const kDataPacketSysExPacket[] = {
  MIDI_SYSTEM_EXCLUSIVE,
  MIDI_NON_REAL_TIME_ID, 0x10, MIDI_DATA_PACKET,
  0x04,
  0x51, 0x6C, 0x70, 0x40, 0x34, 0x20, 0x39, 0x1F,
  0x30, 0x16, 0x31, 0x5B, 0x4B, 0x0E, 0x58, 0x1D,
  0x5A, 0x05, 0x16, 0x3B, 0x5A, 0x1B, 0x26, 0x43,
  0x12, 0x33, 0x79, 0x6D, 0x08, 0x7C, 0x13, 0x7B,
  0x09, 0x7C, 0x31, 0x77, 0x3F, 0x5D, 0x55, 0x5A,
  0x6E, 0x3D, 0x69, 0x28, 0x12, 0x68, 0x52, 0x10,
  0x67, 0x40, 0x02, 0x03, 0x29, 0x28, 0x78, 0x68,
  0x72, 0x6D, 0x1D, 0x05, 0x08, 0x1C, 0x60, 0x22,
  0x22, 0x57, 0x11, 0x36, 0x7E, 0x6F, 0x08, 0x23,
  0x72, 0x5F, 0x01, 0x08, 0x63, 0x61, 0x66, 0x3B,
  0x24, 0x2E, 0x58, 0x59, 0x3E, 0x76, 0x45, 0x61,
  0x12, 0x49, 0x0C, 0x3F, 0x10, 0x60, 0x7D, 0x6D,
  0x33, 0x46, 0x0B, 0x1A, 0x1B, 0x29, 0x23, 0x28,
  0x26, 0x28, 0x52, 0x23, 0x3E, 0x78, 0x29, 0x6D,
  0x27, 0x0A, 0x2F, 0x76, 0x14, 0x53, 0x26, 0x74,
  0x4B,
  MIDI_END_SYSTEM_EXCLUSIVE
};
static midi_message_t const kDataPacketSysExMessage = {
  .type = MIDI_SYSTEM_EXCLUSIVE,
  .sys_ex = {
    .id = {MIDI_NON_REAL_TIME_ID, 0x00, 0x00},
    .device_id = 0x10,
    .sub_id = MIDI_DATA_PACKET,
    .data_packet = {
      .number = 0x04,
      .data = NULL,
      .length = 0,
      .checksum = 0x4B
    }
  }
};

static void TestMidiReceiver_Initialize(void) {
  TEST_ASSERT_FALSE(MidiInitializeReceiverCtx(NULL));
  midi_rx_ctx_t rx_ctx;
  TEST_ASSERT_TRUE(MidiInitializeReceiverCtx(&rx_ctx));
}

static void TestMidiReceiver_InvalidParameters(void) {
  midi_rx_ctx_t rx_ctx;
  midi_message_t message;
  MidiInitializeReceiverCtx(&rx_ctx);
  TEST_ASSERT_EQUAL(0, MidiReceiveData(
      NULL, kNoteOnPacket, sizeof(kNoteOnPacket), &message));
  TEST_ASSERT_EQUAL(0, MidiReceiveData(&rx_ctx, NULL, 4, &message));
  TEST_ASSERT_EQUAL(0, MidiReceiveData(
      &rx_ctx, kNoteOnPacket, sizeof(kNoteOnPacket), NULL));
}

static void TestMidiReceiver_NoData_Empty(void) {
  midi_rx_ctx_t rx_ctx;
  midi_message_t message;
  MidiInitializeReceiverCtx(&rx_ctx);
  TEST_ASSERT_EQUAL(1, MidiReceiveData(&rx_ctx, NULL, 0, &message));
}

static void TestMidiReceiver_NoData_WithStatus_Complete(void) {
  midi_rx_ctx_t rx_ctx;
  midi_message_t message;
  MidiInitializeReceiverCtx(&rx_ctx);
  rx_ctx.status = MIDI_CONTINUE;
  TEST_ASSERT_EQUAL(0, MidiReceiveData(&rx_ctx, NULL, 0, &message));
  TEST_ASSERT_EQUAL(MIDI_CONTINUE, message.type);
  TEST_ASSERT_TRUE(MidiIsValidMessage(&message));
  /* Nothing should change. */
  TEST_ASSERT_EQUAL(0, MidiReceiveData(&rx_ctx, NULL, 0, &message));
  TEST_ASSERT_EQUAL(MIDI_CONTINUE, message.type);
}

static void TestMidiReceiver_NoData_WithStatus_Incomplete(void) {
  midi_rx_ctx_t rx_ctx;
  midi_message_t message;
  MidiInitializeReceiverCtx(&rx_ctx);
  rx_ctx.status = MIDI_PITCH_WHEEL;
  TEST_ASSERT_EQUAL(2, MidiReceiveData(&rx_ctx, NULL, 0, &message));
  TEST_ASSERT_EQUAL(MIDI_NONE, message.type);
}

static void TestMidiReceiver_SingleByteMessage(void) {
  midi_rx_ctx_t rx_ctx;
  midi_message_t message;
  MidiInitializeReceiverCtx(&rx_ctx);
  TEST_ASSERT_EQUAL(sizeof(kContinuePacket), MidiReceiveData(
      &rx_ctx, kContinuePacket, sizeof(kContinuePacket), &message));
  TEST_ASSERT_EQUAL(MIDI_CONTINUE, message.type);
  /* Data-less messages should clear status. */
  TEST_ASSERT_EQUAL(MIDI_NONE, rx_ctx.status);
  TEST_ASSERT_EQUAL(0, rx_ctx.size);
}

static void TestMidiReceiver_MultiByteMessage(void) {
  midi_rx_ctx_t rx_ctx;
  midi_message_t message;
  MidiInitializeReceiverCtx(&rx_ctx);
  TEST_ASSERT_EQUAL(sizeof(kNoteOnPacket), MidiReceiveData(
      &rx_ctx, kNoteOnPacket, sizeof(kNoteOnPacket), &message));
  /* Check messages. */
  TEST_ASSERT_EQUAL(kNoteOnMessage.type, message.type);
  TEST_ASSERT_EQUAL(kNoteOnMessage.channel, message.channel);
  TEST_ASSERT_EQUAL(kNoteOnMessage.note.key, message.note.key);
  TEST_ASSERT_EQUAL(kNoteOnMessage.note.velocity, message.note.velocity);

  /* Messages with data (other than SysEx) should leave status set. */
  TEST_ASSERT_EQUAL(
      kNoteOnMessage.type | kNoteOnMessage.channel, rx_ctx.status);
  TEST_ASSERT_EQUAL(0, rx_ctx.size);
  /* Should only require the data bytes for deserialization. */
  TEST_ASSERT_EQUAL(sizeof(kNoteOnPacket) - 1, MidiReceiveData(
      &rx_ctx, NULL, 0, &message));

  uint8_t const kSecondNote[2] = { /* MIDI_NOTE_ON, */ 0x30, 0x50 };
  TEST_ASSERT_EQUAL(sizeof(kSecondNote), MidiReceiveData(
      &rx_ctx, kSecondNote, sizeof(kSecondNote), &message));
  TEST_ASSERT_EQUAL(kNoteOnMessage.type, message.type);
  TEST_ASSERT_EQUAL(kNoteOnMessage.channel, message.channel);
  TEST_ASSERT_EQUAL(0x30, message.note.key);
  TEST_ASSERT_EQUAL(0x50, message.note.velocity);

  TEST_ASSERT_EQUAL(
      kNoteOnMessage.type | kNoteOnMessage.channel, rx_ctx.status);
}

static void TestMidiReceiver_MultiByteMessage_Transition(void) {
  midi_rx_ctx_t rx_ctx;
  midi_message_t message;
  memset(&message, 0xE5, sizeof(message));
  MidiInitializeReceiverCtx(&rx_ctx);
  TEST_ASSERT_EQUAL(sizeof(kNoteOnPacket), MidiReceiveData(
      &rx_ctx, kNoteOnPacket, sizeof(kNoteOnPacket), &message));
  TEST_ASSERT_EQUAL(kNoteOnMessage.type, message.type);
  TEST_ASSERT_EQUAL(kNoteOnMessage.channel, message.channel);
  TEST_ASSERT_EQUAL(kNoteOnMessage.note.key, message.note.key);
  TEST_ASSERT_EQUAL(kNoteOnMessage.note.velocity, message.note.velocity);

  TEST_ASSERT_EQUAL(
      kNoteOnMessage.type | kNoteOnMessage.channel, rx_ctx.status);
  TEST_ASSERT_EQUAL(0, rx_ctx.size);

  memset(&message, 0xE5, sizeof(message));
  TEST_ASSERT_EQUAL(sizeof(kNoteOffPacket), MidiReceiveData(
      &rx_ctx, kNoteOffPacket, sizeof(kNoteOffPacket), &message));
  TEST_ASSERT_EQUAL(kNoteOffMessage.type, message.type);
  TEST_ASSERT_EQUAL(kNoteOffMessage.channel, message.channel);
  TEST_ASSERT_EQUAL(kNoteOffMessage.note.key, message.note.key);
  TEST_ASSERT_EQUAL(kNoteOffMessage.note.velocity, message.note.velocity);

  TEST_ASSERT_EQUAL(
      kNoteOffMessage.type | kNoteOffMessage.channel, rx_ctx.status);
}

static void TestMidiReceiver_MultiByteMessage_StatusRun(void) {
  static uint8_t const kNoteOnPackets[] = {
    MIDI_NOTE_ON | MIDI_CHANNEL_6,
    MIDI_MIDDLE_C, MIDI_NOTE_ON_VELOCITY,
    MIDI_MIDDLE_C + 4, MIDI_NOTE_ON_VELOCITY,
    MIDI_MIDDLE_C + 7, MIDI_NOTE_ON_VELOCITY
  };
  static midi_message_t const kNoteOnMessages[] = {
    { .type = MIDI_NOTE_ON, .channel = MIDI_CHANNEL_6,
      .note = { .key = MIDI_MIDDLE_C, .velocity = MIDI_NOTE_ON_VELOCITY }},
    { .type = MIDI_NOTE_ON, .channel = MIDI_CHANNEL_6,
      .note = { .key = MIDI_MIDDLE_C + 4, .velocity = MIDI_NOTE_ON_VELOCITY }},
    { .type = MIDI_NOTE_ON, .channel = MIDI_CHANNEL_6,
      .note = { .key = MIDI_MIDDLE_C + 7, .velocity = MIDI_NOTE_ON_VELOCITY }}
  };
  midi_rx_ctx_t rx_ctx;
  midi_message_t message;
  MidiInitializeReceiverCtx(&rx_ctx);
  memset(&message, 0xE5, sizeof(message));
  TEST_ASSERT_EQUAL(3, MidiReceiveData(
      &rx_ctx, &kNoteOnPackets[0], 1, &message));
  TEST_ASSERT_EQUAL(MIDI_NONE, message.type);

  for (size_t i = 0; i < 3; ++i) {
    TEST_ASSERT_EQUAL(2, MidiReceiveData(
        &rx_ctx, &kNoteOnPackets[1 + 2*i], 2, &message));
    /* Receiver state */
    TEST_ASSERT_EQUAL(MIDI_NOTE_ON | MIDI_CHANNEL_6, rx_ctx.status);
    TEST_ASSERT_EQUAL(0, rx_ctx.size);
    /* Status-run state. */
    TEST_ASSERT_EQUAL(MIDI_NOTE_ON, message.type);
    TEST_ASSERT_EQUAL(MIDI_CHANNEL_6, message.channel);
    /* Note data. */
    TEST_ASSERT_EQUAL(kNoteOnMessages[i].note.key, message.note.key);
    TEST_ASSERT_EQUAL(kNoteOnMessages[i].note.velocity, message.note.velocity);
  }
}

static void TestMidiReceiver_SysEx_SmallMessage(void) {
  static uint8_t const kDeviceInquiryPacket[] = {
    MIDI_SYSTEM_EXCLUSIVE, MIDI_NON_REAL_TIME_ID, MIDI_ALL_CALL,
    MIDI_GENERAL_INFO, MIDI_DEVICE_INQUIRY_RESPONSE,
    0x00, 0x12, 0x34,
    0x7C, 0x1F,
    0x33, 0x66,
    'M', 'I', 'D', 'I',
    MIDI_END_SYSTEM_EXCLUSIVE
  };
  static midi_message_t const kDeviceInquiryMessage = {
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
  midi_rx_ctx_t rx_ctx;
  midi_message_t message;
  MidiInitializeReceiverCtx(&rx_ctx);
  memset(&message, 0xE5, sizeof(message));
  TEST_ASSERT_EQUAL(sizeof(kDeviceInquiryPacket), MidiReceiveData(
      &rx_ctx, kDeviceInquiryPacket, sizeof(kDeviceInquiryPacket), &message));
  TEST_ASSERT_EQUAL(MIDI_NONE, rx_ctx.status);
  TEST_ASSERT_EQUAL(0, rx_ctx.size);

  TEST_ASSERT_EQUAL(kDeviceInquiryMessage.type, message.type);
  TEST_ASSERT_EQUAL_MEMORY(
      kDeviceInquiryMessage.sys_ex.id, message.sys_ex.id,
      sizeof(midi_manufacturer_id_t));
  TEST_ASSERT_EQUAL(
      kDeviceInquiryMessage.sys_ex.device_id, message.sys_ex.device_id);
  TEST_ASSERT_EQUAL(
      kDeviceInquiryMessage.sys_ex.sub_id, message.sys_ex.sub_id);
  TEST_ASSERT_EQUAL(
      kDeviceInquiryMessage.sys_ex.device_inquiry.sub_id,
      message.sys_ex.device_inquiry.sub_id);
  TEST_ASSERT_EQUAL_MEMORY(
      kDeviceInquiryMessage.sys_ex.device_inquiry.id,
      message.sys_ex.device_inquiry.id, sizeof(midi_manufacturer_id_t));
  TEST_ASSERT_EQUAL(
      kDeviceInquiryMessage.sys_ex.device_inquiry.device_family_code,
      message.sys_ex.device_inquiry.device_family_code);
  TEST_ASSERT_EQUAL(
      kDeviceInquiryMessage.sys_ex.device_inquiry.device_family_member_code,
      message.sys_ex.device_inquiry.device_family_member_code);
  TEST_ASSERT_EQUAL_MEMORY(
      kDeviceInquiryMessage.sys_ex.device_inquiry.software_revision_level,
      message.sys_ex.device_inquiry.software_revision_level,
      MIDI_SOFTWARE_REVISION_SIZE);
}

static void TestMidiReceiver_SysEx_Large(void) {
  TEST_ASSERT_TRUE(sizeof(kDataPacketSysExPacket) <= MIDI_RX_BUFFER_SIZE);
  midi_rx_ctx_t rx_ctx;
  midi_message_t message;
  memset(&message, 0xE5, sizeof(message));
  MidiInitializeReceiverCtx(&rx_ctx);

  size_t data_consumed = 0;
  while (data_consumed < sizeof(kDataPacketSysExPacket)) {
    size_t const to_consume =
        ((sizeof(kDataPacketSysExPacket) - data_consumed) > 4) ?
        4 : (sizeof(kDataPacketSysExPacket) - data_consumed);
    size_t const res = MidiReceiveData(
        &rx_ctx, &kDataPacketSysExPacket[data_consumed], to_consume, &message);
    if (res == 0) {
      TEST_FAIL_MESSAGE("Receiver returned 0");
      return;
    }
    if (res > to_consume) {
      data_consumed += to_consume;
    } else {
      data_consumed += res;
    }
  }
  TEST_ASSERT_EQUAL(sizeof(kDataPacketSysExPacket), data_consumed);
  /* Check message. */
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
  TEST_ASSERT_EQUAL(NULL, message.sys_ex.data_packet.data);
  TEST_ASSERT_EQUAL(0, message.sys_ex.data_packet.length);
  TEST_ASSERT_EQUAL(
      kDataPacketSysExMessage.sys_ex.data_packet.checksum,
      message.sys_ex.data_packet.checksum);

  TEST_ASSERT_EQUAL(MIDI_NONE,  rx_ctx.status);
}

void MidiTransceiverTest(void) {
  RUN_TEST(TestMidiReceiver_Initialize);
  RUN_TEST(TestMidiReceiver_InvalidParameters);
  RUN_TEST(TestMidiReceiver_NoData_Empty);
  RUN_TEST(TestMidiReceiver_NoData_WithStatus_Complete);
  RUN_TEST(TestMidiReceiver_NoData_WithStatus_Incomplete);
  RUN_TEST(TestMidiReceiver_SingleByteMessage);
  RUN_TEST(TestMidiReceiver_MultiByteMessage);
  RUN_TEST(TestMidiReceiver_MultiByteMessage_Transition);
  RUN_TEST(TestMidiReceiver_MultiByteMessage_StatusRun);
  RUN_TEST(TestMidiReceiver_SysEx_SmallMessage);
  RUN_TEST(TestMidiReceiver_SysEx_Large);
}
