/*
 * MIDI Controller - Smart String
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#ifndef _SMART_STRING_H_
#define _SMART_STRING_H_

#include "base.h"

C_SECTION_BEGIN;

#define SS_MAX_STRING_LENGTH 127
#define SS_MAX_STRING_BUFFER_SIZE (SS_MAX_STRING_LENGTH + 1)

size_t SmartStringLength(char const *src);
size_t SmartStringSet(char ch, size_t count, char *dest, size_t dest_size);
size_t SmartStringCopy(char const *src, char *dest, size_t dest_size);
size_t SmartStringAppend(char const *src, char *dest, size_t dest_size);
void SmartStringClear(char *dest, size_t dest_size);

#define SS_PADLESS      0x01
#define SS_LOWER_ALPHA  0x02
/* #define SS_SPACE_PAD    0x04 */
#define SS_ZERO_X       0x08
#define SS_SINGLE       0x10
#define SS_DOUBLE       0x20
#define SS_QUAD         0x30

size_t SmartStringHexFormat(
    uint32_t value, uint8_t flags,
    char *dest, size_t dest_size);

size_t SmartStringDecFormat(uint32_t value, char *dest, size_t dest_size);

void SmartStringSetHexEncodeCase(bool_t lower);
size_t SmartStringHexEncode(
    void const *src, size_t src_size,
    char *dest, size_t dest_size);
size_t SmartStringHexDecode(char const *src, void *dest, size_t dest_size);

C_SECTION_END;

#endif  /* _SMART_STRING_H_ */
