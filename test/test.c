/*
 * MIDI Controller - Test Header
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#include <unity.h>
#include "test.h"

/* Test Main for all test */
void main(void) {
  UNITY_BEGIN();
  SmartStringTest();
  PlatformAttributes();
  UNITY_END();
}
