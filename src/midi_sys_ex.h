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

/*
 * MIDI Manufacturer ID
 */

typedef enum {
  MIDI_REGION_UNKNOWN = 0,
  MIDI_REGION_AMERICA,
  MIDI_REGION_EUROPEAN,
  MIDI_REGION_JAPANESE,
  MIDI_REGION_OTHER,
  MIDI_REGION_SPECIAL
} midi_region_t;

typedef uint8_t midi_manufacturer_id_t[3];
typedef uint8_t *midi_manufacturer_id_ref_t;
typedef uint8_t const *midi_manufacturer_id_cref_t;

bool_t MidiIsValidManufacturerId(midi_manufacturer_id_cref_t man_id);
bool_t MidiIsBlankManufacturerId(midi_manufacturer_id_cref_t man_id);
bool_t MidiInitializeManufacturerId(midi_manufacturer_id_ref_t man_id);
midi_region_t MidiManufacturerRegion(midi_manufacturer_id_cref_t man_id);
bool_t MidiManufacturerIdIsUniversal(midi_manufacturer_id_cref_t man_id);

bool_t MidiIsSpecialSubId(uint8_t sub_id);

/*
 * MIDI System Exclusive
 */

typedef struct {
  midi_manufacturer_id_t id;
  uint8_t sub_id_1;
  uint8_t sub_id_2;
  /* TODO: System common messages */
} midi_sys_ex_t;

/* Unlike the other initialization functions, it is possible to call
 * this successfully and still have the output in a non-valid state.
 * This will occur if a blank or NULL |man_id| is supplied.
 */
bool_t MidiInitializeSysEx(
  midi_sys_ex_t *sys_ex, midi_manufacturer_id_cref_t man_id);

bool_t MidiIsValidSysEx(midi_sys_ex_t const *sys_ex);

#endif  /* _MIDI_SYSTEM_EXCLUSIVE_H_ */
