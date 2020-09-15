/*
 * MIDI Controller - Platform Attributes
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#include "platform_attributes.h"
#include "program_memory.h"

#ifdef PLATFORM_NAME
static char const kPlatformName[] __ROM_SECTION =
    __STRINIFY_MACRO(PLATFORM_NAME);
#else
#warning "Platform not specified"
static char const kPlatformName[] __ROM_SECTION = "UNKNOWN";
#endif

#ifdef FRAMWORK_NAME
static char const kFrameworkName[] __ROM_SECTION =
    __STRINIFY_MACRO(FRAMWORK_NAME);
#else
#warning "Framework not specified"
static char const kFrameworkName[] = "UNKNOWN";
#endif

#ifdef BUILD_TIME
static char const kBuildTimeRepr[] __ROM_SECTION = __STRINIFY_MACRO(BUILD_TIME);
#else
#warning "Build time not specified"
static char const kBuildTimeRepr[] = "UNKNOWN";
#endif

size_t PlatformGetPlatform(char *name, size_t name_size) {
  if (name == NULL && name_size > 0) return 0;
  return ProgMemoryCopyString(kPlatformName, name, name_size);
}

size_t PlatformGetFramework(char *name, size_t name_size) {
  if (name == NULL && name_size > 0) return 0;
  return ProgMemoryCopyString(kFrameworkName, name, name_size);
}

size_t PlatformGetBuildTimeStamp(char *name, size_t name_size) {
  if (name == NULL && name_size > 0) return 0;
  return ProgMemoryCopyString(kBuildTimeRepr, name, name_size);
}
