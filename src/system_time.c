/*
 * MIDI Controller - System Time
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#include "system_time.h"

#define MAX_NANOSECONDS 999999u
#define MAX_SECONDS 4294967295u

#define SystemTimeMax(time) \
  (time)->seconds = MAX_SECONDS; \
  (time)->nanoseconds = MAX_NANOSECONDS

#define SystemTimeZero(time) \
  (time)->seconds = 0; \
  (time)->nanoseconds = 0

#define NanoToSeconds(ns) ((ns) / 1000000)
#define NanoToMilli(ns) ((ns) / 1000)
#define MilliToSeconds(ms) ((ms) / 1000)
#define MilliToNano(ms) ((ms) * 1000)
#define SecondsToNano(s) ((s) * 1000000)
#define SecondsToMilli(s) ((s) * 1000)

inline static bool_t SystemTimeIsValid(system_time_t const *time) {
  if (time == NULL) return false;
  return time->nanoseconds <= MAX_NANOSECONDS;
}

/* Total orderings */
bool_t SystemTimeLessThan(system_time_t const *a, system_time_t const *b) {
  if (!SystemTimeIsValid(a) || !SystemTimeIsValid(b)) return false;
  return (a->seconds == b->seconds) ? (a->nanoseconds < b->nanoseconds)
                                    : (a->seconds < b->seconds);
}

bool_t SystemTimeLessThanOrEqual(
    system_time_t const *a, system_time_t const *b) {
  if (!SystemTimeIsValid(a) || !SystemTimeIsValid(b)) return false;
  return (a->seconds == b->seconds) ? (a->nanoseconds <= b->nanoseconds)
                                    : (a->seconds < b->seconds);
}

bool_t SystemTimeEqual(system_time_t const *a, system_time_t const *b) {
  if (!SystemTimeIsValid(a) || !SystemTimeIsValid(b)) return false;
  return (a->seconds == b->seconds) && (a->nanoseconds == b->nanoseconds);
}

bool_t SystemTimeGreaterThanOrEqual(
    system_time_t const *a, system_time_t const *b) {
  if (!SystemTimeIsValid(a) || !SystemTimeIsValid(b)) return false;
  return (a->seconds == b->seconds) ? (a->nanoseconds >= b->nanoseconds)
                                    : (a->seconds > b->seconds);
}

bool_t SystemTimeGreaterThan(system_time_t const *a, system_time_t const *b) {
  if (!SystemTimeIsValid(a) || !SystemTimeIsValid(b)) return false;
  return (a->seconds == b->seconds) ? (a->nanoseconds > b->nanoseconds)
                                    : (a->seconds > b->seconds);
}

/* Time deltas */
bool_t SystemTimeSecondsDelta(
    system_time_t const *a, system_time_t const *b, uint32_t *seconds) {
  if (!SystemTimeIsValid(a) || !SystemTimeIsValid(b) || seconds == NULL)
    return false;
  if (a->seconds == b->seconds) {
    *seconds = 0;
  } else if (a->seconds < b->seconds) {
    *seconds = (b->seconds - a->seconds) ;
    if (a->nanoseconds > b->nanoseconds) {
      *seconds -= 1;
    }
  } else /* (a->seconds > b->seconds) */ {
    *seconds = (a->seconds - b->seconds);
    if (a->nanoseconds < b->nanoseconds) {
      *seconds -= 1;
    }
  }
  return true;
}

#define MAX_SECONDS_FOR_MILLISECONDS 4294967u
#define MAX_NANOSECONDS_FOR_MILLISECONDS 295000u

bool_t SystemTimeMillisecondsDelta(
    system_time_t const *a, system_time_t const *b, uint32_t *milliseconds) {
  if (!SystemTimeIsValid(a) || !SystemTimeIsValid(b) || milliseconds == NULL)
    return false;
  if (a->seconds == b->seconds) {
    if (a->nanoseconds != b->nanoseconds) {
      *milliseconds = NanoToMilli(
          (a->nanoseconds < b->nanoseconds) ? b->nanoseconds - a->nanoseconds
                                            : a->nanoseconds - b->nanoseconds);
    } else {
      *milliseconds = 0;
    }
  } else if (a->seconds < b->seconds) {
    uint32_t const seconds_delta = b->seconds - a->seconds;
    /* Check for overflow */
    if (seconds_delta > MAX_SECONDS_FOR_MILLISECONDS) return false;
    if (seconds_delta == MAX_SECONDS_FOR_MILLISECONDS &&
        (a->nanoseconds < b->nanoseconds) &&
        (b->nanoseconds - a->nanoseconds) > MAX_NANOSECONDS_FOR_MILLISECONDS) {
      return false;
    }
    *milliseconds = seconds_delta * 1000;
    if (a->nanoseconds < b->nanoseconds) {
      *milliseconds += NanoToMilli(b->nanoseconds - a->nanoseconds);
    } else if (a->nanoseconds > b->nanoseconds) {
      uint32_t const nanoseconds_delta = a->nanoseconds - b->nanoseconds;
      *milliseconds -= NanoToMilli(nanoseconds_delta);
      if ((nanoseconds_delta % 1000) != 0) {
        --(*milliseconds);
      }
    }
  } else /* (a->seconds > b->seconds) */ {
    uint32_t const seconds_delta = a->seconds - b->seconds;
    /* Check for overflow */
    if (seconds_delta > MAX_SECONDS_FOR_MILLISECONDS) return false;
    if (seconds_delta == MAX_SECONDS_FOR_MILLISECONDS &&
        (a->nanoseconds > b->nanoseconds) &&
        (a->nanoseconds - b->nanoseconds) > MAX_NANOSECONDS_FOR_MILLISECONDS) {
      return false;
    }
    *milliseconds = SecondsToMilli(seconds_delta);
    if (a->nanoseconds > b->nanoseconds) {
      *milliseconds += ((a->nanoseconds - b->nanoseconds) / 1000);
    } else if (a->nanoseconds < b->nanoseconds) {
      uint32_t const nanoseconds_delta = b->nanoseconds - a->nanoseconds;
      *milliseconds -= (nanoseconds_delta / 1000);
      if ((nanoseconds_delta % 1000) != 0) {
        *milliseconds -= 1;
      }
    }
  }
  return true;
}

#define MAX_SECONDS_FOR_NANOSECONDS 4294u
#define MAX_NANOSECONDS_FOR_NANOSECONDS 967295u

bool_t SystemTimeNanosecondsDelta(
    system_time_t const *a, system_time_t const *b, uint32_t *nanoseconds) {
  if (!SystemTimeIsValid(a) || !SystemTimeIsValid(b) || nanoseconds == NULL)
    return false;
  if (a->seconds == b->seconds) {
    if (a->nanoseconds < b->nanoseconds) {
      *nanoseconds = (b->nanoseconds - a->nanoseconds);
    } else if (a->nanoseconds > b->nanoseconds) {
      *nanoseconds = (a->nanoseconds - b->nanoseconds);
    } else {
      *nanoseconds = 0;
    }
  } else if (a->seconds < b->seconds) {
    uint32_t const seconds_delta = b->seconds - a->seconds;
    /* Check for overflow */
    if (seconds_delta > MAX_SECONDS_FOR_NANOSECONDS) return false;
    if (seconds_delta == MAX_SECONDS_FOR_NANOSECONDS &&
        (a->nanoseconds < b->nanoseconds) &&
        (b->nanoseconds - a->nanoseconds) > MAX_NANOSECONDS_FOR_NANOSECONDS) {
      return false;
    }
    *nanoseconds = SecondsToNano(seconds_delta);
    if (a->nanoseconds < b->nanoseconds) {
      *nanoseconds += (b->nanoseconds - a->nanoseconds);
    } else if (a->nanoseconds > b->nanoseconds) {
      *nanoseconds -= (a->nanoseconds - b->nanoseconds);
    }
  } else /* (a->seconds > b->seconds) */ {
    uint32_t const seconds_delta = a->seconds - b->seconds;
    /* Check for overflow */
    if (seconds_delta > MAX_SECONDS_FOR_NANOSECONDS) return false;
    if (seconds_delta == MAX_SECONDS_FOR_NANOSECONDS &&
        (a->nanoseconds > b->nanoseconds) &&
        (a->nanoseconds - b->nanoseconds) > MAX_NANOSECONDS_FOR_NANOSECONDS) {
      return false;
    }
    *nanoseconds = SecondsToNano(seconds_delta);
    if (a->nanoseconds > b->nanoseconds) {
      *nanoseconds += (a->nanoseconds - b->nanoseconds);
    } else if (a->nanoseconds < b->nanoseconds) {
      *nanoseconds -= (b->nanoseconds - a->nanoseconds);
    }
  }
  return true;
}

/* Arithmetic Functions */


bool_t SystemTimeIncrementSeconds(system_time_t *time, uint32_t seconds) {
  if (!SystemTimeIsValid(time)) return false;
  uint32_t const new_seconds = time->seconds + seconds;
  if (new_seconds < seconds || new_seconds < time->seconds) {
    SystemTimeMax(time);
  } else {
    time->seconds = new_seconds;
  }
  return true;
}

bool_t SystemTimeDecrementSeconds(system_time_t *time, uint32_t seconds) {
  if (!SystemTimeIsValid(time)) return false;
  if (time->seconds < seconds) {
    SystemTimeZero(time);
  } else {
    time->seconds -= seconds;
  }
  return true;
}

bool_t SystemTimeIncrementMilliseconds(
    system_time_t *time, uint32_t milliseconds) {
  if (!SystemTimeIsValid(time)) return false;
  uint32_t const nanoseconds =
      time->nanoseconds + MilliToNano(milliseconds % 1000);
  uint32_t const seconds =
      time->seconds + MilliToSeconds(milliseconds) +
      (nanoseconds > MAX_NANOSECONDS ? 1 : 0);
  if (seconds < time->seconds) {
    SystemTimeMax(time);
  } else {
    time->seconds = seconds;
    time->nanoseconds =
        nanoseconds - (nanoseconds > MAX_NANOSECONDS ? 1000000 : 0);
  }
  return true;
}

bool_t SystemTimeDecrementMilliseconds(
    system_time_t *time, uint32_t milliseconds) {
  if (!SystemTimeIsValid(time)) return false;
  uint32_t const nanoseconds = MilliToNano(milliseconds % 1000);
  uint32_t const seconds = MilliToSeconds(milliseconds)
      + (nanoseconds > time->nanoseconds ? 1 : 0);
  if (seconds > time->seconds) {
    SystemTimeZero(time);
  } else {
    time->seconds -= seconds;
    if (nanoseconds > time->nanoseconds) {
      time->nanoseconds += (1000000 - nanoseconds);
    } else {
      time->nanoseconds -= nanoseconds;
    }
  }
  return true;
}

bool_t SystemTimeIncrementNanoseconds(
    system_time_t *time, uint32_t nanoseconds) {
  if (!SystemTimeIsValid(time)) return false;
  uint32_t const new_nanoseconds = time->nanoseconds + (nanoseconds % 1000000);
  uint32_t const new_seconds =
      time->seconds + NanoToSeconds(nanoseconds) +
      (new_nanoseconds > MAX_NANOSECONDS ? 1 : 0);
  if (new_seconds < time->seconds) {
    SystemTimeMax(time);
  } else {
    time->seconds = new_seconds;
    time->nanoseconds =
      new_nanoseconds - ((new_nanoseconds > MAX_NANOSECONDS) ? 1000000 : 0);
  }
  return true;
}

bool_t SystemTimeDecrementNanoseconds(
    system_time_t *time, uint32_t nanoseconds) {
  if (!SystemTimeIsValid(time)) return false;
  uint32_t const new_nanoseconds = nanoseconds % 1000000;
  uint32_t const seconds =
      NanoToSeconds(nanoseconds) +
      (new_nanoseconds > time->nanoseconds ? 1 : 0);
  if (seconds > time->seconds) {
    SystemTimeZero(time);
  } else {
    time->seconds -= seconds;
    if (new_nanoseconds > time->nanoseconds) {
      time->nanoseconds += (1000000 - new_nanoseconds);
    } else {
      time->nanoseconds -= new_nanoseconds;
    }
  }
  return true;
}
