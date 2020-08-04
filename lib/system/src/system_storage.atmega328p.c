/*
 * MIDI Controller - System Storage - ATmega328P Platform
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#ifdef _PLATFORM_AVR
#ifndef _PLATFORM_ARDUINO

#include <avr/io.h>
#include <avr/interrupt.h>

#include "base.h"
#include "system_storage.h"

/*
 *  ATmega328 EEPROM
 *
 * EEPROM Address Register (EEAR = EEARH|EEARL)
 * EEPROM Data Register (EEDR)
 * EEPROM Control Register (EECR)
 * | --- | --- |EEPM1|EEPM0|EERIE|EEMPE| EEPE| EERE|
 *    Programming Mode (EEPM[1:0]) = 00 -> Atomic operations
 *    EEPROM Ready Interrupt Enable (EERIE) = 1 -> Enabled (when writing)
 *    Master Write Enable (EEMPE) = 1 -> Write (after writing to EEPE)
 *    Write Enable (EEPE) = 1 -> Write
 *    Read Enable (EERE) = 1 -> Read
 *
 *  |EEPM1|EEPM0| Programming Mode Bits
 *  |  0  |  0  | Erase and Write (atomic)
 *  |  0  |  1  | Erase Only
 *  |  1  |  0  | Write Only
 */

#define ATMEGA328P_STORAGE_SIZE 1024u
#define ADDRESS_MASK 0x03FF

typedef struct {
  bool_t active;
  uint16_t base_address;
  uint8_t const *buffer;
  size_t size;
  size_t idx;
} eeprom_job_t;

static eeprom_job_t sStorageJob = {};
static uint8_t sLocalByte;
static bool_t sStorageInitialized = false;

ISR(EE_READY_vect) {
  if (!sStorageJob.active) {
    /* Disable interrupt if job is done. */
    EECR &= ~_BV(EERIE);
    return;
  }
  if (sStorageJob.idx >= sStorageJob.size) {
    sStorageJob.active = false;
    sStorageJob.size = 0;
    sStorageJob.idx = 0;
    EECR &= ~_BV(EERIE);
    return;
  }
  /* Set address */
  EEAR = (uint16_t)
      ((((size_t) sStorageJob.base_address) + sStorageJob.idx) & ADDRESS_MASK);
  /* Set data */
  EEDR = sStorageJob.buffer[sStorageJob.idx];
  ++sStorageJob.idx;
  /* Enable write */
  EECR |= _BV(EEMPE);
  EECR |= _BV(EEPE);
}

size_t SystemStorageSize(void) {
  return ATMEGA328P_STORAGE_SIZE;
}

void SystemStorageInitialize(void) {
  if (sStorageInitialized) return;
  cli();
  sStorageJob.active = false;
  sStorageJob.size = 0;
  sStorageJob.idx = 0;
  sStorageInitialized = true;
  sei();
}

bool_t SystemStorageIsReady(void) {
  if (!sStorageInitialized) return false;
  cli();
  bool_t const flag = !sStorageJob.active;
  sei();
  return flag;
}

bool_t SystemStorageWrite(uint16_t address, uint8_t byte) {
  if (!sStorageInitialized) return false;
  if (address >= ATMEGA328P_STORAGE_SIZE) return false;
  cli();
  if (sStorageJob.active) {
    sei();
    return false;
  }
  sLocalByte = byte;
  sStorageJob = (eeprom_job_t) {
    .active = true,
    .base_address = address,
    .buffer = &sLocalByte,
    .size = 1,
    .idx = 0
  };
  /* Enable EEPROM ready interrupt */
  EECR |= _BV(EERIE);
  sei();
  return true;
}

bool_t SystemStorageWriteBuffer(
    uint16_t address, uint8_t const *buffer, size_t size) {
  if (!sStorageInitialized) return false;
  if (address >= ATMEGA328P_STORAGE_SIZE || buffer == NULL || size == 0)
    return false;
  cli();
  if (sStorageJob.active) {
    sei();
    return false;
  }
  sStorageJob = (eeprom_job_t) {
    .active = true,
    .base_address = address,
    .buffer = buffer,
    .size = size,
    .idx = 0
  };
  /* Enable EEPROM ready interrupt */
  EECR |= _BV(EERIE);
  sei();
  return true;
}

bool_t SystemStorageRead(uint16_t address, uint8_t *byte) {
  if (!sStorageInitialized) return false;
  if (address >= ATMEGA328P_STORAGE_SIZE || byte == NULL) return false;
  cli();
  if (sStorageJob.active) {
    sei();
    return false;
  }
  /* Set address. */
  EEAR = address;
  /* Enable read. */
  EECR |= _BV(EERE);
  /* Data is available immediately. */
  *byte = EEDR;
  sei();
  return true;
}

bool_t SystemStorageReadBuffer(
    uint16_t base_address, uint8_t *buffer, size_t count) {
  if (!sStorageInitialized) return false;
  if (base_address >= ATMEGA328P_STORAGE_SIZE || buffer == NULL || count == 0)
    return false;
  cli();
  if (sStorageJob.active) {
    sei();
    return false;
  }
  for (size_t i = 0; i < count; ++i) {
    /* Set address. */
    EEAR = (uint16_t) ((((size_t) base_address) + i) & ADDRESS_MASK);
    /* Enable read. */
    EECR |= _BV(EERE);
    /* Data is available immediately. */
    buffer[i] = EEDR;
  }
  sei();
  return true;
}

#endif  /* _PLATFORM_ARDUINO */
#endif  /* _PLATFORM_AVR */
