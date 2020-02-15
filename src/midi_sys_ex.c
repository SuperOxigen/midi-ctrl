/*
 * MIDI Controller - MIDI System Exclusive.
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#include <string.h>

#include "midi_bytes.h"
#include "midi_defs.h"
#include "midi_sys_ex.h"

/*
 * MIDI System Exclusive
 */

static bool_t MidiIsValidNonRealtimeSubId(uint8_t sub_id) {
  if (!MidiIsDataByte(sub_id)) return false;
  switch (sub_id) {
    case MIDI_NONE:
      return false;
    case MIDI_DUMP_HEADER:
    case MIDI_DUMP_REQUEST:
    case MIDI_DATA_PACKET:
    case MIDI_SAMPLE_DUMP:
    case MIDI_GENERAL_INFO:
    case MIDI_GENERAL_MIDI:
    case MIDI_EOF:
    case MIDI_WAIT:
    case MIDI_CANCEL:
    case MIDI_NAK:
    case MIDI_ACK:
      return true;
    /* Not supported */
    case MIDI_FILE_DUMP:
    case MIDI_TUNING_DUMP:
      return false;
  }
  return false;
}

static bool_t MidiIsValidRealtimeSubId(uint8_t sub_id) {
  if (!MidiIsDataByte(sub_id)) return false;
  switch (sub_id) {
    case MIDI_NONE:
      return false;
    case MIDI_DEVICE_CONTROL:
      return true;
  }
  return false;
}

bool_t MidiIsSpecialSysExId(midi_manufacturer_id_cref_t id) {
  if (!MidiIsValidManufacturerId(id)) return false;
  return id[0] == MIDI_NON_REAL_TIME_ID || id[0] == MIDI_REAL_TIME_ID;
}

inline static bool_t MidiPartialIsValidSysEx(midi_sys_ex_t const *sys_ex) {
  if (sys_ex == NULL) return false;
  if (!MidiIsValidManufacturerId(sys_ex->id)) return false;
  if (!MidiIsValidDeviceId(sys_ex->device_id)) return false;
  if (!MidiIsDataByte(sys_ex->sub_id)) return false;
  return true;
}

bool_t MidiIsValidSysEx(midi_sys_ex_t const *sys_ex) {
  if (!MidiPartialIsValidSysEx(sys_ex)) return false;
  /* No special support, recognized as a valid message without data. */
  if (!MidiIsSpecialSysExId(sys_ex->id)) return true;
  if (sys_ex->id[0] == MIDI_NON_REAL_TIME_ID) switch (sys_ex->sub_id) {
    case MIDI_DUMP_HEADER:
      return MidiIsValidDumpHeader(&sys_ex->dump_header);
    case MIDI_DUMP_REQUEST:
      return MidiIsValidDumpRequest(&sys_ex->dump_request);
    case MIDI_DATA_PACKET:
      return MidiIsValidDataPacket(&sys_ex->data_packet);
    case MIDI_SAMPLE_DUMP:
      return MidiIsValidSampleDump(&sys_ex->sample_dump);
    case MIDI_GENERAL_INFO:
      return MidiIsValidDeviceInquiry(&sys_ex->device_inquiry);
    case MIDI_FILE_DUMP:
      return false;  /* Not supported */
    case MIDI_TUNING_DUMP:
      return false;  /* Not supported */
    case MIDI_GENERAL_MIDI:
      return MidiIsValidGeneralMidiMode(sys_ex->gm_mode);
    case MIDI_EOF:
    case MIDI_WAIT:
    case MIDI_CANCEL:
    case MIDI_NAK:
    case MIDI_ACK:
      return MidiIsValidPacketNumber(sys_ex->packet_number);
  }
  if (sys_ex->id[0] == MIDI_REAL_TIME_ID) switch (sys_ex->sub_id) {
    case MIDI_DEVICE_CONTROL:
      return MidiIsValidDeviceControl(&sys_ex->device_control);
  }
  return false;
}

bool_t MidiIsHandShakeSysEx(midi_sys_ex_t const *sys_ex) {
  if (!MidiPartialIsValidSysEx(sys_ex)) return false;
  return sys_ex->id[0] == MIDI_NON_REAL_TIME_ID &&
         MidiIsHandShakeSubId(sys_ex->sub_id);
}

bool_t MidiInitializeSysEx(
    midi_sys_ex_t *sys_ex, midi_manufacturer_id_cref_t man_id) {
  if (sys_ex == NULL) return NULL;
  if (man_id != NULL && !MidiIsBlankManufacturerId(man_id)) {
    if (!MidiIsValidManufacturerId(man_id)) {
      return false;
    }
  }
  memset(sys_ex, 0, sizeof(midi_sys_ex_t));
  if (man_id != NULL) {
    memcpy(sys_ex->id, man_id, sizeof(midi_manufacturer_id_t));
  }
  return true;
}

bool_t MidiHandShakeSysEx(
    midi_sys_ex_t *sys_ex, midi_device_id_t device_id,
    uint8_t handshake_sub_id, midi_packet_number_t packet_number) {
  if (!MidiIsHandShakeSubId(handshake_sub_id)) return false;
  if (!MidiIsValidPacketNumber(packet_number)) return false;
  if (!MidiInitializeSysUni(sys_ex, false, device_id, handshake_sub_id))
    return false;
  sys_ex->packet_number = packet_number;
  return true;
}

bool_t MidiGeneralMidiModeSysEx(
    midi_sys_ex_t *sys_ex, midi_device_id_t device_id, bool_t on) {
  if (sys_ex == NULL) return false;
  if (!MidiInitializeSysUni(sys_ex, false, device_id, MIDI_GENERAL_MIDI))
    return false;
  sys_ex->gm_mode = on ? MIDI_GENERAL_MIDI_ON : MIDI_GENERAL_MIDI_OFF;
  return true;
}

bool_t MidiInitializeSysUni(
    midi_sys_ex_t *sys_ex, bool_t real_time,
    midi_device_id_t device_id, uint8_t sub_id) {
  if (sys_ex == NULL) return false;
  if (!MidiIsValidDeviceId(device_id)) return false;
  memset(sys_ex, 0, sizeof(midi_sys_ex_t));
  if (real_time) {
    if (!MidiIsValidRealtimeSubId(sub_id)) return false;
    sys_ex->id[0] = MIDI_REAL_TIME_ID;
    return true;
  } else {
    if (!MidiIsValidNonRealtimeSubId(sub_id)) return false;
    sys_ex->id[0] = MIDI_NON_REAL_TIME_ID;
  }
  sys_ex->device_id = device_id;
  sys_ex->sub_id = sub_id;
  return true;
}

size_t MidiSerializeSysEx(
    midi_sys_ex_t const *sys_ex, uint8_t *data, size_t data_size) {
  if (data == NULL && data_size > 0) return 0;
  if (!MidiIsValidSysEx(sys_ex)) return 0;
  if (!MidiIsSpecialSysExId(sys_ex->id)) {
    /* Don't support serializing non-standard messages. */
    return 0;
  }
  MidiSerializeManufacturerId(sys_ex->id, data, data_size);
  if (data_size >= 2) data[1] = sys_ex->device_id;
  if (data_size >= 3) data[2] = sys_ex->sub_id;
  size_t const di = (data_size > 3) ? 3 : data_size;
  size_t sub_response = 0;
  if (sys_ex->id[0] == MIDI_NON_REAL_TIME_ID) switch (sys_ex->sub_id) {
    /* Non-Realtime */
    case MIDI_DUMP_HEADER: {
      sub_response = MidiSerializeDumpHeader(
          &sys_ex->dump_header, &data[di], data_size - di);
    } break;
    case MIDI_DUMP_REQUEST: {
      sub_response = MidiSerializeDumpRequest(
          &sys_ex->dump_request, &data[di], data_size - di);
    } break;
    case MIDI_DATA_PACKET:
      sub_response = MidiSerializeDataPacket(
          &sys_ex->data_packet, &sys_ex->device_id, &data[di], data_size - di);
      break;
    case MIDI_SAMPLE_DUMP:
      sub_response = MidiSerializeSampleDump(
          &sys_ex->sample_dump, &data[di], data_size - di);
      break;
    case MIDI_GENERAL_INFO:
      sub_response = MidiSerializeDeviceInquiry(
          &sys_ex->device_inquiry, &data[di], data_size - di);
      break;
    /* Not supported */
    case MIDI_FILE_DUMP:
    case MIDI_TUNING_DUMP:
      break;
    case MIDI_GENERAL_MIDI: {
      if (data_size >= 4) {
        data[3] = sys_ex->gm_mode;
      }
      sub_response = 1;
    } break;
    case MIDI_EOF:
    case MIDI_WAIT:
    case MIDI_CANCEL:
    case MIDI_NAK:
    case MIDI_ACK: {
      if (data_size >= 4) {
        data[3] = sys_ex->packet_number;
      }
      sub_response = 1;
    } break;
  } else if (sys_ex->id[0] == MIDI_REAL_TIME_ID) switch (sys_ex->sub_id) {
    /* Realtime */
    case MIDI_DEVICE_CONTROL:
      sub_response = MidiSerializeDeviceControl(
          &sys_ex->device_control, &data[di], data_size - di);
      break;
  }
  if (sub_response == 0) return 0;
  return sub_response + 3;
}

size_t MidiDeserializeSysEx(
    uint8_t const *data, size_t data_size, midi_sys_ex_t *sys_ex) {
  if (data == NULL && data_size > 0) return 0;
  if (sys_ex == NULL) return 0;
  memset(sys_ex, 0, sizeof(midi_sys_ex_t));
  /* Manufacturer ID */
  size_t const man_id_size = MidiDeserializeManufacturerId(
    data, data_size, sys_ex->id);
  if (man_id_size == 0 || man_id_size > data_size) return man_id_size;
  /* Device ID */
  if (data_size < (man_id_size + 1)) return man_id_size + 2;
  sys_ex->device_id = data[man_id_size];
  if (!MidiIsValidDeviceId(sys_ex->device_id)) return 0;
  /* Sub ID */
  if (data_size < (man_id_size + 2)) return man_id_size + 2;
  sys_ex->sub_id = data[man_id_size + 1];
  if (!MidiIsDataByte(sys_ex->sub_id)) return 0;
  /* Check for non-standard messages. */
  if (!MidiIsSpecialSysExId(sys_ex->id)) {
    /* For non-standard messages, return the total number of bytes before
     * the EndSysEx code. */
    size_t i;
    for (i = man_id_size + 2; i < data_size; ++i) {
      if (!MidiIsDataByte(data[i])) break;
    }
    if (i == data_size) return data_size + 1;  /*  Incomplete. */
    if (data[i] != MIDI_END_SYSTEM_EXCLUSIVE) return 0;  /* Unexpected. */
    return i;
  }
  size_t sub_response = 0;
  if (sys_ex->id[0] == MIDI_NON_REAL_TIME_ID) switch (sys_ex->sub_id) {
    /* Non-Realtime */
    case MIDI_DUMP_HEADER:
      sub_response = MidiDeserializeDumpHeader(
          &data[3], data_size - 3, &sys_ex->dump_header);
      break;
    case MIDI_DUMP_REQUEST:
      sub_response = MidiDeserializeDumpRequest(
          &data[3], data_size - 3, &sys_ex->dump_request);
      break;
    case MIDI_DATA_PACKET:
      sub_response = MidiDeserializeDataPacket(
          &data[3], data_size - 3, &sys_ex->data_packet, NULL, 0u);
      break;
    case MIDI_SAMPLE_DUMP:
      sub_response = MidiDeserializeSampleDump(
          &data[3], data_size - 3, &sys_ex->sample_dump);
      break;
    case MIDI_GENERAL_INFO:
      sub_response = MidiDeserializeDeviceInquiry(
          &data[3], data_size - 3, &sys_ex->device_inquiry);
      break;
    /* Not supported */
    case MIDI_FILE_DUMP:
    case MIDI_TUNING_DUMP:
      break;
    case MIDI_GENERAL_MIDI: {
      if (data_size >= 4) {
        sys_ex->gm_mode = data[3];
        if (!MidiIsValidGeneralMidiMode(sys_ex->gm_mode)) return 0;
      }
      sub_response = 1;
    } break;
    case MIDI_EOF:
    case MIDI_WAIT:
    case MIDI_CANCEL:
    case MIDI_NAK:
    case MIDI_ACK: {
      if (data_size >= 4) {
        sys_ex->packet_number = data[3];
        if (!MidiIsValidPacketNumber(sys_ex->packet_number)) return 0;
      }
      sub_response = 1;
    } break;
  } else if (sys_ex->id[0] == MIDI_REAL_TIME_ID) switch (sys_ex->sub_id) {
    /* Realtime */
    case MIDI_DEVICE_CONTROL:
      sub_response = MidiDeserializeDeviceControl(
          &data[3], data_size - 3, &sys_ex->device_control);
      break;
  }
  if (sub_response == 0) return 0;
  return sub_response + 3;
}
