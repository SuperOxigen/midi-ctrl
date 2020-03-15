/*
 * MIDI Controller - System Time
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#include "system_time.h"

/*
 *  Maximum System Time Values.
 */

#define MAX_U32 4294967295u
#define MAX_NANOSECONDS 999999999u
#define MAX_SECONDS MAX_U32

#define SystemTimeSetMax(time) \
  (time)->seconds = MAX_SECONDS; \
  (time)->nanoseconds = MAX_NANOSECONDS

#define SystemTimeSetZero(time) \
  (time)->seconds = 0u; \
  (time)->nanoseconds = 0u

/*
 *  Time Unit Conversion Macros.
 */

#define NANO_PER_SECONDS  1000000000u
#define NANO_PER_MILLI    1000000u
#define NANO_PER_MICRO    1000u
#define MICRO_PER_SECONDS 1000000u
#define MICRO_PER_MILLI   1000u
#define MILLI_PER_SECONDS 1000u

#define NanoToSeconds(ns)   ((ns) / NANO_PER_SECONDS)
#define NanoToMilli(ns)     ((ns) / NANO_PER_MILLI)
#define NanoToMicro(ns)     ((ns) / NANO_PER_MICRO)
#define MicroToSeconds(us)  ((us) / NANO_PER_MILLI)
#define MicroToMilli(us)    ((us) / MICRO_PER_MILLI)
#define MicroToNano(us)     ((us) * NANO_PER_MICRO)
#define MilliToSeconds(ms)  ((ms) / MILLI_PER_SECONDS)
#define MilliToMicro(ms)    ((ms) * MICRO_PER_MILLI)
#define MilliToNano(ms)     ((ms) * NANO_PER_MILLI)
#define SecondsToMilli(s)   ((s) * MILLI_PER_SECONDS)
#define SecondsToMicro(s)   ((s) * MICRO_PER_SECONDS)
#define SecondsToNano(s)    ((s) * NANO_PER_SECONDS)

/*
 * Validation
 */

inline static bool_t SystemTimeIsValid(system_time_t const *time) {
  if (time == NULL) return false;
  return time->nanoseconds <= MAX_NANOSECONDS;
}

/*
 * Total Orderings
 */

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

/* TODO: Add not equal? */

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

/*
 * Time Delta Calculations
 */

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

#define MAX_SECONDS_FOR_MILLI 4294967u
#define MAX_NANO_FOR_MILLI    295999999u

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
    if (seconds_delta > MAX_SECONDS_FOR_MILLI) return false;
    if (seconds_delta == MAX_SECONDS_FOR_MILLI &&
        (a->nanoseconds < b->nanoseconds) &&
        (b->nanoseconds - a->nanoseconds) > MAX_NANO_FOR_MILLI) {
      return false;
    }
    *milliseconds = SecondsToMilli(seconds_delta);
    if (a->nanoseconds < b->nanoseconds) {
      *milliseconds += NanoToMilli(b->nanoseconds - a->nanoseconds);
    } else if (a->nanoseconds > b->nanoseconds) {
      uint32_t const nanoseconds_delta = a->nanoseconds - b->nanoseconds;
      *milliseconds -= NanoToMilli(nanoseconds_delta);
      if ((nanoseconds_delta % NANO_PER_MILLI) != 0) {
        --(*milliseconds);
      }
    }
  } else /* (a->seconds > b->seconds) */ {
    uint32_t const seconds_delta = a->seconds - b->seconds;
    /* Check for overflow */
    if (seconds_delta > MAX_SECONDS_FOR_MILLI) return false;
    if (seconds_delta == MAX_SECONDS_FOR_MILLI &&
        (a->nanoseconds > b->nanoseconds) &&
        (a->nanoseconds - b->nanoseconds) > MAX_NANO_FOR_MILLI) {
      return false;
    }
    *milliseconds = SecondsToMilli(seconds_delta);
    if (a->nanoseconds > b->nanoseconds) {
      *milliseconds += NanoToMilli(a->nanoseconds - b->nanoseconds);
    } else if (a->nanoseconds < b->nanoseconds) {
      uint32_t const nanoseconds_delta = b->nanoseconds - a->nanoseconds;
      *milliseconds -= NanoToMilli(nanoseconds_delta);
      if ((nanoseconds_delta % NANO_PER_MILLI) != 0) {
        --(*milliseconds);
      }
    }
  }
  return true;
}

#define MAX_SECONDS_FOR_MICRO 4294u
#define MAX_NANO_FOR_MICRO    967295999u

bool_t SystemTimeMicrosecondsDelta(
    system_time_t const *a, system_time_t const *b, uint32_t *microseconds) {
  if (!SystemTimeIsValid(a) || !SystemTimeIsValid(b) || microseconds == NULL)
    return false;
  if (a->seconds == b->seconds) {
    if (a->nanoseconds != b->nanoseconds) {
      *microseconds = NanoToMicro(
          (a->nanoseconds < b->nanoseconds) ? b->nanoseconds - a->nanoseconds
                                            : a->nanoseconds - b->nanoseconds);
    } else {
      *microseconds = 0;
    }
  } else if (a->seconds < b->seconds) {
    uint32_t const seconds_delta = b->seconds - a->seconds;
    /* Check for overflow */
    if (seconds_delta > MAX_SECONDS_FOR_MICRO) return false;
    if (seconds_delta == MAX_SECONDS_FOR_MICRO &&
        (a->nanoseconds < b->nanoseconds) &&
        (b->nanoseconds - a->nanoseconds) > MAX_NANO_FOR_MICRO) {
      return false;
    }
    *microseconds = SecondsToMicro(seconds_delta);
    if (a->nanoseconds < b->nanoseconds) {
      *microseconds += NanoToMicro(b->nanoseconds - a->nanoseconds);
    } else if (a->nanoseconds > b->nanoseconds) {
      uint32_t const nanoseconds_delta = a->nanoseconds - b->nanoseconds;
      *microseconds -= NanoToMicro(nanoseconds_delta);
      if ((nanoseconds_delta % NANO_PER_MICRO) != 0) {
        --(*microseconds);
      }
    }
  } else /* (a->seconds > b->seconds) */ {
    uint32_t const seconds_delta = a->seconds - b->seconds;
    /* Check for overflow */
    if (seconds_delta > MAX_SECONDS_FOR_MICRO) return false;
    if (seconds_delta == MAX_SECONDS_FOR_MICRO &&
        (a->nanoseconds > b->nanoseconds) &&
        (a->nanoseconds - b->nanoseconds) > MAX_NANO_FOR_MICRO) {
      return false;
    }
    *microseconds = SecondsToMicro(seconds_delta);
    if (a->nanoseconds > b->nanoseconds) {
      *microseconds += NanoToMicro(a->nanoseconds - b->nanoseconds);
    } else if (a->nanoseconds < b->nanoseconds) {
      uint32_t const nanoseconds_delta = b->nanoseconds - a->nanoseconds;
      *microseconds -= NanoToMicro(nanoseconds_delta);
      if ((nanoseconds_delta % NANO_PER_MICRO) != 0) {
        --(*microseconds);
      }
    }
  }
  return true;
}

#define MAX_SECONDS_FOR_NANO 4u
#define MAX_NANO_FOR_NANO    294967295u

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
    if (seconds_delta > MAX_SECONDS_FOR_NANO) return false;
    if (seconds_delta == MAX_SECONDS_FOR_NANO &&
        (a->nanoseconds < b->nanoseconds) &&
        (b->nanoseconds - a->nanoseconds) > MAX_NANO_FOR_NANO) {
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
    if (seconds_delta > MAX_SECONDS_FOR_NANO) return false;
    if (seconds_delta == MAX_SECONDS_FOR_NANO &&
        (a->nanoseconds > b->nanoseconds) &&
        (a->nanoseconds - b->nanoseconds) > MAX_NANO_FOR_NANO) {
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
    SystemTimeSetMax(time);
  } else {
    time->seconds = new_seconds;
  }
  return true;
}

bool_t SystemTimeDecrementSeconds(system_time_t *time, uint32_t seconds) {
  if (!SystemTimeIsValid(time)) return false;
  if (time->seconds < seconds) {
    SystemTimeSetZero(time);
  } else {
    time->seconds -= seconds;
  }
  return true;
}

bool_t SystemTimeIncrementMilliseconds(
    system_time_t *time, uint32_t milliseconds) {
  if (!SystemTimeIsValid(time)) return false;
  uint32_t const nanoseconds =
      time->nanoseconds + MilliToNano(milliseconds % MILLI_PER_SECONDS);
  uint32_t const seconds =
      time->seconds + MilliToSeconds(milliseconds) +
      (nanoseconds > MAX_NANOSECONDS ? 1 : 0);
  if (seconds < time->seconds) {
    SystemTimeSetMax(time);
  } else {
    time->seconds = seconds;
    time->nanoseconds =
        nanoseconds - (nanoseconds > MAX_NANOSECONDS ? NANO_PER_SECONDS : 0);
  }
  return true;
}

bool_t SystemTimeDecrementMilliseconds(
    system_time_t *time, uint32_t milliseconds) {
  if (!SystemTimeIsValid(time)) return false;
  uint32_t const nanoseconds = MilliToNano(milliseconds % MILLI_PER_SECONDS);
  uint32_t const seconds = MilliToSeconds(milliseconds);
  if (seconds > time->seconds) {
    SystemTimeSetZero(time);
  } else {
    time->seconds -= seconds;
    if (nanoseconds > time->nanoseconds) {
      --(time->seconds);
      time->nanoseconds += (NANO_PER_SECONDS - nanoseconds);
    } else {
      time->nanoseconds -= nanoseconds;
    }
  }
  return true;
}

bool_t SystemTimeIncrementMicroseconds(
    system_time_t *time, uint32_t microseconds) {
  if (!SystemTimeIsValid(time)) return false;
  uint32_t const nanoseconds =
      time->nanoseconds + MicroToNano(microseconds % MICRO_PER_SECONDS);
  uint32_t const seconds =
      time->seconds + MicroToSeconds(microseconds) +
      (nanoseconds > MAX_NANOSECONDS ? 1 : 0);
  if (seconds < time->seconds) {
    SystemTimeSetMax(time);
  } else {
    time->seconds = seconds;
    time->nanoseconds =
        nanoseconds - (nanoseconds > MAX_NANOSECONDS ? NANO_PER_SECONDS : 0);
  }
  return true;
}

bool_t SystemTimeDecrementMicroseconds(
    system_time_t *time, uint32_t microseconds) {
  if (!SystemTimeIsValid(time)) return false;
  uint32_t const nanoseconds = MicroToNano(microseconds % MICRO_PER_SECONDS);
  uint32_t const seconds =
      MicroToSeconds(microseconds) +
      (nanoseconds > time->nanoseconds ? 1 : 0);
  if (seconds > time->seconds) {
    SystemTimeSetZero(time);
  } else {
    time->seconds -= seconds;
    if (nanoseconds > time->nanoseconds) {
      time->nanoseconds += (NANO_PER_SECONDS - nanoseconds);
    } else {
      time->nanoseconds -= nanoseconds;
    }
  }
  return true;
}

bool_t SystemTimeIncrementNanoseconds(
    system_time_t *time, uint32_t nanoseconds) {
  if (!SystemTimeIsValid(time)) return false;
  uint32_t const new_nanoseconds =
      time->nanoseconds + (nanoseconds % NANO_PER_SECONDS);
  uint32_t const new_seconds =
      time->seconds + NanoToSeconds(nanoseconds) +
      (new_nanoseconds > MAX_NANOSECONDS ? 1 : 0);
  if (new_seconds < time->seconds) {
    SystemTimeSetMax(time);
  } else {
    time->seconds = new_seconds;
    time->nanoseconds =
      new_nanoseconds -
      ((new_nanoseconds > MAX_NANOSECONDS) ? NANO_PER_SECONDS : 0);
  }
  return true;
}

bool_t SystemTimeDecrementNanoseconds(
    system_time_t *time, uint32_t nanoseconds) {
  if (!SystemTimeIsValid(time)) return false;
  uint32_t const new_nanoseconds = nanoseconds % NANO_PER_SECONDS;
  uint32_t const seconds =
      NanoToSeconds(nanoseconds) +
      (new_nanoseconds > time->nanoseconds ? 1 : 0);
  if (seconds > time->seconds) {
    SystemTimeSetZero(time);
  } else {
    time->seconds -= seconds;
    if (new_nanoseconds > time->nanoseconds) {
      time->nanoseconds += (NANO_PER_SECONDS - new_nanoseconds);
    } else {
      time->nanoseconds -= new_nanoseconds;
    }
  }
  return true;
}
