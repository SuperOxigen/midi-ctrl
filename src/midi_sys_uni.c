/*
 * MIDI Controller - MIDI System Universal.
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#include <string.h>

#include "midi_defs.h"
#include "midi_sys_uni.h"

/* Data Packet Buffers */
#define MIDI_MAX_DATA_PACKET_BUFFERS 4
static uint8_t *gBufferTable[MIDI_MAX_DATA_PACKET_BUFFERS];
static uint8_t gBufferTableSize = 0;
static uint8_t gBufferTableIdx = 0;

bool_t MidiPushGlobalDataPacketBuffer(uint8_t *buffer, size_t buffer_size) {
  if (buffer == NULL) {
    /* Clear table on null. */
    if (buffer_size > 0) return false;
    memset(gBufferTable, 0, sizeof(gBufferTable));
    gBufferTableSize = 0;
    gBufferTableIdx = 0;
    return true;
  }
  /* Data too small. */
  if (buffer_size < MIDI_DATA_PACKET_DATA_LENGTH) return false;
  /* Table full. */
  if (gBufferTableSize >= MIDI_MAX_DATA_PACKET_BUFFERS) return false;
  gBufferTable[gBufferTableSize++] = buffer;
  return true;
}

uint8_t *MidiGetGlobalDataPacketBuffer(void) {
  if (gBufferTableSize == 0) return NULL;
  uint8_t *buffer = gBufferTable[gBufferTableIdx++];
  gBufferTableIdx %= gBufferTableSize;
  return buffer;
}

/*
 * Handshake Message.
 *  Format: pp
 *    pp - Packet number (ignored for EOF)
 */

bool_t MidiIsHandShakeSubId(uint8_t sub_id) {
  switch (sub_id) {
    case MIDI_EOF:
    case MIDI_WAIT:
    case MIDI_CANCEL:
    case MIDI_NAK:
    case MIDI_ACK:
      return true;
  }
  return false;
}

/*
 * Sample Dump Header Message.
 *  Format:  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
 *          ss|ss|ee|ff|ff|ff|gg|gg|gg|hh|hh|hh|ii|ii|ii|jj
 *
 *    ss ss    - Sample number
 *    ee       - Sample format, range of [8,28]
 *    ff ff ff - Sample period, in nanoseconds
 *    gg gg gg - Sample length, in bytes
 *    hh hh hh - Sustain loop start point, in byte number
 *    ii ii ii - Sustain loop end point, in byte number
 *    jj       - Loop type
 */

#define MidiIsValidSampleFormat(format) ((format) >= 8 && ((format) <= 28))

#define MidiIsValidLoopType(loop_type) \
    ((loop_type) == MIDI_LOOP_FORWARD_ONLY || \
     (loop_type) == MIDI_LOOP_BIDIRECTIONAL || \
     (loop_type) == MIDI_LOOP_OFF)

bool_t MidiIsValidDumpHeader(midi_dump_header_t const *header) {
  if (header == NULL) return false;
  return MidiIsDataWord(header->sample_number)
      && MidiIsValidSampleFormat(header->sample_format)
      && MidiIsDataTriByte(header->sample_period)
      && MidiIsDataTriByte(header->sample_length)
      && MidiIsDataTriByte(header->sustain_loop_start_point)
      && MidiIsDataTriByte(header->sustain_loop_end_point)
      && MidiIsValidLoopType(header->loop_type);
}

size_t MidiSerializeDumpHeader(
    midi_dump_header_t const *header,
    uint8_t *data, size_t data_size) {
  if (data == NULL && data_size > 0) return 0;
  if (!MidiIsValidDumpHeader(header)) return 0;
  if (data == NULL || data_size < MIDI_DUMP_HEADER_PAYLOAD_SIZE)
      return MIDI_DUMP_HEADER_PAYLOAD_SIZE;
  data[0] = MidiGetDataWordLsb(header->sample_number);
  data[1] = MidiGetDataWordMsb(header->sample_number);
  data[2] = header->sample_format;
  MidiSerializeTriByte(header->sample_period, &data[3]);
  MidiSerializeTriByte(header->sample_length, &data[6]);
  MidiSerializeTriByte(header->sustain_loop_start_point, &data[9]);
  MidiSerializeTriByte(header->sustain_loop_end_point, &data[12]);
  data[15] = header->loop_type;
  return MIDI_DUMP_HEADER_PAYLOAD_SIZE;
}

size_t MidiDeserializeDumpHeader(
    uint8_t const *data, size_t data_size,
    midi_dump_header_t *header) {
  if (header == NULL) return 0;
  if (data == NULL && data_size > 0) return 0;
  if (data_size < MIDI_DUMP_HEADER_PAYLOAD_SIZE)
    return MIDI_DUMP_HEADER_PAYLOAD_SIZE;
  if (!MidiIsDataArray(data, MIDI_DUMP_HEADER_PAYLOAD_SIZE)) return 0;
  header->sample_number = MidiDataWordFromBytes(data[1], data[0]);
  header->sample_format = data[2];
  if (!MidiIsValidSampleFormat(header->sample_format)) return 0;
  MidiDeserializeTriByte(&data[3], &header->sample_period);
  MidiDeserializeTriByte(&data[6], &header->sample_length);
  MidiDeserializeTriByte(&data[9], &header->sustain_loop_start_point);
  MidiDeserializeTriByte(&data[12], &header->sustain_loop_end_point);
  header->loop_type = data[15];
  if (!MidiIsValidLoopType(header->loop_type)) return 0;
  return MIDI_DUMP_HEADER_PAYLOAD_SIZE;
}

/*
 * Sample Dump Request Message.
 *  Format: ss|ss
 *    ss ss    - Sample number
 */

bool_t MidiIsValidDumpRequest(midi_dump_request_t const *dump_request) {
  if (dump_request == NULL) return false;
  return MidiIsDataWord(dump_request->sample_number);
}

bool_t MidiInitializeDumpRequest(
    midi_dump_request_t *request, uint16_t sample) {
  if (request == NULL) return false;
  if (!MidiIsDataWord(sample)) return false;
  request->sample_number = sample;
  return true;
}

size_t MidiSerializeDumpRequest(
    midi_dump_request_t const *request,
    uint8_t *data, size_t data_size) {
  if (data == NULL && data_size > 0) return 0;
  if (!MidiIsValidDumpRequest(request)) return 0;
  if (data_size >= MIDI_DUMP_REQUEST_PAYLOAD_SIZE) {
    data[0] = MidiGetDataWordLsb(request->sample_number);
    data[1] = MidiGetDataWordMsb(request->sample_number);
  }
  return MIDI_DUMP_REQUEST_PAYLOAD_SIZE;
}

size_t MidiDeserializeDumpRequest(
    uint8_t const *data, size_t data_size,
    midi_dump_request_t *request) {
  if (request == NULL) return 0;
  if (data == NULL && data_size > 0) return 0;
  if (data_size < MIDI_DUMP_REQUEST_PAYLOAD_SIZE)
    return MIDI_DUMP_REQUEST_PAYLOAD_SIZE;
  if (!MidiIsDataArray(data, MIDI_DUMP_REQUEST_PAYLOAD_SIZE)) return 0;
  request->sample_number = MidiDataWordFromBytes(data[1], data[0]);
  return MIDI_DUMP_REQUEST_PAYLOAD_SIZE;
}

/*
 * Data Packet Message.
 *  Format: pp|dd<120>|ll
 *    pp      - Packet number
 *    dd<120> - Packet data (120 bytes)
 *    ll      - Checksum (7E + Man ID + 02 + pp + dd<120>)
 */

/* Calculates checksum for a data packet.  Will treat any missing
 * data bytes as zeros.
 * Caution: This function assumes inputs are valid.
 */
static uint8_t MidiCalculateCheckSum(
    midi_data_packet_t const *packet, midi_device_id_t device_id) {
  uint8_t checksum = (MIDI_NON_REAL_TIME_ID ^ MIDI_DATA_PACKET);
  checksum ^= (device_id ^ packet->number);
  if (packet->data == NULL) return checksum;
  for (uint8_t i = 0; i < packet->length && i < MIDI_DATA_PACKET_DATA_LENGTH; ++i) {
    checksum ^= packet->data[i];
  }
  return checksum;
}

/* Validates without checksum. */
inline static bool_t MidiPartialIsValidDataPacket(
    midi_data_packet_t const *packet) {
  if (packet == NULL) return false;
  if (!MidiIsValidPacketNumber(packet->number)) return false;
  /* Data */
  if (packet->data == NULL && packet->length > 0) return false;
  if (packet->data == NULL) return true;
  if (packet->length > MIDI_DATA_PACKET_DATA_LENGTH) return false;
  if (!MidiIsDataArray(packet->data, packet->length)) return false;
  return true;
}

bool_t MidiIsValidDataPacket(
    midi_data_packet_t const *packet) {
  if (!MidiPartialIsValidDataPacket(packet)) return false;
  return packet->checksum == MIDI_NULL_CHECKSUM ||
         MidiIsDataByte(packet->checksum);
}

bool_t MidiInitializeDataPacket(
    midi_data_packet_t *packet, midi_packet_number_t number) {
  if (packet == NULL) return false;
  if (!MidiIsValidPacketNumber(number)) return false;
  memset(packet, 0, sizeof(midi_data_packet_t));
  packet->number = number;
  packet->checksum = MIDI_NULL_CHECKSUM;
  return true;
}

bool_t MidiFillDataPacketChecksum(
    midi_data_packet_t *packet, midi_device_id_t device_id) {
  if (!MidiIsValidDeviceId(device_id)) return false;
  if (!MidiPartialIsValidDataPacket(packet)) return false;
  packet->checksum = MidiCalculateCheckSum(packet, device_id);
  return true;
}

bool_t MidiVerifyDataPacketChecksum(
    midi_data_packet_t const *packet, midi_device_id_t device_id) {
  if (!MidiIsValidDeviceId(device_id)) return false;
  if (!MidiIsValidDataPacket(packet)) return false;
  uint8_t const checksum = MidiCalculateCheckSum(packet, device_id);
  return packet->checksum == checksum;
}

bool_t MidiSetDataPacketDataBuffer(
    midi_data_packet_t *packet, uint8_t *data, size_t data_size) {
  if (packet == NULL || !MidiIsValidPacketNumber(packet->number)) return false;
  if (data == NULL && data_size > 0) return false;
  if (data_size > MIDI_DATA_PACKET_DATA_LENGTH) return false;
  if (data != NULL && !MidiIsDataArray(data, data_size)) return false;
  packet->data = data;
  packet->length = data_size;
  packet->checksum = MIDI_NULL_CHECKSUM;
  return true;
}

size_t MidiSerializeDataPacket(
    midi_data_packet_t const *packet,
    midi_device_id_t const *device_id,
    uint8_t *data, size_t data_size) {
  if (data == NULL && data_size > 0) return 0;
  if (!MidiIsValidDataPacket(packet)) return 0;
  if (device_id != NULL && !MidiIsValidDeviceId(*device_id)) return 0;
  if (data_size < MIDI_DATA_PACKET_PAYLOAD_SIZE)
    return MIDI_DATA_PACKET_PAYLOAD_SIZE;
  data[0] = packet->number;
  uint8_t i;
  for (i = 0; i < packet->length; ++i) {
    data[i + 1] = packet->data[i];
  }
  /* Zero pad */
  for (; i < MIDI_DATA_PACKET_DATA_LENGTH; ++i) {
    data[i + 1] = 0x00;
  }
  if (device_id == NULL) {
    data[121] = (packet->checksum == MIDI_NULL_CHECKSUM) ?
                0x00 : packet->checksum;
  } else {
    data[121] = MidiCalculateCheckSum(packet, *device_id);
  }
  return MIDI_DATA_PACKET_PAYLOAD_SIZE;
}

size_t MidiDeserializeDataPacket(
    uint8_t const *data, size_t data_size,
    midi_data_packet_t *packet,
    uint8_t *buffer, size_t buffer_size) {
  if (packet == NULL) return 0;
  if (data == NULL && data_size > 0) return 0;
  if (buffer == NULL && buffer_size > 0) return 0;
  if (data_size < MIDI_DATA_PACKET_PAYLOAD_SIZE)
    return MIDI_DATA_PACKET_PAYLOAD_SIZE;
  if (!MidiIsValidPacketNumber(data[0])) return 0;
  /* Validate data and checksum (+1). */
  if (!MidiIsDataArray(&data[1], MIDI_DATA_PACKET_DATA_LENGTH + 1)) return 0;
  *packet = (midi_data_packet_t) {
    .number = data[0],
    .length = 0,
    .data = NULL,
    .checksum = data[121]
  };

  if (buffer != NULL && buffer_size > 0) {
    packet->data = buffer;
    packet->length = (buffer_size > MIDI_DATA_PACKET_DATA_LENGTH)
        ? MIDI_DATA_PACKET_DATA_LENGTH
        : buffer_size;
  } else {
    packet->data = MidiGetGlobalDataPacketBuffer();  /* Might return NULL. */
    if (packet->data != NULL) {
      packet->length = MIDI_DATA_PACKET_DATA_LENGTH;
    }
  }

  if (packet->data != NULL && packet->length > 0) {
    memcpy(packet->data, &data[1], packet->length);
  }
  return MIDI_DATA_PACKET_PAYLOAD_SIZE;
}

/*
 * Sample Dump: Loop Points Message.
 *  Format:  0  1  2  3  4  5  6  7  8  9 10 11
 *          tt|ss|ss|bb|bb|cc|dd|dd|dd|ee|ee|ee|
 *                        '-- Response Only ---'
 *
 * Request and response fields:
 *    tt       - Sub ID, message type (request or response)
 *    ss ss    - Sample number
 *    bb bb    - Loop number
 * Response only fields:
 *    cc       - Loop type (Forward, Backward, Off)
 *    dd dd dd - Loop start address
 *    ee ee ee - Loop end address
 */

#define MidiIsValidSampleDumpSubId(sub_id) \
    ((sub_id) == MIDI_SAMPLE_LOOP_RESPONSE || \
     (sub_id) == MIDI_SAMPLE_LOOP_REQUEST)

bool_t MidiIsValidSampleDump(midi_sample_dump_t const *sample_dump) {
  if (sample_dump == NULL) return false;
  if (!MidiIsValidSampleDumpSubId(sample_dump->sub_id)) return false;
  if (!MidiIsDataWord(sample_dump->sample_number) ||
      !MidiIsDataWord(sample_dump->loop_number)) return false;
  /* Requests validation done. */
  if (sample_dump->sub_id == MIDI_SAMPLE_LOOP_REQUEST) return true;
  if (!MidiIsValidLoopType(sample_dump->loop_type)) return false;
  return MidiIsDataTriByte(sample_dump->loop_start_address) &&
         MidiIsDataTriByte(sample_dump->loop_end_address);
}

bool_t MidiInitializeSampleDumpRequest(
    midi_sample_dump_t *sample_dump,
    uint16_t sample_number, uint16_t loop_number) {
  if (sample_dump == NULL) return false;
  if (!MidiIsDataWord(sample_number) || !MidiIsDataWord(loop_number))
    return false;
  memset(sample_dump, 0, sizeof(midi_sample_dump_t));
  sample_dump->sub_id = MIDI_SAMPLE_LOOP_REQUEST;
  sample_dump->sample_number = sample_number;
  sample_dump->loop_number = loop_number;
  return true;
}

bool_t MidiInitializeSampleDumpResponse(
    midi_sample_dump_t *sample_dump,
    uint16_t sample_number, uint16_t loop_number,
    uint8_t loop_type,
    uint32_t loop_start_address, uint32_t loop_end_address) {
  if (sample_dump == NULL) return false;
  if (!MidiIsDataWord(sample_number) || !MidiIsDataWord(loop_number))
    return false;
  if (!MidiIsValidLoopType(loop_type)) return false;
  if (!MidiIsDataTriByte(loop_start_address) ||
      !MidiIsDataTriByte(loop_end_address)) return false;
  memset(sample_dump, 0, sizeof(midi_sample_dump_t));
  sample_dump->sub_id = MIDI_SAMPLE_LOOP_RESPONSE;
  sample_dump->sample_number = sample_number;
  sample_dump->loop_number = loop_number;
  sample_dump->loop_type = loop_type;
  sample_dump->loop_start_address = loop_start_address;
  sample_dump->loop_end_address = loop_end_address;
  return true;
}

size_t MidiSerializeSampleDump(
    midi_sample_dump_t const *sample_dump,
    uint8_t *data, size_t data_size) {
  if (data == NULL && data_size > 0) return 0;
  if (!MidiIsValidSampleDump(sample_dump)) return 0;
  size_t const expected_size =
      (sample_dump->sub_id == MIDI_SAMPLE_LOOP_RESPONSE)
      ? MIDI_SAMPLE_LOOP_RESPONSE_PAYLOAD_SIZE
      : MIDI_SAMPLE_LOOP_REQUEST_PAYLOAD_SIZE;
  if (data_size < expected_size) return expected_size;
  data[0] = sample_dump->sub_id;
  data[1] = MidiGetDataWordLsb(sample_dump->sample_number);
  data[2] = MidiGetDataWordMsb(sample_dump->sample_number);
  data[3] = MidiGetDataWordLsb(sample_dump->loop_number);
  data[4] = MidiGetDataWordMsb(sample_dump->loop_number);
  if (sample_dump->sub_id == MIDI_SAMPLE_LOOP_RESPONSE) {
    data[5] = sample_dump->loop_type;
    MidiSerializeTriByte(sample_dump->loop_start_address, &data[6]);
    MidiSerializeTriByte(sample_dump->loop_end_address, &data[9]);
  }
  return expected_size;
}

size_t MidiDeserializeSampleDump(
    uint8_t const *data, size_t data_size,
    midi_sample_dump_t *sample_dump) {
  if (data == NULL && data_size > 0) return 0;
  if (sample_dump == NULL) return 0;
  if (data_size == 0) return 1;
  sample_dump->sub_id = MIDI_NONE;
  if (!MidiIsValidSampleDumpSubId(data[0])) return 0;
  uint8_t const sub_id = data[0];
  size_t const expected_size =
      (sub_id == MIDI_SAMPLE_LOOP_RESPONSE) ?
      MIDI_SAMPLE_LOOP_RESPONSE_PAYLOAD_SIZE :
      MIDI_SAMPLE_LOOP_REQUEST_PAYLOAD_SIZE;
  if (data_size < expected_size) return expected_size;
  if (!MidiIsDataArray(&data[1], expected_size - 1)) return 0;
  sample_dump->sample_number = MidiDataWordFromBytes(data[2], data[1]);
  sample_dump->loop_number = MidiDataWordFromBytes(data[4], data[3]);
  if (sub_id == MIDI_SAMPLE_LOOP_RESPONSE) {
    sample_dump->loop_type = data[5];
    if (!MidiIsValidLoopType(sample_dump->loop_type)) return 0;
    MidiDeserializeTriByte(&data[6], &sample_dump->loop_start_address);
    MidiDeserializeTriByte(&data[9], &sample_dump->loop_end_address);
  }
  sample_dump->sub_id = sub_id;
  return expected_size;
}

/*
 * Device Inquiry Message.
 *  Format:  0  1  2  3  4  5  6  7  8  9 10 11
 *          tt|mm|ff|ff|dd|dd|ss|ss|ss|ss|
 *
 *           0  1  2  3  4  5  6  7  8  9 10 11
 *          tt|mm|mm|mm|ff|ff|dd|dd|ss|ss|ss|ss|
 *
 * Request and response fields:
 *    tt          - Sub ID, message type (request or response)
 * Response only fields:
 *    mm[mm mm]   - Manufacturer ID
 *    ff ff       - Device family.
 *    dd dd       - Device family member code.
 *    ss ss ss ss - Software revision level.
 */

#define MidiIsValidInquiryDeviceSubId(sub_id) \
    ((sub_id) == MIDI_DEVICE_INQUIRY_RESPONSE || \
     (sub_id) == MIDI_DEVICE_INQUIRY_REQUEST)

bool_t MidiIsValidDeviceInquiry(midi_device_inquiry_t const *device_inquiry) {
  if (device_inquiry == NULL) return false;
  if (!MidiIsValidInquiryDeviceSubId(device_inquiry->sub_id)) return false;
  if (device_inquiry->sub_id == MIDI_DEVICE_INQUIRY_REQUEST) return true;
  return MidiIsValidSystemInfo(&device_inquiry->info);
}

bool_t MidiInitializeDeviceInquiryRequest(
    midi_device_inquiry_t *device_inquiry) {
  if (device_inquiry == NULL) return false;
  memset(device_inquiry, 0, sizeof(midi_device_inquiry_t));
  device_inquiry->sub_id = MIDI_DEVICE_INQUIRY_REQUEST;
  return true;
}

bool_t MidiInitializeDeviceInquiryResponse(
    midi_device_inquiry_t *device_inquiry, midi_sys_info_t const *sys_info) {
  if (device_inquiry == NULL) return false;
  if (!MidiIsValidSystemInfo(sys_info)) return false;
  device_inquiry->sub_id = MIDI_DEVICE_INQUIRY_RESPONSE;
  memcpy(&device_inquiry->info, sys_info, sizeof(midi_sys_info_t));
  return true;
}

size_t MidiSerializeDeviceInquiry(
    midi_device_inquiry_t const *device_inquiry,
    uint8_t *data, size_t data_size) {
  if (data == NULL && data_size > 0) return 0;
  if (!MidiIsValidDeviceInquiry(device_inquiry)) return 0;
  size_t const expected_size =
      (device_inquiry->sub_id == MIDI_DEVICE_INQUIRY_REQUEST) ?
       MIDI_DEVICE_INQUIRY_REQUEST_PAYLOAD_SIZE :
       ((device_inquiry->info.id[0] == 0x00) ?
        MIDI_DEVICE_INQUIRY_RESPONSE_LARGE_PAYLOAD_SIZE :
        MIDI_DEVICE_INQUIRY_RESPONSE_SMALL_PAYLOAD_SIZE);
  if (data_size < expected_size) return expected_size;
  data[0] = device_inquiry->sub_id;
  if (device_inquiry->sub_id == MIDI_DEVICE_INQUIRY_RESPONSE) {
    midi_sys_info_t const *sys_info = &device_inquiry->info;
    size_t const offset =  MidiSerializeManufacturerId(
        sys_info->id, &data[1], data_size - 1);
    data[offset + 1] = MidiGetDataWordLsb(sys_info->device_family_code);
    data[offset + 2] = MidiGetDataWordMsb(sys_info->device_family_code);
    data[offset + 3] = MidiGetDataWordLsb(sys_info->device_family_member_code);
    data[offset + 4] = MidiGetDataWordMsb(sys_info->device_family_member_code);
    memcpy(&data[offset + 5], sys_info->software_revision_level,
           MIDI_SOFTWARE_REVISION_SIZE);
  }
  return expected_size;
}

size_t MidiDeserializeDeviceInquiry(
    uint8_t const *data, size_t data_size,
    midi_device_inquiry_t *device_inquiry) {
  if (data == NULL && data_size > 0) return 0;
  if (device_inquiry == NULL) return 0;
  if (data_size == 0) return 1;
  if (!MidiIsValidInquiryDeviceSubId(data[0])) return 0;
  if (data[0] == MIDI_DEVICE_INQUIRY_REQUEST) {
    MidiInitializeDeviceInquiryRequest(device_inquiry);
    return MIDI_DEVICE_INQUIRY_REQUEST_PAYLOAD_SIZE;
  }
  if (data_size < 2) return 2;
  if (!MidiIsDataByte(data[1])) return 0;
  /* Check if it is a 1 or 3 byte manufacture ID. */
  size_t const expected_size =
      (data[1] == 0x00) ?
      MIDI_DEVICE_INQUIRY_RESPONSE_LARGE_PAYLOAD_SIZE :
      MIDI_DEVICE_INQUIRY_RESPONSE_SMALL_PAYLOAD_SIZE;
  if (data_size < expected_size) return expected_size;
  device_inquiry->sub_id = data[0];
  if (!MidiIsDataArray(data, expected_size)) return 0;
  midi_sys_info_t *sys_info = &device_inquiry->info;
  size_t const offset = MidiDeserializeManufacturerId(
      &data[1], data_size - 1, sys_info->id);
  sys_info->device_family_code = MidiDataWordFromBytes(
      data[offset + 2], data[offset + 1]);
  sys_info->device_family_member_code = MidiDataWordFromBytes(
      data[offset + 4], data[offset + 3]);
  memcpy(sys_info->software_revision_level, &data[offset + 5],
         MIDI_SOFTWARE_REVISION_SIZE);
  return expected_size;
}

/* General MIDI Mode. */

bool_t MidiIsValidGeneralMidiMode(midi_general_midi_mode_t mode) {
  return mode == MIDI_GENERAL_MIDI_ON || mode == MIDI_GENERAL_MIDI_OFF;
}

/*
 * Realtime Time Code Message.
 *  Full Time Code Message:
 *    01|hh|mm|ss|ff
 *
 * Fields:
 *    01 - Full time code message sub ID.
 *    hh - Hours and frame type
 *    mm - Minutes
 *    ss - Seconds
 *    ff - Frame
 */
#define MIDI_FULL_TIME_CODE_FPS_MASK      0x60
#define MIDI_FULL_TIME_CODE_HOURS_MASK    0x1F

#define MidiIsValidRealtimeTimeCodeSubId(sub_id) \
    ((sub_id) == MIDI_FULL_TIME_CODE || \
     (sub_id) == MIDI_USER_BITS)

bool_t MidiIsValidRealtimeTimeCode(midi_rt_time_code_t const *rt_time) {
  if (rt_time == NULL) return false;
  switch (rt_time->sub_id) {
    case MIDI_FULL_TIME_CODE:
      return MidiIsValidTime(&rt_time->time);
    case MIDI_USER_BITS:
      return MidiIsValidUserBits(&rt_time->user_bits);
  }
  return false;
}

bool_t MidiInitializeFullTimeCodeMessage(
    midi_rt_time_code_t *rt_time, midi_time_t const *time) {
  if (rt_time == NULL) return false;
  if (!MidiIsValidTime(time)) return false;
  memset(rt_time, 0, sizeof(midi_rt_time_code_t));
  rt_time->sub_id = MIDI_FULL_TIME_CODE;
  memcpy(&rt_time->time, time, sizeof(midi_time_t));
  return true;
}

bool_t MidiInitializeUserBitsTimeCode(
    midi_rt_time_code_t *rt_time, midi_user_bits_t const *user_bits) {
  if (rt_time == NULL) return false;
  if (!MidiIsValidUserBits(user_bits)) return false;
  memset(rt_time, 0, sizeof(midi_rt_time_code_t));
  rt_time->sub_id = MIDI_USER_BITS;
  memcpy(&rt_time->user_bits, user_bits, sizeof(midi_user_bits_t));
  return true;
}

size_t MidiSerializeRealtimeTimeCode(
    midi_rt_time_code_t const *rt_time, uint8_t *data, size_t data_size) {
  if (data == NULL && data_size > 0) return 0;
  if (!MidiIsValidRealtimeTimeCode(rt_time)) return 0;
  if (data_size > 0) data[0] = rt_time->sub_id;
  switch (rt_time->sub_id) {
    case MIDI_FULL_TIME_CODE: {
      if (data_size >= MIDI_FULL_TIME_CODE_MESSAGE_PAYLOAD_SIZE) {
        data[1] = rt_time->time.hours | rt_time->time.fps;
        data[2] = rt_time->time.minutes;
        data[3] = rt_time->time.seconds;
        data[4] = rt_time->time.frame;
      }
      return MIDI_FULL_TIME_CODE_MESSAGE_PAYLOAD_SIZE;
    } break;
    case MIDI_USER_BITS: {
      size_t const res = MidiSerializeUserBits(
          &rt_time->user_bits, data == NULL ? NULL : &data[1],
          data_size == 0 ? 0 : data_size - 1);
      return res == 0 ? 0 : MIDI_SMPTE_USER_BITS_PAYLOAD_SIZE;
    } break;
  }
  return 0;
}

size_t MidiDeserializeRealtimeTimeCode(
    uint8_t const *data, size_t data_size, midi_rt_time_code_t *rt_time) {
  if (data == NULL && data_size > 0) return 0;
  if (rt_time == NULL) return 0;
  if (data_size == 0) return 1;   /* Need sub ID type. */
  if (!MidiIsValidRealtimeTimeCodeSubId(data[0]));
  memset(rt_time, 0, sizeof(midi_rt_time_code_t));
  rt_time->sub_id = data[0];
  switch (rt_time->sub_id) {
    case MIDI_FULL_TIME_CODE: {
      if (data_size >= MIDI_FULL_TIME_CODE_MESSAGE_PAYLOAD_SIZE) {
        if (!MidiIsDataArray(
            &data[1], MIDI_FULL_TIME_CODE_MESSAGE_PAYLOAD_SIZE - 1))
          return 0;
        midi_time_t *time = &rt_time->time;
        time->fps = data[1] & MIDI_FULL_TIME_CODE_FPS_MASK;
        time->hours = data[1] & MIDI_FULL_TIME_CODE_HOURS_MASK;
        time->minutes = data[2];
        time->seconds = data[3];
        time->frame = data[4];
        if (!MidiIsValidTime(time)) return 0;
      }
      return MIDI_FULL_TIME_CODE_MESSAGE_PAYLOAD_SIZE;
    } break;
    case MIDI_USER_BITS: {
      size_t const res = MidiDeserializeUserBits(
          data == NULL ? NULL : &data[1], data_size == 0 ? 0 : data_size - 1,
          &rt_time->user_bits);
      return res == 0 ? 0 : MIDI_SMPTE_USER_BITS_PAYLOAD_SIZE;
    } break;
  }
  return 0;
}

/*
 * Device Control Message.
 *  Format: tt|vv|vv
 *
 * Fields:
 *    tt     - Control type
 *    vv vv  - Control value
 */
#define MidiIsValidDeviceControlSubId(sub_id) \
    ((sub_id) == MIDI_MASTER_VOLUME || \
     (sub_id) == MIDI_MASTER_BALANCE)

bool_t MidiIsValidDeviceControl(midi_device_control_t const *control) {
  if (control == NULL) return false;
  switch (control->sub_id) {
    case MIDI_MASTER_VOLUME:
      return MidiIsValidMasterVolume(control->volume);
    case MIDI_MASTER_BALANCE:
      return MidiIsValidMasterBalance(control->balance);
  }
  return false;
}

bool_t MidiInitializeDeviceControl(
    midi_device_control_t *control, uint8_t sub_id, uint16_t value) {
  if (control == NULL) return false;
  memset(control, 0, sizeof(midi_device_control_t));
  control->sub_id = sub_id;
  switch (control->sub_id) {
    case MIDI_MASTER_VOLUME:
      if (!MidiIsValidMasterVolume(value)) return false;
      control->volume = value;
      return true;
    case MIDI_MASTER_BALANCE:
      if (!MidiIsValidMasterBalance(value)) return false;
      control->balance = value;
      return true;
  }
  return false;
}

size_t MidiSerializeDeviceControl(
    midi_device_control_t const *control, uint8_t *data, size_t data_size) {
  if (data == NULL && data_size > 0) return 0;
  if (!MidiIsValidDeviceControl(control)) return 0;
  if (data_size >= MIDI_DEVICE_CONTROL_PAYLOAD_SIZE) {
    data[0] = control->sub_id;
    uint16_t const value = (control->sub_id == MIDI_MASTER_VOLUME)
        ? control->volume : control->balance;
    data[1] = MidiGetDataWordLsb(value);
    data[2] = MidiGetDataWordMsb(value);
  }
  return MIDI_DEVICE_CONTROL_PAYLOAD_SIZE;
}

size_t MidiDeserializeDeviceControl(
    uint8_t const *data, size_t data_size, midi_device_control_t *control) {
  if (data == NULL && data_size > 0) return 0;
  if (control == NULL) return 0;
  if (data_size >= MIDI_DEVICE_CONTROL_PAYLOAD_SIZE) {
    if (!MidiIsDataArray(data, MIDI_DEVICE_CONTROL_PAYLOAD_SIZE)) return 0;
    control->sub_id = data[0];
    switch (control->sub_id) {
      case MIDI_MASTER_VOLUME:
        control->volume = MidiDataWordFromBytes(data[2], data[1]);
        break;
      case MIDI_MASTER_BALANCE:
        control->balance = MidiDataWordFromBytes(data[2], data[1]);
        break;
      default:
        return 0;
    }
  }
  return MIDI_DEVICE_CONTROL_PAYLOAD_SIZE;
}
