/*
 * MIDI Controller - Bit Array Test.
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#include <string.h>
#include <unity.h>

#include "bit_array.h"

static void TestBitArray_Initialize(void) {
  bit_array_t array;
  uint8_t buffer[BIT_ARRAY_MAX_BUFFER_SIZE + 2];
  TEST_ASSERT_FALSE(
      BitArrayInitialize(NULL, buffer, BIT_ARRAY_MAX_BUFFER_SIZE));
  TEST_ASSERT_FALSE(
      BitArrayInitialize(&array, NULL, BIT_ARRAY_MAX_BUFFER_SIZE));
  TEST_ASSERT_FALSE(BitArrayInitialize(&array, buffer, 0));
  TEST_ASSERT_FALSE(BitArrayInitialize(
      &array, buffer, BIT_ARRAY_MAX_BUFFER_SIZE + 2));

  memset(&array, 0, sizeof(bit_array_t));
  TEST_ASSERT_TRUE(BitArrayInitialize(&array, buffer, 1));
  TEST_ASSERT_EQUAL(buffer, array.buffer);
  TEST_ASSERT_EQUAL(8, array.bit_size);

  TEST_ASSERT_TRUE(BitArrayInitialize(
      &array, buffer, BIT_ARRAY_MAX_BUFFER_SIZE));
  TEST_ASSERT_EQUAL(8 * BIT_ARRAY_MAX_BUFFER_SIZE, array.bit_size);
}

static void TestBitArray_InitializeAsIs(void) {
  bit_array_t array;
  uint8_t buffer[BIT_ARRAY_MAX_BUFFER_SIZE + 2];
  TEST_ASSERT_FALSE(
      BitArrayInitializeAsIs(NULL, buffer, BIT_ARRAY_MAX_BUFFER_SIZE));
  TEST_ASSERT_FALSE(
      BitArrayInitializeAsIs(&array, NULL, BIT_ARRAY_MAX_BUFFER_SIZE));
  TEST_ASSERT_FALSE(BitArrayInitializeAsIs(&array, buffer, 0));
  TEST_ASSERT_FALSE(BitArrayInitializeAsIs(
      &array, buffer, BIT_ARRAY_MAX_BUFFER_SIZE + 2));

  uint8_t buffer2[2] = {0xF0, 0x0F};
  TEST_ASSERT_TRUE(BitArrayInitializeAsIs(&array, buffer2, sizeof(buffer2)));

  size_t i = 0;
  do {
    TEST_ASSERT_FALSE(BitArrayTestBit(&array, i));
  } while (++i < 4);
  do {
    TEST_ASSERT_TRUE(BitArrayTestBit(&array, i));
  } while (++i < 12);
  do {
    TEST_ASSERT_FALSE(BitArrayTestBit(&array, i));
  } while (++i < 16);
  TEST_ASSERT_TRUE(BitArrayClear(&array));
  TEST_ASSERT_TRUE(BitArrayNone(&array));

  i = 0;
  do {
    TEST_ASSERT_TRUE(BitArrayClearBit(&array,  i));
  } while (++i < 8);
  do {
    TEST_ASSERT_TRUE(BitArraySetBit(&array, i));
  } while (++i < 16);
  TEST_ASSERT_TRUE(BitArrayInitializeAsIs(&array, buffer2, sizeof(buffer2)));
  uint8_t const kExpected[2] = {0x00, 0xFF};
  TEST_ASSERT_EQUAL_MEMORY(kExpected, buffer2, 2);
}

static void TestBitArray_BitWiseOperations(void) {
  bit_array_t array;
  uint8_t buffer[2];

  /* Should not work on uninitialized bit-array. */
  memset(&array, 0, sizeof(array));
  TEST_ASSERT_FALSE(BitArraySetBit(&array, 0));
  TEST_ASSERT_FALSE(BitArrayClearBit(&array, 0));
  TEST_ASSERT_FALSE(BitArrayTestBit(&array, 0));
  TEST_ASSERT_FALSE(BitArrayClear(&array));

  TEST_ASSERT_TRUE(BitArrayInitialize(&array, buffer, sizeof(buffer)));

  /* Bad input. */
  TEST_ASSERT_FALSE(BitArraySetBit(NULL, 4));
  TEST_ASSERT_FALSE(BitArraySetBit(&array, 16));
  TEST_ASSERT_FALSE(BitArrayClearBit(NULL, 4));
  TEST_ASSERT_FALSE(BitArrayClearBit(&array, 16));
  TEST_ASSERT_FALSE(BitArrayTestBit(NULL, 4));
  TEST_ASSERT_FALSE(BitArrayTestBit(&array, 16));
  TEST_ASSERT_FALSE(BitArrayClear(NULL));

  /* Test whole array. */
  for (size_t i = 0; i < 16; ++i) {
    TEST_ASSERT_FALSE(BitArrayTestBit(&array, i));
    TEST_ASSERT_TRUE(BitArraySetBit(&array, i));

    TEST_ASSERT_TRUE(BitArrayTestBit(&array, i));
    TEST_ASSERT_TRUE(BitArrayClearBit(&array, i));

    TEST_ASSERT_FALSE(BitArrayTestBit(&array, i));
    TEST_ASSERT_TRUE(BitArraySetBit(&array, i));
  }

  TEST_ASSERT_TRUE(BitArrayClear(&array));
  for (size_t i = 0; i < 16; ++i) {
    TEST_ASSERT_FALSE(BitArrayTestBit(&array, i));
  }
}

static void TestBitArray_SetWiseOperations(void) {
  bit_array_t array;
  uint8_t buffer[2];

  /* Should not work on uninitialized bit-array. */
  memset(&array, 0, sizeof(array));
  TEST_ASSERT_FALSE(BitArrayNone(NULL));
  TEST_ASSERT_FALSE(BitArrayNone(&array));
  TEST_ASSERT_FALSE(BitArrayAny(NULL));
  TEST_ASSERT_FALSE(BitArrayAny(&array));
  TEST_ASSERT_FALSE(BitArrayAll(NULL));
  TEST_ASSERT_FALSE(BitArrayAll(&array));

  TEST_ASSERT_TRUE(BitArrayInitialize(&array, buffer, sizeof(buffer)));

  TEST_ASSERT_TRUE(BitArrayNone(&array));
  TEST_ASSERT_FALSE(BitArrayAny(&array));
  TEST_ASSERT_FALSE(BitArrayAll(&array));

  BitArraySetBit(&array, 7);
  TEST_ASSERT_FALSE(BitArrayNone(&array));
  TEST_ASSERT_TRUE(BitArrayAny(&array));
  TEST_ASSERT_FALSE(BitArrayAll(&array));

  BitArraySetBit(&array, 13);
  TEST_ASSERT_FALSE(BitArrayNone(&array));
  TEST_ASSERT_TRUE(BitArrayAny(&array));
  TEST_ASSERT_FALSE(BitArrayAll(&array));

  BitArrayClearBit(&array, 7);
  BitArrayClearBit(&array, 13);
  TEST_ASSERT_TRUE(BitArrayNone(&array));
  TEST_ASSERT_FALSE(BitArrayAny(&array));
  TEST_ASSERT_FALSE(BitArrayAll(&array));

  for (size_t i = 0; i < 16; ++i) {
    BitArraySetBit(&array, i);
  }
  TEST_ASSERT_FALSE(BitArrayNone(&array));
  TEST_ASSERT_TRUE(BitArrayAny(&array));
  TEST_ASSERT_TRUE(BitArrayAll(&array));

  BitArrayClearBit(&array, 5);
  TEST_ASSERT_FALSE(BitArrayNone(&array));
  TEST_ASSERT_TRUE(BitArrayAny(&array));
  TEST_ASSERT_FALSE(BitArrayAll(&array));

  BitArrayClear(&array);
  TEST_ASSERT_TRUE(BitArrayNone(&array));
  TEST_ASSERT_FALSE(BitArrayAny(&array));
  TEST_ASSERT_FALSE(BitArrayAll(&array));
}

void BitArrayTest(void) {
  RUN_TEST(TestBitArray_Initialize);
  RUN_TEST(TestBitArray_BitWiseOperations);
  RUN_TEST(TestBitArray_SetWiseOperations);
  RUN_TEST(TestBitArray_InitializeAsIs);
}
