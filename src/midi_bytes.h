/*
 * MIDI Controller - MIDI Bytes Info
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#ifndef _MIDI_BYTES_H_
#define _MIDI_BYTES_H_

#define MidiIsStatusByte(byte) ((byte) & 0x80)    /* uint8_t -> bool_t */
#define MidiIsDataByte(byte) (!((byte) & 0x80))   /* uint8_t -> bool_t */
#define MidiIsDataWord(word) (!((word) & 0xC000))  /* uint16_t -> bool_t */

#endif  /* _MIDI_BYTES_H_ */
