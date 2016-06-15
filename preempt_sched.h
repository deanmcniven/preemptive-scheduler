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
    volatile uint16_t wait_ticks;
} process_t;

extern process_t *current_process;

void scheduler_init(void);
void add_process(process_fn_t process, void *stack_ptr);
void init_stack(process_fn_t process, void *stack_ptr, process_t *pcb);

void sleep(uint16_t wait_ticks);
void restore_processes(void);
void schedule(void);

#endif
