/*
 * MIDI Controller - System Serial
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#ifndef _SYSTEM_SERIAL_H_
#define _SYSTEM_SERIAL_H_

#include "base.h"

C_SECTION_BEGIN;

/* Initializes the serial interface, typically MIDI based. */
void SystemSerialInitialize(void);

/* Asynchronously writes and reads data from the serial port. */
size_t SystemSerialWrite(uint8_t const *data, size_t count);
size_t SystemSerialRead(uint8_t *data, size_t data_size);

/* Flushes the read buffer. */
void SystemSerialFlush(void);

C_SECTION_END;

#endif  /* _SYSTEM_SERIAL_H_ */
