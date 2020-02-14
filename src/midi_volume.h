/*
 * MIDI Controller - Master Volume and Balance
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#ifndef _MIDI_VOLUME_H_
#define _MIDI_VOLUME_H_

#include "base.h"
#include "midi_bytes.h"

C_SECTION_BEGIN;

typedef uint16_t midi_master_volume_t;
typedef uint16_t midi_master_balance_t;

#define MidiIsValidMasterVolume MidiIsDataWord
#define MidiIsValidMasterBalance MidiIsDataWord

C_SECTION_END;

#endif  /* _MIDI_VOLUME_H_ */
