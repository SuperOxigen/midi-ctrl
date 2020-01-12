/*
 * MIDI Controller - Project Base
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#ifndef _MIDI_BASE_H_
#define _MIDI_BASE_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef bool bool_t;

#ifdef __cplusplus
#define C_SECTION_BEGIN extern "C" {
#define C_SECTION_END }
#define C_FUNCTION extern "C"
#else
#define C_SECTION_BEGIN
#define C_SECTION_END
#define C_FUNCTION
#endif

#endif  /* _MIDI_BASE_H_ */
