/*
* Pre-Emption code based heavily on the tutorial available at:
* http://kevincuzner.com/2015/12/31/writing-a-preemptive-task-scheduler-for-avr/
*/
#ifndef _PREEMPT_SCHED_H_
#define _PREEMPT_SCHED_H_ 1

#include <avr/io.h>
#include <util/atomic.h>

#include "scheduler_settings.h"

typedef void (*process_fn_t)(void);

typedef enum {
    RUNNABLE = 1,
    WAIT
} process_state_t;

typedef struct {
    void *sp;
    process_state_t state;
    struct process_t *next;
    void *status_pointer;
} process_t;

typedef struct {
    int8_t value;
} semaphore_t;

extern process_t *current_process;

void isr_enter(void);
void isr_exit(void);

semaphore_t *semaphore_init(int8_t value);
void semaphore_post(semaphore_t *semaphore);
void semaphore_pend(semaphore_t *semaphore);

void scheduler_init(void);
void add_process(process_fn_t process, void *stack_ptr);
void schedule(void);
void process_dispatch(process_t *process);

#endif
