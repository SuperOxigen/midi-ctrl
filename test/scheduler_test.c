/*
 * MIDI Controller - Scheduler
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#include <string.h>
#include <unity.h>

#include "scheduler.h"

static system_time_t const kInitTime = {
  .seconds = 10,
  .nanoseconds = 0
};

static system_time_t const kPreInitTime = {
  .seconds = 5,
  .nanoseconds = 0
};

static system_time_t const kPostInitTime = {
  .seconds = 15,
  .nanoseconds = 0
};

static void TestScheduler_Initialize(void) {
  scheduler_t scheduler;
  TEST_ASSERT_FALSE(SchedulerInitialize(NULL, &kInitTime));
  TEST_ASSERT_FALSE(SchedulerInitialize(&scheduler, NULL));
  TEST_ASSERT_TRUE(SchedulerInitialize(&scheduler, &kInitTime));
}

typedef struct {
  bool_t received;
  system_time_t time;
  bool_t time_set;
} timer_ctx_t;

static bool_t gTimerCtxNull = false;

static void TimerCallback(void *ctx_ptr, system_time_t const *time) {
  if (ctx_ptr == NULL) {
    gTimerCtxNull = true;
    return;
  }
  gTimerCtxNull = false;
  timer_ctx_t *ctx = ctx_ptr;
  ctx->received = true;
  if (time == NULL) {
    ctx->time_set = false;
    return;
  }
  ctx->time = (system_time_t) {
    .seconds = time->seconds,
    .nanoseconds = time->nanoseconds
  };
  ctx->time_set = true;
}

static void TestSchedulerPeriodicCallbacks_Register(void) {
  scheduler_t scheduler;
  TEST_ASSERT_TRUE(SchedulerInitialize(&scheduler, &kInitTime));

  timer_ctx_t ctx;
  TEST_ASSERT_FALSE(SchedulerSetPeriodicCallbackMicroseconds(
      NULL, 30, &kInitTime, TimerCallback, &ctx));
  TEST_ASSERT_FALSE(SchedulerSetPeriodicCallbackMicroseconds(
      &scheduler, 0, &kInitTime, TimerCallback, &ctx));
  TEST_ASSERT_FALSE(SchedulerSetPeriodicCallbackMicroseconds(
      &scheduler, 30, &kPreInitTime, TimerCallback, &ctx));
  TEST_ASSERT_FALSE(SchedulerSetPeriodicCallbackMicroseconds(
      &scheduler, 30, &kInitTime, NULL, &ctx));

  TEST_ASSERT_TRUE(SchedulerSetPeriodicCallbackMicroseconds(
      &scheduler, 30, &kInitTime, TimerCallback, &ctx));
  TEST_ASSERT_TRUE(SchedulerSetPeriodicCallbackMicroseconds(
      &scheduler, 30, NULL, TimerCallback, &ctx));
  TEST_ASSERT_TRUE(SchedulerSetPeriodicCallbackMicroseconds(
      &scheduler, 30, &kInitTime, TimerCallback, NULL));

  TEST_ASSERT_EQUAL(3, scheduler.entry_count);

  TEST_ASSERT_FALSE(SchedulerSetPeriodicCallbackSeconds(
      NULL, 30, &kInitTime, TimerCallback, &ctx));
  TEST_ASSERT_FALSE(SchedulerSetPeriodicCallbackSeconds(
      &scheduler, 0, &kInitTime, TimerCallback, &ctx));
  TEST_ASSERT_FALSE(SchedulerSetPeriodicCallbackSeconds(
      &scheduler, 30, &kPreInitTime, TimerCallback, &ctx));
  TEST_ASSERT_FALSE(SchedulerSetPeriodicCallbackSeconds(
      &scheduler, 30, &kInitTime, NULL, &ctx));

  TEST_ASSERT_TRUE(SchedulerSetPeriodicCallbackSeconds(
      &scheduler, 30, &kInitTime, TimerCallback, &ctx));
  TEST_ASSERT_TRUE(SchedulerSetPeriodicCallbackSeconds(
      &scheduler, 30, NULL, TimerCallback, &ctx));
  TEST_ASSERT_TRUE(SchedulerSetPeriodicCallbackSeconds(
      &scheduler, 30, &kInitTime, TimerCallback, NULL));

  TEST_ASSERT_EQUAL(6, scheduler.entry_count);
}  /* TestSchedulerPeriodicCallbacks_Register */

static void TestSchedulerDelayCallbacks_Register(void) {
  scheduler_t scheduler;
  TEST_ASSERT_TRUE(SchedulerInitialize(&scheduler, &kInitTime));

  timer_ctx_t ctx;
  TEST_ASSERT_FALSE(SchedulerSetDelayedCallbackMicroseconds(
      NULL, 30, &kInitTime, TimerCallback, &ctx));
  TEST_ASSERT_FALSE(SchedulerSetDelayedCallbackMicroseconds(
      &scheduler, 30, &kPreInitTime, TimerCallback, &ctx));
  TEST_ASSERT_FALSE(SchedulerSetDelayedCallbackMicroseconds(
      &scheduler, 30, &kInitTime, NULL, &ctx));

  TEST_ASSERT_TRUE(SchedulerSetDelayedCallbackMicroseconds(
      &scheduler, 30, &kInitTime, TimerCallback, &ctx));
  TEST_ASSERT_TRUE(SchedulerSetDelayedCallbackMicroseconds(
      &scheduler, 30, NULL, TimerCallback, &ctx));
  TEST_ASSERT_TRUE(SchedulerSetDelayedCallbackMicroseconds(
      &scheduler, 30, &kInitTime, TimerCallback, NULL));
  TEST_ASSERT_TRUE(SchedulerSetDelayedCallbackMicroseconds(
      &scheduler, 0, &kInitTime, TimerCallback, &ctx));

  TEST_ASSERT_EQUAL(4, scheduler.entry_count);

  TEST_ASSERT_FALSE(SchedulerSetDelayedCallbackSeconds(
      NULL, 30, &kInitTime, TimerCallback, &ctx));
  TEST_ASSERT_FALSE(SchedulerSetDelayedCallbackSeconds(
      &scheduler, 30, &kPreInitTime, TimerCallback, &ctx));
  TEST_ASSERT_FALSE(SchedulerSetDelayedCallbackSeconds(
      &scheduler, 30, &kInitTime, NULL, &ctx));

  TEST_ASSERT_TRUE(SchedulerSetDelayedCallbackSeconds(
      &scheduler, 30, &kInitTime, TimerCallback, &ctx));
  TEST_ASSERT_TRUE(SchedulerSetDelayedCallbackSeconds(
      &scheduler, 30, NULL, TimerCallback, &ctx));
  TEST_ASSERT_TRUE(SchedulerSetDelayedCallbackSeconds(
      &scheduler, 30, &kInitTime, TimerCallback, NULL));
  TEST_ASSERT_TRUE(SchedulerSetDelayedCallbackSeconds(
      &scheduler, 0, &kInitTime, TimerCallback, &ctx));

  TEST_ASSERT_EQUAL(8, scheduler.entry_count);
}

static void TestSchedulerPeriodicCallbacks_Single(void) {
  scheduler_t scheduler;
  TEST_ASSERT_TRUE(SchedulerInitialize(&scheduler, &kInitTime));

  timer_ctx_t ctx = {};
  TEST_ASSERT_TRUE(SchedulerSetPeriodicCallbackSeconds(
      &scheduler, 10, NULL, TimerCallback, &ctx));

  static system_time_t const kPreCallbackTime = {
    .seconds = kInitTime.seconds + 9
  };
  TEST_ASSERT_EQUAL(0, SchedulerDoCallbacks(&scheduler, NULL));
  TEST_ASSERT_EQUAL(0, SchedulerDoCallbacks(&scheduler, &kPreInitTime));
  TEST_ASSERT_EQUAL(0, SchedulerDoCallbacks(&scheduler, &kInitTime));
  TEST_ASSERT_EQUAL(0, SchedulerDoCallbacks(&scheduler, &kPreCallbackTime));
  TEST_ASSERT_FALSE(ctx.received);

  static system_time_t const kCallbackTime = {
    .seconds = kInitTime.seconds + 10
  };
  TEST_ASSERT_EQUAL(1, SchedulerDoCallbacks(&scheduler, &kCallbackTime));
  TEST_ASSERT_TRUE(ctx.received);
  TEST_ASSERT_TRUE(ctx.time_set);
  TEST_ASSERT_EQUAL(kCallbackTime.seconds, ctx.time.seconds);
  TEST_ASSERT_EQUAL(kCallbackTime.nanoseconds, ctx.time.nanoseconds);
  memset(&ctx, 0, sizeof(ctx));

  TEST_ASSERT_EQUAL(0, SchedulerDoCallbacks(&scheduler, &kCallbackTime));

  static system_time_t const kPreNextCallbackTime = {
    .seconds = kCallbackTime.seconds + 9
  };
  TEST_ASSERT_EQUAL(0, SchedulerDoCallbacks(
      &scheduler, &kPreNextCallbackTime));

  static system_time_t const kNextCallbackTime = {
    .seconds = kCallbackTime.seconds + 10
  };
  static system_time_t const kPostNextCallbackTime = {
    .seconds = kNextCallbackTime.seconds + 1
  };
  TEST_ASSERT_EQUAL(1, SchedulerDoCallbacks(
      &scheduler, &kPostNextCallbackTime));
  TEST_ASSERT_TRUE(ctx.received);
  TEST_ASSERT_TRUE(ctx.time_set);
  TEST_ASSERT_EQUAL(kPostNextCallbackTime.seconds, ctx.time.seconds);
  TEST_ASSERT_EQUAL(kPostNextCallbackTime.nanoseconds, ctx.time.nanoseconds);
  memset(&ctx, 0, sizeof(ctx));

  static system_time_t const kDoubleNextCallbackTime = {
    .seconds = kNextCallbackTime.seconds + 20
  };
  TEST_ASSERT_EQUAL(1, SchedulerDoCallbacks(
      &scheduler, &kDoubleNextCallbackTime));
  TEST_ASSERT_TRUE(ctx.received);
  TEST_ASSERT_TRUE(ctx.time_set);
  TEST_ASSERT_EQUAL(kDoubleNextCallbackTime.seconds, ctx.time.seconds);
  TEST_ASSERT_EQUAL(kDoubleNextCallbackTime.nanoseconds, ctx.time.nanoseconds);
  memset(&ctx, 0, sizeof(ctx));

  TEST_ASSERT_EQUAL(0, SchedulerDoCallbacks(
      &scheduler, &kDoubleNextCallbackTime));

  static system_time_t const kAnotherNextCallbackTime = {
    .seconds = kDoubleNextCallbackTime.seconds + 15
  };
  TEST_ASSERT_EQUAL(1, SchedulerDoCallbacks(
      &scheduler, &kAnotherNextCallbackTime));
  TEST_ASSERT_TRUE(ctx.received);
  TEST_ASSERT_TRUE(ctx.time_set);
  TEST_ASSERT_EQUAL(kAnotherNextCallbackTime.seconds, ctx.time.seconds);
  TEST_ASSERT_EQUAL(kAnotherNextCallbackTime.nanoseconds, ctx.time.nanoseconds);
}

static void TestSchedulerDelayCallbacks_Single_Immediate(void) {
  scheduler_t scheduler;
  TEST_ASSERT_TRUE(SchedulerInitialize(&scheduler, &kInitTime));

  timer_ctx_t ctx = {};
  TEST_ASSERT_TRUE(SchedulerSetDelayedCallbackSeconds(
      &scheduler, 0, NULL, TimerCallback, &ctx));

  TEST_ASSERT_EQUAL(1, SchedulerDoCallbacks(&scheduler, &kInitTime));
  TEST_ASSERT_TRUE(ctx.received);
  TEST_ASSERT_TRUE(ctx.time_set);
  TEST_ASSERT_EQUAL(kInitTime.seconds, ctx.time.seconds);
  TEST_ASSERT_EQUAL(kInitTime.nanoseconds, ctx.time.nanoseconds);
  TEST_ASSERT_EQUAL(0, scheduler.entry_count);

  TEST_ASSERT_EQUAL(0, SchedulerDoCallbacks(&scheduler, &kInitTime));
  TEST_ASSERT_EQUAL(0, SchedulerDoCallbacks(&scheduler, &kPostInitTime));
}

static void TestSchedulerDelayCallbacks_Single(void) {
  scheduler_t scheduler;
  TEST_ASSERT_TRUE(SchedulerInitialize(&scheduler, &kInitTime));

  timer_ctx_t ctx = {};
  TEST_ASSERT_TRUE(SchedulerSetDelayedCallbackSeconds(
      &scheduler, 10, NULL, TimerCallback, &ctx));
  TEST_ASSERT_EQUAL(0, SchedulerDoCallbacks(&scheduler, &kInitTime));
  TEST_ASSERT_EQUAL(1, scheduler.entry_count);

  static system_time_t const kCallbackTime = {
    .seconds = kInitTime.seconds + 10
  };

  TEST_ASSERT_EQUAL(1, SchedulerDoCallbacks(&scheduler, &kCallbackTime));
  TEST_ASSERT_TRUE(ctx.received);
  TEST_ASSERT_TRUE(ctx.time_set);
  TEST_ASSERT_EQUAL(kCallbackTime.seconds, ctx.time.seconds);
  TEST_ASSERT_EQUAL(kCallbackTime.nanoseconds, ctx.time.nanoseconds);
  TEST_ASSERT_EQUAL(0, scheduler.entry_count);

  TEST_ASSERT_EQUAL(0, SchedulerDoCallbacks(&scheduler, &kCallbackTime));

  static system_time_t const kPostNextCallbackTime = {
    .seconds = kCallbackTime.seconds + 15
  };
  TEST_ASSERT_EQUAL(0, SchedulerDoCallbacks(
      &scheduler, &kPostNextCallbackTime));
}

static void TestSchedulerDelayCallbacks_Multiple_OneAtATime(void) {
  scheduler_t scheduler;
  TEST_ASSERT_TRUE(SchedulerInitialize(&scheduler, &kInitTime));

  timer_ctx_t ctx_a = {};
  TEST_ASSERT_TRUE(SchedulerSetDelayedCallbackSeconds(
      &scheduler, 10, NULL, TimerCallback, &ctx_a));
  timer_ctx_t ctx_b = {};
  TEST_ASSERT_TRUE(SchedulerSetDelayedCallbackSeconds(
      &scheduler, 20, NULL, TimerCallback, &ctx_b));
  TEST_ASSERT_TRUE(SchedulerSetDelayedCallbackSeconds(
      &scheduler, 30, NULL, TimerCallback, NULL));

  TEST_ASSERT_EQUAL(0, SchedulerDoCallbacks(&scheduler, &kInitTime));
  TEST_ASSERT_EQUAL(3, scheduler.entry_count);

  static system_time_t const kCallbackOneTime = {
    .seconds = kInitTime.seconds + 10
  };
  TEST_ASSERT_EQUAL(1, SchedulerDoCallbacks(&scheduler, &kCallbackOneTime));
  TEST_ASSERT_TRUE(ctx_a.received);
  TEST_ASSERT_TRUE(ctx_a.time_set);
  TEST_ASSERT_EQUAL(kCallbackOneTime.seconds, ctx_a.time.seconds);
  TEST_ASSERT_EQUAL(kCallbackOneTime.nanoseconds, ctx_a.time.nanoseconds);
  TEST_ASSERT_EQUAL(2, scheduler.entry_count);

  TEST_ASSERT_EQUAL(0, SchedulerDoCallbacks(&scheduler, &kCallbackOneTime));

  static system_time_t const kCallbackTwoTime = {
    .seconds = kInitTime.seconds + 20
  };
  TEST_ASSERT_EQUAL(1, SchedulerDoCallbacks(&scheduler, &kCallbackTwoTime));
  TEST_ASSERT_TRUE(ctx_b.received);
  TEST_ASSERT_TRUE(ctx_b.time_set);
  TEST_ASSERT_EQUAL(kCallbackTwoTime.seconds, ctx_b.time.seconds);
  TEST_ASSERT_EQUAL(kCallbackTwoTime.nanoseconds, ctx_b.time.nanoseconds);
  TEST_ASSERT_EQUAL(1, scheduler.entry_count);

  static system_time_t const kCallbackThreeTime = {
    .seconds = kInitTime.seconds + 30
  };
  TEST_ASSERT_FALSE(gTimerCtxNull);
  TEST_ASSERT_EQUAL(1, SchedulerDoCallbacks(&scheduler, &kCallbackThreeTime));
  TEST_ASSERT_TRUE(gTimerCtxNull);
  TEST_ASSERT_EQUAL(0, scheduler.entry_count);
}

static void TestSchedulerDelayCallbacks_Multiple_AllAtOnce(void) {
  scheduler_t scheduler;
  TEST_ASSERT_TRUE(SchedulerInitialize(&scheduler, &kInitTime));

  timer_ctx_t ctx_a = {};
  TEST_ASSERT_TRUE(SchedulerSetDelayedCallbackSeconds(
      &scheduler, 10, NULL, TimerCallback, &ctx_a));
  timer_ctx_t ctx_b = {};
  TEST_ASSERT_TRUE(SchedulerSetDelayedCallbackSeconds(
      &scheduler, 20, NULL, TimerCallback, &ctx_b));
  timer_ctx_t ctx_c = {};
  TEST_ASSERT_TRUE(SchedulerSetDelayedCallbackSeconds(
      &scheduler, 30, NULL, TimerCallback, &ctx_c));

  static system_time_t const kCallbackThreeTime = {
    .seconds = kInitTime.seconds + 35
  };
  TEST_ASSERT_EQUAL(3, SchedulerDoCallbacks(&scheduler, &kCallbackThreeTime));
  TEST_ASSERT_TRUE(ctx_a.received);
  TEST_ASSERT_TRUE(ctx_a.time_set);
  TEST_ASSERT_EQUAL(kCallbackThreeTime.seconds, ctx_a.time.seconds);
  TEST_ASSERT_EQUAL(kCallbackThreeTime.nanoseconds, ctx_a.time.nanoseconds);
  TEST_ASSERT_TRUE(ctx_b.received);
  TEST_ASSERT_TRUE(ctx_b.time_set);
  TEST_ASSERT_EQUAL(kCallbackThreeTime.seconds, ctx_b.time.seconds);
  TEST_ASSERT_EQUAL(kCallbackThreeTime.nanoseconds, ctx_b.time.nanoseconds);
  TEST_ASSERT_TRUE(ctx_c.received);
  TEST_ASSERT_TRUE(ctx_c.time_set);
  TEST_ASSERT_EQUAL(kCallbackThreeTime.seconds, ctx_c.time.seconds);
  TEST_ASSERT_EQUAL(kCallbackThreeTime.nanoseconds, ctx_c.time.nanoseconds);
  TEST_ASSERT_EQUAL(0, scheduler.entry_count);
}

typedef struct {
  bool_t received;
  system_time_t time;
  bool_t time_set;
  scheduler_event_id_t event;
} event_ctx_t;

static bool_t gEventCtxNull = false;

static void EventCallback(
    void *ctx_ptr, scheduler_event_id_t event, system_time_t const *time) {
  if (ctx_ptr == NULL) {
    gEventCtxNull = true;
    return;
  }
  gEventCtxNull = false;
  event_ctx_t *ctx = ctx_ptr;
  ctx->received = true;
  ctx->event = event;
  if (time == NULL) {
    ctx->time_set = false;
    return;
  }
  ctx->time = (system_time_t) {
    .seconds = time->seconds,
    .nanoseconds = time->nanoseconds
  };
  ctx->time_set = true;
}

static scheduler_event_id_t const kEventOne = 1;
static scheduler_event_id_t const kEventTwo = 2;

static void TestSchedulerEventCallbacks_Register(void) {
  scheduler_t scheduler;
  TEST_ASSERT_TRUE(SchedulerInitialize(&scheduler, &kInitTime));

  event_ctx_t ctx = {};
  TEST_ASSERT_FALSE(SchedulerSetEventCallback(
      NULL, kEventOne, false, EventCallback, &ctx));
  TEST_ASSERT_FALSE(SchedulerSetEventCallback(
      &scheduler, 0, false, EventCallback, &ctx));
  TEST_ASSERT_FALSE(SchedulerSetEventCallback(
      &scheduler, kEventOne, false, NULL, &ctx));

  TEST_ASSERT_TRUE(SchedulerSetEventCallback(
      &scheduler, kEventOne, false, EventCallback, &ctx));
  TEST_ASSERT_TRUE(SchedulerSetEventCallback(
      &scheduler, kEventOne, true, EventCallback, &ctx));
  TEST_ASSERT_TRUE(SchedulerSetEventCallback(
      &scheduler, kEventOne, false, EventCallback, NULL));
  TEST_ASSERT_EQUAL(3, scheduler.entry_count);
}

static void TestSchedulerEventCallbacks_Single(void) {
  scheduler_t scheduler;
  TEST_ASSERT_TRUE(SchedulerInitialize(&scheduler, &kInitTime));

  event_ctx_t ctx = {};
  TEST_ASSERT_TRUE(SchedulerSetEventCallback(
      &scheduler, kEventOne, false, EventCallback, &ctx));

  TEST_ASSERT_FALSE(SchedulerTiggerEvent(NULL, kEventOne));
  TEST_ASSERT_FALSE(SchedulerTiggerEvent(&scheduler, 0));
  TEST_ASSERT_EQUAL(0, SchedulerDoCallbacks(&scheduler, &kInitTime));

  TEST_ASSERT_TRUE(SchedulerTiggerEvent(&scheduler, kEventTwo));
  TEST_ASSERT_EQUAL(1, scheduler.event_count);
  TEST_ASSERT_EQUAL(0, SchedulerDoCallbacks(&scheduler, &kInitTime));
  TEST_ASSERT_EQUAL(0, scheduler.event_count);

  TEST_ASSERT_TRUE(SchedulerTiggerEvent(&scheduler, kEventOne));
  TEST_ASSERT_TRUE(SchedulerTiggerEvent(&scheduler, kEventOne));
  TEST_ASSERT_TRUE(SchedulerTiggerEvent(&scheduler, kEventTwo));
  TEST_ASSERT_EQUAL(2, scheduler.event_count);

  TEST_ASSERT_EQUAL(1, scheduler.entry_count);
  TEST_ASSERT_EQUAL(0, SchedulerDoCallbacks(&scheduler, &kPreInitTime));
  TEST_ASSERT_EQUAL(1, SchedulerDoCallbacks(&scheduler, &kPostInitTime));
  TEST_ASSERT_EQUAL(0, scheduler.event_count);
  TEST_ASSERT_EQUAL(0, scheduler.entry_count);

  TEST_ASSERT_TRUE(ctx.received);
  TEST_ASSERT_TRUE(ctx.time_set);
  TEST_ASSERT_EQUAL(kPostInitTime.seconds, ctx.time.seconds);
  TEST_ASSERT_EQUAL(kPostInitTime.nanoseconds, ctx.time.nanoseconds);
  TEST_ASSERT_EQUAL(kEventOne, ctx.event);
}

static void TestSchedulerEventCallbacks_Single_Reoccuring(void) {
  scheduler_t scheduler;
  TEST_ASSERT_TRUE(SchedulerInitialize(&scheduler, &kInitTime));

  event_ctx_t ctx = {};
  TEST_ASSERT_TRUE(SchedulerSetEventCallback(
      &scheduler, kEventOne, true, EventCallback, &ctx));
  TEST_ASSERT_EQUAL(0, SchedulerDoCallbacks(&scheduler, &kInitTime));

  TEST_ASSERT_TRUE(SchedulerTiggerEvent(&scheduler, kEventOne));
  TEST_ASSERT_EQUAL(1, scheduler.event_count);

  static system_time_t const kCallbackTime = {
    .seconds = kInitTime.seconds + 15
  };

  TEST_ASSERT_EQUAL(1, SchedulerDoCallbacks(&scheduler, &kCallbackTime));
  TEST_ASSERT_EQUAL(0, scheduler.event_count);
  TEST_ASSERT_EQUAL(1, scheduler.entry_count);
  TEST_ASSERT_TRUE(ctx.received);
  TEST_ASSERT_TRUE(ctx.time_set);
  TEST_ASSERT_EQUAL(kCallbackTime.seconds, ctx.time.seconds);
  TEST_ASSERT_EQUAL(kCallbackTime.nanoseconds, ctx.time.nanoseconds);
  TEST_ASSERT_EQUAL(kEventOne, ctx.event);
  memset(&ctx, 0, sizeof(ctx));

  TEST_ASSERT_TRUE(SchedulerTiggerEvent(&scheduler, kEventOne));
  TEST_ASSERT_EQUAL(0, SchedulerDoCallbacks(&scheduler, &kInitTime));

  static system_time_t const kNextCallbackTime = {
    .seconds = kInitTime.seconds + 30
  };

  TEST_ASSERT_EQUAL(1, SchedulerDoCallbacks(&scheduler, &kNextCallbackTime));
  TEST_ASSERT_TRUE(ctx.received);
  TEST_ASSERT_TRUE(ctx.time_set);
  TEST_ASSERT_EQUAL(kNextCallbackTime.seconds, ctx.time.seconds);
  TEST_ASSERT_EQUAL(kNextCallbackTime.nanoseconds, ctx.time.nanoseconds);
  TEST_ASSERT_EQUAL(kEventOne, ctx.event);
  memset(&ctx, 0, sizeof(ctx));

  static system_time_t const kNextNextCallbackTime = {
    .seconds = kInitTime.seconds + 45
  };
  TEST_ASSERT_EQUAL(0, SchedulerDoCallbacks(&scheduler, &kNextNextCallbackTime));
}

static void TestSchedulerEventCallbacks_Multiple_OneAtATime(void) {
  scheduler_t scheduler;
  TEST_ASSERT_TRUE(SchedulerInitialize(&scheduler, &kInitTime));

  event_ctx_t ctx = {};
  TEST_ASSERT_TRUE(SchedulerSetEventCallback(
      &scheduler, kEventOne, false, EventCallback, &ctx));
  TEST_ASSERT_TRUE(SchedulerSetEventCallback(
      &scheduler, kEventTwo, false, EventCallback, &ctx));
  TEST_ASSERT_EQUAL(2, scheduler.entry_count);

  static system_time_t const kCallbackTime = {
    .seconds = kInitTime.seconds + 15
  };
  TEST_ASSERT_EQUAL(0, SchedulerDoCallbacks(&scheduler, &kCallbackTime));

  TEST_ASSERT_TRUE(SchedulerTiggerEvent(&scheduler, kEventOne));
  TEST_ASSERT_EQUAL(1, scheduler.event_count);

  TEST_ASSERT_EQUAL(1, SchedulerDoCallbacks(&scheduler, &kCallbackTime));
  TEST_ASSERT_EQUAL(0, scheduler.event_count);
  TEST_ASSERT_EQUAL(1, scheduler.entry_count);
  TEST_ASSERT_TRUE(ctx.received);
  TEST_ASSERT_TRUE(ctx.time_set);
  TEST_ASSERT_EQUAL(kCallbackTime.seconds, ctx.time.seconds);
  TEST_ASSERT_EQUAL(kCallbackTime.nanoseconds, ctx.time.nanoseconds);
  TEST_ASSERT_EQUAL(kEventOne, ctx.event);
  memset(&ctx, 0, sizeof(ctx));

  TEST_ASSERT_TRUE(SchedulerTiggerEvent(&scheduler, kEventTwo));

  static system_time_t const kNextCallbackTime = {
    .seconds = kInitTime.seconds + 30
  };
  TEST_ASSERT_EQUAL(1, SchedulerDoCallbacks(&scheduler, &kNextCallbackTime));
  TEST_ASSERT_EQUAL(0, scheduler.event_count);
  TEST_ASSERT_EQUAL(0, scheduler.entry_count);
  TEST_ASSERT_TRUE(ctx.received);
  TEST_ASSERT_TRUE(ctx.time_set);
  TEST_ASSERT_EQUAL(kNextCallbackTime.seconds, ctx.time.seconds);
  TEST_ASSERT_EQUAL(kNextCallbackTime.nanoseconds, ctx.time.nanoseconds);
  TEST_ASSERT_EQUAL(kEventTwo, ctx.event);

  TEST_ASSERT_TRUE(SchedulerTiggerEvent(&scheduler, kEventOne));
  TEST_ASSERT_TRUE(SchedulerTiggerEvent(&scheduler, kEventTwo));
  TEST_ASSERT_EQUAL(2, scheduler.event_count);
  TEST_ASSERT_EQUAL(0, SchedulerDoCallbacks(&scheduler, &kNextCallbackTime));
  TEST_ASSERT_EQUAL(0, scheduler.event_count);
}

static void TestSchedulerEventCallbacks_Multiple_AllAtOnce(void) {
  scheduler_t scheduler;
  TEST_ASSERT_TRUE(SchedulerInitialize(&scheduler, &kInitTime));

  event_ctx_t ctx_a = {};
  TEST_ASSERT_TRUE(SchedulerSetEventCallback(
      &scheduler, kEventOne, false, EventCallback, &ctx_a));
  event_ctx_t ctx_b = {};
  TEST_ASSERT_TRUE(SchedulerSetEventCallback(
      &scheduler, kEventTwo, false, EventCallback, &ctx_b));
  TEST_ASSERT_EQUAL(2, scheduler.entry_count);

  TEST_ASSERT_TRUE(SchedulerTiggerEvent(&scheduler, kEventOne));
  TEST_ASSERT_TRUE(SchedulerTiggerEvent(&scheduler, kEventTwo));
  TEST_ASSERT_EQUAL(2, scheduler.event_count);

  static system_time_t const kCallbackTime = {
    .seconds = kInitTime.seconds + 15
  };
  TEST_ASSERT_EQUAL(2, SchedulerDoCallbacks(&scheduler, &kCallbackTime));
  TEST_ASSERT_EQUAL(0, scheduler.event_count);
  TEST_ASSERT_EQUAL(0, scheduler.entry_count);

  TEST_ASSERT_TRUE(ctx_a.received);
  TEST_ASSERT_TRUE(ctx_a.time_set);
  TEST_ASSERT_EQUAL(kCallbackTime.seconds, ctx_a.time.seconds);
  TEST_ASSERT_EQUAL(kCallbackTime.nanoseconds, ctx_a.time.nanoseconds);
  TEST_ASSERT_EQUAL(kEventOne, ctx_a.event);

  TEST_ASSERT_TRUE(ctx_b.received);
  TEST_ASSERT_TRUE(ctx_b.time_set);
  TEST_ASSERT_EQUAL(kCallbackTime.seconds, ctx_b.time.seconds);
  TEST_ASSERT_EQUAL(kCallbackTime.nanoseconds, ctx_b.time.nanoseconds);
  TEST_ASSERT_EQUAL(kEventTwo, ctx_b.event);
}

static void TestSchedulerEventCallbacks_Multiple_OneReoccuring(void) {
  scheduler_t scheduler;
  TEST_ASSERT_TRUE(SchedulerInitialize(&scheduler, &kInitTime));

  event_ctx_t ctx_a = {};
  TEST_ASSERT_TRUE(SchedulerSetEventCallback(
      &scheduler, kEventOne, false, EventCallback, &ctx_a));
  event_ctx_t ctx_b = {};
  TEST_ASSERT_TRUE(SchedulerSetEventCallback(
      &scheduler, kEventTwo, true, EventCallback, &ctx_b));
  TEST_ASSERT_EQUAL(2, scheduler.entry_count);

  TEST_ASSERT_TRUE(SchedulerTiggerEvent(&scheduler, kEventOne));
  TEST_ASSERT_TRUE(SchedulerTiggerEvent(&scheduler, kEventTwo));
  TEST_ASSERT_EQUAL(2, scheduler.event_count);

  static system_time_t const kCallbackTime = {
    .seconds = kInitTime.seconds + 15
  };
  TEST_ASSERT_EQUAL(2, SchedulerDoCallbacks(&scheduler, &kCallbackTime));
  TEST_ASSERT_EQUAL(0, scheduler.event_count);
  TEST_ASSERT_EQUAL(1, scheduler.entry_count);

  TEST_ASSERT_TRUE(ctx_a.received);
  TEST_ASSERT_TRUE(ctx_a.time_set);
  TEST_ASSERT_EQUAL(kCallbackTime.seconds, ctx_a.time.seconds);
  TEST_ASSERT_EQUAL(kCallbackTime.nanoseconds, ctx_a.time.nanoseconds);
  TEST_ASSERT_EQUAL(kEventOne, ctx_a.event);
  memset(&ctx_a, 0, sizeof(ctx_a));

  TEST_ASSERT_TRUE(ctx_b.received);
  TEST_ASSERT_TRUE(ctx_b.time_set);
  TEST_ASSERT_EQUAL(kCallbackTime.seconds, ctx_b.time.seconds);
  TEST_ASSERT_EQUAL(kCallbackTime.nanoseconds, ctx_b.time.nanoseconds);
  TEST_ASSERT_EQUAL(kEventTwo, ctx_b.event);
  memset(&ctx_b, 0, sizeof(ctx_b));

  static system_time_t const kNextCallbackTime = {
    .seconds = kInitTime.seconds + 30
  };
  TEST_ASSERT_EQUAL(0, SchedulerDoCallbacks(&scheduler, &kNextCallbackTime));
  TEST_ASSERT_TRUE(SchedulerTiggerEvent(&scheduler, kEventOne));
  TEST_ASSERT_EQUAL(0, SchedulerDoCallbacks(&scheduler, &kNextCallbackTime));
  TEST_ASSERT_EQUAL(0, ctx_b.event);
  TEST_ASSERT_TRUE(SchedulerTiggerEvent(&scheduler, kEventTwo));
  TEST_ASSERT_EQUAL(1, SchedulerDoCallbacks(&scheduler, &kNextCallbackTime));
  TEST_ASSERT_EQUAL(0, scheduler.event_count);
  TEST_ASSERT_EQUAL(1, scheduler.entry_count);

  TEST_ASSERT_TRUE(ctx_b.received);
  TEST_ASSERT_TRUE(ctx_b.time_set);
  TEST_ASSERT_EQUAL(kNextCallbackTime.seconds, ctx_b.time.seconds);
  TEST_ASSERT_EQUAL(kNextCallbackTime.nanoseconds, ctx_b.time.nanoseconds);
  TEST_ASSERT_EQUAL(kEventTwo, ctx_b.event);
}

static void TestSchedulerEventCallbacks_ClearEvents(void) {
  scheduler_t scheduler;
  TEST_ASSERT_TRUE(SchedulerInitialize(&scheduler, &kInitTime));

  event_ctx_t ctx_a = {};
  TEST_ASSERT_TRUE(SchedulerSetEventCallback(
      &scheduler, kEventOne, true, EventCallback, &ctx_a));
  event_ctx_t ctx_b = {};
  TEST_ASSERT_TRUE(SchedulerSetEventCallback(
      &scheduler, kEventTwo, true, EventCallback, &ctx_b));
  TEST_ASSERT_EQUAL(2, scheduler.entry_count);

  TEST_ASSERT_TRUE(SchedulerTiggerEvent(&scheduler, kEventOne));
  TEST_ASSERT_TRUE(SchedulerTiggerEvent(&scheduler, kEventTwo));
  TEST_ASSERT_EQUAL(2, scheduler.event_count);

  TEST_ASSERT_FALSE(SchedulerClearEvents(NULL));
  TEST_ASSERT_TRUE(SchedulerClearEvents(&scheduler));
  TEST_ASSERT_EQUAL(0, scheduler.event_count);

  static system_time_t const kCallbackTime = {
    .seconds = kInitTime.seconds + 15
  };
  TEST_ASSERT_EQUAL(0, SchedulerDoCallbacks(&scheduler, &kCallbackTime));
}

static void TestSchedulerCallbacks_Multiple(void) {
  scheduler_t scheduler;
  TEST_ASSERT_TRUE(SchedulerInitialize(&scheduler, &kInitTime));

  timer_ctx_t tctx = {};
  TEST_ASSERT_TRUE(SchedulerSetDelayedCallbackMicroseconds(
      &scheduler, 0, NULL, TimerCallback, &tctx));
  event_ctx_t ectx = {};
  TEST_ASSERT_TRUE(SchedulerSetEventCallback(
      &scheduler, kEventOne, false, EventCallback, &ectx));
  TEST_ASSERT_EQUAL(2, scheduler.entry_count);
  TEST_ASSERT_TRUE(SchedulerTiggerEvent(&scheduler, kEventOne));
  TEST_ASSERT_TRUE(SchedulerTiggerEvent(&scheduler, kEventTwo));

  TEST_ASSERT_EQUAL(0, SchedulerDoCallbacks(&scheduler, &kPreInitTime));

  static system_time_t const kCallbackTime = {
    .seconds = kInitTime.seconds + 15
  };
  TEST_ASSERT_EQUAL(2, SchedulerDoCallbacks(&scheduler, &kCallbackTime));
  TEST_ASSERT_EQUAL(0, scheduler.entry_count);

  TEST_ASSERT_TRUE(tctx.received);
  TEST_ASSERT_TRUE(tctx.time_set);
  TEST_ASSERT_EQUAL(kCallbackTime.seconds, tctx.time.seconds);
  TEST_ASSERT_EQUAL(kCallbackTime.nanoseconds, tctx.time.nanoseconds);

  TEST_ASSERT_TRUE(ectx.received);
  TEST_ASSERT_TRUE(ectx.time_set);
  TEST_ASSERT_EQUAL(kCallbackTime.seconds, ectx.time.seconds);
  TEST_ASSERT_EQUAL(kCallbackTime.nanoseconds, ectx.time.nanoseconds);
  TEST_ASSERT_EQUAL(kEventOne, ectx.event);
}

static void TestSchedulerCallbacks_Multiple_Reoccuring(void) {
  scheduler_t scheduler;
  TEST_ASSERT_TRUE(SchedulerInitialize(&scheduler, &kInitTime));

  timer_ctx_t tctx = {};
  TEST_ASSERT_TRUE(SchedulerSetPeriodicCallbackSeconds(
      &scheduler, 10, NULL, TimerCallback, &tctx));
  event_ctx_t ectx = {};
  TEST_ASSERT_TRUE(SchedulerSetEventCallback(
      &scheduler, kEventOne, true, EventCallback, &ectx));
  TEST_ASSERT_EQUAL(2, scheduler.entry_count);
  TEST_ASSERT_TRUE(SchedulerTiggerEvent(&scheduler, kEventOne));
  TEST_ASSERT_TRUE(SchedulerTiggerEvent(&scheduler, kEventTwo));

  TEST_ASSERT_EQUAL(0, SchedulerDoCallbacks(&scheduler, &kPreInitTime));

  static system_time_t const kCallbackTime = {
    .seconds = kInitTime.seconds + 10
  };
  TEST_ASSERT_EQUAL(2, SchedulerDoCallbacks(&scheduler, &kCallbackTime));
  TEST_ASSERT_EQUAL(2, scheduler.entry_count);

  TEST_ASSERT_TRUE(tctx.received);
  TEST_ASSERT_TRUE(tctx.time_set);
  TEST_ASSERT_EQUAL(kCallbackTime.seconds, tctx.time.seconds);
  TEST_ASSERT_EQUAL(kCallbackTime.nanoseconds, tctx.time.nanoseconds);
  memset(&tctx, 0, sizeof(tctx));

  TEST_ASSERT_TRUE(ectx.received);
  TEST_ASSERT_TRUE(ectx.time_set);
  TEST_ASSERT_EQUAL(kCallbackTime.seconds, ectx.time.seconds);
  TEST_ASSERT_EQUAL(kCallbackTime.nanoseconds, ectx.time.nanoseconds);
  TEST_ASSERT_EQUAL(kEventOne, ectx.event);
  memset(&ectx, 0, sizeof(ectx));

  TEST_ASSERT_EQUAL(0, SchedulerDoCallbacks(&scheduler, &kCallbackTime));
  static system_time_t const kNextCallbackTime = {
    .seconds = kCallbackTime.seconds + 10
  };
  TEST_ASSERT_EQUAL(2, scheduler.entry_count);
  TEST_ASSERT_EQUAL(1, SchedulerDoCallbacks(&scheduler, &kNextCallbackTime));
  TEST_ASSERT_TRUE(tctx.received);
  TEST_ASSERT_TRUE(tctx.time_set);
  TEST_ASSERT_EQUAL(kNextCallbackTime.seconds, tctx.time.seconds);
  TEST_ASSERT_EQUAL(kNextCallbackTime.nanoseconds, tctx.time.nanoseconds);
  memset(&tctx, 0, sizeof(tctx));

  TEST_ASSERT_EQUAL(0, ectx.event);

  TEST_ASSERT_TRUE(SchedulerTiggerEvent(&scheduler, kEventOne));
  TEST_ASSERT_EQUAL(1, SchedulerDoCallbacks(&scheduler, &kNextCallbackTime));
  TEST_ASSERT_TRUE(ectx.received);
  TEST_ASSERT_TRUE(ectx.time_set);
  TEST_ASSERT_EQUAL(kNextCallbackTime.seconds, ectx.time.seconds);
  TEST_ASSERT_EQUAL(kNextCallbackTime.nanoseconds, ectx.time.nanoseconds);
  TEST_ASSERT_EQUAL(kEventOne, ectx.event);
  memset(&ectx, 0, sizeof(ectx));

  TEST_ASSERT_EQUAL(0, SchedulerDoCallbacks(&scheduler, &kNextCallbackTime));
}

static void TestSchedulerCallbacks_Multiple_ClearCallbacks(void) {
  scheduler_t scheduler;
  TEST_ASSERT_TRUE(SchedulerInitialize(&scheduler, &kInitTime));

  timer_ctx_t tctx = {};
  TEST_ASSERT_TRUE(SchedulerSetPeriodicCallbackSeconds(
      &scheduler, 10, NULL, TimerCallback, &tctx));
  event_ctx_t ectx = {};
  TEST_ASSERT_TRUE(SchedulerSetEventCallback(
      &scheduler, kEventOne, true, EventCallback, &ectx));
  TEST_ASSERT_EQUAL(2, scheduler.entry_count);
  TEST_ASSERT_TRUE(SchedulerTiggerEvent(&scheduler, kEventOne));
  TEST_ASSERT_TRUE(SchedulerTiggerEvent(&scheduler, kEventTwo));

  TEST_ASSERT_FALSE(SchedulerClearCallbacks(NULL));
  TEST_ASSERT_TRUE(SchedulerClearCallbacks(&scheduler));
  TEST_ASSERT_EQUAL(2, scheduler.event_count);
  TEST_ASSERT_EQUAL(0, scheduler.entry_count);

  static system_time_t const kCallbackTime = {
    .seconds = kInitTime.seconds + 10
  };
  TEST_ASSERT_EQUAL(0, SchedulerDoCallbacks(&scheduler, &kCallbackTime));
  TEST_ASSERT_EQUAL(0, scheduler.event_count);
  TEST_ASSERT_FALSE(tctx.received);
  TEST_ASSERT_FALSE(ectx.received);
}

void SchedulerTest(void) {
  RUN_TEST(TestScheduler_Initialize);

  RUN_TEST(TestSchedulerPeriodicCallbacks_Register);
  RUN_TEST(TestSchedulerDelayCallbacks_Register);
  RUN_TEST(TestSchedulerPeriodicCallbacks_Single);
  RUN_TEST(TestSchedulerDelayCallbacks_Single_Immediate);
  RUN_TEST(TestSchedulerDelayCallbacks_Single);
  RUN_TEST(TestSchedulerDelayCallbacks_Multiple_OneAtATime);
  RUN_TEST(TestSchedulerDelayCallbacks_Multiple_AllAtOnce);

  RUN_TEST(TestSchedulerEventCallbacks_Register);
  RUN_TEST(TestSchedulerEventCallbacks_Single);
  RUN_TEST(TestSchedulerEventCallbacks_Single_Reoccuring);
  RUN_TEST(TestSchedulerEventCallbacks_Multiple_OneAtATime);
  RUN_TEST(TestSchedulerEventCallbacks_Multiple_AllAtOnce);
  RUN_TEST(TestSchedulerEventCallbacks_Multiple_OneReoccuring);
  RUN_TEST(TestSchedulerEventCallbacks_ClearEvents);

  RUN_TEST(TestSchedulerCallbacks_Multiple);
  RUN_TEST(TestSchedulerCallbacks_Multiple_Reoccuring);
  RUN_TEST(TestSchedulerCallbacks_Multiple_ClearCallbacks);
}
