/*
 * MIDI Controller - MIDI System Universal.
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#ifndef _MIDI_SYSTEM_UNIVERSAL_H_
#define _MIDI_SYSTEM_UNIVERSAL_H_

#include "base.h"
#include "midi_bytes.h"
#include "midi_man_id.h"
#include "midi_sys_info.h"
#include "midi_time.h"
#include "midi_user_bits.h"
#include "midi_volume.h"

C_SECTION_BEGIN;

/* Device ID */
typedef uint8_t midi_device_id_t;
#define MidiIsValidDeviceId(device_id) MidiIsDataByte(device_id)

/*
 *  Non-Realtime - System Universal Message.
 */
/* Handshake Message. */
bool_t MidiIsHandShakeSubId(uint8_t sub_id);

typedef uint8_t midi_packet_number_t;
#define MidiIsValidPacketNumber(packet_number) MidiIsDataByte(packet_number)

/* Dump Header Message. */
#define MIDI_DUMP_HEADER_PAYLOAD_SIZE 16
typedef struct {
  uint16_t sample_number;
  uint8_t sample_format;  /* Number of significant bits (8-28) */
  uint32_t sample_period;  /* In nanoseconds */
  uint32_t sample_length;  /* Sample length, in words. */
  uint32_t sustain_loop_start_point;
  uint32_t sustain_loop_end_point;
  uint8_t loop_type;
} midi_dump_header_t;

bool_t MidiIsValidDumpHeader(midi_dump_header_t const *dump_header);

size_t MidiSerializeDumpHeader(
  midi_dump_header_t const *dump_header,
  uint8_t *data, size_t data_size);
size_t MidiDeserializeDumpHeader(
  uint8_t const *data, size_t data_size,
  midi_dump_header_t *dump_header);

/* Data Packet Message. */
#define MIDI_DATA_PACKET_PAYLOAD_SIZE 122
typedef struct {
  midi_packet_number_t number;
  uint8_t *data;  /* Optional (NULL is as no data) */
  uint8_t length;
  uint8_t checksum;  /*  */
} midi_data_packet_t;

/* The maximum byte capacity that can be stored in a single
 * data packet. */
#define MIDI_DATA_PACKET_DATA_LENGTH  120
typedef uint8_t midi_data_packet_buffer_t[MIDI_DATA_PACKET_DATA_LENGTH];

/* Special value to indicate that the checksum has not been
 * set.  This is an invalid checksum otherwise. */
#define MIDI_NULL_CHECKSUM 0xFF

/* Validates everything except the validity of the checksum (other than
 * being a valid value). */
bool_t MidiIsValidDataPacket(midi_data_packet_t const *data_packet);

bool_t MidiInitializeDataPacket(
  midi_data_packet_t *data_packet, midi_packet_number_t packet_number);

bool_t MidiFillDataPacketChecksum(
  midi_data_packet_t *data_packet, midi_device_id_t device_id);
bool_t MidiVerifyDataPacketChecksum(
  midi_data_packet_t const *data_packet, midi_device_id_t device_id);

/* Setting the data buffer will nullify the checksum. */
bool_t MidiSetDataPacketDataBuffer(
  midi_data_packet_t *data_packet, uint8_t *data, size_t data_size);

/* If the device ID is provided, serialization will compute the
 * checksum for serialization and ignore the message's checksum. */
size_t MidiSerializeDataPacket(
  midi_data_packet_t const *data_packet,
  midi_device_id_t const *device_id, /* Optional/nullable */
  uint8_t *data, size_t data_size);
/* Checksum is populated from data.  Caller is responsible for
 * verifying data was received successfully.
 * Optionally, a buffer can be given to set to be assigned to the
 * packet and store the deserialized data.  If the buffer is not
 * large enough, data will be truncated.
 */
size_t MidiDeserializeDataPacket(
  uint8_t const *data, size_t data_size,
  midi_data_packet_t *data_packet,
  /* |data_packet_buffer| is optional/nullable */
  uint8_t *data_packet_buffer, size_t data_packet_buffer_size);

/* Pushes a global buffer that can be used by deserialized data packet
 * store the data section.  Each buffer will be used sequentially,
 * though no guarentee about the next buffer immediately after pushing
 * a new one.  Ideally, this function is only called at the beginning
 * of the program.
 *
 * Unless a buffer is provided, all deserialized data packets will
 * discard its data portion.
 *
 * Each buffer must be atleast 120 bytes.
 * Passing in NULL with size 0 will clear the existing buffer set.
 *
 * Note: This will only store a limited number of buffers.
 */
bool_t MidiPushGlobalDataPacketBuffer(uint8_t *buffer, size_t buffer_size);
uint8_t *MidiGetGlobalDataPacketBuffer(void);

/* Dump Request Message. */
#define MIDI_DUMP_REQUEST_PAYLOAD_SIZE 2
typedef struct {
  uint16_t sample_number;
} midi_dump_request_t;

bool_t MidiIsValidDumpRequest(midi_dump_request_t const *dump_request);
bool_t MidiInitializeDumpRequest(
  midi_dump_request_t *dump_request, uint16_t sample);
size_t MidiSerializeDumpRequest(
  midi_dump_request_t const *dump_request,
  uint8_t *data, size_t data_size);
size_t MidiDeserializeDumpRequest(
  uint8_t const *data, size_t data_size,
  midi_dump_request_t *dump_request);

/* Sample Dump Ext: Loop Points. */
#define MIDI_SAMPLE_LOOP_REQUEST_PAYLOAD_SIZE 5
#define MIDI_SAMPLE_LOOP_RESPONSE_PAYLOAD_SIZE 12
typedef struct {
  uint8_t sub_id;
  /* Request and response. */
  uint16_t sample_number;
  uint16_t loop_number;
  /* Response only. */
  uint8_t loop_type;
  uint32_t loop_start_address;  /* Tri-byte */
  uint32_t loop_end_address;    /* Tri-byte */
} midi_sample_dump_t;

bool_t MidiIsValidSampleDump(midi_sample_dump_t const *sample_dump);

bool_t MidiInitializeSampleDumpRequest(
  midi_sample_dump_t *sample_dump,
  uint16_t sample_number, uint16_t loop_number);
bool_t MidiInitializeSampleDumpResponse(
  midi_sample_dump_t *sample_dump,
  uint16_t sample_number, uint16_t loop_number,
  uint8_t loop_type,
  uint32_t loop_start_address, uint32_t loop_end_address);

size_t MidiSerializeSampleDump(
  midi_sample_dump_t const *sample_dump,
  uint8_t *data, size_t data_size);
size_t MidiDeserializeSampleDump(
  uint8_t const *data, size_t data_size,
  midi_sample_dump_t *sample_dump);

/* General Information */
#define MIDI_DEVICE_INQUIRY_REQUEST_PAYLOAD_SIZE 1
#define MIDI_DEVICE_INQUIRY_RESPONSE_SMALL_PAYLOAD_SIZE 10
#define MIDI_DEVICE_INQUIRY_RESPONSE_LARGE_PAYLOAD_SIZE 12
typedef struct {
  /* Reply and request. */
  uint8_t sub_id;
  /* Reply only */
  midi_sys_info_t info;
} midi_device_inquiry_t;

bool_t MidiIsValidDeviceInquiry(midi_device_inquiry_t const *device_inquiry);

bool_t MidiInitializeDeviceInquiryRequest(
  midi_device_inquiry_t *device_inquiry);
bool_t MidiInitializeDeviceInquiryResponse(
  midi_device_inquiry_t *device_inquiry, midi_sys_info_t const *sys_info);

size_t MidiSerializeDeviceInquiry(
  midi_device_inquiry_t const *device_inquiry,
  uint8_t *data, size_t data_size);
size_t MidiDeserializeDeviceInquiry(
  uint8_t const *data, size_t data_size,
  midi_device_inquiry_t *device_inquiry);

/* General MIDI Mode. */
typedef uint8_t midi_general_midi_mode_t;
bool_t MidiIsValidGeneralMidiMode(midi_general_midi_mode_t mode);

/*
 *  Realtime - System Universal Message.
 */
/* Realtime Time Code Message */
#define MIDI_FULL_TIME_CODE_MESSAGE_PAYLOAD_SIZE 5
#define MIDI_SMPTE_USER_BITS_PAYLOAD_SIZE 10
typedef struct {
  uint8_t sub_id;
  union {
    /* Full Time Code Message */
    midi_time_t time;
    /* User bits */
    midi_user_bits_t user_bits;
  };
} midi_rt_time_code_t;

bool_t MidiIsValidRealtimeTimeCode(midi_rt_time_code_t const *rt_time);

bool_t MidiInitializeFullTimeCodeMessage(
  midi_rt_time_code_t *rt_time, midi_time_t const *time);
bool_t MidiInitializeUserBitsTimeCode(
  midi_rt_time_code_t *rt_time, midi_user_bits_t const *user_bits);

size_t MidiSerializeRealtimeTimeCode(
  midi_rt_time_code_t const *rt_time, uint8_t *data, size_t data_size);
size_t MidiDeserializeRealtimeTimeCode(
  uint8_t const *data, size_t data_size, midi_rt_time_code_t *rt_time);

/* Device Control */
#define MIDI_DEVICE_CONTROL_PAYLOAD_SIZE 3
typedef struct {
  uint8_t sub_id;
  union {
    midi_master_volume_t volume;
    midi_master_balance_t balance;
  };
} midi_device_control_t;

bool_t MidiIsValidDeviceControl(midi_device_control_t const *device_control);

bool_t MidiInitializeDeviceControl(
  midi_device_control_t *device_control, uint8_t sub_id, uint16_t value);

size_t MidiSerializeDeviceControl(
  midi_device_control_t const *device_control, uint8_t *data, size_t data_size);
size_t MidiDeserializeDeviceControl(
  uint8_t const *data, size_t data_size, midi_device_control_t *device_control);

C_SECTION_END;

#endif  /* _MIDI_SYSTEM_UNIVERSAL_H_ */
