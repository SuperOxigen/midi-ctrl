/*
 * MIDI Controller - MIDI Serialize / Deserialize Messages.
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#ifndef _MIDI_SERIALIZE_
#define _MIDI_SERIALIZE_

#include "base.h"
#include "midi_message.h"

C_SECTION_BEGIN;

/* Returns the expected data size for a given message type, excecpt
 * MIDI_SYSTEM_EXCLUSIVE, which is variable. */
size_t MidiMessageDataSize(midi_message_type_t type);

/* Serializing.
 * Returns the number of bytes required to serialize whole message.
 * If the return number of bytes is larger than |data_size|, this
 * implies that actual |data| is truncated (and probably that it
 * it should not be used).  A result of 0 likely implies an error
 * with the input arguments.
 */
size_t MidiSerializeMessage(
  midi_message_t const *message, bool_t skip_status,
  uint8_t *data, size_t data_size);

/* Special serialization for MIDI time. */
size_t MidiSerializeTimeAsPacket(
  midi_time_t const *time, midi_time_direction_t direction,
  uint8_t *data, size_t data_size);

/* Deserialize.
 * Returns the number of bytes that have been consumed in populating
 * the message.  Note that not all messages will be populated or
 * populated correctly.  Check the message type for a non-MIDI_NONE
 * value.  A result of 0 likely implies an error with the input
 * arguments, or that data is truncated.
 */
size_t MidiDeserializeMessage(
  uint8_t const *data, size_t data_size,
  midi_status_t status_override, midi_message_t *message);

C_SECTION_END;

#endif  /* _MIDI_SERIALIZE_ */
