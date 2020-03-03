/*
 * MIDI Controller - MIDI System Information.
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#ifndef _MIDI_SYSTEM_INFORMATION_H_
#define _MIDI_SYSTEM_INFORMATION_H_

#include "base.h"

#include "midi_bytes.h"
#include "midi_man_id.h"

C_SECTION_BEGIN;

typedef uint16_t midi_device_family_code_t;
typedef uint16_t midi_device_family_member_code_t;

#define MidiIsValidDeviceFamilyCode MidiIsDataWord
#define MidiIsValidDeviceFamilyMemberCode MidiIsDataWord

#define MIDI_SOFTWARE_REVISION_SIZE 4
#define MidiIsValidSoftwareRevisionLevel(rev_level) \
  MidiIsDataArray((rev_level), MIDI_SOFTWARE_REVISION_SIZE)

/* The MIDI System Information struct is to be created by the
 * framework user to be able to identify the MIDI system and its
 * capabilities.
 * By setting the MIDI system's information, the MIDI system will be
 * able to take action to certain standard messages on behalf of the
 * user.  Such as auto responding to device inquiry requests. */

typedef struct {
  /* System's manufacturer ID.  If left unset, this value will assumed
   * to be SPECIAL_ID (0x7D), used for experimental systems. */
  midi_manufacturer_id_t id;
  /* Device family and member code. */
  midi_device_family_code_t device_family_code;
  midi_device_family_member_code_t device_family_member_code;
  /* Software revision level. */
  uint8_t software_revision_level[MIDI_SOFTWARE_REVISION_SIZE];
} midi_sys_info_t;

bool_t MidiIsValidSystemInfo(midi_sys_info_t const *sys_info);

bool_t MidiInitializeSystemInfo(
  midi_sys_info_t *sys_info, midi_manufacturer_id_cref_t man_id,
  uint16_t device_family_code, uint16_t device_family_member_code,
  uint8_t const *software_revision_level);

C_SECTION_END;

#endif  /* _MIDI_SYSTEM_INFORMATION_H_ */
