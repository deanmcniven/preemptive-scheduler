#ifndef _PREEMPT_SCHED_H_
#define _PREEMPT_SCHED_H_ 1

typedef void (*task_fn_t)(void);

typedef enum {
    RUNNABLE = 1,
    HALT = 2,
    WAIT = 3
} task_state_t;

typedef struct {
    task_fn_t     entry;
    task_state_t  state;
    uint32_t      wait_ticks;
    uint8_t       tick_roll;
} task_t;

#endif
