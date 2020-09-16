/*
 * MIDI Controller - MIDI Control Change Test
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#include <unity.h>

#include "midi_control.h"
#include "midi_defs.h"

static void TestMidiControlNumber_DefinedRanges(void) {
  /* Channel controller. */
  TEST_ASSERT_TRUE(MidiControlNumberIsController(MIDI_PORTAMENTO_TIME_MSB));
  TEST_ASSERT_TRUE(MidiControlNumberIsController(MIDI_BANK_SELECT_LSB));
  TEST_ASSERT_TRUE(MidiControlNumberIsController(MIDI_GENERAL_CONTROLLER_6));
  TEST_ASSERT_TRUE(MidiControlNumberIsController(MIDI_PORTAMENTO));
  TEST_ASSERT_TRUE(MidiControlNumberIsController(MIDI_DATA_INCREMENT));
  TEST_ASSERT_TRUE(MidiControlNumberIsController(MIDI_RPN_LSB));

  TEST_ASSERT_FALSE(MidiControlNumberIsController(MIDI_ALL_SOUND_OFF));
  TEST_ASSERT_FALSE(MidiControlNumberIsController(MIDI_MONO_MODE_ON));
  TEST_ASSERT_FALSE(MidiControlNumberIsController(MIDI_POLY_MODE_ON));

  TEST_ASSERT_FALSE(MidiControlNumberIsController(0x80 | MIDI_PORTAMENTO));
  TEST_ASSERT_FALSE(MidiControlNumberIsController(0x80 | MIDI_MONO_MODE_ON));

  /* Channel mode. */
  TEST_ASSERT_FALSE(MidiControlNumberIsChannelMode(MIDI_PORTAMENTO_TIME_MSB));
  TEST_ASSERT_FALSE(MidiControlNumberIsChannelMode(MIDI_BANK_SELECT_LSB));
  TEST_ASSERT_FALSE(MidiControlNumberIsChannelMode(MIDI_GENERAL_CONTROLLER_6));
  TEST_ASSERT_FALSE(MidiControlNumberIsChannelMode(MIDI_PORTAMENTO));
  TEST_ASSERT_FALSE(MidiControlNumberIsChannelMode(MIDI_DATA_INCREMENT));
  TEST_ASSERT_FALSE(MidiControlNumberIsChannelMode(MIDI_RPN_LSB));

  TEST_ASSERT_TRUE(MidiControlNumberIsChannelMode(MIDI_ALL_SOUND_OFF));
  TEST_ASSERT_TRUE(MidiControlNumberIsChannelMode(MIDI_MONO_MODE_ON));
  TEST_ASSERT_TRUE(MidiControlNumberIsChannelMode(MIDI_POLY_MODE_ON));

  TEST_ASSERT_FALSE(MidiControlNumberIsChannelMode(0x80 | MIDI_PORTAMENTO));

  /* Channel controller sub-ranges. */
  /* MSB controller numbers */
  TEST_ASSERT_TRUE(MidiControlNumberIsMsb(MIDI_BANK_SELECT_MSB));
  TEST_ASSERT_TRUE(MidiControlNumberIsMsb(MIDI_EFFECT_CONTROLLER_2_MSB));
  TEST_ASSERT_TRUE(MidiControlNumberIsMsb(MIDI_GENERAL_CONTROLLER_4_MSB));
  TEST_ASSERT_TRUE(MidiControlNumberIsMsb(0x1F));  /* Highest MSB number */
  TEST_ASSERT_FALSE(MidiControlNumberIsMsb(MIDI_BANK_SELECT_LSB));
  TEST_ASSERT_FALSE(MidiControlNumberIsMsb(MIDI_EFFECT_CONTROLLER_2_LSB));
  TEST_ASSERT_FALSE(MidiControlNumberIsMsb(MIDI_GENERAL_CONTROLLER_4_LSB));
  TEST_ASSERT_FALSE(MidiControlNumberIsMsb(0x3F));  /* Highest LSB number */
  TEST_ASSERT_FALSE(MidiControlNumberIsMsb(MIDI_DAMBER_PEDAL));
  TEST_ASSERT_FALSE(MidiControlNumberIsMsb(MIDI_HOLD_2));
  TEST_ASSERT_FALSE(MidiControlNumberIsMsb(MIDI_PHASER_DEPTH));
  TEST_ASSERT_FALSE(MidiControlNumberIsMsb(MIDI_DATA_INCREMENT));
  TEST_ASSERT_FALSE(MidiControlNumberIsMsb(MIDI_NRPN_MSB));
  TEST_ASSERT_FALSE(MidiControlNumberIsMsb(MIDI_RPN_MSB));
  TEST_ASSERT_FALSE(MidiControlNumberIsMsb(0x66));  /* Lowest undefined */
  TEST_ASSERT_FALSE(MidiControlNumberIsMsb(0x70));
  TEST_ASSERT_FALSE(MidiControlNumberIsMsb(0x77));  /* Highest undefined */
  TEST_ASSERT_FALSE(MidiControlNumberIsMsb(MIDI_ALL_SOUND_OFF));
  TEST_ASSERT_FALSE(MidiControlNumberIsMsb(MIDI_ALL_NOTES_OFF));
  TEST_ASSERT_FALSE(MidiControlNumberIsMsb(MIDI_POLY_MODE_ON));
  /* LSB controller numbers */
  TEST_ASSERT_FALSE(MidiControlNumberIsLsb(MIDI_BANK_SELECT_MSB));
  TEST_ASSERT_FALSE(MidiControlNumberIsLsb(MIDI_EFFECT_CONTROLLER_2_MSB));
  TEST_ASSERT_FALSE(MidiControlNumberIsLsb(MIDI_GENERAL_CONTROLLER_4_MSB));
  TEST_ASSERT_FALSE(MidiControlNumberIsLsb(0x1F));  /* Highest MSB number */
  TEST_ASSERT_TRUE(MidiControlNumberIsLsb(MIDI_BANK_SELECT_LSB));
  TEST_ASSERT_TRUE(MidiControlNumberIsLsb(MIDI_EFFECT_CONTROLLER_2_LSB));
  TEST_ASSERT_TRUE(MidiControlNumberIsLsb(MIDI_GENERAL_CONTROLLER_4_LSB));
  TEST_ASSERT_TRUE(MidiControlNumberIsLsb(0x3F));  /* Highest LSB number */
  TEST_ASSERT_FALSE(MidiControlNumberIsLsb(MIDI_DAMBER_PEDAL));
  TEST_ASSERT_FALSE(MidiControlNumberIsLsb(MIDI_HOLD_2));
  TEST_ASSERT_FALSE(MidiControlNumberIsLsb(MIDI_PHASER_DEPTH));
  TEST_ASSERT_FALSE(MidiControlNumberIsLsb(MIDI_DATA_INCREMENT));
  TEST_ASSERT_FALSE(MidiControlNumberIsLsb(MIDI_NRPN_MSB));
  TEST_ASSERT_FALSE(MidiControlNumberIsLsb(MIDI_RPN_MSB));
  TEST_ASSERT_FALSE(MidiControlNumberIsLsb(0x66));  /* Lowest undefined */
  TEST_ASSERT_FALSE(MidiControlNumberIsLsb(0x70));
  TEST_ASSERT_FALSE(MidiControlNumberIsLsb(0x77));  /* Highest undefined */
  TEST_ASSERT_FALSE(MidiControlNumberIsLsb(MIDI_ALL_SOUND_OFF));
  TEST_ASSERT_FALSE(MidiControlNumberIsLsb(MIDI_ALL_NOTES_OFF));
  TEST_ASSERT_FALSE(MidiControlNumberIsLsb(MIDI_POLY_MODE_ON));
  /* Single byte number */
  TEST_ASSERT_FALSE(MidiControlNumberIsSingleByte(MIDI_BANK_SELECT_MSB));
  TEST_ASSERT_FALSE(MidiControlNumberIsSingleByte(
      MIDI_EFFECT_CONTROLLER_2_MSB));
  TEST_ASSERT_FALSE(MidiControlNumberIsSingleByte(
      MIDI_GENERAL_CONTROLLER_4_MSB));
  TEST_ASSERT_FALSE(MidiControlNumberIsSingleByte(
      0x1F));  /* Highest MSB number */
  TEST_ASSERT_FALSE(MidiControlNumberIsSingleByte(MIDI_BANK_SELECT_LSB));
  TEST_ASSERT_FALSE(MidiControlNumberIsSingleByte(
      MIDI_EFFECT_CONTROLLER_2_LSB));
  TEST_ASSERT_FALSE(MidiControlNumberIsSingleByte(
      MIDI_GENERAL_CONTROLLER_4_LSB));
  TEST_ASSERT_FALSE(MidiControlNumberIsSingleByte(
      0x3F));  /* Highest LSB number */
  TEST_ASSERT_TRUE(MidiControlNumberIsSingleByte(MIDI_DAMBER_PEDAL));
  TEST_ASSERT_TRUE(MidiControlNumberIsSingleByte(MIDI_HOLD_2));
  TEST_ASSERT_TRUE(MidiControlNumberIsSingleByte(MIDI_PHASER_DEPTH));
  TEST_ASSERT_FALSE(MidiControlNumberIsSingleByte(MIDI_DATA_INCREMENT));
  TEST_ASSERT_FALSE(MidiControlNumberIsSingleByte(MIDI_NRPN_MSB));
  TEST_ASSERT_FALSE(MidiControlNumberIsSingleByte(MIDI_RPN_MSB));
  TEST_ASSERT_FALSE(MidiControlNumberIsSingleByte(0x66));
  TEST_ASSERT_FALSE(MidiControlNumberIsSingleByte(0x70));
  TEST_ASSERT_FALSE(MidiControlNumberIsSingleByte(0x77));
  TEST_ASSERT_FALSE(MidiControlNumberIsSingleByte(MIDI_ALL_SOUND_OFF));
  TEST_ASSERT_FALSE(MidiControlNumberIsSingleByte(MIDI_ALL_NOTES_OFF));
  TEST_ASSERT_FALSE(MidiControlNumberIsSingleByte(MIDI_POLY_MODE_ON));
  /* Increment/decrement */
  TEST_ASSERT_FALSE(MidiControlNumberIsIncDec(MIDI_BANK_SELECT_MSB));
  TEST_ASSERT_FALSE(MidiControlNumberIsIncDec(MIDI_EFFECT_CONTROLLER_2_MSB));
  TEST_ASSERT_FALSE(MidiControlNumberIsIncDec(MIDI_GENERAL_CONTROLLER_4_MSB));
  TEST_ASSERT_FALSE(MidiControlNumberIsIncDec(0x1F));  /* Highest MSB number */
  TEST_ASSERT_FALSE(MidiControlNumberIsIncDec(MIDI_BANK_SELECT_LSB));
  TEST_ASSERT_FALSE(MidiControlNumberIsIncDec(MIDI_EFFECT_CONTROLLER_2_LSB));
  TEST_ASSERT_FALSE(MidiControlNumberIsIncDec(MIDI_GENERAL_CONTROLLER_4_LSB));
  TEST_ASSERT_FALSE(MidiControlNumberIsIncDec(0x3F));  /* Highest LSB number */
  TEST_ASSERT_FALSE(MidiControlNumberIsIncDec(MIDI_DAMBER_PEDAL));
  TEST_ASSERT_FALSE(MidiControlNumberIsIncDec(MIDI_HOLD_2));
  TEST_ASSERT_FALSE(MidiControlNumberIsIncDec(MIDI_PHASER_DEPTH));
  TEST_ASSERT_TRUE(MidiControlNumberIsIncDec(MIDI_DATA_INCREMENT));
  TEST_ASSERT_TRUE(MidiControlNumberIsIncDec(MIDI_NRPN_MSB));
  TEST_ASSERT_TRUE(MidiControlNumberIsIncDec(MIDI_RPN_MSB));
  TEST_ASSERT_FALSE(MidiControlNumberIsIncDec(0x66));  /* Lowest undefined */
  TEST_ASSERT_FALSE(MidiControlNumberIsIncDec(0x70));
  TEST_ASSERT_FALSE(MidiControlNumberIsIncDec(0x77));  /* Highest undefined */
  TEST_ASSERT_FALSE(MidiControlNumberIsIncDec(MIDI_ALL_SOUND_OFF));
  TEST_ASSERT_FALSE(MidiControlNumberIsIncDec(MIDI_ALL_NOTES_OFF));
  TEST_ASSERT_FALSE(MidiControlNumberIsIncDec(MIDI_POLY_MODE_ON));
  /* Undefined single byte */
  TEST_ASSERT_FALSE(MidiControlNumberIsUndefinedSingleByte(
      MIDI_BANK_SELECT_MSB));
  TEST_ASSERT_FALSE(MidiControlNumberIsUndefinedSingleByte(
      MIDI_EFFECT_CONTROLLER_2_MSB));
  TEST_ASSERT_FALSE(MidiControlNumberIsUndefinedSingleByte(
      MIDI_GENERAL_CONTROLLER_4_MSB));
  TEST_ASSERT_FALSE(MidiControlNumberIsUndefinedSingleByte(
      0x1F));  /* Highest MSB number */
  TEST_ASSERT_FALSE(MidiControlNumberIsUndefinedSingleByte(
      MIDI_BANK_SELECT_LSB));
  TEST_ASSERT_FALSE(MidiControlNumberIsUndefinedSingleByte(
      MIDI_EFFECT_CONTROLLER_2_LSB));
  TEST_ASSERT_FALSE(MidiControlNumberIsUndefinedSingleByte(
      MIDI_GENERAL_CONTROLLER_4_LSB));
  TEST_ASSERT_FALSE(MidiControlNumberIsUndefinedSingleByte(
      0x3F));  /* Highest LSB number */
  TEST_ASSERT_FALSE(MidiControlNumberIsUndefinedSingleByte(
      MIDI_DAMBER_PEDAL));
  TEST_ASSERT_FALSE(MidiControlNumberIsUndefinedSingleByte(
      MIDI_HOLD_2));
  TEST_ASSERT_FALSE(MidiControlNumberIsUndefinedSingleByte(
      MIDI_PHASER_DEPTH));
  TEST_ASSERT_FALSE(MidiControlNumberIsUndefinedSingleByte(
      MIDI_DATA_INCREMENT));
  TEST_ASSERT_FALSE(MidiControlNumberIsUndefinedSingleByte(
      MIDI_NRPN_MSB));
  TEST_ASSERT_FALSE(MidiControlNumberIsUndefinedSingleByte(
      MIDI_RPN_MSB));
  TEST_ASSERT_TRUE(MidiControlNumberIsUndefinedSingleByte(
      0x66));  /* Lowest undefined */
  TEST_ASSERT_TRUE(MidiControlNumberIsUndefinedSingleByte(0x70));
  TEST_ASSERT_TRUE(MidiControlNumberIsUndefinedSingleByte(
      0x77));  /* Highest undefined */
  TEST_ASSERT_FALSE(MidiControlNumberIsUndefinedSingleByte(
      MIDI_ALL_SOUND_OFF));
  TEST_ASSERT_FALSE(MidiControlNumberIsUndefinedSingleByte(
      MIDI_ALL_NOTES_OFF));
  TEST_ASSERT_FALSE(MidiControlNumberIsUndefinedSingleByte(
      MIDI_POLY_MODE_ON));
  /* Channel Mode. */
  TEST_ASSERT_FALSE(MidiControlNumberIsChannelMode(MIDI_BANK_SELECT_MSB));
  TEST_ASSERT_FALSE(MidiControlNumberIsChannelMode(
      MIDI_EFFECT_CONTROLLER_2_MSB));
  TEST_ASSERT_FALSE(MidiControlNumberIsChannelMode(
      MIDI_GENERAL_CONTROLLER_4_MSB));
  TEST_ASSERT_FALSE(MidiControlNumberIsChannelMode(
      0x1F));  /* Highest MSB number */
  TEST_ASSERT_FALSE(MidiControlNumberIsChannelMode(MIDI_BANK_SELECT_LSB));
  TEST_ASSERT_FALSE(MidiControlNumberIsChannelMode(
      MIDI_EFFECT_CONTROLLER_2_LSB));
  TEST_ASSERT_FALSE(MidiControlNumberIsChannelMode(
      MIDI_GENERAL_CONTROLLER_4_LSB));
  TEST_ASSERT_FALSE(MidiControlNumberIsChannelMode(
      0x3F));  /* Highest LSB number */
  TEST_ASSERT_FALSE(MidiControlNumberIsChannelMode(MIDI_DAMBER_PEDAL));
  TEST_ASSERT_FALSE(MidiControlNumberIsChannelMode(MIDI_HOLD_2));
  TEST_ASSERT_FALSE(MidiControlNumberIsChannelMode(MIDI_PHASER_DEPTH));
  TEST_ASSERT_FALSE(MidiControlNumberIsChannelMode(MIDI_DATA_INCREMENT));
  TEST_ASSERT_FALSE(MidiControlNumberIsChannelMode(MIDI_NRPN_MSB));
  TEST_ASSERT_FALSE(MidiControlNumberIsChannelMode(MIDI_RPN_MSB));
  TEST_ASSERT_FALSE(MidiControlNumberIsChannelMode(
      0x66));  /* Lowest undefined */
  TEST_ASSERT_FALSE(MidiControlNumberIsChannelMode(0x70));
  TEST_ASSERT_FALSE(MidiControlNumberIsChannelMode(
      0x77));  /* Highest undefined */
  TEST_ASSERT_TRUE(MidiControlNumberIsChannelMode(MIDI_ALL_SOUND_OFF));
  TEST_ASSERT_TRUE(MidiControlNumberIsChannelMode(MIDI_ALL_NOTES_OFF));
  TEST_ASSERT_TRUE(MidiControlNumberIsChannelMode(MIDI_POLY_MODE_ON));
}

static void TestMidiControlChange_IsValid(void) {
  TEST_ASSERT_FALSE(MidiIsValidControlChange(NULL));
  midi_control_change_t cc = {};
  TEST_ASSERT_TRUE(MidiIsValidControlChange(&cc));
  cc.number = 0x80;
  TEST_ASSERT_FALSE(MidiIsValidControlChange(&cc));
  cc.value = 0xFF;
  TEST_ASSERT_FALSE(MidiIsValidControlChange(&cc));
  cc.number = 0x7F;
  TEST_ASSERT_FALSE(MidiIsValidControlChange(&cc));
  cc.value = 0x10;
  TEST_ASSERT_TRUE(MidiIsValidControlChange(&cc));
}

static void TestMidiControlChange_Constructor(void) {
  TEST_ASSERT_FALSE(MidiControlChange(NULL, MIDI_BANK_SELECT_LSB, 0x02));
  midi_control_change_t cc = {
    .number = MIDI_MAIN_VOLUME_LSB,
    .value = 0x10
  };
  TEST_ASSERT_TRUE(MidiIsValidControlChange(&cc));
  TEST_ASSERT_FALSE(MidiControlChange(&cc, MIDI_BANK_SELECT_LSB | 0x80, 0x02));
  TEST_ASSERT_FALSE(MidiControlChange(&cc, MIDI_BANK_SELECT_LSB, 0x02 | 0x80));
  /* Remain unchanged. */
  TEST_ASSERT_TRUE(MidiIsValidControlChange(&cc));
  TEST_ASSERT_EQUAL(MIDI_MAIN_VOLUME_LSB, cc.number);
  TEST_ASSERT_EQUAL(0x10, cc.value);

  TEST_ASSERT_TRUE(MidiControlChange(&cc, MIDI_BANK_SELECT_LSB, 0x02));
  TEST_ASSERT_TRUE(MidiIsValidControlChange(&cc));
  TEST_ASSERT_EQUAL(MIDI_BANK_SELECT_LSB, cc.number);
  TEST_ASSERT_EQUAL(0x02, cc.value);
}

void MidiControlTest(void) {
  RUN_TEST(TestMidiControlNumber_DefinedRanges);
  RUN_TEST(TestMidiControlChange_IsValid);
  RUN_TEST(TestMidiControlChange_Constructor);
}
