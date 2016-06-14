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
} process_t;

static process_t *process_ll_head;
extern process_t *current_process;

void isr_enter(void);
void isr_exit(void);

void scheduler_init(void);
void add_process(process_fn_t process, void *stack_ptr);
void yield(void);
void schedule(void);
void process_dispatch(process_t *process);

#endif
