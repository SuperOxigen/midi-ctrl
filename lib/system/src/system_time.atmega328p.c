/*
 * MIDI Controller - System Time - ATmega328 Platform
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#ifdef _PLATFORM_AVR
#ifndef _PLATFORM_ARDUINO

#include <avr/interrupt.h>
#include <avr/io.h>

#include "base.h"
#include "system_time.h"

/*
 *  ATmega328 System Time
 *    Use timer 0 to increament system time at set intervals.
 *
 * Timer/Counter Control Register A (TCCR0A) = 0000--00
 * |COM0A1|COM0A0|COM0B1|COM0B0|  --  |  --  | WGM01| WGM00|
 *    Compare Match Output A Mode (COM0A[1:0]) = 00 -> No output
 *    Compare Match Output B Mode (COM0B[1:0]) = 00 -> No output
 *    Waveform Generation Mode (WGM0[1:0])     = 10 -> CTC Mode
 * Timer/Counter Control Register B (TCCR0B) = 00
 * | FOC0A| FOC0B|  --  |  --  | WGM02| CS02 | CS01 | CS00 |
 *    Force Output Compare A (FOC0A)   = 0  -- No function in Normal Mode
 *    Force Output Compare B (FOC0B)   = 0  -- No function in Normal Mode
 *    Waveform Generation Mode (WGM02) = 0  -> CTC Mode
 *    Clock Select (CS0[2:0])          = T0_CLOCK_SELECT
 *        001 - F_CPU (no prescaler)
 *        010 - F_CPU/8
 *        011 - F_CPU/64
 *        100 - F_CPU/256
 *        101 - F_CPU/1024
 * Timer Control Register (TCNT0) -- Unused
 * Output Compare Registers A (OCR0A) = T0_COMPARE
 * Output Compare Registers B (OCR0B) -- Unused
 * Timer Interrupt Mask Register (TIMSK0) = -----001
 * |  --  |  --  |  --  |  --  |  --  |OCIE0B|OCIE0A| TOIE0|
 *    Output Compare Match B Interrupt Enable (OCIE0B) = 0 -> Disabled
 *    Output Compare Match A Interrupt Enable (OCIE0A) = 1 -> Enabled
 *    Timer Overflow Interrupt Enable (TOIE0)          = 0 -> Disabled
 * Timer Interrupt Flag Register (TIFR0) = -----xxx
 * |  --  |  --  |  --  |  --  |  --  | OCF0B| OCF0A| TOV0 |
 *    Output Compare Match B Flag (OCF0B)
 *    Output Compare Match A Flag (OCF0A): overflow -> 1, ISR -> 0
 *    Timer Overflow Flag (TOV0)
 *
 * Interrupt Frequency:
 *    F_T0 = F_CPU / (N * (1 + OCR0A))
 */
#if F_CPU == 20000000L
#define RESOLUTION_NS   20000L /* 20us -> F_T0 = 50 kHz */
#define T0_CLOCK_SELECT 0x2
#define T0_COMPARE 49
#elif F_CPU == 16000000L
#define RESOLUTION_NS   25000L /* 25us -> F_T0 = 40 kHz */
#define T0_CLOCK_SELECT 0x2
#define T0_COMPARE 49
#elif F_CPU == 12000000L
#define RESOLUTION_NS   50000L /* 50us -> F_T0 = 20 kHz */
#define T0_CLOCK_SELECT 0x2
#define T0_COMPARE 74
#elif F_CPU == 10000000L
#define RESOLUTION_NS   40000L /* 40us -> F_T0 = 25 kHz */
#define T0_CLOCK_SELECT 0x2
#define T0_COMPARE 49
#elif F_CPU == 8000000L
#define RESOLUTION_NS   50000L /* 50us -> F_T0 = 20 kHz */
#define T0_CLOCK_SELECT 0x2
#define T0_COMPARE 49
#elif F_CPU == 6000000L
#define RESOLUTION_NS   100000L /* 100us -> F_T0 = 10 kHz */
#define T0_CLOCK_SELECT 0x2
#define T0_COMPARE 74
#elif F_CPU == 5000000L
#define RESOLUTION_NS   80000L /* 80us -> F_T0 = 12.5 kHz */
#define T0_CLOCK_SELECT 0x2
#define T0_COMPARE 49
#elif F_CPU == 4000000L
#define RESOLUTION_NS   100000L /* 100us -> F_T0 = 10.0 kHz */
#define T0_CLOCK_SELECT 0x2
#define T0_COMPARE 49
#elif F_CPU == 2000000L
#define RESOLUTION_NS   200000L /* 200us -> F_T0 = 5.0 kHz */
#define T0_CLOCK_SELECT 0x2
#define T0_COMPARE 49
#elif F_CPU == 1000000L
#define RESOLUTION_NS   400000L  /* 400us -> F_T0 = 2.5 kHz*/
#define T0_CLOCK_SELECT 0x2
#define T0_COMPARE 49
#else
#error "Unsupported CPU speed"
#endif

static system_time_t sSystemTime = {
  .seconds = 0,
  .nanoseconds = 0
};

static bool_t sSystemTimeInitialized = false;

ISR(TIMER0_COMPA_vect) {
  SystemTimeIncrementNanoseconds(&sSystemTime, RESOLUTION_NS);
}

void SystemTimeInitialize(void) {
  if (sSystemTimeInitialized) return;
  cli();
  TCCR0A = _BV(WGM01);
  OCR0A = T0_COMPARE;
  TIFR0 = 0;
  TIMSK0 = _BV(OCIE0A);
  /* Set last, enables timer. */
  TCCR0B = (T0_CLOCK_SELECT << CS00);
  sSystemTimeInitialized = true;
  sei();
}

bool_t SystemTimeNow(system_time_t *time) {
  if (time == NULL || !sSystemTimeInitialized) return false;
  cli();
  time->seconds = sSystemTime.seconds;
  time->nanoseconds = sSystemTime.nanoseconds;
  sei();
  return true;
}

#endif  /* _PLATFORM_ARDUINO */
#endif  /* _PLATFORM_AVR */
