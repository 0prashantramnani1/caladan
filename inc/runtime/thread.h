/*
 * thread.h - support for user-level threads
 */

#pragma once

#include <base/types.h>
#include <base/compiler.h>
#include <runtime/preempt.h>
#include <iokernel/control.h>

struct thread;
typedef void (*thread_fn_t)(void *arg);
typedef struct thread thread_t;

extern thread_t* app_threads[2];
extern int app_thread_cnt;

// #define PIN 0
/*
 * Low-level routines, these are helpful for bindings and synchronization
 * primitives.
 */

extern void thread_park_and_unlock_np(spinlock_t *l);
extern void thread_park_and_preempt_enable(void);
extern void thread_ready(thread_t *thread);
extern void thread_ready_head(thread_t *thread);
extern thread_t *thread_create(thread_fn_t fn, void *arg);
extern thread_t *thread_create_type(thread_fn_t fn, void *arg, int type);
extern thread_t *thread_create_with_buf(thread_fn_t fn, void **buf, size_t len);

extern __thread thread_t *__self;
extern thread_t *__u_main;
extern volatile thread_t *__secondary_data_thread;
extern int pthreads[2];
extern __thread unsigned int kthread_idx;

static inline unsigned int get_current_affinity(void)
{
	return kthread_idx;
}

/**
 * thread_self - gets the currently running thread
 */
inline thread_t *thread_self(void)
{
	return __self;
}


extern uint64_t get_uthread_specific(void);
extern void set_uthread_specific(uint64_t val);


/*
 * High-level routines, use this API most of the time.
 */

extern void thread_yield(void);
extern void thread_yield_without_ready(void);
extern int thread_spawn(thread_fn_t fn, void *arg);
extern int thread_spawn_type(thread_fn_t fn, void *arg, int type);
extern thread_t* thread_spawn_pointer(thread_fn_t fn, void *arg);
extern void thread_exit(void) __noreturn;
