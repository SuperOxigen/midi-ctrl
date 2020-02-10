/*
 * MIDI Controller - MIDI Manufacturer ID.
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#include <string.h>

#include "midi_bytes.h"
#include "midi_defs.h"
#include "midi_man_id.h"
#include "smart_string.h"

/* Too simple for public reference. */
#define MidiManufacturerIdIsOneByte(man_id) ((man_id)[0] != 0)

bool_t MidiClearManufacturerId(midi_manufacturer_id_ref_t man_id) {
  if (man_id == NULL) return false;
  memset(man_id, 0, sizeof(midi_manufacturer_id_t));
  return true;
}

bool_t MidiIsValidManufacturerId(midi_manufacturer_id_cref_t man_id) {
  if (man_id == NULL) return false;
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
  for (size_t i = 0; i < sizeof(midi_manufacturer_id_t); ++i) {
    if (man_id[i] != 0) return false;
  }
  return true;
}

bool_t MidiManufacturerIdIsUniversal(midi_manufacturer_id_cref_t man_id) {
  if (!MidiIsValidManufacturerId(man_id)) return false;
  return man_id[0] == MIDI_NON_REAL_TIME_ID || man_id[0] == MIDI_REAL_TIME_ID;
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

size_t MidiManufacturerIdToString(
    midi_manufacturer_id_cref_t man_id, char *str, size_t str_size) {
  if (str == NULL) return 0;
  if (!MidiIsValidManufacturerId(man_id)) return 0;
  if (MidiManufacturerIdIsOneByte(man_id)) {
    return SmartStringHexEncode(man_id, 1, str, str_size);
  }
  size_t used_data = SmartStringHexEncode(&man_id[1], 1, str, str_size);
  if (used_data < str_size) {
    SmartStringAppend("-", str, str_size);
  }
  ++used_data;
  if (used_data + 2 < str_size) {
    char octet[3];
    SmartStringHexEncode(&man_id[2], 1, octet, sizeof(octet));
    SmartStringAppend(octet, str, str_size);
  }
  used_data += 2;
  return used_data;
}

size_t MidiSerializeManufacturerId(
    midi_manufacturer_id_cref_t man_id, uint8_t *data, size_t data_size) {
  if (data == NULL && data_size > 0) return 0;
  if (!MidiIsValidManufacturerId(man_id)) return 0;
  if (data_size >= 1) data[0] = man_id[0];
  if (MidiManufacturerIdIsOneByte(man_id)) return 1;
  if (data_size >= 2) data[1] = man_id[1];
  if (data_size >= 3) data[2] = man_id[2];
  return 3;
}

size_t MidiDeserializeManufacturerId(
    uint8_t const *data, size_t data_size, midi_manufacturer_id_ref_t man_id) {
  if (data == NULL && data_size > 0) return 0;
  if (man_id == NULL) return 0;
  if (data_size == 0) return 1;
  if (!MidiIsDataByte(data[0])) return 0;
  MidiClearManufacturerId(man_id);
  if (data[0] != 0x00) {
    man_id[0] = data[0];
    return 1;
  }
  if (data_size < 3) return 3;
  if (!MidiIsValidManufacturerId(data)) return 0;
  memcpy(man_id, data, 3);
  return 3;
}
