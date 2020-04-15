/*
 * MIDI Controller - Test Header
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#include <stdio.h>
#include <unity.h>
#include "test.h"

/* Test Main for all test */
int main(void) {
  UNITY_BEGIN();
  printf("\n==== Smart Strings Test ====\n");
  SmartStringTest();

  printf("\n==== Controller Test ====\n");
  PlatformAttributesTest();

  printf("\n==== Misc Utilities Test ====\n");
  BitArrayTest();
  SystemTimeTest();

  /* MIDI test. */
  printf("\n==== MIDI Tests ====\n");
  MidiBytesTest();
  MidiNoteTest();
  MidiTimeTest();
  MidiTimeTrackerTest();
  MidiControlTest();
  MidiFrameTest();
  MidiUserBitsTest();
  MidiSystemInfoTest();

  MidiManufacturerIdTest();
  MidiSystemUniversalTest();
  MidiSystemExclusiveTest();

  MidiProgramTest();
  MidiMessageTest();
  MidiSerializeTest();
  MidiCallbackTest();

  MidiTransceiverTest();
  UNITY_END();
  return 0;
}
