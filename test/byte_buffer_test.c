/*
 * MIDI Controller - Byte Buffer Test
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#include <string.h>
#include <unity.h>

#include "byte_buffer.h"

#define TEST_BUFFER_SIZE 12

typedef uint8_t buffer_data_t[TEST_BUFFER_SIZE];

static void FillArray(uint8_t *data, size_t size) {
  for (size_t i = 0; i < size; ++i) {
    data[i] = (uint8_t) (i & 0xFF);
  }
}

static void TestByteBuffer_Initialize(void) {
  buffer_data_t data;
  byte_buffer_t buffer;
  TEST_ASSERT_FALSE(ByteBufferInitialize(NULL, data, sizeof(data)));
  TEST_ASSERT_FALSE(ByteBufferInitialize(&buffer, NULL, sizeof(data)));
  TEST_ASSERT_FALSE(ByteBufferInitialize(&buffer, data, 0));

  TEST_ASSERT_TRUE(ByteBufferInitialize(&buffer, data, sizeof(data)));

  TEST_ASSERT_FALSE(ByteBufferIsEmpty(NULL));
  TEST_ASSERT_FALSE(ByteBufferIsFull(NULL));

  TEST_ASSERT_TRUE(ByteBufferIsEmpty(&buffer));
  TEST_ASSERT_FALSE(ByteBufferIsFull(&buffer));
}

static void TestByteBuffer_SingleByte(void) {
  buffer_data_t data;
  byte_buffer_t buffer;
  TEST_ASSERT_TRUE(ByteBufferInitialize(&buffer, data, sizeof(data)));

  TEST_ASSERT_FALSE(ByteBufferEnqueueByte(NULL, 0xFF));

  uint8_t byte = 0;
  /* Should fail from no data. */
  TEST_ASSERT_FALSE(ByteBufferPeakByte(&buffer, &byte));
  TEST_ASSERT_FALSE(ByteBufferDequeueByte(&buffer, &byte));

  TEST_ASSERT_TRUE(ByteBufferEnqueueByte(&buffer, 0xFF));

  /* Should fail due to NULL pointer. */
  TEST_ASSERT_FALSE(ByteBufferPeakByte(&buffer, NULL));
  TEST_ASSERT_FALSE(ByteBufferDequeueByte(&buffer, NULL));

  byte = 0;
  TEST_ASSERT_TRUE(ByteBufferPeakByte(&buffer, &byte));
  TEST_ASSERT_EQUAL(0xFF, byte);
  byte = 0;
  TEST_ASSERT_TRUE(ByteBufferDequeueByte(&buffer, &byte));
  TEST_ASSERT_EQUAL(0xFF, byte);

  byte = 0;
  TEST_ASSERT_FALSE(ByteBufferPeakByte(&buffer, &byte));
  TEST_ASSERT_FALSE(ByteBufferDequeueByte(&buffer, &byte));
}

static void TestByteBuffer_ByteByByte(void) {
  buffer_data_t data;
  byte_buffer_t buffer;
  TEST_ASSERT_TRUE(ByteBufferInitialize(&buffer, data, sizeof(data)));

  TEST_ASSERT_TRUE(ByteBufferEnqueueByte(&buffer, 0x90));
  TEST_ASSERT_TRUE(ByteBufferEnqueueByte(&buffer, 0x06));
  TEST_ASSERT_TRUE(ByteBufferEnqueueByte(&buffer, 0x60));

  uint8_t byte = 0;
  TEST_ASSERT_TRUE(ByteBufferPeakByte(&buffer, &byte));
  TEST_ASSERT_EQUAL(0x90, byte);

  byte = 0;
  TEST_ASSERT_TRUE(ByteBufferDequeueByte(&buffer, &byte));
  TEST_ASSERT_EQUAL(0x90, byte);

  TEST_ASSERT_TRUE(ByteBufferDequeueByte(&buffer, &byte));
  TEST_ASSERT_EQUAL(0x06, byte);

  TEST_ASSERT_TRUE(ByteBufferEnqueueByte(&buffer, 0x09));

  TEST_ASSERT_TRUE(ByteBufferDequeueByte(&buffer, &byte));
  TEST_ASSERT_EQUAL(0x60, byte);

  TEST_ASSERT_TRUE(ByteBufferDequeueByte(&buffer, &byte));
  TEST_ASSERT_EQUAL(0x09, byte);

  TEST_ASSERT_TRUE(ByteBufferIsEmpty(&buffer));
  TEST_ASSERT_FALSE(ByteBufferDequeueByte(&buffer, &byte));

  for (uint8_t i = 0; i < TEST_BUFFER_SIZE; ++i) {
    TEST_ASSERT_TRUE(ByteBufferEnqueueByte(&buffer, i));
  }
  TEST_ASSERT_FALSE(ByteBufferIsEmpty(&buffer));
  TEST_ASSERT_TRUE(ByteBufferIsFull(&buffer));

  TEST_ASSERT_FALSE(ByteBufferEnqueueByte(&buffer, 0xFF));

  TEST_ASSERT_FALSE(ByteBufferIsEmpty(&buffer));
  TEST_ASSERT_TRUE(ByteBufferIsFull(&buffer));

  TEST_ASSERT_TRUE(ByteBufferClear(&buffer));

  TEST_ASSERT_TRUE(ByteBufferIsEmpty(&buffer));
  TEST_ASSERT_FALSE(ByteBufferIsFull(&buffer));
}

static void TestByteBuffer_MultiBytes(void) {
  buffer_data_t data;
  byte_buffer_t buffer;
  TEST_ASSERT_TRUE(ByteBufferInitialize(&buffer, data, sizeof(data)));

  buffer_data_t test_data;
  FillArray(test_data, sizeof(test_data));
  TEST_ASSERT_EQUAL(TEST_BUFFER_SIZE, ByteBufferEnqueueBytes(
      &buffer, test_data, sizeof(test_data)));

  TEST_ASSERT_FALSE(ByteBufferIsEmpty(&buffer));
  TEST_ASSERT_TRUE(ByteBufferIsFull(&buffer));

  buffer_data_t extracted_data;
  TEST_ASSERT_EQUAL(4, ByteBufferPeakBytes(&buffer, extracted_data, 4));
  TEST_ASSERT_EQUAL_MEMORY(test_data, extracted_data, 4);

  TEST_ASSERT_TRUE(ByteBufferIsFull(&buffer));

  memset(extracted_data, 0, sizeof(extracted_data));
  TEST_ASSERT_EQUAL(4, ByteBufferDequeueBytes(&buffer, extracted_data, 4));
  TEST_ASSERT_EQUAL_MEMORY(test_data, extracted_data, 4);

  TEST_ASSERT_FALSE(ByteBufferIsFull(&buffer));

  memset(extracted_data, 0, sizeof(extracted_data));
  TEST_ASSERT_EQUAL((TEST_BUFFER_SIZE - 4), ByteBufferDequeueBytes(
      &buffer, extracted_data, sizeof(extracted_data)));
  TEST_ASSERT_EQUAL_MEMORY(&test_data[4], extracted_data, (TEST_BUFFER_SIZE - 4));

  TEST_ASSERT_TRUE(ByteBufferIsEmpty(&buffer));

  TEST_ASSERT_EQUAL(TEST_BUFFER_SIZE, ByteBufferEnqueueBytes(
      &buffer, test_data, sizeof(test_data)));
  TEST_ASSERT_TRUE(ByteBufferIsFull(&buffer));

  TEST_ASSERT_EQUAL(7, ByteBufferClearBytes(&buffer, 7));

  TEST_ASSERT_EQUAL(7, ByteBufferEnqueueBytes(
      &buffer, test_data, sizeof(test_data)));

  TEST_ASSERT_EQUAL((TEST_BUFFER_SIZE - 7), ByteBufferDequeueBytes(
      &buffer, extracted_data, TEST_BUFFER_SIZE - 7));

  TEST_ASSERT_EQUAL_MEMORY(
      &test_data[7], extracted_data, (TEST_BUFFER_SIZE - 7));

  TEST_ASSERT_EQUAL(7, ByteBufferDequeueBytes(&buffer, extracted_data, 7));
  TEST_ASSERT_EQUAL_MEMORY(test_data, extracted_data, 7);

  TEST_ASSERT_TRUE(ByteBufferIsEmpty(&buffer));
}

void ByteBufferTest(void) {
  RUN_TEST(TestByteBuffer_Initialize);
  RUN_TEST(TestByteBuffer_SingleByte);
  RUN_TEST(TestByteBuffer_ByteByByte);
  RUN_TEST(TestByteBuffer_MultiBytes);
}
