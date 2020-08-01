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

static uint8_t gNativeStorage[NATIVE_STORAGE_SIZE];
static uint16_t gNativeStorageAddress = 0x0000;
static bool_t gNativeStorageInitialized = false;

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
  if (!gNativeStorageInitialized) {
    memset(gNativeStorage, 0, NATIVE_STORAGE_SIZE);
    gNativeStorageInitialized = true;
  }
}

/* Storage Write. */

bool_t SystemStorageWriteReady(void) {
  return gNativeStorageInitialized;
}

bool_t SystemStorageWrite(
    uint16_t address, uint8_t data) {
  if (address >= NATIVE_STORAGE_SIZE || !gNativeStorageInitialized)
    return false;
  gNativeStorageAddress = address & NATIVE_STORAGE_MASK;
  gNativeStorage[gNativeStorageAddress] = data;
  return true;
}

bool_t SystemStorageIsWriting(void) {
  return false;
}

/* Storage Read. */

bool_t SystemStorageReadReady(void) {
  return gNativeStorageInitialized;
}

bool_t SystemStorageStartRead(uint16_t address) {
  if (!gNativeStorageInitialized) return false;
  gNativeStorageAddress = address & NATIVE_STORAGE_MASK;
  return true;
}

bool_t SystemStorageIsReading(void) {
  return false;
}

bool_t SystemStorageGetByte(uint8_t *byte) {
  if (byte == NULL || !gNativeStorageInitialized) return false;
  *byte = gNativeStorage[gNativeStorageAddress];
  return true;
}

#endif  /* _PLATFORM_NATIVE */
