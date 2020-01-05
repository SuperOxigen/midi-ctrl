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

/* Test Main. */
void main(void) {
  SetupVeryLongString();

  UNITY_BEGIN();
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
  UNITY_END();
}
