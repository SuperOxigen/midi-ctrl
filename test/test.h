/*
 * MIDI Controller - Test Header
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#ifndef _TEST_H_
#define _TEST_H_

void SmartStringTest(void);
void PlatformAttributesTest(void);
void BitArrayTest(void);
void ByteBufferTest(void);
void SystemTimeTest(void);
void SchedulerTest(void);

/* MIDI Tests */
void MidiBytesTest(void);
void MidiNoteTest(void);
void MidiTimeTest(void);
void MidiTimeTrackerTest(void);
void MidiControlTest(void);
void MidiFrameTest(void);
void MidiUserBitsTest(void);
void MidiSystemInfoTest(void);
void MidiGeneralMidiOneTest(void);

void MidiManufacturerIdTest(void);
void MidiSystemUniversalTest(void);
void MidiSystemExclusiveTest(void);

void MidiProgramTest(void);

void MidiMessageTest(void);

void MidiSerializeTest(void);
void MidiCallbackTest(void);

void MidiTransceiverTest(void);

#endif  /* _TEST_H_ */
