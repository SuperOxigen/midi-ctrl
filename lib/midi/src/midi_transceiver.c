/*
 * MIDI Controller - MIDI Tx and Rx Context.
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#include <string.h>

#include "logging.h"

#include "midi_defs.h"
#include "midi_serialize.h"
#include "midi_transceiver.h"

#ifdef _RX_TRACE_ENABLED
# define LOG_RX_TRACE(...) LOG_TRACE(__VA_ARGS__)
#else
# define LOG_RX_TRACE(...)
#endif

#ifdef _RX_LOG_ENABLED
# define LOG_RX_DEBUG(...) LOG_DEBUG(__VA_ARGS__)
#else
# define LOG_RX_DEBUG(...)
#endif

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
  LOG_RX_TRACE("rx_ctx = %p, data = %p, data_size = %zu",
               rx_ctx, data, data_size);
  LOG_RX_DEBUG("rx_ctx->size = %zu", rx_ctx->size);
  LOG_RX_DEBUG("rx_ctx->status = 0x%02x", rx_ctx->status);
  LOG_RX_DEBUG("rx_ctx->flags = %s",
               (rx_ctx->flags & MIDI_RX_SYS_EX_MODE) ?
               "MIDI_RX_SYS_EX_MODE" : "0");
  /* Assume all variables are valid. |data| != NULL and |data_size| > 0. */
  rx_ctx->status = MIDI_NONE;
  rx_ctx->flags = MIDI_NONE;
  if (rx_ctx->size > 0) {
    LOG_RX_DEBUG("Clearing receiver buffer");
    memset(rx_ctx->buffer, 0, MIDI_RX_BUFFER_SIZE);
    rx_ctx->size = 0;
  }
  for (size_t i = 0; i < data_size;  ++i) {
    if (data[i] == MIDI_END_SYSTEM_EXCLUSIVE) continue;
    if (MidiIsStatusByte(data[i])) {
      rx_ctx->status = data[i];
      LOG_RX_DEBUG("Status found: data[%zu] = 0x%02x", i, data[i]);
      if (rx_ctx->status == MIDI_SYSTEM_EXCLUSIVE) {
        LOG_RX_DEBUG("Starting SysEx mode");
        rx_ctx->flags |= MIDI_RX_SYS_EX_MODE;
      }
      return i + 1;
    }
  }
  /* All data was consumed, but need more. */
  LOG_RX_DEBUG("No status byte found");
  return data_size + 1;
}

static size_t MidiReceiveSysExDataInternal(
    midi_rx_ctx_t *rx_ctx, uint8_t const *data, size_t data_size) {
  LOG_RX_TRACE("rx_ctx = %p, data = %p, data_size = %zu",
               rx_ctx, data, data_size);
  LOG_RX_DEBUG("rx_ctx->size = %zu", rx_ctx->size);
  LOG_RX_DEBUG("rx_ctx->status = 0x%02x", rx_ctx->status);
  LOG_RX_DEBUG("rx_ctx->flags = %s",
      (rx_ctx->flags & MIDI_RX_SYS_EX_MODE) ? "MIDI_RX_SYS_EX_MODE" : "0");
  /* Sanity check, status should be SysEx. */
  rx_ctx->status = MIDI_SYSTEM_EXCLUSIVE;
  size_t i;
  for (i = 0; i < data_size && (rx_ctx->flags & MIDI_RX_SYS_EX_MODE); ++i) {
    if (data[i] == MIDI_END_SYSTEM_EXCLUSIVE) {
      LOG_RX_DEBUG("Ending SysEx mode: index = %zu", i);
      rx_ctx->flags &= ~MIDI_RX_SYS_EX_MODE;
    } else if (!MidiIsDataByte(data[i])) {
      LOG_RX_DEBUG("Non-data byte in SysEx: data[%zu] = 0x%02x", i, data[i]);
      rx_ctx->status = MIDI_NONE;
      break;
    }
    if (rx_ctx->size >= MIDI_RX_BUFFER_SIZE) {
      LOG_RX_DEBUG("Receiver overflow in SysEx: index = %zu", i);
      rx_ctx->status = MIDI_NONE;
      break;
    }
    rx_ctx->buffer[rx_ctx->size++] = data[i];
  }
  /* If still in SysEx mode, then more data is needed. */
  if (rx_ctx->flags & MIDI_RX_SYS_EX_MODE) return data_size + 1;
  LOG_RX_DEBUG("SysEx complete: rx_ctx->size = %zu", rx_ctx->size);
  return i;
}

static size_t MidiReceiverDeserializeMessage(
    midi_rx_ctx_t *rx_ctx, midi_message_t *message) {
  LOG_RX_TRACE("rx_ctx = %p, message = %p", rx_ctx, message);
  LOG_RX_DEBUG("rx_ctx->size = %zu", rx_ctx->size);
  LOG_RX_DEBUG("rx_ctx->status = 0x%02x", rx_ctx->status);
  LOG_RX_DEBUG("rx_ctx->flags = %s",
      (rx_ctx->flags & MIDI_RX_SYS_EX_MODE) ? "MIDI_RX_SYS_EX_MODE" : "0");
  size_t const res = MidiDeserializeMessage(
      rx_ctx->buffer, rx_ctx->size, rx_ctx->status, message);
  if (res == 0 || res > MIDI_RX_BUFFER_SIZE) {
    /* Either no data required, error, or message is beyond buffer data
     * limit (unlikely).  In any case, the receiver status needs to be
     * cleared.  If the message doesn't require any data, then status-run
     * should be prevented. */
    rx_ctx->status = MIDI_NONE;
    if (rx_ctx->size > 0) {
      LOG_RX_DEBUG("Clearing receiver buffer");
      memset(rx_ctx->buffer, 0, MIDI_RX_BUFFER_SIZE);
      rx_ctx->size = 0;
    }
    if (res > MIDI_RX_BUFFER_SIZE) {
      LOG_RX_DEBUG("Deserialization overflow: required_size = %zu", res);
      message->type = MIDI_NONE;
    }
  } else if (res <= rx_ctx->size) {
    /* Complete */
    LOG_RX_DEBUG("Shifting buffer: shift = %zu, new_size = %zu",
        res, rx_ctx->size - res);
    for (size_t i = 0; i < (rx_ctx->size - res); ++i) {
      rx_ctx->buffer[i] = rx_ctx->buffer[i + res];
    }
    rx_ctx->size -= res;
    if (rx_ctx->status == MIDI_SYSTEM_EXCLUSIVE) {
      LOG_RX_DEBUG("Force clearing SysEx status");
      rx_ctx->status = MIDI_NONE;
    }
  } else {
    /* Incomplete */
    LOG_RX_DEBUG("Message incomplete: required_size = %zu", res);
    message->type = MIDI_NONE;
  }
  return (res > MIDI_RX_BUFFER_SIZE) ? 0 : res;
}

static size_t MidiReceiveDataInternal(
    midi_rx_ctx_t *rx_ctx, uint8_t const *data, size_t data_size,
    midi_message_t *message) {
  LOG_RX_TRACE("rx_ctx = %p, data = %p, data_size = %zu, message = %p",
      rx_ctx, data, data_size, message);
  LOG_RX_DEBUG("rx_ctx->size = %zu", rx_ctx->size);
  LOG_RX_DEBUG("rx_ctx->status = 0x%02x", rx_ctx->status);
  LOG_RX_DEBUG("rx_ctx->flags = %s",
      (rx_ctx->flags & MIDI_RX_SYS_EX_MODE) ? "MIDI_RX_SYS_EX_MODE" : "0");
  /* Assume all variables are valid. |data| != NULL and |data_size| > 0. */
  size_t di = 0;
  if (rx_ctx->status == MIDI_NONE) {
    size_t const res = MidiSeekStatusInternal(rx_ctx, data, data_size);
    if (res > data_size) {
      LOG_RX_DEBUG("Incomplete seek: res = %zu", res);
      return res;
    }
    LOG_RX_DEBUG("Increament data index: prev_index = %zu, post_index = %zu",
        di, di + res);
    di += res;
  }
  /* If in SysEx mode, seek for EndSysEx. */
  if (rx_ctx->flags & MIDI_RX_SYS_EX_MODE) {
    size_t const res = MidiReceiveSysExDataInternal(
        rx_ctx, &data[di], data_size - di);
    if (res > (data_size - di)) {
      LOG_RX_DEBUG("Incomplete SysEx: res = %zu", di + res);
      return di + res;
    }
    LOG_RX_DEBUG("Increament data index: prev_index = %zu, post_index = %zu",
        di, di + res);
    di += res;
  }

  if (rx_ctx->status == MIDI_NONE) {
    LOG_RX_DEBUG("No receiver status: data_used = %zu", di);
    return di;
  }

  size_t const res = MidiReceiverDeserializeMessage(rx_ctx, message);
  if (res == 0 || message->type != MIDI_NONE) {
    if (message->type != MIDI_NONE) {
      LOG_RX_DEBUG("Deserialization complete: data_used = %zu", di);
    }
    return di;
  }

  LOG_RX_DEBUG("Consuming data: required_data = %zu, available_data = %zu",
      res - rx_ctx->size, data_size - di);
  while (di < data_size && rx_ctx->size < res) {
    if (!MidiIsDataByte(data[di])) {
      LOG_RX_DEBUG("Unexpected non-data byte: data[%zu] = 0x%02x",
                   di, data[di]);
      rx_ctx->status = MIDI_NONE;
      return di;
    }
    rx_ctx->buffer[rx_ctx->size++] = data[di++];
  }
  LOG_RX_DEBUG("Return: data_used = %zu, required_data = %zu",
      di, res - rx_ctx->size);
  return di + (res - rx_ctx->size);
}

size_t MidiReceiveData(
    midi_rx_ctx_t *rx_ctx,
    uint8_t const *data, size_t data_size,
    midi_message_t *message) {
  LOG_RX_TRACE("rx_ctx = %p, data = %p, data_size = %zu, message = %p",
      rx_ctx, data, data_size, message);
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
    LOG_RX_DEBUG("Receive peak: res = %zu", res);
    if (res > rx_ctx->size) {
      message->type = MIDI_NONE;
    }
    if (res == 0) {
      return (message->type == MIDI_NONE) ? 1 : 0;
    }
    LOG_RX_DEBUG("Receiver peak: required_data = %zu", res - rx_ctx->size);
    return res - rx_ctx->size;
  }

  size_t res = 0;
  while (res <= data_size && message->type == MIDI_NONE) {
    size_t const sub_res = MidiReceiveDataInternal(
        rx_ctx, &data[res], data_size - res, message);
    res += sub_res;
  }
  LOG_RX_DEBUG("Receiver done: res = %zu", res);
  return res;
}

/*
 *  Transmitter Context
 */
/* When enabled, the status byte will be omitted between message
 * having the same status value. */
#define MIDI_TX_STATUS_RUN_ENABLED  0x01

#define MidiTransmitterRunEnabled(tx_ctx) \
  (tx_ctx->flags & MIDI_TX_STATUS_RUN_ENABLED)

bool_t MidiInitializeTransmitterCtx(midi_tx_ctx_t *tx_ctx, bool_t status_run) {
  if (tx_ctx == NULL) return false;
  tx_ctx->status = MIDI_NONE;
  tx_ctx->flags = status_run ? MIDI_TX_STATUS_RUN_ENABLED : 0;
  return true;
}

static size_t MidiTransmitterSerializeMessageInternal(
    midi_tx_ctx_t *tx_ctx, midi_message_t const *message,
    uint8_t *data, size_t data_size) {
  if (!MidiIsValidMessage(message)) return 0;
  midi_status_t const status = MidiMessageStatus(message);
  bool_t const skip_status =
      MidiTransmitterRunEnabled(tx_ctx) &&
      status == tx_ctx->status;
  size_t const data_used = MidiSerializeMessage(
      message, skip_status, data, data_size);
  if (MidiTransmitterRunEnabled(tx_ctx) &&
      status != MIDI_SYSTEM_EXCLUSIVE &&
      status != MIDI_END_SYSTEM_EXCLUSIVE &&
      data_used > (skip_status ? 0 : 1)) {
    tx_ctx->status = status;
  } else {
    tx_ctx->status = MIDI_NONE;
  }
  return data_used;
}

size_t MidiTransmitterSerializeMessage(
    midi_tx_ctx_t *tx_ctx, midi_message_t const *message,
    uint8_t *data, size_t data_size) {
  if (tx_ctx == NULL) return 0;
  if (data == NULL && data_size > 0) return 0;
  return MidiTransmitterSerializeMessageInternal(
      tx_ctx, message, data, data_size);
}

size_t MidiTransmitterSerializeMessages(
    midi_tx_ctx_t *tx_ctx,
    midi_message_t const *messages, size_t message_count,
    uint8_t *data, size_t data_size) {
  if (tx_ctx == NULL) return 0;
  if (data == NULL && data_size > 0) return 0;
  if (messages == NULL || message_count == 0) return 0;
  size_t mi = 0, di = 0;
  size_t total_message_size = 0;
  while (mi < message_count) {
    uint8_t *message_data = (data == NULL) ? NULL : &data[di];
    size_t const message_size = MidiTransmitterSerializeMessageInternal(
        tx_ctx, &messages[mi++], message_data, data_size - di);
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
