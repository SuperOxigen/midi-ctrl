/*
 * MIDI Controller - MIDI System Univeral Test
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include "midi_defs.h"
#include "midi_sys_uni.h"

static void TestMidiDeviceId_Validator(void) {
  TEST_ASSERT_FALSE(MidiIsValidDeviceId(0x80));
  TEST_ASSERT_TRUE(MidiIsValidDeviceId(0x73));
  TEST_ASSERT_TRUE(MidiIsValidDeviceId(0x00));
}

static void TestMidiHandShakeId_Validator(void) {
  TEST_ASSERT_FALSE(MidiIsHandShakeSubId(0x00));
  TEST_ASSERT_FALSE(MidiIsHandShakeSubId(0x80));
  TEST_ASSERT_FALSE(MidiIsHandShakeSubId(0xFF));

  TEST_ASSERT_TRUE(MidiIsHandShakeSubId(MIDI_EOF));
  TEST_ASSERT_TRUE(MidiIsHandShakeSubId(MIDI_ACK));

  TEST_ASSERT_FALSE(MidiIsHandShakeSubId(MIDI_DUMP_HEADER));
  TEST_ASSERT_FALSE(MidiIsHandShakeSubId(MIDI_GENERAL_MIDI));
  TEST_ASSERT_FALSE(MidiIsHandShakeSubId(0x20));
}

/* Dump Header Message. */

static midi_dump_header_t const kGoodDumpHeader = {
  .sample_number = 0x0410,
  .sample_format = 13,
  .sample_period = 0x00040000,
  .sample_length = 0x00080000,
  .sustain_loop_start_point = 0x00010000,
  .sustain_loop_end_point = 0x00020000,
  .loop_type = MIDI_LOOP_OFF
};
static uint8_t const kGoodDumpHeaderData[MIDI_DUMP_HEADER_PAYLOAD_SIZE] = {
  0x10, 0x08,
  13,
  0x00, 0x00, 0x10,
  0x00, 0x00, 0x20,
  0x00, 0x00, 0x04,
  0x00, 0x00, 0x08,
  MIDI_LOOP_OFF
};
static midi_dump_header_t const kBadDumpHeader = {
  .sample_number = 0x0410,
  .sample_format = 30,
  .sample_period = 0x00040000,
  .sample_length = 0x00080000,
  .sustain_loop_start_point = 0x00010000,
  .sustain_loop_end_point = 0x00020000,
  .loop_type = MIDI_LOOP_OFF
};
static uint8_t const kBadDumpHeaderData[MIDI_DUMP_HEADER_PAYLOAD_SIZE] = {
  0x10, 0x08,
  13,
  0x00, 0x00, 0x10,
  0x00, 0x00, 0x20,
  0x00, MIDI_END_SYSTEM_EXCLUSIVE, 0x04,
  0x00, 0x00, 0x08,
  MIDI_LOOP_OFF
};

static void TestMidiDumpHeader_Validator(void) {
  midi_dump_header_t bad_dump_header = kGoodDumpHeader;

  TEST_ASSERT_FALSE(MidiIsValidDumpHeader(NULL));
  bad_dump_header.loop_type = 0x55;
  TEST_ASSERT_FALSE(MidiIsValidDumpHeader(&bad_dump_header));
  bad_dump_header.loop_type = MIDI_LOOP_BIDIRECTIONAL;
  bad_dump_header.sample_format = 30;
  TEST_ASSERT_FALSE(MidiIsValidDumpHeader(&bad_dump_header));
  bad_dump_header.sample_format = 14;
  bad_dump_header.sample_period = 0x00800000;
  TEST_ASSERT_FALSE(MidiIsValidDumpHeader(&bad_dump_header));

  TEST_ASSERT_TRUE(MidiIsValidDumpHeader(&kGoodDumpHeader));
}

static void TestMidiDumpHeader_Serialize(void) {
  uint8_t dump_header_data[MIDI_DUMP_HEADER_PAYLOAD_SIZE];
  /* Invalid inputs. */
  TEST_ASSERT_EQUAL(0, MidiSerializeDumpHeader(
      NULL, dump_header_data, sizeof(dump_header_data)));
  TEST_ASSERT_EQUAL(0, MidiSerializeDumpHeader(
      &kGoodDumpHeader, NULL, sizeof(dump_header_data)));
  TEST_ASSERT_EQUAL(0, MidiSerializeDumpHeader(
      &kBadDumpHeader, dump_header_data, sizeof(dump_header_data)));

  /* Incomplete serialization. */
  TEST_ASSERT_EQUAL(
      MIDI_DUMP_HEADER_PAYLOAD_SIZE,
      MidiSerializeDumpHeader(&kGoodDumpHeader, NULL, 0));
  TEST_ASSERT_EQUAL(
      MIDI_DUMP_HEADER_PAYLOAD_SIZE,
      MidiSerializeDumpHeader(&kGoodDumpHeader, dump_header_data, 0));

  /* Successful serialization. */
  TEST_ASSERT_EQUAL(
      MIDI_DUMP_HEADER_PAYLOAD_SIZE,
      MidiSerializeDumpHeader(
          &kGoodDumpHeader, dump_header_data, sizeof(dump_header_data)));
  TEST_ASSERT_EQUAL_MEMORY(
      kGoodDumpHeaderData, dump_header_data, MIDI_DUMP_HEADER_PAYLOAD_SIZE);
}

static void TestMidiDumpHeader_Deserialize(void) {
  midi_dump_header_t dump_header;
  /* Invalid inputs. */
  TEST_ASSERT_EQUAL(0, MidiDeserializeDumpHeader(
      NULL, MIDI_DUMP_HEADER_PAYLOAD_SIZE, &dump_header));
  TEST_ASSERT_EQUAL(0, MidiDeserializeDumpHeader(
      kGoodDumpHeaderData, MIDI_DUMP_HEADER_PAYLOAD_SIZE, NULL));
  TEST_ASSERT_EQUAL(0, MidiDeserializeDumpHeader(
      kBadDumpHeaderData, MIDI_DUMP_HEADER_PAYLOAD_SIZE, &dump_header));

  /* Incomplete deserialization. */
  TEST_ASSERT_EQUAL(MIDI_DUMP_HEADER_PAYLOAD_SIZE, MidiDeserializeDumpHeader(
      NULL, 0, &dump_header));
  TEST_ASSERT_EQUAL(MIDI_DUMP_HEADER_PAYLOAD_SIZE, MidiDeserializeDumpHeader(
      kGoodDumpHeaderData, 0, &dump_header));
  TEST_ASSERT_EQUAL(MIDI_DUMP_HEADER_PAYLOAD_SIZE, MidiDeserializeDumpHeader(
      kGoodDumpHeaderData, MIDI_DUMP_HEADER_PAYLOAD_SIZE / 2, &dump_header));

  /* Successful deserialization. */
  TEST_ASSERT_EQUAL(MIDI_DUMP_HEADER_PAYLOAD_SIZE, MidiDeserializeDumpHeader(
      kGoodDumpHeaderData, MIDI_DUMP_HEADER_PAYLOAD_SIZE, &dump_header));
  TEST_ASSERT_EQUAL(kGoodDumpHeader.sample_number, dump_header.sample_number);
  TEST_ASSERT_EQUAL(kGoodDumpHeader.sample_format, dump_header.sample_format);
  TEST_ASSERT_EQUAL(kGoodDumpHeader.sample_period, dump_header.sample_period);
  TEST_ASSERT_EQUAL(kGoodDumpHeader.sample_length, dump_header.sample_length);
  TEST_ASSERT_EQUAL(
      kGoodDumpHeader.sustain_loop_start_point,
      dump_header.sustain_loop_start_point);
  TEST_ASSERT_EQUAL(
      kGoodDumpHeader.sustain_loop_end_point,
      dump_header.sustain_loop_end_point);
  TEST_ASSERT_EQUAL(kGoodDumpHeader.loop_type, dump_header.loop_type);
}

/* Dump Request Message. */

static midi_dump_request_t const kGoodDumpRequest = {
  .sample_number = 0x0202
};
static uint8_t const kGoodDumpRequestData[MIDI_DUMP_REQUEST_PAYLOAD_SIZE] = {
  0x02, 0x04
};
static midi_dump_request_t const kBadDumpRequest = {
  .sample_number = 0x4202
};
static uint8_t const kBadDumpRequestData[MIDI_DUMP_REQUEST_PAYLOAD_SIZE] = {
  0x02, 0x84
};

static void TestMidiDumpRequest_Validator(void) {
  TEST_ASSERT_FALSE(MidiIsValidDumpRequest(NULL));
  TEST_ASSERT_FALSE(MidiIsValidDumpRequest(&kBadDumpRequest));
  TEST_ASSERT_TRUE(MidiIsValidDumpRequest(&kGoodDumpRequest));
}

static void TestMidiDumpRequest_Serialize(void) {
  uint8_t dump_request_data[MIDI_DUMP_REQUEST_PAYLOAD_SIZE];
  /* Invalid inputs. */
  TEST_ASSERT_EQUAL(0, MidiSerializeDumpRequest(
      NULL, dump_request_data, sizeof(dump_request_data)));
  TEST_ASSERT_EQUAL(0, MidiSerializeDumpRequest(
      &kGoodDumpRequest, NULL, sizeof(dump_request_data)));
  TEST_ASSERT_EQUAL(0, MidiSerializeDumpRequest(
      &kBadDumpRequest, dump_request_data, sizeof(dump_request_data)));

  /* Incomplete serialization. */
  TEST_ASSERT_EQUAL(
      MIDI_DUMP_REQUEST_PAYLOAD_SIZE,
      MidiSerializeDumpRequest(&kGoodDumpRequest, NULL, 0));
  TEST_ASSERT_EQUAL(
      MIDI_DUMP_REQUEST_PAYLOAD_SIZE,
      MidiSerializeDumpRequest(&kGoodDumpRequest, dump_request_data, 0));

  /* Successful serialization. */
  TEST_ASSERT_EQUAL(
      MIDI_DUMP_REQUEST_PAYLOAD_SIZE,
      MidiSerializeDumpRequest(
          &kGoodDumpRequest, dump_request_data, sizeof(dump_request_data)));
  TEST_ASSERT_EQUAL_MEMORY(
      kGoodDumpRequestData, dump_request_data, MIDI_DUMP_REQUEST_PAYLOAD_SIZE);
}

static void TestMidiDumpRequest_Deserialize(void) {
  midi_dump_request_t dump_request;
  /* Invalid inputs. */
  TEST_ASSERT_EQUAL(0, MidiDeserializeDumpRequest(
      NULL, MIDI_DUMP_REQUEST_PAYLOAD_SIZE, &dump_request));
  TEST_ASSERT_EQUAL(0, MidiDeserializeDumpRequest(
      kGoodDumpRequestData, MIDI_DUMP_REQUEST_PAYLOAD_SIZE, NULL));
  TEST_ASSERT_EQUAL(0, MidiDeserializeDumpRequest(
      kBadDumpRequestData, MIDI_DUMP_REQUEST_PAYLOAD_SIZE, &dump_request));

  /* Incomplete deserialization. */
  TEST_ASSERT_EQUAL(MIDI_DUMP_REQUEST_PAYLOAD_SIZE, MidiDeserializeDumpRequest(
      NULL, 0, &dump_request));
  TEST_ASSERT_EQUAL(MIDI_DUMP_REQUEST_PAYLOAD_SIZE, MidiDeserializeDumpRequest(
      kGoodDumpRequestData, 0, &dump_request));
  TEST_ASSERT_EQUAL(MIDI_DUMP_REQUEST_PAYLOAD_SIZE, MidiDeserializeDumpRequest(
      kGoodDumpRequestData, MIDI_DUMP_REQUEST_PAYLOAD_SIZE / 2, &dump_request));

  /* Successful deserialization. */
  TEST_ASSERT_EQUAL(MIDI_DUMP_REQUEST_PAYLOAD_SIZE, MidiDeserializeDumpRequest(
      kGoodDumpRequestData, MIDI_DUMP_REQUEST_PAYLOAD_SIZE, &dump_request));
  TEST_ASSERT_EQUAL(kGoodDumpRequest.sample_number, dump_request.sample_number);
}

/* Data Packet Message. */

static void TestMidiGlobalDataPacketBuffer(void) {
  static midi_data_packet_buffer_t sGlobalBufferOne = {};
  static midi_data_packet_buffer_t sGlobalBufferTwo = {};
  TEST_ASSERT_EQUAL(NULL, MidiGetGlobalDataPacketBuffer());

  TEST_ASSERT_FALSE(MidiPushGlobalDataPacketBuffer(
      NULL, MIDI_DATA_PACKET_DATA_LENGTH));
  TEST_ASSERT_FALSE(MidiPushGlobalDataPacketBuffer(sGlobalBufferOne, 0));
  TEST_ASSERT_FALSE(MidiPushGlobalDataPacketBuffer(
      sGlobalBufferOne, MIDI_DATA_PACKET_DATA_LENGTH - 1));
  TEST_ASSERT_TRUE(MidiPushGlobalDataPacketBuffer(
      sGlobalBufferOne, MIDI_DATA_PACKET_DATA_LENGTH));

  TEST_ASSERT_EQUAL(
      ((uint8_t*)sGlobalBufferOne), MidiGetGlobalDataPacketBuffer());

  TEST_ASSERT_TRUE(MidiPushGlobalDataPacketBuffer(NULL, 0));
  TEST_ASSERT_EQUAL(NULL, MidiGetGlobalDataPacketBuffer());

  TEST_ASSERT_TRUE(MidiPushGlobalDataPacketBuffer(
      sGlobalBufferTwo, MIDI_DATA_PACKET_DATA_LENGTH));
  TEST_ASSERT_EQUAL(
      ((uint8_t*)sGlobalBufferTwo), MidiGetGlobalDataPacketBuffer());
  TEST_ASSERT_TRUE(MidiPushGlobalDataPacketBuffer(
      sGlobalBufferOne, MIDI_DATA_PACKET_DATA_LENGTH));
  TEST_ASSERT_EQUAL(
      ((uint8_t*)sGlobalBufferTwo), MidiGetGlobalDataPacketBuffer());
  TEST_ASSERT_EQUAL(
      ((uint8_t*)sGlobalBufferOne), MidiGetGlobalDataPacketBuffer());
  TEST_ASSERT_EQUAL(
      ((uint8_t*)sGlobalBufferTwo), MidiGetGlobalDataPacketBuffer());

  TEST_ASSERT_TRUE(MidiPushGlobalDataPacketBuffer(NULL, 0));
  TEST_ASSERT_EQUAL(NULL, MidiGetGlobalDataPacketBuffer());
}

static midi_device_id_t const kDataPacketDeviceId = 0x10;
static midi_data_packet_buffer_t gGoodDataPacketBuffer = {
  0x6A, 0x3A, 0x1D, 0x6A, 0x3B, 0x37, 0x37, 0x4B,
  0x62, 0x38, 0x00, 0x39, 0x4B, 0x0E, 0x5C, 0x46,
  0x24, 0x4C, 0x7F, 0x74, 0x43, 0x05, 0x49, 0x39,
  0x0A, 0x2E, 0x35, 0x1C, 0x58, 0x15, 0x59, 0x6A,
  0x60, 0x6B, 0x48, 0x28, 0x00, 0x40, 0x20, 0x4E,
  0x10, 0x01, 0x6B, 0x02, 0x38, 0x77, 0x0D, 0x4C,
  0x49, 0x31, 0x74, 0x34, 0x25, 0x26, 0x13, 0x16,
  0x73, 0x38, 0x5A, 0x67, 0x60, 0x19, 0x4B, 0x76,
  0x55, 0x4D, 0x49, 0x58, 0x34, 0x15, 0x5F, 0x28,
  0x6C, 0x5D, 0x6D, 0x31, 0x20, 0x69, 0x0B, 0x75,
  0x47, 0x28, 0x16, 0x12, 0x01, 0x1E, 0x71, 0x38,
  0x17, 0x70, 0x76, 0x48, 0x06, 0x76, 0x4C, 0x0A,
  0x0F, 0x21, 0x0C, 0x01, 0x05, 0x69, 0x36, 0x76,
  0x60, 0x1A, 0x6C, 0x10, 0x79, 0x24, 0x3B, 0x14,
  0x4A, 0x68, 0x7F, 0x48, 0x18, 0x44, 0x22, 0x5A
};
static midi_data_packet_t const kGoodDataPacket = {
  .number = 0x30,
  .data = gGoodDataPacketBuffer,
  .length = MIDI_DATA_PACKET_DATA_LENGTH,
  .checksum = 0x33
};
static uint8_t const kGoodDataPacketData[MIDI_DATA_PACKET_PAYLOAD_SIZE] = {
  0x30,
  0x6A, 0x3A, 0x1D, 0x6A, 0x3B, 0x37, 0x37, 0x4B,
  0x62, 0x38, 0x00, 0x39, 0x4B, 0x0E, 0x5C, 0x46,
  0x24, 0x4C, 0x7F, 0x74, 0x43, 0x05, 0x49, 0x39,
  0x0A, 0x2E, 0x35, 0x1C, 0x58, 0x15, 0x59, 0x6A,
  0x60, 0x6B, 0x48, 0x28, 0x00, 0x40, 0x20, 0x4E,
  0x10, 0x01, 0x6B, 0x02, 0x38, 0x77, 0x0D, 0x4C,
  0x49, 0x31, 0x74, 0x34, 0x25, 0x26, 0x13, 0x16,
  0x73, 0x38, 0x5A, 0x67, 0x60, 0x19, 0x4B, 0x76,
  0x55, 0x4D, 0x49, 0x58, 0x34, 0x15, 0x5F, 0x28,
  0x6C, 0x5D, 0x6D, 0x31, 0x20, 0x69, 0x0B, 0x75,
  0x47, 0x28, 0x16, 0x12, 0x01, 0x1E, 0x71, 0x38,
  0x17, 0x70, 0x76, 0x48, 0x06, 0x76, 0x4C, 0x0A,
  0x0F, 0x21, 0x0C, 0x01, 0x05, 0x69, 0x36, 0x76,
  0x60, 0x1A, 0x6C, 0x10, 0x79, 0x24, 0x3B, 0x14,
  0x4A, 0x68, 0x7F, 0x48, 0x18, 0x44, 0x22, 0x5A,
  0x33
};

static midi_data_packet_t const kEmptyDataPacket = {
  .number = 0x24,
  .data = NULL,
  .length = 0,
  .checksum = 0x48
};
static uint8_t const kEmptyDataPacketData[MIDI_DATA_PACKET_PAYLOAD_SIZE] = {
  [0] = 0x24, [121] = 0x48
};

static uint8_t const kBadDataPacketData[MIDI_DATA_PACKET_PAYLOAD_SIZE] = {
  [0] = 0x24, [121] = 0x48,
  [64] = 0x80
};

static void TestMidiDataPacket_Validator(void) {
  TEST_ASSERT_FALSE(MidiIsValidDataPacket(NULL));
  midi_data_packet_t bad_packet = kEmptyDataPacket;
  bad_packet.length = 22;
  TEST_ASSERT_FALSE(MidiIsValidDataPacket(&bad_packet));
  uint8_t buffer[16] = {};
  buffer[10] = 0x83;
  bad_packet.data = buffer;
  bad_packet.length = 16;
  TEST_ASSERT_FALSE(MidiIsValidDataPacket(&bad_packet));
  buffer[10] = 0x43;
  bad_packet.number = 0xE4;
  TEST_ASSERT_FALSE(MidiIsValidDataPacket(&bad_packet));
  bad_packet.number = 0x54;
  bad_packet.checksum = 0x99;
  TEST_ASSERT_FALSE(MidiIsValidDataPacket(&bad_packet));

  bad_packet.checksum = MIDI_NULL_CHECKSUM;
  TEST_ASSERT_TRUE(MidiIsValidDataPacket(&bad_packet));
  TEST_ASSERT_TRUE(MidiIsValidDataPacket(&kEmptyDataPacket));
  TEST_ASSERT_TRUE(MidiIsValidDataPacket(&kGoodDataPacket));
}

static void TestMidiDataPacket_Initialize(void) {
  midi_data_packet_t packet;
  TEST_ASSERT_FALSE(MidiInitializeDataPacket(NULL, 0x30));
  TEST_ASSERT_FALSE(MidiInitializeDataPacket(&packet, 0x80));
  TEST_ASSERT_TRUE(MidiInitializeDataPacket(&packet, 0x47))
  TEST_ASSERT_EQUAL(0x47, packet.number);
  TEST_ASSERT_EQUAL(NULL, packet.data);
  TEST_ASSERT_EQUAL(0, packet.length);
  TEST_ASSERT_EQUAL(MIDI_NULL_CHECKSUM, packet.checksum);
}

static void TestMidiDataPacket_Checksum(void) {
  midi_data_packet_t packet = kEmptyDataPacket;
  packet.checksum = MIDI_NULL_CHECKSUM;
  TEST_ASSERT_FALSE(MidiFillDataPacketChecksum(NULL, kDataPacketDeviceId));
  TEST_ASSERT_FALSE(MidiFillDataPacketChecksum(
      &packet, kDataPacketDeviceId | 0x80));
  TEST_ASSERT_EQUAL(MIDI_NULL_CHECKSUM, packet.checksum);
  TEST_ASSERT_TRUE(MidiFillDataPacketChecksum(&packet, kDataPacketDeviceId));
  TEST_ASSERT_EQUAL(kEmptyDataPacket.checksum, packet.checksum);
  packet.length = 10;  /* Invalid if data is NULL */
  TEST_ASSERT_FALSE(MidiFillDataPacketChecksum(&packet, kDataPacketDeviceId));

  TEST_ASSERT_FALSE(MidiVerifyDataPacketChecksum(
      NULL, kDataPacketDeviceId));
  TEST_ASSERT_FALSE(MidiVerifyDataPacketChecksum(
      &kEmptyDataPacket, kDataPacketDeviceId | 0x80));
  TEST_ASSERT_FALSE(MidiVerifyDataPacketChecksum(
      &packet, kDataPacketDeviceId));
  packet.length = 0;
  TEST_ASSERT_TRUE(MidiVerifyDataPacketChecksum(
      &packet, kDataPacketDeviceId));
  TEST_ASSERT_TRUE(MidiVerifyDataPacketChecksum(
      &kEmptyDataPacket, kDataPacketDeviceId));

  /* Test with good data */
  packet = kGoodDataPacket;
  packet.checksum = MIDI_NULL_CHECKSUM;
  TEST_ASSERT_TRUE(MidiFillDataPacketChecksum(
      &packet, kDataPacketDeviceId));
  TEST_ASSERT_EQUAL(kGoodDataPacket.checksum, packet.checksum);
  TEST_ASSERT_TRUE(MidiVerifyDataPacketChecksum(
      &packet, kDataPacketDeviceId));
  TEST_ASSERT_TRUE(MidiVerifyDataPacketChecksum(
      &kGoodDataPacket, kDataPacketDeviceId));
}

static void TestMidiDataPacket_SetData(void) {
  midi_data_packet_t packet;
  MidiInitializeDataPacket(&packet, 0x12);
  uint8_t buffer[MIDI_DATA_PACKET_DATA_LENGTH + 3];
  memset(buffer, 0x55, sizeof(buffer));

  TEST_ASSERT_FALSE(MidiSetDataPacketDataBuffer(
      NULL, buffer, MIDI_DATA_PACKET_DATA_LENGTH));
  TEST_ASSERT_FALSE(MidiSetDataPacketDataBuffer(
      &packet, NULL, MIDI_DATA_PACKET_DATA_LENGTH));
  TEST_ASSERT_FALSE(MidiSetDataPacketDataBuffer(
      &packet, buffer, MIDI_DATA_PACKET_DATA_LENGTH + 3)); /* No overrun. */
  packet.number = 0x92;
  TEST_ASSERT_FALSE(MidiSetDataPacketDataBuffer(
    &packet, buffer, MIDI_DATA_PACKET_DATA_LENGTH));
  packet.number = 0x12;
  buffer[50] = 0x80;
  TEST_ASSERT_FALSE(MidiSetDataPacketDataBuffer(
    &packet, buffer, MIDI_DATA_PACKET_DATA_LENGTH));
  buffer[50] = 0x55;

  TEST_ASSERT_TRUE(MidiSetDataPacketDataBuffer(
      &packet, buffer, MIDI_DATA_PACKET_DATA_LENGTH));
  TEST_ASSERT_EQUAL(((uint8_t*) buffer), packet.data);
  TEST_ASSERT_EQUAL(MIDI_DATA_PACKET_DATA_LENGTH, packet.length);
  TEST_ASSERT_EQUAL(MIDI_NULL_CHECKSUM, packet.checksum);

  TEST_ASSERT_TRUE(MidiSetDataPacketDataBuffer(
      &packet, &buffer[10], MIDI_DATA_PACKET_DATA_LENGTH - 10));
  TEST_ASSERT_EQUAL(((uint8_t*) (&buffer[10])), packet.data);
  TEST_ASSERT_EQUAL(MIDI_DATA_PACKET_DATA_LENGTH - 10, packet.length);
  TEST_ASSERT_EQUAL(MIDI_NULL_CHECKSUM, packet.checksum);
}

static void TestMidiDataPacket_Serialize(void) {
  static midi_device_id_t const kBadDeviceId = 0x80;
  uint8_t packet_data[MIDI_DATA_PACKET_PAYLOAD_SIZE];
  midi_data_packet_t bad_packet = kEmptyDataPacket;
  bad_packet.length = 22;
  /* Invalid inputs. */
  TEST_ASSERT_EQUAL(0, MidiSerializeDataPacket(
      NULL, NULL, packet_data, sizeof(packet_data)));
  TEST_ASSERT_EQUAL(0, MidiSerializeDataPacket(
      &kGoodDataPacket, NULL, NULL, sizeof(packet_data)));
  TEST_ASSERT_EQUAL(0, MidiSerializeDataPacket(
      &kGoodDataPacket, &kBadDeviceId, packet_data, sizeof(packet_data)));
  TEST_ASSERT_EQUAL(0, MidiSerializeDataPacket(
      &bad_packet, NULL, packet_data, sizeof(packet_data)));

  /* Incomplete serialization. */
  TEST_ASSERT_EQUAL(MIDI_DATA_PACKET_PAYLOAD_SIZE, MidiSerializeDataPacket(
      &kEmptyDataPacket, NULL, NULL, 0));
  TEST_ASSERT_EQUAL(MIDI_DATA_PACKET_PAYLOAD_SIZE, MidiSerializeDataPacket(
      &kEmptyDataPacket, NULL, packet_data, 0));
  TEST_ASSERT_EQUAL(MIDI_DATA_PACKET_PAYLOAD_SIZE, MidiSerializeDataPacket(
      &kEmptyDataPacket, NULL, packet_data, MIDI_DATA_PACKET_PAYLOAD_SIZE / 2));

  /* Successful serialization.  Empty data packet. */
  /* Using correct cached checksum */
  TEST_ASSERT_EQUAL(
      MIDI_DATA_PACKET_PAYLOAD_SIZE,
      MidiSerializeDataPacket(
          &kEmptyDataPacket, NULL, packet_data, sizeof(packet_data)));
  TEST_ASSERT_EQUAL_MEMORY(
      kEmptyDataPacketData, packet_data, MIDI_DATA_PACKET_PAYLOAD_SIZE);
  /* Using incorrect cached checksum (should succeed) */
  midi_data_packet_t packet = kEmptyDataPacket;
  packet.checksum = 0x40;
  TEST_ASSERT_EQUAL(
      MIDI_DATA_PACKET_PAYLOAD_SIZE,
      MidiSerializeDataPacket(
          &packet, NULL,
          packet_data, sizeof(packet_data)));
  TEST_ASSERT_EQUAL_MEMORY(
      kEmptyDataPacketData, packet_data, MIDI_DATA_PACKET_PAYLOAD_SIZE - 1);
  TEST_ASSERT_EQUAL(0x40, packet_data[MIDI_DATA_PACKET_PAYLOAD_SIZE - 1]);
  /* Calculate checksum. */
  packet.checksum = 0x70;
  TEST_ASSERT_EQUAL(
      MIDI_DATA_PACKET_PAYLOAD_SIZE,
      MidiSerializeDataPacket(
          &packet, &kDataPacketDeviceId,
          packet_data, sizeof(packet_data)));
  TEST_ASSERT_EQUAL_MEMORY(
      kEmptyDataPacketData, packet_data, MIDI_DATA_PACKET_PAYLOAD_SIZE);
  /* Ignore NULL checksum. */
  packet.checksum = MIDI_NULL_CHECKSUM;
  TEST_ASSERT_EQUAL(
      MIDI_DATA_PACKET_PAYLOAD_SIZE,
      MidiSerializeDataPacket(
          &packet, NULL,
          packet_data, sizeof(packet_data)));
  TEST_ASSERT_EQUAL(0x00, packet_data[MIDI_DATA_PACKET_PAYLOAD_SIZE - 1]);

  /* Successful serialization.  Good data packet. */
  /* Using correct cached checksum. */
  TEST_ASSERT_EQUAL(MIDI_DATA_PACKET_PAYLOAD_SIZE, MidiSerializeDataPacket(
      &kGoodDataPacket, NULL, packet_data, MIDI_DATA_PACKET_PAYLOAD_SIZE));
  TEST_ASSERT_EQUAL_MEMORY(
      kGoodDataPacketData, packet_data, MIDI_DATA_PACKET_PAYLOAD_SIZE);
  /* Using incorrect cached (should succeed). */
  packet = kGoodDataPacket;
  packet.checksum ^= 0x55;
  TEST_ASSERT_EQUAL(
      MIDI_DATA_PACKET_PAYLOAD_SIZE,
      MidiSerializeDataPacket(
          &packet, NULL, packet_data, sizeof(packet_data)));
  TEST_ASSERT_EQUAL_MEMORY(
      kGoodDataPacketData, packet_data, MIDI_DATA_PACKET_PAYLOAD_SIZE - 1);
  TEST_ASSERT_EQUAL(
      packet.checksum, packet_data[MIDI_DATA_PACKET_PAYLOAD_SIZE - 1]);
  /* Calculate checksum. */
  TEST_ASSERT_EQUAL(
      MIDI_DATA_PACKET_PAYLOAD_SIZE,
      MidiSerializeDataPacket(
          &packet, &kDataPacketDeviceId, packet_data, sizeof(packet_data)));
  TEST_ASSERT_EQUAL_MEMORY(
      kGoodDataPacketData, packet_data, MIDI_DATA_PACKET_PAYLOAD_SIZE);
}

static void TestMidiDataPacket_Deserialize(void) {
  midi_data_packet_t packet;

  /* Invalid inputs. */
  TEST_ASSERT_EQUAL(0, MidiDeserializeDataPacket(
      NULL, MIDI_DATA_PACKET_PAYLOAD_SIZE, &packet, NULL, 0u));
  TEST_ASSERT_EQUAL(0, MidiDeserializeDataPacket(
      kGoodDataPacketData, MIDI_DATA_PACKET_PAYLOAD_SIZE, NULL, NULL, 0u));
  TEST_ASSERT_EQUAL(0, MidiDeserializeDataPacket(
      kBadDataPacketData, MIDI_DATA_PACKET_PAYLOAD_SIZE, &packet, NULL, 0u));
  TEST_ASSERT_EQUAL(0, MidiDeserializeDataPacket(
      kGoodDataPacketData, MIDI_DATA_PACKET_PAYLOAD_SIZE,
      &packet, NULL, MIDI_DATA_PACKET_DATA_LENGTH));

  /* Incomplete deserialization. */
  TEST_ASSERT_EQUAL(MIDI_DATA_PACKET_PAYLOAD_SIZE, MidiDeserializeDataPacket(
      NULL, 0, &packet, NULL, 0u));
  TEST_ASSERT_EQUAL(MIDI_DATA_PACKET_PAYLOAD_SIZE, MidiDeserializeDataPacket(
      kGoodDataPacketData, 0, &packet, NULL, 0u));
  TEST_ASSERT_EQUAL(MIDI_DATA_PACKET_PAYLOAD_SIZE, MidiDeserializeDataPacket(
      kGoodDataPacketData, MIDI_DATA_PACKET_PAYLOAD_SIZE / 2,
      &packet, NULL, 0u));

  /* Successful deserialization. */
  /* No data buffers. */
  TEST_ASSERT_EQUAL(MIDI_DATA_PACKET_PAYLOAD_SIZE, MidiDeserializeDataPacket(
      kGoodDataPacketData, MIDI_DATA_PACKET_PAYLOAD_SIZE,
      &packet, NULL, 0u));
  TEST_ASSERT_EQUAL(kGoodDataPacket.number, packet.number);
  TEST_ASSERT_EQUAL(NULL, packet.data);
  TEST_ASSERT_EQUAL(0u, packet.length);
  TEST_ASSERT_EQUAL(kGoodDataPacket.checksum, packet.checksum);
  /* With global buffer. */
  midi_data_packet_buffer_t gbuffer;
  TEST_ASSERT_TRUE(MidiPushGlobalDataPacketBuffer(gbuffer, sizeof(gbuffer)));
  TEST_ASSERT_EQUAL(MIDI_DATA_PACKET_PAYLOAD_SIZE, MidiDeserializeDataPacket(
      kGoodDataPacketData, MIDI_DATA_PACKET_PAYLOAD_SIZE,
      &packet, NULL, 0u));
  TEST_ASSERT_EQUAL(kGoodDataPacket.number, packet.number);
  TEST_ASSERT_EQUAL(((uint8_t*) gbuffer), packet.data);
  TEST_ASSERT_EQUAL(MIDI_DATA_PACKET_DATA_LENGTH, packet.length);
  TEST_ASSERT_EQUAL_MEMORY(
      kGoodDataPacket.data, gbuffer, MIDI_DATA_PACKET_DATA_LENGTH);
  TEST_ASSERT_EQUAL(kGoodDataPacket.checksum, packet.checksum);
  /* With local buffers. */
  midi_data_packet_buffer_t lbuffer;
  TEST_ASSERT_EQUAL(MIDI_DATA_PACKET_PAYLOAD_SIZE, MidiDeserializeDataPacket(
      kGoodDataPacketData, MIDI_DATA_PACKET_PAYLOAD_SIZE,
      &packet, lbuffer, sizeof(lbuffer)));
  TEST_ASSERT_EQUAL(kGoodDataPacket.number, packet.number);
  TEST_ASSERT_EQUAL(((uint8_t*) lbuffer), packet.data);
  TEST_ASSERT_EQUAL(MIDI_DATA_PACKET_DATA_LENGTH, packet.length);
  TEST_ASSERT_EQUAL_MEMORY(
      kGoodDataPacket.data, lbuffer, MIDI_DATA_PACKET_DATA_LENGTH);
  TEST_ASSERT_EQUAL(kGoodDataPacket.checksum, packet.checksum);
  /* With small local buffer. */
  memset(lbuffer, 0x40, sizeof(lbuffer));
  TEST_ASSERT_EQUAL(MIDI_DATA_PACKET_PAYLOAD_SIZE, MidiDeserializeDataPacket(
      kGoodDataPacketData, MIDI_DATA_PACKET_PAYLOAD_SIZE,
      &packet, lbuffer, MIDI_DATA_PACKET_DATA_LENGTH / 2));
  midi_data_packet_buffer_t tbuffer;  /* Test buffer. */
  memset(tbuffer, 0x40, sizeof(tbuffer));
  memcpy(tbuffer, gGoodDataPacketBuffer, MIDI_DATA_PACKET_DATA_LENGTH / 2);
  TEST_ASSERT_EQUAL(kGoodDataPacket.number, packet.number);
  TEST_ASSERT_EQUAL(((uint8_t*) lbuffer), packet.data);
  TEST_ASSERT_EQUAL(MIDI_DATA_PACKET_DATA_LENGTH / 2, packet.length);
  TEST_ASSERT_EQUAL_MEMORY(
      tbuffer, lbuffer, MIDI_DATA_PACKET_DATA_LENGTH);
  TEST_ASSERT_EQUAL(kGoodDataPacket.checksum, packet.checksum);

  TEST_ASSERT_TRUE(MidiPushGlobalDataPacketBuffer(NULL, 0));
}

/* Sample Dump: Loop Points. */

static midi_sample_dump_t const kGoodSampleDumpRequest = {
  .sub_id = MIDI_SAMPLE_LOOP_REQUEST,
  .sample_number = 0x0410,
  .loop_number = 0x1004
};
static uint8_t const kGoodSampleDumpRequestData[MIDI_SAMPLE_LOOP_REQUEST_PAYLOAD_SIZE] = {
  MIDI_SAMPLE_LOOP_REQUEST,
  0x10, 0x08,
  0x04, 0x20
};
static midi_sample_dump_t const kGoodSampleDumpResponse = {
  .sub_id = MIDI_SAMPLE_LOOP_RESPONSE,
  .sample_number = 0x0410,
  .loop_number = 0x1004,

  .loop_type = MIDI_LOOP_FORWARD_ONLY,
  .loop_start_address = 0x00040000,
  .loop_end_address = 0x00080000
};
static uint8_t const kGoodSampleDumpResponseData[MIDI_SAMPLE_LOOP_RESPONSE_PAYLOAD_SIZE] = {
  MIDI_SAMPLE_LOOP_RESPONSE,
  0x10, 0x08,
  0x04, 0x20,
  MIDI_LOOP_FORWARD_ONLY,
  0x00, 0x00, 0x10,
  0x00, 0x00, 0x20
};

static midi_sample_dump_t const kBadSampleDumpRequest = {
  .sub_id = MIDI_SAMPLE_LOOP_REQUEST,
  .sample_number = 0x0410,
  .loop_number = 0x4004
};
static uint8_t const kBadSampleDumpRequestData[MIDI_SAMPLE_LOOP_REQUEST_PAYLOAD_SIZE] = {
  MIDI_SAMPLE_LOOP_REQUEST,
  0x10, 0x08,
  0x04, 0x80
};
static midi_sample_dump_t const kBadSampleDumpResponse = {
  .sub_id = MIDI_SAMPLE_LOOP_RESPONSE,
  .sample_number = 0x0410,
  .loop_number = 0x1004,

  .loop_type = MIDI_LOOP_OFF,
  .loop_start_address = 0x00400000,
  .loop_end_address = 0x00800000
};
static uint8_t const kBadSampleDumpResponseData[MIDI_SAMPLE_LOOP_RESPONSE_PAYLOAD_SIZE] = {
  MIDI_SAMPLE_LOOP_RESPONSE,
  0x10, 0x08,
  0x04, 0x20,
  MIDI_LOOP_FORWARD_ONLY + 0x03,
  0x00, 0x00, 0x10,
  0x00, 0x00, 0x20
};
static uint8_t const kBadSampleDumpData[MIDI_SAMPLE_LOOP_REQUEST_PAYLOAD_SIZE] = {
  0x05,  /* Not a valid sud ID */
  0x10, 0x08,
  0x04, 0x20
};

static void TestMidiSampleDump_Validator(void) {
  TEST_ASSERT_FALSE(MidiIsValidSampleDump(NULL));
  TEST_ASSERT_TRUE(MidiIsValidSampleDump(&kGoodSampleDumpRequest));
  TEST_ASSERT_TRUE(MidiIsValidSampleDump(&kGoodSampleDumpResponse));

  /* Bad Requests */
  midi_sample_dump_t bad_sample_dump = kBadSampleDumpRequest;
  TEST_ASSERT_FALSE(MidiIsValidSampleDump(&bad_sample_dump));
  bad_sample_dump.loop_number = 0x0001;
  bad_sample_dump.sample_number = 0x8000;
  TEST_ASSERT_FALSE(MidiIsValidSampleDump(&bad_sample_dump));

  /* Bad Response */
  bad_sample_dump = kBadSampleDumpResponse;
  TEST_ASSERT_FALSE(MidiIsValidSampleDump(&bad_sample_dump));
  bad_sample_dump.loop_start_address = 0x00000001;
  bad_sample_dump.loop_end_address = 0x00000001;
  TEST_ASSERT_TRUE(MidiIsValidSampleDump(&bad_sample_dump));
  bad_sample_dump.loop_type = 0x55;
  TEST_ASSERT_FALSE(MidiIsValidSampleDump(&bad_sample_dump));

  bad_sample_dump = kGoodSampleDumpResponse;
  bad_sample_dump.sub_id = 0x10;
  TEST_ASSERT_FALSE(MidiIsValidSampleDump(&bad_sample_dump));
}

static void TestMidiSampleDump_Initializer(void) {
  midi_sample_dump_t sample_dump = {
    .sub_id = 0x77
  };

  /* Requests */
  TEST_ASSERT_FALSE(MidiInitializeSampleDumpRequest(
      NULL, 0x0001, 0x0002));
  TEST_ASSERT_FALSE(MidiInitializeSampleDumpRequest(
      &sample_dump, 0x4001, 0x0002));
  TEST_ASSERT_FALSE(MidiInitializeSampleDumpRequest(
      &sample_dump, 0x0001, 0x4002));
  TEST_ASSERT_TRUE(MidiInitializeSampleDumpRequest(
      &sample_dump, 0x0020, 0x0120));
  TEST_ASSERT_EQUAL(MIDI_SAMPLE_LOOP_REQUEST, sample_dump.sub_id);
  TEST_ASSERT_EQUAL(0x0020, sample_dump.sample_number);
  TEST_ASSERT_EQUAL(0x0120, sample_dump.loop_number);

  /* Response */
  TEST_ASSERT_FALSE(MidiInitializeSampleDumpResponse(
      NULL, 0x0050, 0x0330, MIDI_LOOP_OFF,
      0x00010203, 0x00020304));
  TEST_ASSERT_FALSE(MidiInitializeSampleDumpResponse(
      &sample_dump, 0x4050, 0x0330, MIDI_LOOP_OFF,
      0x00010203, 0x00020304));
  TEST_ASSERT_FALSE(MidiInitializeSampleDumpResponse(
      &sample_dump, 0x0050, 0x4330, MIDI_LOOP_OFF,
      0x00010203, 0x00020304));
  TEST_ASSERT_FALSE(MidiInitializeSampleDumpResponse(
      &sample_dump, 0x0050, 0x0330, 0x5A,
      0x00010203, 0x00020304));
  TEST_ASSERT_FALSE(MidiInitializeSampleDumpResponse(
      &sample_dump, 0x0050, 0x0330, MIDI_LOOP_OFF,
      0x70010203, 0x00020304));
  TEST_ASSERT_FALSE(MidiInitializeSampleDumpResponse(
      &sample_dump, 0x0050, 0x0330, MIDI_LOOP_OFF,
      0x00010203, 0x40020304));

  TEST_ASSERT_TRUE(MidiInitializeSampleDumpResponse(
      &sample_dump, 0x0050, 0x0330, MIDI_LOOP_OFF,
      0x00010203, 0x00020304));
  TEST_ASSERT_EQUAL(MIDI_SAMPLE_LOOP_RESPONSE, sample_dump.sub_id);
  TEST_ASSERT_EQUAL(0x0050, sample_dump.sample_number);
  TEST_ASSERT_EQUAL(0x0330, sample_dump.loop_number);
  TEST_ASSERT_EQUAL(MIDI_LOOP_OFF, sample_dump.loop_type);
  TEST_ASSERT_EQUAL(0x00010203, sample_dump.loop_start_address);
  TEST_ASSERT_EQUAL(0x00020304, sample_dump.loop_end_address);
}

static void TestMidiSampleDump_Serialize(void) {
  uint8_t sample_dump_data[MIDI_SAMPLE_LOOP_RESPONSE_PAYLOAD_SIZE];
  /* Invalid inputs. */
  TEST_ASSERT_EQUAL(0, MidiSerializeSampleDump(
      NULL, sample_dump_data, sizeof(sample_dump_data)));
  TEST_ASSERT_EQUAL(0, MidiSerializeSampleDump(
      &kGoodSampleDumpRequest, NULL, sizeof(sample_dump_data)));
  TEST_ASSERT_EQUAL(0, MidiSerializeSampleDump(
      &kGoodSampleDumpResponse, NULL, sizeof(sample_dump_data)));
  TEST_ASSERT_EQUAL(0, MidiSerializeSampleDump(
      &kBadSampleDumpRequest, sample_dump_data, sizeof(sample_dump_data)));
  TEST_ASSERT_EQUAL(0, MidiSerializeSampleDump(
      &kBadSampleDumpResponse, sample_dump_data, sizeof(sample_dump_data)));

  /* Incomplete serialization. */
  TEST_ASSERT_EQUAL(
      MIDI_SAMPLE_LOOP_REQUEST_PAYLOAD_SIZE,
      MidiSerializeSampleDump(&kGoodSampleDumpRequest, NULL, 0));
  TEST_ASSERT_EQUAL(
      MIDI_SAMPLE_LOOP_REQUEST_PAYLOAD_SIZE,
      MidiSerializeSampleDump(&kGoodSampleDumpRequest, sample_dump_data, 0));
  TEST_ASSERT_EQUAL(
      MIDI_SAMPLE_LOOP_REQUEST_PAYLOAD_SIZE,
      MidiSerializeSampleDump(
          &kGoodSampleDumpRequest, sample_dump_data,
          MIDI_SAMPLE_LOOP_REQUEST_PAYLOAD_SIZE / 2));
  TEST_ASSERT_EQUAL(
      MIDI_SAMPLE_LOOP_RESPONSE_PAYLOAD_SIZE,
      MidiSerializeSampleDump(&kGoodSampleDumpResponse, NULL, 0));
  TEST_ASSERT_EQUAL(
      MIDI_SAMPLE_LOOP_RESPONSE_PAYLOAD_SIZE,
      MidiSerializeSampleDump(&kGoodSampleDumpResponse, sample_dump_data, 0));
  TEST_ASSERT_EQUAL(
      MIDI_SAMPLE_LOOP_RESPONSE_PAYLOAD_SIZE,
      MidiSerializeSampleDump(
          &kGoodSampleDumpResponse, sample_dump_data,
          MIDI_SAMPLE_LOOP_RESPONSE_PAYLOAD_SIZE / 2));

  /* Successful serialization. */
  TEST_ASSERT_EQUAL(
      MIDI_SAMPLE_LOOP_REQUEST_PAYLOAD_SIZE,
      MidiSerializeSampleDump(
          &kGoodSampleDumpRequest,
          sample_dump_data, sizeof(sample_dump_data)));
  TEST_ASSERT_EQUAL_MEMORY(
      kGoodSampleDumpRequestData, sample_dump_data,
      MIDI_SAMPLE_LOOP_REQUEST_PAYLOAD_SIZE);

  TEST_ASSERT_EQUAL(
      MIDI_SAMPLE_LOOP_RESPONSE_PAYLOAD_SIZE,
      MidiSerializeSampleDump(
          &kGoodSampleDumpResponse,
          sample_dump_data, sizeof(sample_dump_data)));
  TEST_ASSERT_EQUAL_MEMORY(
      kGoodSampleDumpResponseData, sample_dump_data,
      MIDI_SAMPLE_LOOP_RESPONSE_PAYLOAD_SIZE);
}

static void TestMidiSampleDump_Deserialize(void) {
  midi_sample_dump_t sample_dump;
  /* Invalid input */
  TEST_ASSERT_EQUAL(0, MidiDeserializeSampleDump(
      NULL, MIDI_SAMPLE_LOOP_RESPONSE_PAYLOAD_SIZE, &sample_dump));
  TEST_ASSERT_EQUAL(0, MidiDeserializeSampleDump(
      kGoodSampleDumpRequestData,
      MIDI_SAMPLE_LOOP_RESPONSE_PAYLOAD_SIZE, NULL));
  TEST_ASSERT_EQUAL(0, MidiDeserializeSampleDump(
      kBadSampleDumpRequestData, MIDI_SAMPLE_LOOP_RESPONSE_PAYLOAD_SIZE,
      &sample_dump));
  TEST_ASSERT_EQUAL(0, MidiDeserializeSampleDump(
      kBadSampleDumpResponseData, MIDI_SAMPLE_LOOP_RESPONSE_PAYLOAD_SIZE,
      &sample_dump));
  TEST_ASSERT_EQUAL(0, MidiDeserializeSampleDump(
      kBadSampleDumpData, MIDI_SAMPLE_LOOP_RESPONSE_PAYLOAD_SIZE,
      &sample_dump));

  /* Incomplete deserialization. */
  /* Because the size is variable, at least 1 byte is required to
   * determine the type. */
  TEST_ASSERT_EQUAL(1, MidiDeserializeSampleDump(NULL, 0, &sample_dump));
  TEST_ASSERT_EQUAL(1, MidiDeserializeSampleDump(
      kGoodSampleDumpRequestData, 0, &sample_dump));
  /* Partial requests */
  TEST_ASSERT_EQUAL(
      MIDI_SAMPLE_LOOP_REQUEST_PAYLOAD_SIZE,
      MidiDeserializeSampleDump(kGoodSampleDumpRequestData, 1, &sample_dump));
  TEST_ASSERT_EQUAL(
      MIDI_SAMPLE_LOOP_REQUEST_PAYLOAD_SIZE,
      MidiDeserializeSampleDump(
          kGoodSampleDumpRequestData,
          MIDI_SAMPLE_LOOP_REQUEST_PAYLOAD_SIZE / 2, &sample_dump));
  /* Partial responses */
  TEST_ASSERT_EQUAL(
      MIDI_SAMPLE_LOOP_RESPONSE_PAYLOAD_SIZE,
      MidiDeserializeSampleDump(kGoodSampleDumpResponseData, 1, &sample_dump));
  TEST_ASSERT_EQUAL(
      MIDI_SAMPLE_LOOP_RESPONSE_PAYLOAD_SIZE,
      MidiDeserializeSampleDump(
          kGoodSampleDumpResponseData,
          MIDI_SAMPLE_LOOP_RESPONSE_PAYLOAD_SIZE / 2, &sample_dump));

  /* Successful deserialization. */
  /* Request. */
  TEST_ASSERT_EQUAL(
      MIDI_SAMPLE_LOOP_REQUEST_PAYLOAD_SIZE,
      MidiDeserializeSampleDump(
          kGoodSampleDumpRequestData, MIDI_SAMPLE_LOOP_REQUEST_PAYLOAD_SIZE,
          &sample_dump));
  TEST_ASSERT_EQUAL(kGoodSampleDumpRequest.sub_id, sample_dump.sub_id);
  TEST_ASSERT_EQUAL(
      kGoodSampleDumpRequest.sample_number, sample_dump.sample_number);
  TEST_ASSERT_EQUAL(
      kGoodSampleDumpRequest.loop_number, sample_dump.loop_number);
  /* Response. */
  TEST_ASSERT_EQUAL(
      MIDI_SAMPLE_LOOP_RESPONSE_PAYLOAD_SIZE,
      MidiDeserializeSampleDump(
          kGoodSampleDumpResponseData, MIDI_SAMPLE_LOOP_RESPONSE_PAYLOAD_SIZE,
          &sample_dump));
  TEST_ASSERT_EQUAL(kGoodSampleDumpResponse.sub_id, sample_dump.sub_id);
  TEST_ASSERT_EQUAL(
      kGoodSampleDumpResponse.sample_number, sample_dump.sample_number);
  TEST_ASSERT_EQUAL(
      kGoodSampleDumpResponse.loop_number, sample_dump.loop_number);
  TEST_ASSERT_EQUAL(kGoodSampleDumpResponse.loop_type, sample_dump.loop_type);
  TEST_ASSERT_EQUAL(
      kGoodSampleDumpResponse.loop_start_address,
      sample_dump.loop_start_address);
  TEST_ASSERT_EQUAL(
      kGoodSampleDumpResponse.loop_end_address, sample_dump.loop_end_address);
}

/* Device Inquiry */

static uint8_t const kSmallDeviceInquiryManId[3] = {0x69, 0x00, 0x00};
static uint8_t const kBadDeviceInquiryManId[3] = {0x00, 0x13, 0xF7};
static uint8_t const kDeviceInquiryRev[4] = {0x4D, 0x49, 0x44, 0x49};

static midi_device_inquiry_t const kGoodDeviceInquiryRequest = {
  .sub_id = MIDI_DEVICE_INQUIRY_REQUEST
};
static uint8_t const kGoodDeviceInquiryRequestData[MIDI_DEVICE_INQUIRY_REQUEST_PAYLOAD_SIZE] = {
  MIDI_DEVICE_INQUIRY_REQUEST
};
static midi_device_inquiry_t const kGoodSmallDeviceInquiryResponse = {
  .sub_id = MIDI_DEVICE_INQUIRY_RESPONSE,
  .id = { 0x69, 0x00, 0x00 },
  .device_family_code = 0x0404,
  .device_family_member_code = 0x0107,
  .software_revision_level = { 0x4D, 0x49, 0x44, 0x49 }
};
static uint8_t const kGoodSmallDeviceInquiryResponseData[MIDI_DEVICE_INQUIRY_RESPONSE_SMALL_PAYLOAD_SIZE] = {
  MIDI_DEVICE_INQUIRY_RESPONSE,
  0x69,
  0x04, 0x08,
  0x07, 0x02,
  0x4D, 0x49, 0x44, 0x49
};
static midi_device_inquiry_t const kGoodLargeDeviceInquiryResponse = {
  .sub_id = MIDI_DEVICE_INQUIRY_RESPONSE,
  .id = { 0x00, 0x13, 0x37 },
  .device_family_code = 0x3077,
  .device_family_member_code = 0x1081,
  .software_revision_level = { 0x4D, 0x49, 0x44, 0x49 }
};
static uint8_t const kGoodLargeDeviceInquiryResponseData[MIDI_DEVICE_INQUIRY_RESPONSE_LARGE_PAYLOAD_SIZE] = {
  MIDI_DEVICE_INQUIRY_RESPONSE,
  0x00, 0x13, 0x37,
  0x77, 0x60,
  0x01, 0x21,
  0x4D, 0x49, 0x44, 0x49
};

static midi_device_inquiry_t const kBadDeviceInquiry = {
  .sub_id = 0x06
};
static uint8_t const kBadDeviceInquiryData[MIDI_DEVICE_INQUIRY_REQUEST_PAYLOAD_SIZE] = {
  0x07
};
static midi_device_inquiry_t const kBadSmallDeviceInquiryResponse = {
  .sub_id = MIDI_DEVICE_INQUIRY_RESPONSE,
  .id = { 0x69, 0x00, 0x00 },
  .device_family_code = 0x0404,
  .device_family_member_code = 0x0107,
  .software_revision_level = { 0x4D, 0x49, 0xB4, 0x49 }
};
static uint8_t const kBadSmallDeviceInquiryResponseData[MIDI_DEVICE_INQUIRY_RESPONSE_SMALL_PAYLOAD_SIZE] = {
  MIDI_DEVICE_INQUIRY_RESPONSE,
  0x69,
  0x04, 0x08,
  0x07, 0x02,
  0x4D, 0x49, 0xB4, 0x49
};
static midi_device_inquiry_t const kBadLargeDeviceInquiryResponse = {
  .sub_id = MIDI_DEVICE_INQUIRY_RESPONSE,
  .id = { 0x00, 0x13, 0x37 },
  .device_family_code = 0x7077,
  .device_family_member_code = 0x1081,
  .software_revision_level = { 0x4D, 0x49, 0x44, 0x49 }
};
static uint8_t const kBadLargeDeviceInquiryResponseDataOne[MIDI_DEVICE_INQUIRY_RESPONSE_LARGE_PAYLOAD_SIZE] = {
  MIDI_DEVICE_INQUIRY_RESPONSE,
  0x00, 0x13, 0x37,
  0x77, 0x60,
  0x81, 0x21,
  0x4D, 0x49, 0x44, 0x49
};
static uint8_t const kBadLargeDeviceInquiryResponseDataTwo[MIDI_DEVICE_INQUIRY_RESPONSE_LARGE_PAYLOAD_SIZE] = {
  MIDI_DEVICE_INQUIRY_RESPONSE,
  0x00, 0x13, 0x87,
  0x77, 0x60,
  0x81, 0x21,
  0x4D, 0x49, 0x44, 0x49
};

static void TestMidiDeviceInquiry_Validator(void) {
  TEST_ASSERT_FALSE(MidiIsValidDeviceInquiry(NULL));
  TEST_ASSERT_TRUE(MidiIsValidDeviceInquiry(&kGoodDeviceInquiryRequest));
  TEST_ASSERT_TRUE(MidiIsValidDeviceInquiry(&kGoodSmallDeviceInquiryResponse));
  TEST_ASSERT_TRUE(MidiIsValidDeviceInquiry(&kGoodLargeDeviceInquiryResponse));

  TEST_ASSERT_FALSE(MidiIsValidDeviceInquiry(&kBadDeviceInquiry));

  /* Bad small responses. */
  midi_device_inquiry_t bad_device_inquiry = kBadSmallDeviceInquiryResponse;
  TEST_ASSERT_FALSE(MidiIsValidDeviceInquiry(&bad_device_inquiry));
  memset(bad_device_inquiry.software_revision_level, 0,
         MIDI_SOFTWARE_REVISION_SIZE);
  TEST_ASSERT_TRUE(MidiIsValidDeviceInquiry(&bad_device_inquiry));
  bad_device_inquiry.id[2] = 0x8F;
  TEST_ASSERT_FALSE(MidiIsValidDeviceInquiry(&bad_device_inquiry));

  /* Bad Response */
  bad_device_inquiry = kBadLargeDeviceInquiryResponse;
  TEST_ASSERT_FALSE(MidiIsValidDeviceInquiry(&bad_device_inquiry));
  bad_device_inquiry.device_family_code = 0x0000;
  TEST_ASSERT_TRUE(MidiIsValidDeviceInquiry(&bad_device_inquiry));
}

static void TestMidiDeviceInquiry_Initializer(void) {
  midi_device_inquiry_t device_inquiry = {
    .sub_id = 0x77
  };

  /* Requests */
  TEST_ASSERT_FALSE(MidiInitializeDeviceInquiryRequest(NULL));
  TEST_ASSERT_TRUE(MidiInitializeDeviceInquiryRequest(&device_inquiry));
  TEST_ASSERT_EQUAL(MIDI_DEVICE_INQUIRY_REQUEST, device_inquiry.sub_id);

  /* Response */
  TEST_ASSERT_FALSE(MidiInitializeDeviceInquiryResponse(
      NULL, kSmallDeviceInquiryManId, 0x0050, 0x2330, kDeviceInquiryRev));
  TEST_ASSERT_FALSE(MidiInitializeDeviceInquiryResponse(
      &device_inquiry, NULL, 0x0050, 0x2330, kDeviceInquiryRev));
  TEST_ASSERT_FALSE(MidiInitializeDeviceInquiryResponse(
      &device_inquiry, kBadDeviceInquiryManId, 0x0050, 0x2330,
      kDeviceInquiryRev));
  TEST_ASSERT_FALSE(MidiInitializeDeviceInquiryResponse(
      &device_inquiry, kSmallDeviceInquiryManId, 0x7050, 0x2330,
      kDeviceInquiryRev));
  TEST_ASSERT_FALSE(MidiInitializeDeviceInquiryResponse(
      &device_inquiry, kSmallDeviceInquiryManId, 0x0050, 0x8330,
      kDeviceInquiryRev));
  TEST_ASSERT_FALSE(MidiInitializeDeviceInquiryResponse(
      &device_inquiry, kSmallDeviceInquiryManId, 0x0050, 0x2330, NULL));

  TEST_ASSERT_TRUE(MidiInitializeDeviceInquiryResponse(
      &device_inquiry, kSmallDeviceInquiryManId, 0x0050, 0x2330,
      kDeviceInquiryRev));
  TEST_ASSERT_EQUAL(MIDI_SAMPLE_LOOP_RESPONSE, device_inquiry.sub_id);
  TEST_ASSERT_EQUAL_MEMORY(
      kSmallDeviceInquiryManId, device_inquiry.id,
      sizeof(midi_manufacturer_id_t));
  TEST_ASSERT_EQUAL(0x0050, device_inquiry.device_family_code);
  TEST_ASSERT_EQUAL(0x2330, device_inquiry.device_family_member_code);
  TEST_ASSERT_EQUAL_MEMORY(
      kDeviceInquiryRev, device_inquiry.software_revision_level,
      MIDI_SOFTWARE_REVISION_SIZE);
}

static void TestMidiDeviceInquiry_Serialize(void) {
  uint8_t device_inquiry_data[MIDI_DEVICE_INQUIRY_RESPONSE_LARGE_PAYLOAD_SIZE];
  /* Invalid inputs. */
  TEST_ASSERT_EQUAL(0, MidiSerializeDeviceInquiry(
      NULL, device_inquiry_data, sizeof(device_inquiry_data)));
  TEST_ASSERT_EQUAL(0, MidiSerializeDeviceInquiry(
      &kGoodDeviceInquiryRequest, NULL, sizeof(device_inquiry_data)));
  TEST_ASSERT_EQUAL(0, MidiSerializeDeviceInquiry(
      &kBadDeviceInquiry, device_inquiry_data, sizeof(device_inquiry_data)));
  TEST_ASSERT_EQUAL(0, MidiSerializeDeviceInquiry(
      &kBadSmallDeviceInquiryResponse, device_inquiry_data,
      sizeof(device_inquiry_data)));
  TEST_ASSERT_EQUAL(0, MidiSerializeDeviceInquiry(
      &kBadLargeDeviceInquiryResponse, device_inquiry_data,
      sizeof(device_inquiry_data)));

  /* Incomplete serialization. */
  TEST_ASSERT_EQUAL(
      MIDI_DEVICE_INQUIRY_REQUEST_PAYLOAD_SIZE,
      MidiSerializeDeviceInquiry(
          &kGoodDeviceInquiryRequest, NULL, 0));
  TEST_ASSERT_EQUAL(
      MIDI_DEVICE_INQUIRY_REQUEST_PAYLOAD_SIZE,
      MidiSerializeDeviceInquiry(
          &kGoodDeviceInquiryRequest, device_inquiry_data, 0));
  TEST_ASSERT_EQUAL(
      MIDI_DEVICE_INQUIRY_RESPONSE_SMALL_PAYLOAD_SIZE,
      MidiSerializeDeviceInquiry(
          &kGoodSmallDeviceInquiryResponse, NULL, 0));
  TEST_ASSERT_EQUAL(
      MIDI_DEVICE_INQUIRY_RESPONSE_SMALL_PAYLOAD_SIZE,
      MidiSerializeDeviceInquiry(
          &kGoodSmallDeviceInquiryResponse, device_inquiry_data, 0));
  TEST_ASSERT_EQUAL(
      MIDI_DEVICE_INQUIRY_RESPONSE_SMALL_PAYLOAD_SIZE,
      MidiSerializeDeviceInquiry(
          &kGoodSmallDeviceInquiryResponse, device_inquiry_data,
          MIDI_DEVICE_INQUIRY_RESPONSE_SMALL_PAYLOAD_SIZE / 2));
  TEST_ASSERT_EQUAL(
      MIDI_DEVICE_INQUIRY_RESPONSE_LARGE_PAYLOAD_SIZE,
      MidiSerializeDeviceInquiry(
          &kGoodLargeDeviceInquiryResponse, NULL, 0));
  TEST_ASSERT_EQUAL(
      MIDI_DEVICE_INQUIRY_RESPONSE_LARGE_PAYLOAD_SIZE,
      MidiSerializeDeviceInquiry(
          &kGoodLargeDeviceInquiryResponse, device_inquiry_data, 0));
  TEST_ASSERT_EQUAL(
      MIDI_DEVICE_INQUIRY_RESPONSE_LARGE_PAYLOAD_SIZE,
      MidiSerializeDeviceInquiry(
          &kGoodLargeDeviceInquiryResponse, device_inquiry_data,
          MIDI_DEVICE_INQUIRY_RESPONSE_LARGE_PAYLOAD_SIZE / 2));

  /* Successful serialization. */
  /* Request. */
  TEST_ASSERT_EQUAL(
      MIDI_DEVICE_INQUIRY_REQUEST_PAYLOAD_SIZE,
      MidiSerializeDeviceInquiry(
          &kGoodDeviceInquiryRequest,
          device_inquiry_data, sizeof(device_inquiry_data)));
  TEST_ASSERT_EQUAL_MEMORY(
      kGoodDeviceInquiryRequestData, device_inquiry_data,
      MIDI_DEVICE_INQUIRY_REQUEST_PAYLOAD_SIZE);
  /* Small response. */
  TEST_ASSERT_EQUAL(
      MIDI_DEVICE_INQUIRY_RESPONSE_SMALL_PAYLOAD_SIZE,
      MidiSerializeDeviceInquiry(
          &kGoodSmallDeviceInquiryResponse,
          device_inquiry_data, sizeof(device_inquiry_data)));
  TEST_ASSERT_EQUAL_MEMORY(
      kGoodSmallDeviceInquiryResponseData, device_inquiry_data,
      MIDI_DEVICE_INQUIRY_RESPONSE_SMALL_PAYLOAD_SIZE);
  /* Large response. */
  TEST_ASSERT_EQUAL(
      MIDI_DEVICE_INQUIRY_RESPONSE_LARGE_PAYLOAD_SIZE,
      MidiSerializeDeviceInquiry(
          &kGoodLargeDeviceInquiryResponse,
          device_inquiry_data, sizeof(device_inquiry_data)));
  TEST_ASSERT_EQUAL_MEMORY(
      kGoodLargeDeviceInquiryResponseData, device_inquiry_data,
      MIDI_DEVICE_INQUIRY_RESPONSE_LARGE_PAYLOAD_SIZE);
}

static void TestMidiDeviceInquiry_Deserialize(void) {
  midi_device_inquiry_t device_inquiry;
  /* Invalid input */
  TEST_ASSERT_EQUAL(0, MidiDeserializeDeviceInquiry(
      NULL, MIDI_DEVICE_INQUIRY_REQUEST_PAYLOAD_SIZE, &device_inquiry));
  TEST_ASSERT_EQUAL(0, MidiDeserializeDeviceInquiry(
      kGoodDeviceInquiryRequestData,
      MIDI_DEVICE_INQUIRY_REQUEST_PAYLOAD_SIZE, NULL));
  TEST_ASSERT_EQUAL(0, MidiDeserializeDeviceInquiry(
      kGoodSmallDeviceInquiryResponseData,
      MIDI_DEVICE_INQUIRY_RESPONSE_SMALL_PAYLOAD_SIZE, NULL));
  TEST_ASSERT_EQUAL(0, MidiDeserializeDeviceInquiry(
      kGoodLargeDeviceInquiryResponseData,
      MIDI_DEVICE_INQUIRY_RESPONSE_LARGE_PAYLOAD_SIZE, NULL));
  TEST_ASSERT_EQUAL(0, MidiDeserializeDeviceInquiry(
      kBadDeviceInquiryData, MIDI_DEVICE_INQUIRY_REQUEST_PAYLOAD_SIZE,
      &device_inquiry));
  TEST_ASSERT_EQUAL(0, MidiDeserializeDeviceInquiry(
      kBadSmallDeviceInquiryResponseData,
      MIDI_DEVICE_INQUIRY_RESPONSE_SMALL_PAYLOAD_SIZE, &device_inquiry));
  TEST_ASSERT_EQUAL(0, MidiDeserializeDeviceInquiry(
      kBadLargeDeviceInquiryResponseDataOne,
      MIDI_DEVICE_INQUIRY_RESPONSE_LARGE_PAYLOAD_SIZE, &device_inquiry));
  TEST_ASSERT_EQUAL(0, MidiDeserializeDeviceInquiry(
      kBadLargeDeviceInquiryResponseDataTwo,
      MIDI_DEVICE_INQUIRY_RESPONSE_LARGE_PAYLOAD_SIZE, &device_inquiry));

  /* Incomplete deserialization. */
  /* Variable sized, requires a sub ID (1 byte) to determine if message
   * is a request or a response. */
  TEST_ASSERT_EQUAL(1, MidiDeserializeDeviceInquiry(
      NULL, 0, &device_inquiry));
  TEST_ASSERT_EQUAL(1, MidiDeserializeDeviceInquiry(
      kGoodDeviceInquiryRequestData, 0, &device_inquiry));
  TEST_ASSERT_EQUAL(1, MidiDeserializeDeviceInquiry(
      kGoodSmallDeviceInquiryResponseData, 0, &device_inquiry));
  /* Skip testing requests as they are only one byte. */
  /* Responses can be different lengths depending on the first byte
   * of the contained manufacturer ID (byte 2) */
  TEST_ASSERT_EQUAL(2, MidiDeserializeDeviceInquiry(
      kGoodSmallDeviceInquiryResponseData, 1, &device_inquiry));
  TEST_ASSERT_EQUAL(2, MidiDeserializeDeviceInquiry(
      kGoodLargeDeviceInquiryResponseData, 1, &device_inquiry));
  TEST_ASSERT_EQUAL(
      MIDI_DEVICE_INQUIRY_RESPONSE_SMALL_PAYLOAD_SIZE,
      MidiDeserializeDeviceInquiry(
          kGoodSmallDeviceInquiryResponseData, 2, &device_inquiry));
  TEST_ASSERT_EQUAL(
      MIDI_DEVICE_INQUIRY_RESPONSE_LARGE_PAYLOAD_SIZE,
      MidiDeserializeDeviceInquiry(
          kGoodLargeDeviceInquiryResponseData, 2, &device_inquiry));

  /* Successful deserialization. */
  /* Request. */
  TEST_ASSERT_EQUAL(
      MIDI_DEVICE_INQUIRY_REQUEST_PAYLOAD_SIZE,
      MidiDeserializeDeviceInquiry(
          kGoodDeviceInquiryRequestData,
          MIDI_DEVICE_INQUIRY_REQUEST_PAYLOAD_SIZE, &device_inquiry));
  TEST_ASSERT_EQUAL(kGoodDeviceInquiryRequest.sub_id, device_inquiry.sub_id);
  /* Small response. */
  TEST_ASSERT_EQUAL(
      MIDI_DEVICE_INQUIRY_RESPONSE_SMALL_PAYLOAD_SIZE,
      MidiDeserializeDeviceInquiry(
          kGoodSmallDeviceInquiryResponseData,
          MIDI_DEVICE_INQUIRY_RESPONSE_SMALL_PAYLOAD_SIZE, &device_inquiry));
  TEST_ASSERT_EQUAL(
      kGoodSmallDeviceInquiryResponse.sub_id, device_inquiry.sub_id);
  TEST_ASSERT_EQUAL_MEMORY(
      kGoodSmallDeviceInquiryResponse.id, device_inquiry.id,
      sizeof(midi_manufacturer_id_t));
  TEST_ASSERT_EQUAL(
      kGoodSmallDeviceInquiryResponse.device_family_code,
      device_inquiry.device_family_code);
  TEST_ASSERT_EQUAL(
      kGoodSmallDeviceInquiryResponse.device_family_member_code,
      device_inquiry.device_family_member_code);
  TEST_ASSERT_EQUAL_MEMORY(
      kGoodSmallDeviceInquiryResponse.software_revision_level,
      device_inquiry.software_revision_level, MIDI_SOFTWARE_REVISION_SIZE);
  /* Large response. */
  TEST_ASSERT_EQUAL(
      MIDI_DEVICE_INQUIRY_RESPONSE_LARGE_PAYLOAD_SIZE,
      MidiDeserializeDeviceInquiry(
          kGoodLargeDeviceInquiryResponseData,
          MIDI_DEVICE_INQUIRY_RESPONSE_LARGE_PAYLOAD_SIZE, &device_inquiry));
  TEST_ASSERT_EQUAL(kGoodSmallDeviceInquiryResponse.sub_id, device_inquiry.sub_id);
  TEST_ASSERT_EQUAL_MEMORY(
      kGoodLargeDeviceInquiryResponse.id, device_inquiry.id,
      sizeof(midi_manufacturer_id_t));
  TEST_ASSERT_EQUAL(
      kGoodLargeDeviceInquiryResponse.device_family_code,
      device_inquiry.device_family_code);
  TEST_ASSERT_EQUAL(
      kGoodLargeDeviceInquiryResponse.device_family_member_code,
      device_inquiry.device_family_member_code);
  TEST_ASSERT_EQUAL_MEMORY(
      kGoodLargeDeviceInquiryResponse.software_revision_level,
      device_inquiry.software_revision_level, MIDI_SOFTWARE_REVISION_SIZE);
}

/* General MIDI Mode */

static void TestMidiGeneralMidi_Validator(void) {
  TEST_ASSERT_FALSE(MidiIsValidGeneralMidiMode(0x00))
  TEST_ASSERT_TRUE(MidiIsValidGeneralMidiMode(MIDI_GENERAL_MIDI_OFF));
  TEST_ASSERT_TRUE(MidiIsValidGeneralMidiMode(MIDI_GENERAL_MIDI_ON));
  TEST_ASSERT_FALSE(MidiIsValidGeneralMidiMode(0x10));
  TEST_ASSERT_FALSE(MidiIsValidGeneralMidiMode(0x7F));
  TEST_ASSERT_FALSE(MidiIsValidGeneralMidiMode(0x80));
}

/* Device Control */
static midi_device_control_t const kDeviceControlVolume = {
  .sub_id = MIDI_MASTER_VOLUME,
  .volume = 0x0765
};
static uint8_t const kDeviceControlVolumeData[MIDI_DEVICE_CONTROL_PAYLOAD_SIZE] = {
  MIDI_MASTER_VOLUME, 0x65, 0x0E
};

static midi_device_control_t const kDeviceControlBalance = {
  .sub_id = MIDI_MASTER_BALANCE,
  .balance = 0x3FFF
};
static uint8_t const kDeviceControlBalanceData[MIDI_DEVICE_CONTROL_PAYLOAD_SIZE] = {
  MIDI_MASTER_BALANCE, 0x7F, 0x7F
};

static midi_device_control_t const kInvalidDeviceControl = {
  .sub_id = 0x16,
  .volume = 0x0010
};
static uint8_t const kInvalidDeviceControlData[MIDI_DEVICE_CONTROL_PAYLOAD_SIZE] = {
  0x16, 0x10, 0x00
};

static void TestMidiDeviceControl_Validator(void) {
  TEST_ASSERT_FALSE(MidiIsValidDeviceControl(NULL));
  TEST_ASSERT_FALSE(MidiIsValidDeviceControl(&kInvalidDeviceControl));

  TEST_ASSERT_TRUE(MidiIsValidDeviceControl(&kDeviceControlVolume));
  TEST_ASSERT_TRUE(MidiIsValidDeviceControl(&kDeviceControlBalance));
}

static void TestMidiDeviceControl_Initializer(void) {
  midi_device_control_t control;
  TEST_ASSERT_FALSE(MidiInitializeDeviceControl(
      NULL, MIDI_MASTER_VOLUME, 0x1000));
  TEST_ASSERT_FALSE(MidiInitializeDeviceControl(
      &control, 0x16, 0x1000));
  TEST_ASSERT_FALSE(MidiInitializeDeviceControl(
      &control, MIDI_MASTER_VOLUME, 0x4000));

  TEST_ASSERT_TRUE(MidiInitializeDeviceControl(
      &control, MIDI_MASTER_VOLUME, 0x1000));
  TEST_ASSERT_EQUAL(MIDI_MASTER_VOLUME, control.sub_id);
  TEST_ASSERT_EQUAL(0x1000, control.volume);

  TEST_ASSERT_TRUE(MidiInitializeDeviceControl(
      &control, MIDI_MASTER_BALANCE, 0x2000));
  TEST_ASSERT_EQUAL(MIDI_MASTER_BALANCE, control.sub_id);
  TEST_ASSERT_EQUAL(0x2000, control.balance);
}

static void TestMidiDeviceControl_Serialize(void) {
  uint8_t control_data[MIDI_DEVICE_CONTROL_PAYLOAD_SIZE + 1];
  control_data[MIDI_DEVICE_CONTROL_PAYLOAD_SIZE] = 0xE5;

  /* Invalid input. */
  TEST_ASSERT_EQUAL(0, MidiSerializeDeviceControl(
      NULL, control_data, sizeof(control_data)));
  TEST_ASSERT_EQUAL(0, MidiSerializeDeviceControl(
      &kDeviceControlVolume, NULL, sizeof(control_data)));
  TEST_ASSERT_EQUAL(0, MidiSerializeDeviceControl(
      &kInvalidDeviceControl, control_data, sizeof(control_data)));

  /* Partial serialization. */
  TEST_ASSERT_EQUAL(
      MIDI_DEVICE_CONTROL_PAYLOAD_SIZE,
      MidiSerializeDeviceControl(&kDeviceControlVolume, NULL, 0));
  TEST_ASSERT_EQUAL(
      MIDI_DEVICE_CONTROL_PAYLOAD_SIZE,
      MidiSerializeDeviceControl(&kDeviceControlVolume, control_data, 1));
  TEST_ASSERT_EQUAL(
      MIDI_DEVICE_CONTROL_PAYLOAD_SIZE,
      MidiSerializeDeviceControl(&kDeviceControlBalance, control_data, 1));

  /* Successful serialization */
  TEST_ASSERT_EQUAL(
      MIDI_DEVICE_CONTROL_PAYLOAD_SIZE,
      MidiSerializeDeviceControl(
          &kDeviceControlVolume, control_data, sizeof(control_data)));
  TEST_ASSERT_EQUAL_MEMORY(
      kDeviceControlVolumeData, control_data,
      sizeof(kDeviceControlVolumeData));

  TEST_ASSERT_EQUAL(
      MIDI_DEVICE_CONTROL_PAYLOAD_SIZE,
      MidiSerializeDeviceControl(
          &kDeviceControlBalance, control_data, sizeof(control_data)));
  TEST_ASSERT_EQUAL_MEMORY(
      kDeviceControlBalanceData, control_data,
      sizeof(kDeviceControlBalanceData));

  /* Overrun check. */
  TEST_ASSERT_EQUAL(0xE5, control_data[MIDI_DEVICE_CONTROL_PAYLOAD_SIZE]);
}

static void TestMidiDeviceControl_Deserialize(void) {
  midi_device_control_t control;

  /* Invalid input. */
  TEST_ASSERT_EQUAL(0, MidiDeserializeDeviceControl(
      NULL, sizeof(kDeviceControlVolumeData), &control));
  TEST_ASSERT_EQUAL(0, MidiDeserializeDeviceControl(
      kDeviceControlVolumeData, sizeof(kDeviceControlVolumeData), NULL));
  TEST_ASSERT_EQUAL(0, MidiDeserializeDeviceControl(
      kInvalidDeviceControlData, sizeof(kInvalidDeviceControlData), &control));

  /* Partial deserialization. */
  TEST_ASSERT_EQUAL(
      MIDI_DEVICE_CONTROL_PAYLOAD_SIZE,
      MidiDeserializeDeviceControl(NULL, 0, &control));
  TEST_ASSERT_EQUAL(
      MIDI_DEVICE_CONTROL_PAYLOAD_SIZE,
      MidiDeserializeDeviceControl(kDeviceControlVolumeData, 1, &control));

  /* Successful deserialization. */
  TEST_ASSERT_EQUAL(
      MIDI_DEVICE_CONTROL_PAYLOAD_SIZE,
      MidiDeserializeDeviceControl(
          kDeviceControlVolumeData, sizeof(kDeviceControlVolumeData),
          &control));
  TEST_ASSERT_EQUAL(kDeviceControlVolume.sub_id, control.sub_id);
  TEST_ASSERT_EQUAL(kDeviceControlVolume.volume, control.volume);

  TEST_ASSERT_EQUAL(
      MIDI_DEVICE_CONTROL_PAYLOAD_SIZE,
      MidiDeserializeDeviceControl(
          kDeviceControlBalanceData, sizeof(kDeviceControlBalanceData),
          &control));
  TEST_ASSERT_EQUAL(kDeviceControlBalance.sub_id, control.sub_id);
  TEST_ASSERT_EQUAL(kDeviceControlBalance.volume, control.volume);
}

void MidiSystemUniversalTest(void) {
  RUN_TEST(TestMidiDeviceId_Validator);
  RUN_TEST(TestMidiHandShakeId_Validator);

  RUN_TEST(TestMidiDumpHeader_Validator);
  RUN_TEST(TestMidiDumpHeader_Serialize);
  RUN_TEST(TestMidiDumpHeader_Deserialize);

  RUN_TEST(TestMidiDumpRequest_Validator);
  RUN_TEST(TestMidiDumpRequest_Serialize);
  RUN_TEST(TestMidiDumpRequest_Deserialize);

  RUN_TEST(TestMidiGlobalDataPacketBuffer);
  RUN_TEST(TestMidiDataPacket_Validator);
  RUN_TEST(TestMidiDataPacket_Initialize);
  RUN_TEST(TestMidiDataPacket_Checksum);
  RUN_TEST(TestMidiDataPacket_SetData);
  RUN_TEST(TestMidiDataPacket_Serialize);
  RUN_TEST(TestMidiDataPacket_Deserialize);

  RUN_TEST(TestMidiSampleDump_Validator);
  RUN_TEST(TestMidiSampleDump_Initializer);
  RUN_TEST(TestMidiSampleDump_Serialize);
  RUN_TEST(TestMidiSampleDump_Deserialize);

  RUN_TEST(TestMidiDeviceInquiry_Validator);
  RUN_TEST(TestMidiDeviceInquiry_Initializer);
  RUN_TEST(TestMidiDeviceInquiry_Serialize);
  RUN_TEST(TestMidiDeviceInquiry_Deserialize);

  RUN_TEST(TestMidiGeneralMidi_Validator);

  RUN_TEST(TestMidiDeviceControl_Validator);
  RUN_TEST(TestMidiDeviceControl_Initializer);
  RUN_TEST(TestMidiDeviceControl_Serialize);
  RUN_TEST(TestMidiDeviceControl_Deserialize);
}
