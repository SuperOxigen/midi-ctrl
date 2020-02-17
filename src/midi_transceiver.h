/*
 * MIDI Controller - MIDI Tx and Rx Context.
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#ifndef _MIDI_TRANSCEIVER_H_
#define _MIDI_TRANSCEIVER_H_

#include "base.h"
#include "midi_message.h"

C_SECTION_BEGIN;

#define MIDI_RX_BUFFER_SIZE 128

/*
 *  Receiver Context
 */
typedef struct {
  /* Buffer only contains data bytes. */
  uint8_t buffer[MIDI_RX_BUFFER_SIZE];
  size_t size;
  /* The current status type being processed.. */
  midi_status_t status;
  uint8_t flags;
} midi_rx_ctx_t;

bool_t MidiInitializeReceiverCtx(midi_rx_ctx_t *rx_ctx);
/* Will consume bytes from |data| until the first full message can be
 * formed, or |data_size| is reached.
 * Returns values:
 *   <= |data_size| - Successfully received message.  Consumed |data_size|
 *                    bytes from input.  If 0, check message type to
 *                    determine if successfully deserialized.
 *   >  |data_size| - Consumed all data, but message is not complete.
 */
size_t MidiReceiveData(
  midi_rx_ctx_t *rx_ctx, uint8_t const *data, size_t data_size,
  midi_message_t *message);

/*
 *  Transmitter Context
 */
typedef struct {
  midi_status_t last_status;
  uint8_t flags;
} midi_tx_ctx_t;

bool_t MidiInitializeTransmitterCtx(midi_tx_ctx_t *tx_ctx, bool_t status_run);
size_t MidiTransmitterSerializeMessage(
  midi_tx_ctx_t *tx_ctx, midi_message_t const *message,
  uint8_t *data, size_t data_size);
size_t MidiTransmitterSerializeMessages(
  midi_tx_ctx_t *tx_ctx, midi_message_t const *messages, size_t message_count,
  uint8_t *data, size_t data_size);

C_SECTION_END;

#endif  /* _MIDI_TRANSCEIVER_H_ */
