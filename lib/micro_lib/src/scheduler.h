/*
 * MIDI Controller - Scheduler
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include "base.h"
#include "system_time.h"

C_SECTION_BEGIN;

/*
 *  Timer-Based Callbacks.
 */
typedef void (*scheduler_timer_callback_t)
  (void */* ctx */, system_time_t const */* time */);

typedef struct {
  /* Callback and callbacks user defined context. */
  scheduler_timer_callback_t callback;
  void *ctx;
  /* Time of the event */
  system_time_t timer;
  /* Flags used internally for operation. */
  uint8_t flags;
  /* Period is only used for periodic callbacks. */
  uint32_t period;
} scheduler_timer_callback_entry_t;

/*
 * Event-Based Callbacks.
 */
typedef uint16_t scheduler_event_id_t;
typedef void (*scheduler_event_callback_t)
  (void */* ctx */, scheduler_event_id_t /* event */,
   system_time_t const */* time */);

typedef struct {
  /* Callback and callbacks user defined context. */
  scheduler_event_callback_t callback;
  void *ctx;
  /* Event ID */
  scheduler_event_id_t event;
  /* Flags used internally for operation. */
  uint8_t flags;
} scheduler_event_callback_entry_t;


typedef struct {
  uint8_t type;
  union {
    scheduler_timer_callback_entry_t timer_entry;
    scheduler_event_callback_entry_t event_entry;
  };
} scheduler_callback_entry_t;

/* Number of events that can be registered. */
#ifndef SCHEDULER_EVENT_QUEUE_SIZE
#define SCHEDULER_EVENT_QUEUE_SIZE 8
#endif

/* Number of callbacks that can be registered in the scheduler. */
#ifndef SCHEDULER_CALLBACK_TABLE_SIZE
#define SCHEDULER_CALLBACK_TABLE_SIZE 16
#endif

typedef struct {
  system_time_t last_update;
  /* List of events that have occured and are awaiting callbacks. */
  scheduler_event_id_t events[SCHEDULER_EVENT_QUEUE_SIZE];
  size_t event_count;
  /* List of all callbacks. */
  scheduler_callback_entry_t entries[SCHEDULER_CALLBACK_TABLE_SIZE];
  size_t entry_count;
  /* Flags used internally for operation. */
  uint8_t flags;
} scheduler_t;

bool_t SchedulerInitialize(
  scheduler_t *scheduler, system_time_t const *init_time);
bool_t SchedulerClearEvents(scheduler_t *scheduler);
bool_t SchedulerClearCallbacks(scheduler_t *scheduler);
size_t SchedulerDoCallbacks(
  scheduler_t *scheduler, system_time_t const *time);

/* For the timer callback setter, if |current_time| is NULL, the period is
 * assumed to be from the schedulers last update.*/
bool_t SchedulerSetPeriodicCallbackMicroseconds(
  scheduler_t *scheduler, uint32_t period, system_time_t const *current_time,
  scheduler_timer_callback_t callback, void *callback_ctx);
bool_t SchedulerSetPeriodicCallbackSeconds(
  scheduler_t *scheduler, uint32_t period, system_time_t const *current_time,
  scheduler_timer_callback_t callback, void *callback_ctx);

bool_t SchedulerSetDelayedCallbackMicroseconds(
  scheduler_t *scheduler, uint32_t delay, system_time_t const *current_time,
  scheduler_timer_callback_t callback, void *callback_ctx);
bool_t SchedulerSetDelayedCallbackSeconds(
  scheduler_t *scheduler, uint32_t delay, system_time_t const *current_time,
  scheduler_timer_callback_t callback, void *callback_ctx);

bool_t SchedulerSetEventCallback(
  scheduler_t *scheduler, scheduler_event_id_t event_id, bool reoccuring,
  scheduler_event_callback_t callback, void *callback_ctx);

bool_t SchedulerTiggerEvent(
  scheduler_t *scheduler, scheduler_event_id_t event);

C_SECTION_END;

#endif  /* _SCHEDULER_H_ */
