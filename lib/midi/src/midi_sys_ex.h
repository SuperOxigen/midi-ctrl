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
#include "midi_sys_uni.h"
#include "midi_man_id.h"

C_SECTION_BEGIN;

/*
 * MIDI System Exclusive
 */

/* SysEx module only supports real time and non-real time special
 * manufacture IDs. */
bool_t MidiIsSpecialSysExId(midi_manufacturer_id_cref_t man_id);

/* General System Exec Message. */
typedef struct {
  midi_manufacturer_id_t id;
  midi_device_id_t device_id;
  uint8_t sub_id;  /* Determines which system universal message to use. */
  /* System Universal Message. */
  union {
    /* Non-Realtime */
    /* For all handshake messages. */
    midi_packet_number_t packet_number;
    /* Data dump. */
    midi_dump_header_t dump_header;
    midi_dump_request_t dump_request;
    midi_data_packet_t data_packet;
    midi_sample_dump_t sample_dump;
    midi_device_inquiry_t device_inquiry;
    midi_general_midi_mode_t gm_mode;
    /* Realtime */
    midi_rt_time_code_t rt_time_code;
    midi_device_control_t device_control;
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
  midi_sys_ex_t *sys_ex, midi_device_id_t device_id,
  uint8_t handshake_sub_id, midi_packet_number_t packet_number);

bool_t MidiGeneralMidiModeSysEx(
  midi_sys_ex_t *sys_ex, midi_device_id_t device_id, bool_t on);
#define MidiGeneralMidiModeOnSysEx(sys_ex, device_id) \
    MidiGeneralMidiModeSysEx(sys_ex, device_id, true)
#define MidiGeneralMidiModeOffSysEx(sys_ex, device_id) \
    MidiGeneralMidiModeSysEx(sys_ex, device_id, false)

bool_t MidiInitializeSysUni(
  midi_sys_ex_t *sys_ex, bool_t real_time,
  midi_device_id_t device_id, uint8_t sub_id);

size_t MidiSerializeSysEx(
  midi_sys_ex_t const *sys_ex, uint8_t *data, size_t data_size);
size_t MidiDeserializeSysEx(
  uint8_t const *data, size_t data_size, midi_sys_ex_t *sys_ex);

C_SECTION_END;

#endif  /* _MIDI_SYSTEM_EXCLUSIVE_H_ */
