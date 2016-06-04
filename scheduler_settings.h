#ifndef _SCHEDULER_SETTINGS_H_
#define _SCHEDULER_SETTINGS_H_ 1

// 1k SRAM
// from 0x0100
// to   0x04FF
// 35 bytes needed to context switch
// 17 bytes needed for avr-gcc to handle ISR
//
// ~ 60 bytes per stack should leave room for approx 4 fn calls and a ISR per task
#define PROCESS_STACK_SIZE 60

#define MAX_NUM_PROCESS 3

#define SEMAPHORE
#define MAX_SEMAPHORES 3

#endif
