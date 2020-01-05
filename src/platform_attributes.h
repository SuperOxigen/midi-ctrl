/*
 * MIDI Controller - Platform Attributes
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#ifndef _PLATFORM_ATTRIBUTES_H_
#define _PLATFORM_ATTRIBUTES_H_

#include "base.h"

#ifdef _PLATFORM_AVR
#include <avr/pgmspace.h>
#define __ROM_SECTION PROGMEM
#endif

#ifdef _PLATFORM_NATIVE
#define __ROM_SECTION
#endif

#define __STRINIFY(X) #X
#define __STRINIFY_MACRO_INTERNAL(X...) #X
#define __STRINIFY_MACRO(X...) __STRINIFY_MACRO_INTERNAL(X)

char const *kPlatformName;
char const *kFrameworkName;
char const *kBuildTimeRepr;

#endif  /* _PLATFORM_ATTRIBUTES_H_ */
