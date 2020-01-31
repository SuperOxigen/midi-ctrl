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

bool_t MidiIsValidSysEx(midi_sys_ex_t const *sys_ex) {
  if (sys_ex == NULL) return false;
  if (!MidiIsValidManufacturerId(sys_ex->id)) return false;
  if (!MidiManufacturerIdIsUniversal(sys_ex->id)) return true;
  switch (sys_ex->sub_id) {
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
