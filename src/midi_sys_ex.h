/*
 * MIDI Controller - MIDI System Exclusive.
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#ifndef _MIDI_SYSTEM_EXCLUSIVE_H_
#define _MIDI_SYSTEM_EXCLUSIVE_H_

#include "base.h"
#include "midi_man_id.h"

C_SECTION_BEGIN;

bool_t MidiIsSpecialSubId(uint8_t sub_id);
bool_t MidiIsHandShakeSubId(uint8_t sub_id);

/*
 * MIDI System Exclusive
 */

typedef uint8_t midi_packet_number_t;
#define MidiIsValidPacketNumber(packet_number) MidiIsDataByte(packet_number)

/* System Universal Message. */
typedef struct {
  uint16_t sample_number;
  uint8_t sample_format;  /* Number of significant bits (8-28) */
  uint32_t sample_period;  /* In nanoseconds */
  uint32_t sample_length;  /* Sample length, in words. */
  uint32_t sustain_loop_start_point;
  uint32_t sustain_loop_end_point;
  uint8_t loop_type;
} midi_header_dump_t;

typedef struct {
  uint16_t requested_sample;
} midi_dump_request_t;

typedef struct {
  uint8_t packet_count;
  uint8_t *data;
  uint8_t data_length;
  uint8_t checksum;
} midi_data_packet_t;

typedef struct {
  uint8_t sub_id;
  /* Request and response. */
  uint16_t requested_sample;
  uint16_t requested_loop;
  /* Response only. */
  uint8_t loop_type;
  uint32_t loop_start_address;
  uint32_t loop_end_address;
} midi_sample_loop_t;

typedef struct {
  uint8_t sub_id;
  /* Reply only */
  midi_manufacturer_id_t id;
  uint16_t device_family_code;
  uint16_t device_family_member_code;
  uint8_t software_revision_level[4];
} midi_device_inquiry_t;

bool_t MidiIsValidHeaderDump(midi_header_dump_t const *header_dump);
bool_t MidiIsValidDumpRequest(midi_header_dump_t const *dump_request);
bool_t MidiIsValidDataPacket(midi_data_packet_t const *data_packet);
bool_t MidiIsValidSampleLoop(midi_sample_loop_t const *sample_loop);
bool_t MidiIsValidDeviceIdentity(midi_device_inquiry_t const *device_inquiry);

/* General System Exec Message. */
typedef struct {
  midi_manufacturer_id_t id;
  uint8_t sub_id;
  /* System Universal Message. */
  union {
    /* For all handshake messages. */
    midi_packet_number_t packet_number;
    /* Data dump. */
    midi_header_dump_t header_dump;
    midi_dump_request_t dump_request;
    midi_data_packet_t data_packet;
    midi_sample_loop_t sample_loop;
    midi_device_inquiry_t device_inquiry;
  };
} midi_sys_ex_t;

bool_t MidiIsValidSysEx(midi_sys_ex_t const *sys_ex);
bool_t MidiIsHandShakeSysEx(midi_sys_ex_t const *sys_ex);

/* Unlike the other initialization functions, it is possible to call
 * this successfully and still have the output in a non-valid state.
 * This will occur if a blank or NULL |man_id| is supplied.
 */
bool_t MidiInitializeSysEx(
  midi_sys_ex_t *sys_ex, midi_manufacturer_id_cref_t man_id);

bool_t MidiHandShakeSysEx(
  midi_sys_ex_t *sys_ex, midi_manufacturer_id_cref_t man_id,
  uint8_t type, midi_packet_number_t packet_number);

size_t MidiSerializeSysEx(
  midi_sys_ex_t const *sys_ex, uint8_t *data, size_t data_size);
size_t MidiDeserializeSysEx(
  uint8_t const *data, size_t data_size, midi_sys_ex_t *sys_ex);

C_SECTION_END;

#endif  /* _MIDI_SYSTEM_EXCLUSIVE_H_ */
