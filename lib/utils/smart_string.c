/*
 * MIDI Controller - Smart String
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#include <ctype.h>

#include "smart_string.h"

/* The max string size is used to prevent bad source buffers from
 * causing memory overflows or segfaults. */
static size_t kMaxStringLength = SS_MAX_STRING_LENGTH;

size_t SmartStringLength(char const *src) {
  if (src == NULL) return 0;
  size_t i = 0;
  while (src[i] && i < kMaxStringLength) ++i;
  if (i == kMaxStringLength) {
    /* Oh no! */
  }
  return i;
}

size_t SmartStringSet(char ch, size_t count, char *dest, size_t dest_size) {
  /* Destination should not be NULL; but whatever */
  if (dest == NULL || dest_size == 0 || !isprint(ch)) return 0;
  size_t i = 0;
  while (i < count && i < dest_size && i < kMaxStringLength) {
    dest[i++] = ch;
  }
  if (i < dest_size) {
    dest[i] = 0;
  } else {
    dest[dest_size - 1] = 0;
  }
  return count;
}

size_t SmartStringCopy(char const *src, char *dest, size_t dest_size) {
  if (src == NULL || dest == NULL || dest_size == 0) return 0;
  size_t i = 0;
  while (src[i] && i < dest_size && i < kMaxStringLength) {
    dest[i] = src[i];
    ++i;
  }
  if (i < dest_size) {
    dest[i] = 0;
  } else {
    dest[dest_size - 1] = 0;
  }
  while (src[i] && i < kMaxStringLength) ++i;
  return i;
}

size_t SmartStringAppend(char const *src, char *dest, size_t dest_size) {
  if (src == NULL || dest == NULL || dest_size == 0) return 0;
  /* Track to end of current string. */
  size_t di = 0;
  while (dest[di] && di < dest_size && di < kMaxStringLength) ++di;
  if (di == dest_size) {
    dest[dest_size - 1] = 0;
    return dest_size + SmartStringLength(src);
  }
  if (di == kMaxStringLength) {
    dest[kMaxStringLength] = 0;
    return kMaxStringLength + SmartStringLength(src);
  }
  size_t si = 0;
  while (src[si] && di < dest_size && di < kMaxStringLength) {
    dest[di++] = src[si++];
  }
  if (di < dest_size) {
    dest[di] = 0;
    return di;
  }
  dest[dest_size - 1] = 0;
  return di + SmartStringLength(&src[si]);
}

void SmartStringClear(char *dest, size_t dest_size) {
  if (dest == NULL) return;
  size_t i = 0;
  while (i < dest_size && i <= kMaxStringLength) {
    dest[i++] = 0;
  }
}

/* Formatting */
/* Hexidecimal characters used for integer to string conversions. */
static char kUpperHexCharacters[] = "0123456789ABCDEF";
static char kLowerHexCharacters[] = "0123456789abcdef";

#define SS_WIDTH_MAX  0x30
static uint32_t const kSingleMask = 0xFF;
static uint32_t const kDoubleMask = 0xFFFF;
static uint32_t const kQuadMask = 0xFFFFFFFF;
static uint32_t const kMinQuad = 0x10000;
static uint32_t const kMinDouble = 0x100;

inline static size_t HexFormatDigitWidth(uint32_t value, uint8_t flags) {
  if (flags & SS_PADLESS) {
    if (value == 0) return 1;
    size_t width = 0;
    while (value > 0 && width < 8) {
      ++width;
      value >>= 4;
    }
    return width;
  }
  switch (flags & SS_WIDTH_MAX) {
    case SS_SINGLE: return 2;
    case SS_DOUBLE: return 4;
    case SS_QUAD: return 8;
  }
  if (value >= kMinQuad) {
    return 8;
  }
  if (value >= kMinDouble) {
    return 4;
  }
  return 2;
};

inline static uint32_t HexFormatTruncateValue(uint32_t value, uint8_t flags) {
  switch (flags & SS_WIDTH_MAX) {
    case SS_SINGLE: return value & kSingleMask;
    case SS_DOUBLE: return value & kDoubleMask;
    case SS_QUAD: return value & kQuadMask;
  }
  return value;
}

size_t SmartStringHexFormat(
    uint32_t value, uint8_t flags,
    char *dest, size_t dest_size) {
  if (dest == NULL || dest_size == 0) return 0;
  size_t const digit_width = HexFormatDigitWidth(value, flags);
  size_t const total_width = (flags & SS_ZERO_X)
                             ? digit_width + 2
                             : digit_width;
  value = HexFormatTruncateValue(value, flags);
  char const *kHexCharacters = (flags & SS_LOWER_ALPHA) ?
      kLowerHexCharacters : kUpperHexCharacters;
  /* String is formatted back to front. */
  size_t const di = (total_width < dest_size) ? total_width : dest_size - 1;
  dest[di] = 0;

  size_t i = 1;
  while (i <= digit_width && i <= di) {
    uint8_t const nibble = value & 0x0f;
    value >>= 4;
    dest[di - i] = kHexCharacters[nibble];
    ++i;
  }
  if (flags & SS_ZERO_X) {
    if (i <= di) {
      dest[di - i] = 'x';
      ++i;
    }
    if (i <= di) {
      dest[di - i] = '0';
      ++i;
    }
  }
  return total_width;
}

inline static uint32_t DecFormatDigitWidth(uint32_t value) {
  if (value == 0) return 1;
  uint32_t width = 0;
  while (value > 0) {
    value /= 10;
    ++width;
  }
  return width;
}

size_t SmartStringDecFormat(uint32_t value, char *dest, size_t dest_size) {
  if (dest == NULL || dest_size == 0) return 0;
  size_t const digit_width = DecFormatDigitWidth(value);
  dest[digit_width >= dest_size ? dest_size - 1 : digit_width] = 0;
  size_t i = digit_width;
  while (i >= dest_size) {
    value /= 10;
    --i;
  }
  while (i > 0) {
    dest[(i--) - 1] = (value % 10) + '0';
    value /= 10;
  }
  return digit_width;
}

/* Raw data encoding. */

static char const *kEncodeHexCharacters = kUpperHexCharacters;

void SmartStringSetHexEncodeCase(bool_t lower) {
  if (lower) {
    kEncodeHexCharacters = kLowerHexCharacters;
  } else {
    kEncodeHexCharacters = kUpperHexCharacters;
  }
}

size_t SmartStringHexEncode(
    void const *src, size_t src_size,
    char *dest, size_t dest_size) {
  if (src == NULL || dest == NULL || dest_size == 0) return 0;
  if (src_size == 0) {
    dest[0] = 0;
    return 0;
  }
  /* The encode length must be an even number of characters.
   * should be able to kMaxStringLength even or odd and the encode_length
   * must still be even. */
  size_t const encode_length = (src_size * 2 < kMaxStringLength)
      ? src_size * 2
      : kMaxStringLength - (kMaxStringLength & 1);
  size_t si = 0, di = 0;
  uint8_t const *src_data = (uint8_t const *) src;
  while (si < src_size && (di + 1) < dest_size && (di + 1) < encode_length) {
    dest[di++] = kEncodeHexCharacters[(src_data[si] >> 4) & 0xf];
    dest[di++] = kEncodeHexCharacters[src_data[si++] & 0xf];
  }
  if (di < dest_size) {
    dest[di] = 0;
  } else {
    dest[dest_size - 1] = 0;
  }
  return encode_length;
}

inline static uint8_t DecodeHexCharacter(char ch) {
  if (!isxdigit(ch)) return 0;
  if (isdigit(ch)) {
    return (ch - '0') & 0xf;
  }
  if (islower(ch)) {
    return ((ch - 'a') + 0xa) & 0xf;
  }
  return ((ch - 'A') + 0xa) & 0xf;
}

size_t SmartStringHexDecode(char const *src, void *dest, size_t dest_size) {
  if (src == NULL || dest == NULL) return 0;
  size_t const src_length = SmartStringLength(src);
  if (src_length == 0 || (src_length & 1) != 0) return 0;
  size_t si = 0;
  while (si < src_length) {
    if (!isxdigit(src[si++])) return 0;
  }
  uint8_t *dest_data = (uint8_t *) dest;
  size_t const decode_length = src_length >> 1;  /* Divide by 2 */
  si = 0;
  size_t di = 0;
  while ((si + 1) < src_length && di < dest_size && di < decode_length) {
    dest_data[di] = (DecodeHexCharacter(src[si]) << 4) | DecodeHexCharacter(src[si+1]);
    ++di;
    si += 2;
  }
  return decode_length;
}
