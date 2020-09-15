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

#define __STRINIFY(X) #X
#define __STRINIFY_MACRO_INTERNAL(X...) #X
#define __STRINIFY_MACRO(X...) __STRINIFY_MACRO_INTERNAL(X)

C_SECTION_BEGIN;

size_t PlatformGetPlatform(char *name, size_t name_size);
size_t PlatformGetFramework(char *name, size_t name_size);
size_t PlatformGetBuildTimeStamp(char *name, size_t name_size);

C_SECTION_END;

#endif  /* _PLATFORM_ATTRIBUTES_H_ */
