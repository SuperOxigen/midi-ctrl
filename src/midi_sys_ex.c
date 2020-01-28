/*
 * MIDI Controller - MIDI System Exclusive.
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#include <string.h>

#include "midi_bytes.h"
#include "midi_defs.h"
#include "midi_sys_ex.h"

#define MidiManufacturerIdIsOneByte(man_id) ((man_id)[0] != 0)

bool_t MidiIsValidManufacturerId(midi_manufacturer_id_cref_t man_id) {
  if (man_id == NULL) return false;
  size_t i = 0;
  if (!MidiIsDataByte(man_id[0])) return false;
  if (MidiManufacturerIdIsOneByte(man_id)) {
    return (man_id[1] == 0) && (man_id[2] == 0);
  }
  /* Two byte IDs */
  return MidiIsDataByte(man_id[1]) && MidiIsDataByte(man_id[2])
      && (man_id[1] != 0 || man_id[2] != 0);
}

bool_t MidiIsBlankManufacturerId(midi_manufacturer_id_cref_t man_id) {
  if (man_id == NULL) return false;
  return (man_id[0] == 0 && man_id[1] == 0 && man_id[2] == 0);
}

bool_t MidiInitializeManufacturerId(midi_manufacturer_id_ref_t man_id) {
  if (man_id == NULL) return false;
  memset(man_id, 0, sizeof(midi_manufacturer_id_t));
  return true;
}

midi_region_t MidiManufacturerRegion(midi_manufacturer_id_cref_t man_id) {
  if (!MidiIsValidManufacturerId(man_id)) return MIDI_REGION_UNKNOWN;
  if (MidiManufacturerIdIsOneByte(man_id)) {
    if (man_id[0] >= 0x01 && man_id[0] <= 0x1F) return MIDI_REGION_AMERICA;
    if (man_id[0] >= 0x20 && man_id[0] <= 0x3F) return MIDI_REGION_EUROPEAN;
    if (man_id[0] >= 0x40 && man_id[0] <= 0x5F) return MIDI_REGION_JAPANESE;
    if (man_id[0] >= 0x60 && man_id[0] <= 0x7C) return MIDI_REGION_OTHER;
    return MIDI_REGION_SPECIAL;
  }
  /* Else, two bytes */
  if (man_id[1] >= 0x00 && man_id[1] <= 0x1F) return MIDI_REGION_AMERICA;
  if (man_id[1] >= 0x20 && man_id[1] <= 0x3F) return MIDI_REGION_EUROPEAN;
  if (man_id[1] >= 0x40 && man_id[1] <= 0x5F) return MIDI_REGION_JAPANESE;
  return MIDI_REGION_OTHER;
}

bool_t MidiManufacturerIdIsUniversal(midi_manufacturer_id_cref_t man_id) {
  if (!MidiIsValidManufacturerId(man_id)) return false;
  return man_id[0] == 0x7E || man_id[0] == 0x7F;
}

bool_t MidiIsSpecialSubId(uint8_t sub_id) {
  if (!MidiIsDataByte(sub_id)) return false;
  switch (sub_id) {
    case MIDI_DUMP_HEADER:
    case MIDI_DATA_PACKET:
    case MIDI_DUMP_REQUEST:
    case MIDI_SAMPLE_DUMP:
    case MIDI_GENERAL_INFO:
    case MIDI_FILE_DUMP:
    case MIDI_TUNING_DUMP:
    case MIDI_GENERAL_MIDI:
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
 * MIDI System Exclusive
 */

bool_t MidiInitializeSysEx(
    midi_sys_ex_t *sys_ex, midi_manufacturer_id_cref_t man_id) {
  if (sys_ex == NULL) return NULL;
  if (man_id != NULL && !MidiIsBlankManufacturerId(man_id)) {
    if (!MidiIsValidManufacturerId(man_id)) {
      return false;
    }
  }
  memset(sys_ex, 0, sizeof(midi_sys_ex_t));
  if (man_id != NULL) {
    memcpy(sys_ex->id, man_id, sizeof(midi_manufacturer_id_t));
  }
  return true;
}

bool_t MidiIsValidSysEx(midi_sys_ex_t const *sys_ex) {
  if (sys_ex == NULL) return false;
  if (!MidiIsValidManufacturerId(sys_ex->id)) return false;
  if (!MidiManufacturerIdIsUniversal(sys_ex->id)) return true;
  switch (sys_ex->sub_id_1) {
    case MIDI_NONE:
      return false;
    case MIDI_DUMP_HEADER:
    case MIDI_DATA_PACKET:
    case MIDI_DUMP_REQUEST:
    case MIDI_SAMPLE_DUMP:
    case MIDI_GENERAL_INFO:
    case MIDI_FILE_DUMP:
    case MIDI_TUNING_DUMP:
    case MIDI_GENERAL_MIDI:
    case MIDI_EOF:
    case MIDI_WAIT:
    case MIDI_CANCEL:
    case MIDI_NAK:
    case MIDI_ACK:
      return true;
    default:
      return false;
  }
  return false;
}
