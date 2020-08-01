/*
 * MIDI Controller - System Scheduler
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#include <string.h>

#include "system_interrupt.h"
#include "scheduler.h"

#define EVENT_NULL  0

#define NULL_ENTRY_TYPE 0
#define TIMER_ENTRY_TYPE 1
#define EVENT_ENTRY_TYPE 2

bool_t SchedulerInitialize(
    scheduler_t *scheduler, system_time_t const *init_time) {
  if (scheduler == NULL || init_time == NULL) return false;
  memset(scheduler, 0, sizeof(scheduler_t));
  memcpy(&scheduler->last_update, init_time, sizeof(system_time_t));
  return true;
}

bool_t SchedulerClearEvents(scheduler_t *scheduler) {
  if (scheduler == NULL) return false;
  for (uint8_t i = 0; i < SCHEDULER_EVENT_QUEUE_SIZE; ++i) {
    scheduler->events[i] = EVENT_NULL;
  }
  scheduler->event_count = 0;
  return true;
}

bool_t SchedulerClearCallbacks(scheduler_t *scheduler) {
  if (scheduler == NULL) return false;
  for (uint8_t i = 0; i < SCHEDULER_CALLBACK_TABLE_SIZE; ++i) {
    scheduler->entries[i].type = NULL_ENTRY_TYPE;
  }
  scheduler->entry_count = 0;
  return true;
}

static size_t SchedulerDoTimerCallbacksInternal(scheduler_t *scheduler);
static size_t SchedulerDoEventCallbacksInternal(scheduler_t *scheduler);

size_t SchedulerDoCallbacks(
    scheduler_t *scheduler, system_time_t const *time) {
  if (scheduler == NULL || time == NULL) return 0;
  if (SystemTimeLessThan(time, &scheduler->last_update)) return 0;
  memcpy(&scheduler->last_update, time, sizeof(system_time_t));
  size_t const job_count = SchedulerDoTimerCallbacksInternal(scheduler) +
                           SchedulerDoEventCallbacksInternal(scheduler);
  size_t dest_idx = 0;
  for (size_t src_idx = 0; src_idx < scheduler->entry_count; ++src_idx) {
    if (scheduler->entries[src_idx].type == NULL_ENTRY_TYPE) continue;
    while (dest_idx < src_idx &&
           scheduler->entries[dest_idx].type != NULL_ENTRY_TYPE) {
      ++dest_idx;
    }
    if (dest_idx == src_idx) continue;
    memcpy(&scheduler->entries[dest_idx], &scheduler->entries[src_idx],
           sizeof(scheduler_callback_entry_t));
    scheduler->entries[src_idx].type = NULL_ENTRY_TYPE;
  }
  while (dest_idx < scheduler->entry_count &&
         scheduler->entries[dest_idx].type != NULL_ENTRY_TYPE) {
    ++dest_idx;
  }
  scheduler->entry_count = dest_idx;
  return job_count;
}

#define SchedulerIsFull(scheduler) \
  ((scheduler)->entry_count >= SCHEDULER_CALLBACK_TABLE_SIZE)

#define SchedulerIsEventFull(scheduler) \
  ((scheduler)->event_count >= SCHEDULER_EVENT_QUEUE_SIZE)

#define SchedulerAllocateCallback(scheduler) \
  (&(scheduler)->entries[(scheduler)->entry_count++])

/*
 *  Timer Callbacks
 */
#define TIMER_PERIODIC_S     0x01
#define TIMER_PERIODIC_US    0x02
#define TIMER_PERIODIC (TIMER_PERIODIC_S | TIMER_PERIODIC_US)

static scheduler_timer_callback_entry_t *SchedulerAllocateTimerCallback(
    scheduler_t *scheduler) {
  if (scheduler == NULL) return NULL;
  scheduler_callback_entry_t *entry = SchedulerAllocateCallback(scheduler);
  memset(entry, 0, sizeof(scheduler_callback_entry_t));
  entry->type = TIMER_ENTRY_TYPE;
  return &entry->timer_entry;
}

bool_t SchedulerSetPeriodicCallbackMicroseconds(
    scheduler_t *scheduler, uint32_t period, system_time_t const *current_time,
    scheduler_timer_callback_t callback, void *callback_ctx) {
  if (scheduler == NULL || period == 0 || callback == NULL) return false;
  if (SchedulerIsFull(scheduler)) return false;
  scheduler_timer_callback_entry_t *entry =
      SchedulerAllocateTimerCallback(scheduler);
  *entry = (scheduler_timer_callback_entry_t) {
    .callback = callback,
    .ctx = callback_ctx,
    .period = period,
    .flags = TIMER_PERIODIC_US
  };

  if (current_time == NULL) {
    memcpy(&entry->timer, &scheduler->last_update, sizeof(system_time_t));
  } else {
    if (SystemTimeLessThan(current_time, &scheduler->last_update)) {
      --scheduler->entry_count;
      return false;
    }
    memcpy(&entry->timer, current_time, sizeof(system_time_t));
  }

  if (!SystemTimeIncrementMicroseconds(&entry->timer, period)) {
    --scheduler->entry_count;
    return false;
  }
  return true;
}

bool_t SchedulerSetPeriodicCallbackSeconds(
    scheduler_t *scheduler, uint32_t period, system_time_t const *current_time,
    scheduler_timer_callback_t callback, void *callback_ctx) {
  if (scheduler == NULL || period == 0  || callback == NULL) return false;
  if (SchedulerIsFull(scheduler)) return false;
  scheduler_timer_callback_entry_t *entry =
      SchedulerAllocateTimerCallback(scheduler);
  *entry = (scheduler_timer_callback_entry_t) {
    .callback = callback,
    .ctx = callback_ctx,
    .period = period,
    .flags = TIMER_PERIODIC_S
  };

  if (current_time == NULL) {
    memcpy(&entry->timer, &scheduler->last_update, sizeof(system_time_t));
  } else {
    if (SystemTimeLessThan(current_time, &scheduler->last_update)) {
      --scheduler->entry_count;
      return false;
    }
    memcpy(&entry->timer, current_time, sizeof(system_time_t));
  }

  if (!SystemTimeIncrementSeconds(&entry->timer, period)) {
    --scheduler->entry_count;
    return false;
  }
  return true;
}

bool_t SchedulerSetDelayedCallbackMicroseconds(
    scheduler_t *scheduler, uint32_t delay, system_time_t const *current_time,
    scheduler_timer_callback_t callback, void *callback_ctx) {
  if (scheduler == NULL || callback == NULL) return false;
  if (SchedulerIsFull(scheduler)) return false;
  scheduler_timer_callback_entry_t *entry =
      SchedulerAllocateTimerCallback(scheduler);
  *entry = (scheduler_timer_callback_entry_t) {
    .callback = callback,
    .ctx = callback_ctx
  };

  if (current_time == NULL) {
    memcpy(&entry->timer, &scheduler->last_update, sizeof(system_time_t));
  } else {
    if (SystemTimeLessThan(current_time, &scheduler->last_update)) {
      --scheduler->entry_count;
      return false;
    }
    memcpy(&entry->timer, current_time, sizeof(system_time_t));
  }

  if (!SystemTimeIncrementMicroseconds(&entry->timer, delay)) {
    --scheduler->entry_count;
    return false;
  }
  return true;
}

bool_t SchedulerSetDelayedCallbackSeconds(
    scheduler_t *scheduler, uint32_t delay, system_time_t const *current_time,
    scheduler_timer_callback_t callback, void *callback_ctx) {
  if (scheduler == NULL || callback == NULL) return false;
  if (SchedulerIsFull(scheduler)) return false;
  scheduler_timer_callback_entry_t *entry =
      SchedulerAllocateTimerCallback(scheduler);
  *entry = (scheduler_timer_callback_entry_t) {
    .callback = callback,
    .ctx = callback_ctx
  };

  if (current_time == NULL) {
    memcpy(&entry->timer, &scheduler->last_update, sizeof(system_time_t));
  } else {
    if (SystemTimeLessThan(current_time, &scheduler->last_update)) {
      --scheduler->entry_count;
      return false;
    }
    memcpy(&entry->timer, current_time, sizeof(system_time_t));
  }

  if (!SystemTimeIncrementSeconds(&entry->timer, delay)) {
    --scheduler->entry_count;
    return false;
  }
  return true;
}

static size_t SchedulerDoTimerCallbacksInternal(scheduler_t *scheduler) {
  size_t job_count = 0;
  for (size_t i = 0; i < scheduler->entry_count; ++i) {
    if (scheduler->entries[i].type != TIMER_ENTRY_TYPE) continue;
    scheduler_timer_callback_entry_t *entry =
        &scheduler->entries[i].timer_entry;
    if (SystemTimeLessThan(&scheduler->last_update, &entry->timer)) continue;
    entry->callback(entry->ctx, &scheduler->last_update);
    ++job_count;
    if (entry->flags & TIMER_PERIODIC) {
      bool_t (*incrementer)(system_time_t *, uint32_t) =
          (entry->flags & TIMER_PERIODIC_US) ? SystemTimeIncrementMicroseconds
                                             : SystemTimeIncrementSeconds;
      while (SystemTimeLessThanOrEqual(&entry->timer,
                                       &scheduler->last_update)) {
        if (!incrementer(&entry->timer, entry->period)) {
          scheduler->entries[i].type = NULL_ENTRY_TYPE;
          break;
        }
      }
    } else {
      scheduler->entries[i].type = NULL_ENTRY_TYPE;
    }
  }
  return job_count;
}

/*
 *  Event Callbacks
 */
#define EVENT_REOCCURING  0x01

static scheduler_event_callback_entry_t *SchedulerAllocateEventCallback(
    scheduler_t *scheduler) {
  if (scheduler == NULL) return NULL;
  scheduler_callback_entry_t *entry = SchedulerAllocateCallback(scheduler);
  memset(entry, 0, sizeof(scheduler_callback_entry_t));
  entry->type = EVENT_ENTRY_TYPE;
  return &entry->event_entry;
}

bool_t SchedulerSetEventCallback(
    scheduler_t *scheduler, scheduler_event_id_t event, bool reoccuring,
    scheduler_event_callback_t callback, void *callback_ctx) {
  if (scheduler == NULL || event == EVENT_NULL || callback == NULL) return false;
  if (SchedulerIsFull(scheduler)) return false;
  scheduler_event_callback_entry_t *entry =
      SchedulerAllocateEventCallback(scheduler);
  *entry = (scheduler_event_callback_entry_t) {
    .callback = callback,
    .ctx = callback_ctx,
    .event = event,
    .flags = reoccuring ? EVENT_REOCCURING : 0
  };
  return true;
}

bool_t SchedulerTiggerEvent(
    scheduler_t *scheduler, scheduler_event_id_t event) {
  if (scheduler == NULL || event == EVENT_NULL) return false;
  if (SchedulerIsEventFull(scheduler)) false;
  for (size_t i = 0; i < scheduler->event_count; ++i) {
    if (scheduler->events[i] == event) return true;
  }
  scheduler->events[scheduler->event_count++] = event;
  return true;
}

static size_t SchedulerDoEventCallbacksInternal(scheduler_t *scheduler) {
  size_t job_count = 0;
  for (size_t i = 0; i < scheduler->entry_count; ++i) {
    if (scheduler->entries[i].type != EVENT_ENTRY_TYPE) continue;
    scheduler_event_callback_entry_t *entry =
        &scheduler->entries[i].event_entry;
    bool_t executed = false;
    for (size_t j = 0; j < scheduler->event_count && !executed; ++j) {
      scheduler_event_id_t const event = scheduler->events[j];
      if (entry->event != event) continue;
      entry->callback(entry->ctx, event, &scheduler->last_update);
      executed = true;
      ++job_count;
      if (entry->flags & EVENT_REOCCURING) continue;
      scheduler->entries[i].type = NULL_ENTRY_TYPE;
    }
  }
  SystemDisableInterrupt();
  scheduler->event_count = 0;
  SystemEnableInterrupt();
  return job_count;
}
