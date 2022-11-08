/*
 * MIDI Controller - MIDI Manufacturer ID.
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#ifndef _MIDI_MANUFACTURER_ID_H_
#define _MIDI_MANUFACTURER_ID_H_

#include "base.h"

C_SECTION_BEGIN;

/*
 * MIDI Manufacturer ID
 */

typedef uint8_t midi_manufacturer_id_t[3];
/* References/pointer types for manufacture_id_t. */
typedef uint8_t *midi_manufacturer_id_ref_t;
typedef uint8_t const *midi_manufacturer_id_cref_t;

bool_t MidiClearManufacturerId(midi_manufacturer_id_ref_t man_id);

/* Validators */
bool_t MidiIsValidManufacturerId(midi_manufacturer_id_cref_t man_id);
bool_t MidiIsBlankManufacturerId(midi_manufacturer_id_cref_t man_id);
bool_t MidiManufacturerIdIsUniversal(midi_manufacturer_id_cref_t man_id);

/* Manufacturer Regions, as defined in MIDI standard. */
typedef enum {
  MIDI_REGION_UNKNOWN = 0,
  MIDI_REGION_AMERICA,
  MIDI_REGION_EUROPEAN,
  MIDI_REGION_JAPANESE,
  MIDI_REGION_OTHER,
  MIDI_REGION_SPECIAL
} midi_region_t;
midi_region_t MidiManufacturerRegion(midi_manufacturer_id_cref_t man_id);

/* Converts the manufacture ID into a string representation.
 *    XX for single byte IDs
 *    XX-XX for double byte IDs
 *  "X" is a hexidecimal value.
 *
 * Returns the number of character bytes required to serialize
 * regardless of the actual string buffer size, and not including the NULL
 * term.  Returns 0 if any inputs are NULL or if the manufacture ID is
 * invalid.
 *
 * As long as the string is not NULL or zero-sized.
 */
size_t MidiManufacturerIdToString(
  midi_manufacturer_id_cref_t man_id, char *str, size_t str_size);

/* Serializes the manufacture ID for transmission.  Returns the number
 * of bytes required for successful serialization, regardless of the
 * actual size of the data buffer.
 * Returns:
 *  1 if ID was a single byte
 *  3 if ID was two bytes (zero + 2 data bytes)
 *  0 if inputs are NULL or manufacture ID is invalid. */
size_t MidiSerializeManufacturerId(
  midi_manufacturer_id_cref_t man_id, uint8_t *data, size_t data_size);
/* Deserializes the manufacture ID.  Returns the number of bytes used
 * for deserialization.
 * Returns:
 *  1 if ID was a single byte
 *  3 if ID was two bytes (zero + 2 data bytes)
 *  0 if inputs are NULL or manufacture ID is invalid, or if input data
 *  was smaller than required for the ID. */
size_t MidiDeserializeManufacturerId(
  uint8_t const *data, size_t data_size, midi_manufacturer_id_ref_t man_id);

C_SECTION_END;

#endif  /* _MIDI_MANUFACTURER_ID_H_ */
