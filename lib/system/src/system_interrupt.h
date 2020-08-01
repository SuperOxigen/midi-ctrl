/*
 * MIDI Controller - System Interrupt
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#ifndef _SYSTEM_INTERRUPT_H_
#define _SYSTEM_INTERRUPT_H_

#ifdef _PLATFORM_AVR
#include <avr/interrupt.h>
#define SystemDisableInterrupt() cli()
#define SystemEnableInterrupt() sei()
#else  /* else !_PLATFORM_AVR */
#include "base.h"
C_SECTION_BEGIN;
/* Enables or disable global interrupts.  Does not make any guarentee
 * for non-maskable interrupts.
 * None of these should be called from within an interrupt.
 */
void SystemDisableInterrupt(void);
void SystemEnableInterrupt(void);
C_SECTION_END;
#endif  /* not _PLATFORM_AVR */

#endif  /* _SYSTEM_INTERRUPT_H_ */
