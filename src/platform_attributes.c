/*
 * MIDI Controller - Platform Attributes
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#include "platform_attributes.h"

static char const kUnknown[] __ROM_SECTION= "UNKNOWN";

#ifdef PLATFORM_NAME
static char const kPlatformNameBuf[] __ROM_SECTION = __STRINIFY_MACRO(PLATFORM_NAME);
char const *kPlatformName = kPlatformNameBuf;
#else
#warning "Platform not specified"
char const *kPlatformName = kUnknown;
#endif

#ifdef FRAMWORK_NAME
static char const kFrameworkNameBuf[] __ROM_SECTION = __STRINIFY_MACRO(FRAMWORK_NAME);
char const *kFrameworkName = kFrameworkNameBuf;
#else
#warning "Framework not specified"
char const *kFrameworkName = kUnknown;
#endif

#ifdef BUILD_TIME
static char const kBuildTimeReprBuf[] __ROM_SECTION = __STRINIFY_MACRO(BUILD_TIME);
char const *kBuildTimeRepr = kBuildTimeReprBuf;
#else
#warning "Build time not specified"
char const *kBuildTimeRepr = kUnknown;
#endif
