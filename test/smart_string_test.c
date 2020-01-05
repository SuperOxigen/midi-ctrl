/*
 * MIDI Controller - Smart String Test
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#include <unity.h>

#include "smart_string.h"

static char gVeryLongString[SS_MAX_STRING_LENGTH * 2];
static char gTruncatedVeryLongString[SS_MAX_STRING_LENGTH + 1];
inline static void SetupVeryLongString(void) {
  size_t i = 0;
  while (i < sizeof(gVeryLongString)) {
    gVeryLongString[i] = '0' + (i % 10);
    if (i < SS_MAX_STRING_LENGTH) {
      gTruncatedVeryLongString[i] = gVeryLongString[i];
    }
    ++i;
  }
  gVeryLongString[i - 1] = 0;
  gTruncatedVeryLongString[SS_MAX_STRING_LENGTH] = 0;
}

/*
 *  Test Cases
 */

void TestStringLength_InvalidInput(void) {
  TEST_ASSERT_EQUAL(0, SmartStringLength(NULL));
  TEST_ASSERT_EQUAL(SS_MAX_STRING_LENGTH, SmartStringLength(gVeryLongString));
}

void TestStringLength_ValidInput(void) {
  TEST_ASSERT_EQUAL(0, SmartStringLength(""));
  TEST_ASSERT_EQUAL(13, SmartStringLength("Hello, World!"));
}

void TestStringSet_InvalidInput(void) {
  char buffer[16];
  /* Dest must be non-null. */
  TEST_ASSERT_EQUAL(0, SmartStringSet('a', 1, NULL, sizeof(buffer)));
  /* Dest must be non-zero sized. */
  TEST_ASSERT_EQUAL(0, SmartStringSet('a', 1, buffer, 0));
  /* Filler must be a printable character */
  TEST_ASSERT_EQUAL(0, SmartStringSet(0, 1, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL(0, SmartStringSet(0xff, 1, buffer, sizeof(buffer)));
}

void TestStringSet_ValidInput(void) {
  char buffer[16];
  TEST_ASSERT_EQUAL(0, SmartStringSet('-', 0, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("", buffer);
  /* Fill a few. */
  TEST_ASSERT_EQUAL(1, SmartStringSet('a', 1, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("a", buffer);
  TEST_ASSERT_EQUAL(2, SmartStringSet('b', 2, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("bb", buffer);
  TEST_ASSERT_EQUAL(12, SmartStringSet('c', 12, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("cccccccccccc", buffer);
  /* Over fill */
  TEST_ASSERT_EQUAL(24, SmartStringSet('d', 24, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("ddddddddddddddd", buffer);
  TEST_ASSERT_EQUAL(1000, SmartStringSet('e', 1000, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("eeeeeeeeeeeeeee", buffer);
  /* Max buffer */
  char lbuffer[SS_MAX_STRING_LENGTH * 2];
  size_t const lbuffer_size = sizeof(lbuffer);
  TEST_ASSERT_EQUAL(lbuffer_size - 1, SmartStringSet('f', lbuffer_size - 1, lbuffer, lbuffer_size));
  TEST_ASSERT_EQUAL(SS_MAX_STRING_LENGTH, SmartStringLength(lbuffer));
  TEST_ASSERT_EQUAL('f', lbuffer[SS_MAX_STRING_LENGTH - 1]);
}

void TestStringCopy_InvalidInput(void) {
  char buffer[16];
  /* Source must be non-null. */
  TEST_ASSERT_EQUAL(0, SmartStringCopy(NULL, buffer, sizeof(buffer)));
  /* Dest must be non-null. */
  TEST_ASSERT_EQUAL(0, SmartStringCopy("Hello", NULL, sizeof(buffer)));
  /* Dest must be non-zero sized. */
  TEST_ASSERT_EQUAL(0, SmartStringCopy("Hello", buffer, 0));
}

void TestStringCopy_ValidInput(void) {
  char buffer[16];
  /* Fitted text. */
  TEST_ASSERT_EQUAL(13, SmartStringCopy("Hello, World!", buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("Hello, World!", buffer);
  TEST_ASSERT_EQUAL(15, SmartStringCopy("Goodbye, World!", buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("Goodbye, World!", buffer);
  /* Truncated */
  TEST_ASSERT_EQUAL(20, SmartStringCopy("For once in my live.", buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("For once in my ", buffer);
  /* Over max size */
  char lbuffer[SS_MAX_STRING_LENGTH * 2];
  size_t const lbuffer_size = sizeof(lbuffer);
  TEST_ASSERT_EQUAL(SS_MAX_STRING_LENGTH, SmartStringCopy(gVeryLongString, lbuffer, lbuffer_size));
  TEST_ASSERT_EQUAL_STRING(gTruncatedVeryLongString, lbuffer);
}

void TestStringAppend_InvalidInput(void) {
  char buffer[16];
  /* Source must be non-null. */
  TEST_ASSERT_EQUAL(0, SmartStringAppend(NULL, buffer, sizeof(buffer)));
  /* Dest must be non-null. */
  TEST_ASSERT_EQUAL(0, SmartStringAppend("Hello", NULL, sizeof(buffer)));
  /* Dest must be non-zero sized. */
  TEST_ASSERT_EQUAL(0, SmartStringAppend("Hello", buffer, 0));
}

void TestStringAppend_ValidInput(void) {
  char buffer[16] = {};
  TEST_ASSERT_EQUAL(0, SmartStringAppend("", buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("", buffer);
  TEST_ASSERT_EQUAL(4, SmartStringAppend("aaaa", buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("aaaa", buffer);
  TEST_ASSERT_EQUAL(8, SmartStringAppend("bbbb", buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("aaaabbbb", buffer);
  TEST_ASSERT_EQUAL(12, SmartStringAppend("cccc", buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("aaaabbbbcccc", buffer);
  /* Truncated */
  TEST_ASSERT_EQUAL(16, SmartStringAppend("dddd", buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("aaaabbbbccccddd", buffer);
  TEST_ASSERT_EQUAL(19, SmartStringAppend("eeee", buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("aaaabbbbccccddd", buffer);
}

void TestStringClear_InvalidInput(void) {
  char buffer[16];
  SmartStringClear(NULL, sizeof(buffer));
  SmartStringClear(buffer, 0);
}

void TestStringClear_ValidInput(void) {
  char buffer[16];
  SmartStringSet('a', sizeof(buffer), buffer, sizeof(buffer));
  SmartStringClear(buffer, 4);
  char const four_clear[] = "\0\0\0\0aaaaaaaaaaa";
  TEST_ASSERT_EQUAL_MEMORY(four_clear, buffer, sizeof(buffer));
}

/* Formatting Test */

void TestStringHexFormat_InvalidInput(void) {
  char buffer[16];
  TEST_ASSERT_EQUAL(0, SmartStringHexFormat(123, SS_SINGLE, NULL, sizeof(buffer)));
  TEST_ASSERT_EQUAL(0, SmartStringHexFormat(123, SS_SINGLE, buffer, 0));
}

void TestStringHexFormat_Padless(void) {
  char buffer[16];
  TEST_ASSERT_EQUAL(1, SmartStringHexFormat(0, SS_PADLESS, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("0", buffer);
  TEST_ASSERT_EQUAL(1, SmartStringHexFormat(0x1, SS_PADLESS, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("1", buffer);
  TEST_ASSERT_EQUAL(2, SmartStringHexFormat(0x10, SS_PADLESS, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("10", buffer);
  TEST_ASSERT_EQUAL(3, SmartStringHexFormat(0x101, SS_PADLESS, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("101", buffer);
  TEST_ASSERT_EQUAL(5, SmartStringHexFormat(0xe8e80, SS_PADLESS, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("E8E80", buffer);
  TEST_ASSERT_EQUAL(8, SmartStringHexFormat(0x10000000, SS_PADLESS, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("10000000", buffer);
  TEST_ASSERT_EQUAL(8, SmartStringHexFormat(0xffffffff, SS_PADLESS, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("FFFFFFFF", buffer);
  TEST_ASSERT_EQUAL(8, SmartStringHexFormat(0xdeadbeef, SS_PADLESS, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("DEADBEEF", buffer);
}

void TestStringHexFormat_FixedWidth(void) {
  char buffer[16];
  /* Single Byte */
  TEST_ASSERT_EQUAL(2, SmartStringHexFormat(0, SS_SINGLE, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("00", buffer);
  TEST_ASSERT_EQUAL(2, SmartStringHexFormat(0x0f, SS_SINGLE, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("0F", buffer);
  TEST_ASSERT_EQUAL(2, SmartStringHexFormat(0xf0, SS_SINGLE, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("F0", buffer);
  TEST_ASSERT_EQUAL(2, SmartStringHexFormat(0xff, SS_SINGLE, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("FF", buffer);
  /* Double Byte */
  TEST_ASSERT_EQUAL(4, SmartStringHexFormat(0, SS_DOUBLE, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("0000", buffer);
  TEST_ASSERT_EQUAL(4, SmartStringHexFormat(0xf, SS_DOUBLE, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("000F", buffer);
  TEST_ASSERT_EQUAL(4, SmartStringHexFormat(0xf0, SS_DOUBLE, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("00F0", buffer);
  TEST_ASSERT_EQUAL(4, SmartStringHexFormat(0xf00, SS_DOUBLE, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("0F00", buffer);
  TEST_ASSERT_EQUAL(4, SmartStringHexFormat(0xf000, SS_DOUBLE, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("F000", buffer);
  TEST_ASSERT_EQUAL(4, SmartStringHexFormat(0xffff, SS_DOUBLE, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("FFFF", buffer);
  TEST_ASSERT_EQUAL(4, SmartStringHexFormat(0x0ba7, SS_DOUBLE, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("0BA7", buffer);
  /* Quad Byte */
  TEST_ASSERT_EQUAL(8, SmartStringHexFormat(0, SS_QUAD, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("00000000", buffer);
  TEST_ASSERT_EQUAL(8, SmartStringHexFormat(0xf, SS_QUAD, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("0000000F", buffer);
  TEST_ASSERT_EQUAL(8, SmartStringHexFormat(0xf0, SS_QUAD, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("000000F0", buffer);
  TEST_ASSERT_EQUAL(8, SmartStringHexFormat(0xf00, SS_QUAD, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("00000F00", buffer);
  TEST_ASSERT_EQUAL(8, SmartStringHexFormat(0xf000, SS_QUAD, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("0000F000", buffer);
  TEST_ASSERT_EQUAL(8, SmartStringHexFormat(0xf0000, SS_QUAD, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("000F0000", buffer);
  TEST_ASSERT_EQUAL(8, SmartStringHexFormat(0xf00000, SS_QUAD, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("00F00000", buffer);
  TEST_ASSERT_EQUAL(8, SmartStringHexFormat(0xf000000, SS_QUAD, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("0F000000", buffer);
  TEST_ASSERT_EQUAL(8, SmartStringHexFormat(0xf0000000, SS_QUAD, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("F0000000", buffer);
  TEST_ASSERT_EQUAL(8, SmartStringHexFormat(0xffff, SS_QUAD, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("0000FFFF", buffer);
  TEST_ASSERT_EQUAL(8, SmartStringHexFormat(0xffffffff, SS_QUAD, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("FFFFFFFF", buffer);
  TEST_ASSERT_EQUAL(8, SmartStringHexFormat(0xdeadbeef, SS_QUAD, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("DEADBEEF", buffer);
  /* Numeric truncations */
  TEST_ASSERT_EQUAL(2, SmartStringHexFormat(0x1f2e3d4c, SS_SINGLE, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("4C", buffer);
  TEST_ASSERT_EQUAL(4, SmartStringHexFormat(0x1f2e3d4c, SS_DOUBLE, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("3D4C", buffer);
  /* String truncations */
  TEST_ASSERT_EQUAL(2, SmartStringHexFormat(0x1f2e3d4c, SS_SINGLE, buffer, 2));
  TEST_ASSERT_EQUAL_STRING("C", buffer);
  TEST_ASSERT_EQUAL(4, SmartStringHexFormat(0x1f2e3d4c, SS_DOUBLE, buffer, 3));
  TEST_ASSERT_EQUAL_STRING("4C", buffer);
  TEST_ASSERT_EQUAL(8, SmartStringHexFormat(0x1f2e3d4c, SS_QUAD, buffer, 6));
  TEST_ASSERT_EQUAL_STRING("E3D4C", buffer);
}

void TestStringHexFormat_AutoWidth(void) {
  char buffer[16];
  TEST_ASSERT_EQUAL(2, SmartStringHexFormat(0, 0, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("00", buffer);
  TEST_ASSERT_EQUAL(2, SmartStringHexFormat(0xf, 0, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("0F", buffer);
  TEST_ASSERT_EQUAL(2, SmartStringHexFormat(0xf0, 0, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("F0", buffer);
  TEST_ASSERT_EQUAL(4, SmartStringHexFormat(0xf00, 0, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("0F00", buffer);
  TEST_ASSERT_EQUAL(4, SmartStringHexFormat(0xf000, 0, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("F000", buffer);
  TEST_ASSERT_EQUAL(8, SmartStringHexFormat(0xf0000, 0, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("000F0000", buffer);
  TEST_ASSERT_EQUAL(8, SmartStringHexFormat(0xf00000, 0, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("00F00000", buffer);
  TEST_ASSERT_EQUAL(8, SmartStringHexFormat(0xf000000, 0, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("0F000000", buffer);
  TEST_ASSERT_EQUAL(8, SmartStringHexFormat(0xf0000000, 0, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("F0000000", buffer);
}

void TestStringHexFormat_ZeroXPrefixed(void) {
  char buffer[16];
  /* Padless */
  TEST_ASSERT_EQUAL(3, SmartStringHexFormat(0, SS_PADLESS | SS_ZERO_X, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("0x0", buffer);
  TEST_ASSERT_EQUAL(3, SmartStringHexFormat(1, SS_PADLESS | SS_ZERO_X, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("0x1", buffer);
  TEST_ASSERT_EQUAL(6, SmartStringHexFormat(0x1000, SS_PADLESS | SS_ZERO_X, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("0x1000", buffer);
  TEST_ASSERT_EQUAL(10, SmartStringHexFormat(0xff00ff00, SS_PADLESS | SS_ZERO_X, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("0xFF00FF00", buffer);
  /* Fixed width - Single */
  TEST_ASSERT_EQUAL(4, SmartStringHexFormat(0, SS_SINGLE | SS_ZERO_X, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("0x00", buffer);
  TEST_ASSERT_EQUAL(4, SmartStringHexFormat(1, SS_SINGLE | SS_ZERO_X, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("0x01", buffer);
  TEST_ASSERT_EQUAL(4, SmartStringHexFormat(0x10, SS_SINGLE | SS_ZERO_X, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("0x10", buffer);
  /* Fixed width - Double */
  TEST_ASSERT_EQUAL(6, SmartStringHexFormat(0, SS_DOUBLE | SS_ZERO_X, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("0x0000", buffer);
  TEST_ASSERT_EQUAL(6, SmartStringHexFormat(0x1, SS_DOUBLE | SS_ZERO_X, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("0x0001", buffer);
  TEST_ASSERT_EQUAL(6, SmartStringHexFormat(0x0100, SS_DOUBLE | SS_ZERO_X, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("0x0100", buffer);
  /* Fixed width - Quad */
  TEST_ASSERT_EQUAL(10, SmartStringHexFormat(0, SS_QUAD | SS_ZERO_X, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("0x00000000", buffer);
  TEST_ASSERT_EQUAL(10, SmartStringHexFormat(0x1, SS_QUAD | SS_ZERO_X, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("0x00000001", buffer);
  TEST_ASSERT_EQUAL(10, SmartStringHexFormat(0x01000000, SS_QUAD | SS_ZERO_X, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("0x01000000", buffer);
  /* String Truncation */
  TEST_ASSERT_EQUAL(10, SmartStringHexFormat(0x01000000, SS_QUAD | SS_ZERO_X, buffer, 3));
  TEST_ASSERT_EQUAL_STRING("00", buffer);
  TEST_ASSERT_EQUAL(10, SmartStringHexFormat(0x01000000, SS_QUAD | SS_ZERO_X, buffer, 9));
  TEST_ASSERT_EQUAL_STRING("01000000", buffer);
  TEST_ASSERT_EQUAL(10, SmartStringHexFormat(0x01000000, SS_QUAD | SS_ZERO_X, buffer, 10));
  TEST_ASSERT_EQUAL_STRING("x01000000", buffer);
}

void TestStringHexFormat_LowerCase(void) {
  char buffer[16];
  TEST_ASSERT_EQUAL(8, SmartStringHexFormat(0xdeadbeef, SS_QUAD, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("DEADBEEF", buffer);
  TEST_ASSERT_EQUAL(8, SmartStringHexFormat(0xdeadbeef, SS_QUAD | SS_LOWER_ALPHA, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("deadbeef", buffer);
}

/* Hex Encoding Test */

void TestStringHexEncode_InvalidInput(void) {
  char buffer[16];
  uint8_t const data[] = {0xe5, 0x5e, 0xc3, 0x3c};
  TEST_ASSERT_EQUAL(0, SmartStringHexEncode(NULL, sizeof(data), buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL(0, SmartStringHexEncode(data, sizeof(data), NULL, sizeof(buffer)));
  TEST_ASSERT_EQUAL(0, SmartStringHexEncode(data, sizeof(data), buffer, 0));
}

void TestStringHexEncode_ValidInput(void) {
  char buffer[16];
  uint8_t const data[] = {0xe5, 0x5e, 0xc3, 0x3c};
  /* No Data */
  SmartStringSet('-', sizeof(buffer), buffer, sizeof(buffer));
  TEST_ASSERT_EQUAL(0, SmartStringHexEncode(data, 0, buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("", buffer);
  /* Width data - Upper Case */
  SmartStringSetHexEncodeCase(false);
  TEST_ASSERT_EQUAL(8, SmartStringHexEncode(data, sizeof(data), buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("E55EC33C", buffer);
  /* Width data - Lower Case */
  SmartStringSetHexEncodeCase(true);
  TEST_ASSERT_EQUAL(8, SmartStringHexEncode(data, sizeof(data), buffer, sizeof(buffer)));
  TEST_ASSERT_EQUAL_STRING("e55ec33c", buffer);
  /* Truncated */
  SmartStringSetHexEncodeCase(false);
  /* Whole byte */
  TEST_ASSERT_EQUAL(8, SmartStringHexEncode(data, sizeof(data), buffer, 5));
  TEST_ASSERT_EQUAL_STRING("E55E", buffer);
  /* Half byte */
  TEST_ASSERT_EQUAL(8, SmartStringHexEncode(data, sizeof(data), buffer, 6));
  TEST_ASSERT_EQUAL_STRING("E55EC", buffer);
}

void TestStringHexDecode_InvalidInput(void) {
  char const source[] = "E55EC33C";
  uint8_t dest[4];
  /* Programmatically bad input */
  TEST_ASSERT_EQUAL(0, SmartStringHexDecode(NULL, dest, sizeof(dest)));
  TEST_ASSERT_EQUAL(0, SmartStringHexDecode(source, NULL, sizeof(dest)));
  /* Syntactically bad input */
  TEST_ASSERT_EQUAL(0, SmartStringHexDecode("not hex", dest, sizeof(dest)));
  TEST_ASSERT_EQUAL(0, SmartStringHexDecode("XXXXXX", dest, sizeof(dest)));
  TEST_ASSERT_EQUAL(0, SmartStringHexDecode("12345s", dest, sizeof(dest)));
  TEST_ASSERT_EQUAL(0, SmartStringHexDecode("1234567", dest, sizeof(dest)));
}

void TestStringHexDecode_ValidInput(void) {
  char const source_upper[] = "E55EC33C";
  char const source_lower[] = "e55ec33c";
  uint8_t dest[4];
  uint8_t const expected[] = {0xe5, 0x5e, 0xc3, 0x3c};
  TEST_ASSERT_EQUAL(0, SmartStringHexDecode("", dest, sizeof(dest)));
  TEST_ASSERT_EQUAL(4, SmartStringHexDecode(source_upper, dest, sizeof(dest)));
  TEST_ASSERT_EQUAL_MEMORY(expected, dest, 4);
  memset(dest, 0, sizeof(dest));
  TEST_ASSERT_EQUAL(4, SmartStringHexDecode(source_lower, dest, sizeof(dest)));
  TEST_ASSERT_EQUAL_MEMORY(expected, dest, 4);
  /* Output buffer is not large enough */
  TEST_ASSERT_EQUAL(4, SmartStringHexDecode(source_upper, dest, 0));
}

/* Test Main. */
void main(void) {
  SetupVeryLongString();
  UNITY_BEGIN();
  /* String manipulation */
  RUN_TEST(TestStringLength_InvalidInput);
  RUN_TEST(TestStringLength_ValidInput);
  RUN_TEST(TestStringSet_InvalidInput);
  RUN_TEST(TestStringSet_ValidInput);
  RUN_TEST(TestStringCopy_InvalidInput);
  RUN_TEST(TestStringCopy_ValidInput);
  RUN_TEST(TestStringAppend_InvalidInput);
  RUN_TEST(TestStringAppend_ValidInput);
  RUN_TEST(TestStringClear_InvalidInput);
  RUN_TEST(TestStringClear_ValidInput);
  /* Formatting */
  RUN_TEST(TestStringHexFormat_InvalidInput);
  RUN_TEST(TestStringHexFormat_Padless);
  RUN_TEST(TestStringHexFormat_FixedWidth);
  RUN_TEST(TestStringHexFormat_AutoWidth);
  RUN_TEST(TestStringHexFormat_ZeroXPrefixed);
  RUN_TEST(TestStringHexFormat_LowerCase);
  /* Encoding / Decoding */
  RUN_TEST(TestStringHexEncode_InvalidInput);
  RUN_TEST(TestStringHexEncode_ValidInput);
  RUN_TEST(TestStringHexDecode_InvalidInput);
  RUN_TEST(TestStringHexDecode_ValidInput);

  UNITY_END();
}
