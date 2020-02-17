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
  /* Random bytes, checksum = device_id =  number = 0x42 */
  0x22, 0x02, 0x7B, 0x08, 0x16, 0x7A, 0x75, 0x11,
  0x5E, 0x73, 0x28, 0x37, 0x49, 0x26, 0x17, 0x6C,
  0x0B, 0x02, 0x78, 0x37, 0x28, 0x4E, 0x35, 0x46,
  0x4E, 0x5A, 0x24, 0x36, 0x2A, 0x7C, 0x55, 0x2C,
  0x3A, 0x0A, 0x40, 0x55, 0x74, 0x13, 0x33, 0x77,
  0x09, 0x1F, 0x35, 0x3C, 0x28, 0x72, 0x7D, 0x6E,
  0x5C, 0x72, 0x3D, 0x1F, 0x24, 0x69, 0x5A, 0x06,
  0x7F, 0x3A, 0x70, 0x44, 0x47, 0x6D, 0x4F, 0x02,
  0x5F, 0x11, 0x4D, 0x1D, 0x0D, 0x29, 0x2A, 0x3B,
  0x42, 0x46, 0x4A, 0x1B, 0x0C, 0x6F, 0x6A, 0x16,
  0x51, 0x04, 0x51, 0x35, 0x10, 0x7F, 0x29, 0x5B,
  0x7F, 0x14, 0x5B, 0x68, 0x40, 0x7A, 0x29, 0x0E,
  0x54, 0x47, 0x51, 0x15, 0x02, 0x1A, 0x2D, 0x36,
  0x4E, 0x7E, 0x13, 0x1A, 0x40, 0x51, 0x6B, 0x2F,
  0x6E, 0x09, 0x32, 0x38, 0x32, 0x65, 0x10, 0x3B,
  0x15,
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
      .checksum = 0x15
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
  /* Should clear internal. */
  TEST_ASSERT_EQUAL(1, MidiReceiveData(&rx_ctx, NULL, 0, &message));
  TEST_ASSERT_EQUAL(MIDI_NONE, message.type);
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

static void TestMidiReceiver_SysEx_Large(void) {
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
  // RUN_TEST(TestMidiReceiver_SysEx_Large);
}
