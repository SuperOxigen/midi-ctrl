/*
 * MIDI Controller - MIDI System Information.
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#include <string.h>

#include "midi_sys_info.h"

bool_t MidiIsValidSystemInfo(midi_sys_info_t const *sys_info) {
  if (sys_info == NULL) return false;
  if (!MidiIsValidManufacturerId(sys_info->id)) return false;
  if (!MidiIsValidDeviceFamilyCode(sys_info->device_family_code) ||
      !MidiIsValidDeviceFamilyMemberCode(sys_info->device_family_member_code))
    return false;
  if (!MidiIsValidSoftwareRevisionLevel(sys_info->software_revision_level))
    return false;
  return true;
}

bool_t MidiInitializeSystemInfo(
    midi_sys_info_t *sys_info, midi_manufacturer_id_cref_t man_id,
    uint16_t device_family_code, uint16_t device_family_member_code,
    uint8_t const *software_revision_level) {
  if (sys_info == NULL) return false;
  if (!MidiIsValidManufacturerId(man_id) ||
      !MidiIsValidDeviceFamilyCode(device_family_code) ||
      !MidiIsValidDeviceFamilyMemberCode(device_family_member_code) ||
      !MidiIsValidSoftwareRevisionLevel(software_revision_level))
    return false;
  memset(sys_info, 0, sizeof(midi_sys_info_t));
  memcpy(sys_info->id, man_id, sizeof(midi_manufacturer_id_t));
  sys_info->device_family_code = device_family_code;
  sys_info->device_family_member_code = device_family_member_code;
  memcpy(sys_info->software_revision_level, software_revision_level,
         MIDI_SOFTWARE_REVISION_SIZE);
  return true;
}
