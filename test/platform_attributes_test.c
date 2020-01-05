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
  TEST_ASSERT_NOT_NULL(kPlatformName);
  printf("Platform name = \"%s\"\n", kPlatformName);
  TEST_ASSERT_NOT_NULL(kFrameworkName);
  printf("Framework name = \"%s\"\n", kFrameworkName);
  TEST_ASSERT_NOT_NULL(kBuildTimeRepr);
  printf("Build time = \"%s\"\n", kBuildTimeRepr);
}

void PlatformAttributes(void) {
  RUN_TEST(TestAttributeStrings);
}
