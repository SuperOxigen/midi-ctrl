/*
 * MIDI Controller - System Storage
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#ifndef _SYSTEM_STORAGE_H_
#define _SYSTEM_STORAGE_H_

#include "base.h"

/* Include System Specific. */

C_SECTION_BEGIN;

/*
 *  Non-Volatile Memory
 *    All of these are platform specific.
 */
/* Returns true if platform has persistent storage. */
size_t SystemStorageSize(void);
void SystemStorageInitialize(void);

bool_t SystemStorageIsReady(void);

/* Returns true if a call to SystemWriteStorage() will result in at
 * least one byte. */
bool_t SystemStorageWrite(uint16_t address, uint8_t byte);
bool_t SystemStorageWriteBuffer(
  uint16_t address, uint8_t const *buffer, size_t size);

bool_t SystemStorageRead(uint16_t address, uint8_t *byte);
bool_t SystemStorageReadBuffer(
  uint16_t address, uint8_t *buffer, size_t count);

C_SECTION_END;

#endif  /* _SYSTEM_STORAGE_H_ */
