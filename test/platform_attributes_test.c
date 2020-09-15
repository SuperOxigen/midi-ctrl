/*
 * MIDI Controller - Platform Attributes Test
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#include <stdio.h>
#include <unity.h>

#include "platform_attributes.h"

static void TestAttributeStrings(void) {
  char buffer[16];
  TEST_ASSERT_GREATER_THAN(
      0, PlatformGetPlatform(buffer, sizeof(buffer)));
  printf("Platform name = \"%s\"\n", buffer);
  PlatformGetFramework(buffer, sizeof(buffer));  /* None for native. */
  printf("Framework name = \"%s\"\n", buffer);
  TEST_ASSERT_GREATER_THAN(
      0, PlatformGetBuildTimeStamp(buffer, sizeof(buffer)));
  printf("Build time = \"%s\"\n", buffer);
}

void PlatformAttributesTest(void) {
  RUN_TEST(TestAttributeStrings);
}
