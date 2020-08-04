/*
 * MIDI Controller - System - Native
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#ifdef _PLATFORM_NATIVE

#include <errno.h>
#include <string.h>
#include <time.h>

#include "logging.h"

#include "system_interrupt.h"
#include "system_storage.h"
#include "system_time.h"

#ifndef NATIVE_STORAGE_SIZE
#define NATIVE_STORAGE_SIZE 1024u
#endif

#define NATIVE_STORAGE_MASK (NATIVE_STORAGE_SIZE - 1)

static uint8_t sNativeStorage[NATIVE_STORAGE_SIZE];
static bool_t sNativeStorageInitialized = false;

/* System Time. */
bool_t SystemTimeNow(system_time_t *system_time) {
  if (system_time == NULL) return false;
  struct timespec ts;
  if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
    int const err = errno;
    if (err == EINVAL) {
      LOG_ERROR("Linux CLOCK_MONOTONIC is unavailable");
    } else {
      LOG_ERROR("Unknown error: errno = %d", err);
    }
    return false;
  }
  system_time->seconds = ts.tv_sec;
  system_time->nanoseconds = ts.tv_nsec;
  LOG_DEBUG(
      "Now: time = %u.%09u", system_time->seconds, system_time->nanoseconds);
  return true;
}

void SystemTimeInitialize(void) {}

/* Interrupts */
static bool_t gSignalEnabled = true;

void SystemDisableInterrupt(void) {
  if (!gSignalEnabled) return;
  gSignalEnabled = false;
}

void SystemEnableInterrupt(void) {
  if (gSignalEnabled) return;
  gSignalEnabled = true;
}

/* System Storage. */

size_t SystemStorageSize(void) {
  return NATIVE_STORAGE_SIZE;
}

void SystemStorageInitialize(void) {
  if (!sNativeStorageInitialized) {
    memset(sNativeStorage, 0, NATIVE_STORAGE_SIZE);
    sNativeStorageInitialized = true;
  }
}

bool_t SystemStorageIsReady(void) {
  return sNativeStorageInitialized;
}

bool_t SystemStorageWrite(uint16_t address, uint8_t byte) {
  if (address >= NATIVE_STORAGE_SIZE || !sNativeStorageInitialized)
    return false;
  sNativeStorage[address] = byte;
  return true;
}

bool_t SystemStorageWriteBuffer(
    uint16_t address, uint8_t const *buffer, size_t size) {
  if (!sNativeStorageInitialized) return false;
  if (address >= NATIVE_STORAGE_SIZE || buffer == NULL || size == 0)
    return false;
  for (size_t i = 0; i < size; ++i) {
    sNativeStorage[(address + i) & NATIVE_STORAGE_MASK] = buffer[i];
  }
  return true;
}

bool_t SystemStorageRead(uint16_t address, uint8_t *byte) {
  if (!sNativeStorageInitialized) return false;
  if (address >= NATIVE_STORAGE_SIZE || byte == NULL)
  *byte = sNativeStorage[address];
  return true;
}

bool_t SystemStorageReadBuffer(
    uint16_t address, uint8_t *buffer, size_t count) {
  if (!sNativeStorageInitialized) return false;
  if (address >= NATIVE_STORAGE_SIZE || buffer == NULL || count == 0)
    return false;
  for (size_t i = 0; i < count; ++i) {
    buffer[i] = sNativeStorage[(address + i) & NATIVE_STORAGE_MASK];
  }
  return true;
}

#endif  /* _PLATFORM_NATIVE */
