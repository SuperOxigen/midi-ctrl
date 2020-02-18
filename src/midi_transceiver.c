/*
 * MIDI Controller - MIDI Tx and Rx Context.
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#include <string.h>

#include "midi_defs.h"
#include "midi_serialize.h"
#include "midi_transceiver.h"

/*
 *  Receiver Context
 */

/* Receiver is in SysEx mode, all data bytes will be consumed until
*  the EndSysEx byte. */
#define MIDI_RX_SYS_EX_MODE   0x01

bool_t MidiInitializeReceiverCtx(midi_rx_ctx_t *rx_ctx) {
  if (rx_ctx == NULL) return false;
  memset(rx_ctx->buffer, 0,  MIDI_RX_BUFFER_SIZE);
  rx_ctx->size = 0;
  rx_ctx->status = MIDI_NONE;
  rx_ctx->flags = MIDI_NONE;
  return true;
}

static size_t MidiSeekStatusInternal(
    midi_rx_ctx_t *rx_ctx, uint8_t const *data, size_t data_size) {
  /* Assume all variables are valid. |data| != NULL and |data_size| > 0. */
  rx_ctx->status = MIDI_NONE;
  rx_ctx->flags = MIDI_NONE;
  if (rx_ctx->size > 0) {
    memset(rx_ctx->buffer, 0, MIDI_RX_BUFFER_SIZE);
    rx_ctx->size = 0;
  }
  for (size_t i = 0; i < data_size;  ++i) {
    if (data[i] == MIDI_END_SYSTEM_EXCLUSIVE) continue;
    if (MidiIsStatusByte(data[i])) {
      rx_ctx->status = data[i];
      if (rx_ctx->status == MIDI_SYSTEM_EXCLUSIVE) {
        rx_ctx->flags &= ~MIDI_RX_SYS_EX_MODE;
      }
      return i + 1;
    }
  }
  /* All data was consumed, but need more. */
  return data_size + 1;
}

static size_t MidiReceiveSysExDataInternal(
    midi_rx_ctx_t *rx_ctx, uint8_t const *data, size_t data_size) {
  /* Sanity check, status should be SysEx. */
  rx_ctx->status = MIDI_SYSTEM_EXCLUSIVE;
  size_t i;
  for (i = 0; i < data_size && (rx_ctx->flags & MIDI_RX_SYS_EX_MODE); ++i) {
    if (data[i] == MIDI_END_SYSTEM_EXCLUSIVE) {
      rx_ctx->flags &= ~MIDI_RX_SYS_EX_MODE;
    } else if (!MidiIsDataByte(data[i])) {
      rx_ctx->status = MIDI_NONE;
      break;
    }
    if (rx_ctx->size >= MIDI_RX_BUFFER_SIZE) {
      rx_ctx->status = MIDI_NONE;
      break;
    }
    rx_ctx->buffer[rx_ctx->size++] = data[i];
  }
  /* If still in SysEx mode, then more data is needed. */
  if (rx_ctx->flags & MIDI_RX_SYS_EX_MODE) return data_size + 1;
  return i;
}

static size_t MidiReceiverDeserializeMessage(
    midi_rx_ctx_t *rx_ctx, midi_message_t *message) {
  size_t const res = MidiDeserializeMessage(
      rx_ctx->buffer, rx_ctx->size, rx_ctx->status, message);
  if (res == 0 || res > MIDI_RX_BUFFER_SIZE) {
    /* Either no data required, error, or message is beyond buffer data
     * limit (unlikely).  In any case, the receiver status needs to be
     * cleared.  If the message doesn't require any data, then status-run
     * should be prevented. */
    rx_ctx->status = MIDI_NONE;
    if (rx_ctx->size > 0) {
      memset(rx_ctx->buffer, 0, MIDI_RX_BUFFER_SIZE);
      rx_ctx->size = 0;
    }
    if (res > MIDI_RX_BUFFER_SIZE) {
      message->type = MIDI_NONE;
    }
  } else if (res <= rx_ctx->size) {
    /* Complete */
    for (size_t i = 0; i < (rx_ctx->size - res); ++i) {
      rx_ctx->buffer[i] = rx_ctx->buffer[i + res];
    }
    rx_ctx->size -= res;
  } else {
    /* Incomplete */
    message->type = MIDI_NONE;
  }
  return (res > MIDI_RX_BUFFER_SIZE) ? 0 : res;
}

static size_t MidiReceiveDataInternal(
    midi_rx_ctx_t *rx_ctx, uint8_t const *data, size_t data_size,
    midi_message_t *message) {
  /* Assume all variables are valid. |data| != NULL and |data_size| > 0. */
  size_t di = 0;
  if (rx_ctx->status == MIDI_NONE) {
    size_t const res = MidiSeekStatusInternal(rx_ctx, data, data_size);
    if (res > data_size) {
      return res;
    }
    di += res;
  }
  /* If in SysEx mode, seek for EndSysEx. */
  if (rx_ctx->flags & MIDI_RX_SYS_EX_MODE) {
    size_t const res = MidiReceiveSysExDataInternal(
        rx_ctx, &data[di], data_size - di);
    if (res > (data_size - di)) {
      return di + res;
    }
    di += res;
  }

  if (rx_ctx->status == MIDI_NONE) {
    return di;
  }

  size_t const res = MidiReceiverDeserializeMessage(rx_ctx, message);
  if (res == 0 || message->type != MIDI_NONE) {
    return di;
  }

  while (di < data_size && rx_ctx->size < res) {
    if (!MidiIsDataByte(data[di])) {
      rx_ctx->status = MIDI_NONE;
      return di;
    }
    rx_ctx->buffer[rx_ctx->size++] = data[di++];
  }
  return di + (res - rx_ctx->size);
}

size_t MidiReceiveData(
    midi_rx_ctx_t *rx_ctx,
    uint8_t const *data, size_t data_size,
    midi_message_t *message) {
  if (rx_ctx == NULL || message == NULL) return 0;
  if (data == NULL && data_size > 0) return 0;
  message->type = MIDI_NONE;  /* All messages start clear. */

  /* If data size is zero, then determine the number of bytes required
   * to get the next message, or get a better estimate on number of bytes
   * needed for the message. */
  if (data_size == 0) {
    if (rx_ctx->status == MIDI_NONE) return 1;
    size_t const res = MidiDeserializeMessage(
        rx_ctx->buffer, rx_ctx->size, rx_ctx->status, message);
    if (res > rx_ctx->size) {
      message->type = MIDI_NONE;
    }
    if (res == 0) {
      rx_ctx->status = MIDI_NONE;
      return (message->type == MIDI_NONE) ? 1 : 0;
    }
    return res - rx_ctx->size;
  }

  size_t data_used = 0;
  while (data_used <= data_size && message->type == MIDI_NONE) {
    size_t const res = MidiReceiveDataInternal(
        rx_ctx, &data[data_used], data_size - data_used, message);
    data_used += res;
  }
  return data_used;
}

/*
 *  Transmitter Context
 */
#define MIDI_TX_STATUS_RUN_ENABLED 0x01

bool_t MidiInitializeTransmitterCtx(midi_tx_ctx_t *tx_ctx, bool_t status_run) {
  if (tx_ctx == NULL) return false;
  tx_ctx->last_status = MIDI_NONE;
  tx_ctx->flags = status_run ? MIDI_TX_STATUS_RUN_ENABLED : 0;
  return true;
}

size_t MidiTransmitterSerializeMessage(
    midi_tx_ctx_t *tx_ctx, midi_message_t const *message,
    uint8_t *data, size_t data_size) {
  if (tx_ctx == NULL || message == NULL || data == NULL) return 0;
  if (!MidiIsValidMessage(message)) return 0;
  midi_status_t const status = MidiMessageStatus(message);
  bool_t const skip_status =
      status == tx_ctx->last_status &&
      (tx_ctx->flags & MIDI_TX_STATUS_RUN_ENABLED);
  size_t const data_used = MidiSerializeMessage(
      message, skip_status, data, data_size);
  if (data_used <= data_size) {
    tx_ctx->last_status = status;
  }
  return data_used;
}

size_t MidiTransmitterSerializeMessages(
    midi_tx_ctx_t *tx_ctx,
    midi_message_t const *messages, size_t message_count,
    uint8_t *data, size_t data_size) {
  if (tx_ctx == NULL || messages == NULL ||
      data == NULL || message_count == 0) return 0;
  size_t mi = 0, di = 0;
  size_t total_message_size = 0;
  while (mi < message_count) {
    size_t const message_size = MidiTransmitterSerializeMessage(
        tx_ctx, &messages[mi++], &data[di], di - data_size);
    if (message_size == 0) continue;
    total_message_size += message_size;
    if (total_message_size >= data_size) {
      di = data_size;
    } else {
      di += message_size;
    }
  } /* while */
  return total_message_size;
}
