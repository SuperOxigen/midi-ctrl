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

/* Returns true if a call to SystemWriteStorage() will result in at
 * least one byte. */
bool_t SystemStorageWriteReady(void);
bool_t SystemStorageWrite(
  uint16_t address, uint8_t data);
bool_t SystemStorageIsWriting(void);

bool_t SystemStorageReadReady(void);
bool_t SystemStorageStartRead(uint16_t address);
bool_t SystemStorageIsReading(void);
bool_t SystemStorageGetByte(uint8_t *byte);

C_SECTION_END;

#endif  /* _SYSTEM_STORAGE_H_ */
