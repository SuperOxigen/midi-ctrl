/*
 * MIDI Controller - MIDI Callbacks
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#include <string.h>

#include "midi_callback.h"
#include "midi_callback_internal.h"
#include "midi_defs.h"

bool_t MidiInitializeCallbacks(midi_callbacks_t *callbacks) {
  if (callbacks == NULL) return false;
  memset(callbacks, 0, sizeof(midi_callbacks_t));
  callbacks->next_event_id = 1;
  callbacks->rx.next_rx_event_id = 1;
  callbacks->rx.next_sys_ex_rx_event_id = 1;
  callbacks->tx.next_tx_event_id = 1;
  return true;
}

bool_t MidiIsValidCallbacks(midi_callbacks_t const *callbacks) {
  if (callbacks == NULL) return false;
  if (callbacks->next_event_id == 0) return false;
  midi_rx_callbacks_t const *rx = &callbacks->rx;
  if (rx->next_rx_event_id == 0) return false;
  if (rx->next_sys_ex_rx_event_id == 0) return false;
  midi_tx_callbacks_t const *tx = &callbacks->tx;
  if (tx->next_tx_event_id == 0) return false;
  return true;
}

static void MidiIncrementEventCounter(uint32_t *counter) {
  ++(*counter);
  if (*counter == 0) ++(*counter);
}

static bool_t MidiCallSysExMessageCallback(
    midi_callbacks_t const *callbacks, midi_time_t const *time,
    midi_message_t const *message) {
  midi_rx_callbacks_t const *rx = &callbacks->rx;
  midi_sys_ex_t const *sys_ex = &message->sys_ex;
  midi_sys_ex_rx_event_t sys_ex_event = {
    .rx_event = {
      .general = {
        .event_id = callbacks->next_event_id,
        .time = time
      },
      .rx_event_id = rx->next_rx_event_id,
      .message = message,
      .user_ctx = NULL
    },
    .sys_ex_rx_event_id = rx->next_sys_ex_rx_event_id,
    .device_id = sys_ex->device_id,
    .sys_ex = sys_ex
  };
  midi_rx_event_t *rx_event = &sys_ex_event.rx_event;

  if (rx->OnMessage) {
    rx_event->user_ctx = rx->message_ctx;
    rx->OnMessage(rx_event);
  }

  if (rx->OnSysExMessage) {
    rx_event->user_ctx = rx->sys_ex_message_ctx;
    rx->OnSysExMessage(&sys_ex_event);
  }

  if (sys_ex->id[0] == MIDI_NON_REAL_TIME_ID) {
    switch (sys_ex->sub_id) {
      case MIDI_EOF: {
        if (rx->OnEof != NULL) {
          rx_event->user_ctx = rx->handshake_ctx;
          rx->OnEof(&sys_ex_event);
        }
        return true;
      }
      case MIDI_WAIT: {
        if (rx->OnWait != NULL) {
          rx_event->user_ctx = rx->handshake_ctx;
          rx->OnWait(&sys_ex_event, sys_ex->packet_number);
        }
        return true;
      }
      case MIDI_CANCEL: {
        if (rx->OnCancel != NULL) {
          rx_event->user_ctx = rx->handshake_ctx;
          rx->OnCancel(&sys_ex_event, sys_ex->packet_number);
        }
        return true;
      }
      case MIDI_NAK: {
        if (rx->OnNak != NULL) {
          rx_event->user_ctx = rx->handshake_ctx;
          rx->OnNak(&sys_ex_event, sys_ex->packet_number);
        }
        return true;
      }
      case MIDI_ACK: {
        if (rx->OnAck != NULL) {
          rx_event->user_ctx = rx->handshake_ctx;
          rx->OnAck(&sys_ex_event, sys_ex->packet_number);
        }
        return true;
      }
    }
  } else if (sys_ex->id[0] == MIDI_REAL_TIME_ID) {

  }
  return false;
}

static bool_t MidiCallMessageCallback(
    midi_callbacks_t const *callbacks, midi_time_t const *time,
    midi_message_t const *message) {
  midi_rx_callbacks_t const *rx = &callbacks->rx;
  midi_rx_event_t rx_event = {
    .general = {
      .event_id = callbacks->next_event_id,
      .time = time
    },
    .rx_event_id = rx->next_rx_event_id,
    .message = message,
    .user_ctx = NULL
  };

  if (rx->OnMessage) {
    rx_event.user_ctx = rx->message_ctx;
    rx->OnMessage(&rx_event);
  }

  switch (message->type) {
    case MIDI_NOTE_OFF: {
      if (rx->OnNoteOff != NULL) {
        rx_event.user_ctx = rx->note_ctx;
        rx->OnNoteOff(&rx_event, message->channel, &message->note);
      }
      return true;
    }
    case MIDI_NOTE_ON: {
      if (rx->OnNoteOn != NULL) {
        rx_event.user_ctx = rx->note_ctx;
        rx->OnNoteOn(&rx_event, message->channel, &message->note);
      }
      return true;
    }
    case MIDI_KEY_PRESSURE: {
      if (rx->OnKeyPressure != NULL) {
        rx_event.user_ctx = rx->note_ctx;
        rx->OnKeyPressure(&rx_event, message->channel, &message->note);
      }
      return true;
    }
    case MIDI_CONTROL_CHANGE: {
      if (rx->OnControlChange != NULL) {
        rx_event.user_ctx = rx->control_change_ctx;
        rx->OnControlChange(&rx_event, message->channel, &message->control);
      }
      return true;
    }
    case MIDI_PROGRAM_CHANGE: {
      if (rx->OnProgramChange != NULL) {
        rx_event.user_ctx = rx->program_change_ctx;
        rx->OnProgramChange(&rx_event, message->channel, message->program);
      }
      return true;
    }
    case MIDI_CHANNEL_PRESSURE: {
      if (rx->OnChannelPressureChange != NULL) {
        rx_event.user_ctx = rx->channel_pressure_ctx;
        rx->OnChannelPressureChange(
            &rx_event, message->channel, message->pressure);
      }
      return true;
    }
    case MIDI_PITCH_WHEEL: {
      if (rx->OnPitchWheelChange != NULL) {
        rx_event.user_ctx = rx->pitch_wheel_ctx;
        rx->OnPitchWheelChange(
            &rx_event, message->channel, message->pitch);
      }
      return true;
    }
    case MIDI_SONG_POSITION_POINTER: {
      if (rx->OnSongPosition != NULL) {
        rx_event.user_ctx = rx->song_position_ctx;
        rx->OnSongPosition(&rx_event, message->song_position);
      }
      return true;
    }
    case MIDI_SONG_SELECT: {
      if (rx->OnSongSelect != NULL) {
        rx_event.user_ctx = rx->song_select_ctx;
        rx->OnSongSelect(&rx_event, message->song_number);
      }
      return true;
    }
    case MIDI_TUNE_REQUEST: {
      if (rx->OnTuneRequest != NULL) {
        rx_event.user_ctx = rx->tune_request_ctx;
        rx->OnTuneRequest(&rx_event);
      }
      return true;
    }
    case MIDI_TIMING_CLOCK: {
      if (rx->OnTimingClock != NULL) {
        rx_event.user_ctx = rx->timing_clock_ctx;
        rx->OnTimingClock(&rx_event);
      }
      return true;
    }
    case MIDI_START: {
      if (rx->OnPlayback != NULL) {
        rx_event.user_ctx = rx->playback_ctx;
        rx->OnPlayback(&rx_event, MIDI_START);
      }
      if (rx->OnStartPlayback != NULL) {
        rx_event.user_ctx = rx->playback_ctx;
        rx->OnStartPlayback(&rx_event);
      }
      return true;
    }
    case MIDI_CONTINUE: {
      if (rx->OnPlayback != NULL) {
        rx_event.user_ctx = rx->playback_ctx;
        rx->OnPlayback(&rx_event, MIDI_CONTINUE);
      }
      if (rx->OnContinuePlayback != NULL) {
        rx_event.user_ctx = rx->playback_ctx;
        rx->OnContinuePlayback(&rx_event);
      }
      return true;
    }
    case MIDI_STOP: {
      if (rx->OnPlayback != NULL) {
        rx_event.user_ctx = rx->playback_ctx;
        rx->OnPlayback(&rx_event, MIDI_STOP);
      }
      if (rx->OnStopPlayback != NULL) {
        rx_event.user_ctx = rx->playback_ctx;
        rx->OnStopPlayback(&rx_event);
      }
      return true;
    }
    case MIDI_ACTIVE_SENSING: {
      if (rx->OnActiveSensing != NULL) {
        rx_event.user_ctx = rx->active_sensing_ctx;
        rx->OnActiveSensing(&rx_event);
      }
      return true;
    }
    case MIDI_TIME_CODE:
    case MIDI_SYSTEM_RESET:
    case MIDI_END_SYSTEM_EXCLUSIVE: {
      /* WUT? */
      return false;
    }
  }
  return false;
}

bool_t MidiCallOnMessageCallback(
    midi_callbacks_t *callbacks, midi_time_t const *time,
    midi_message_t const *message) {
  if (callbacks == NULL || message == NULL) return false;
  if (message->type == MIDI_NONE || !MidiIsValidMessageType(message->type))
    return false;

  bool_t sub_result = false;
  switch  (message->type) {
    case MIDI_SYSTEM_EXCLUSIVE: {
      sub_result = MidiCallSysExMessageCallback(callbacks, time, message);
      if (sub_result) {
        MidiIncrementEventCounter(&callbacks->rx.next_sys_ex_rx_event_id);
      }
    } break;
    case MIDI_SYSTEM_RESET:
      /* System reset MUST be handled by MidiCallOnSystemResetCallback(). */
      return false;
    default: {
      sub_result = MidiCallMessageCallback(callbacks, time, message);
    }
  }

  if (sub_result) {
    MidiIncrementEventCounter(&callbacks->next_event_id);
    MidiIncrementEventCounter(&callbacks->rx.next_rx_event_id);
  }
  return sub_result;
}

bool_t MidiCallOnTimeSynchronizeCallback(
    midi_callbacks_t *callbacks, midi_time_t const *time,
    midi_time_direction_t direction) {
  if (callbacks == NULL || time == NULL) return false;
  midi_rx_callbacks_t *rx = &callbacks->rx;
  if (rx->OnTimeUpdate != NULL) {
    midi_rx_event_t const rx_event = {
      .general = {
        .event_id = callbacks->next_event_id,
        .time = time
      },
      .rx_event_id = rx->next_rx_event_id,
      .message = NULL,
      .user_ctx = rx->time_update_ctx
    };
    rx->OnTimeUpdate(&rx_event, direction);
  }
  MidiIncrementEventCounter(&callbacks->next_event_id);
  MidiIncrementEventCounter(&rx->next_rx_event_id);
  return true;
}

bool_t MidiCallOnSystemResetCallback(
    midi_callbacks_t *callbacks, midi_time_t const *time,
    midi_message_t const *message, bool_t *soft_reset) {
  if (callbacks == NULL || message == NULL || soft_reset == NULL) return false;
  if (message->type != MIDI_SYSTEM_RESET) return false;
  midi_rx_callbacks_t *rx = &callbacks->rx;
  midi_rx_event_t rx_event = {
    .general = {
      .event_id = callbacks->next_event_id,
      .time = time
    },
    .rx_event_id = rx->next_rx_event_id,
    .message = message,
    .user_ctx = NULL
  };

  if (rx->OnMessage) {
    rx_event.user_ctx = rx->message_ctx;
    rx->OnMessage(&rx_event);
  }

  if (rx->OnSystemReset != NULL) {
    rx_event.user_ctx = rx->system_reset_ctx;
    rx->OnSystemReset(&rx_event, soft_reset);
  }
  MidiIncrementEventCounter(&callbacks->next_event_id);
  MidiIncrementEventCounter(&rx->next_rx_event_id);
  return true;
}
