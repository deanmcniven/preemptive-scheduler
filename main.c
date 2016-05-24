/*
 * Preemptive Scheduler
 *
 * Created: 17/5/2016 07:17:40 AM
 * Author : Dean McNiven
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "stdio.h"

#include "preempt_sched.h"
#include "display.h"

void setupTimer(void);
void yield(uint8_t pid, uint32_t numTicks);
void taskOne(uint8_t);
void taskTwo(uint8_t);
void taskThree(uint8_t);

#define NUM_TASKS 3
volatile task_t Tasks[NUM_TASKS] = {
    {
        taskOne,
        RUNNABLE,
        0, 0
    }, {
        taskTwo,
        RUNNABLE,
        0, 0
    }, {
        taskThree,
        RUNNABLE,
        0, 0
    }
};

volatile uint32_t ticks = 0;
uint8_t count_h = 0;
uint8_t count_l = 0;

int main()
{
    cli();
    setupTimer();
    setupDisplay();
    sei();

    int currentTask = 0;

    while (1)
    {
        switch (Tasks[currentTask].state) {
        case RUNNABLE:
            (*Tasks[currentTask].entry)(currentTask);
            break;
        case WAIT:
            if ((Tasks[currentTask].tick_roll == 0) && (ticks >= Tasks[currentTask].wait_ticks)) {
                Tasks[currentTask].state = RUNNABLE;
            } else if ((Tasks[currentTask].tick_roll > 0) && ticks < Tasks[currentTask].wait_ticks) {
                Tasks[currentTask].tick_roll = 0;
            }
            break;
        case HALT:
            break;
        }

        currentTask++;
        if (currentTask >= NUM_TASKS) currentTask = 0;
    }
}

void setupTimer()
{
    //Setup Ticks generation
    ASSR = 0x00;    //Timer2: Internal Clock
    TCCR2A = 0x02;  //CTC Mode
    TCCR2B = 0x07;  //Prescale: 1024
    OCR2A = 0x9C;   //Count: 156 (~10ms)
    TIMSK2 = 0x02;  //Enable Compare Interrupt
}

ISR(TIMER2_COMPA_vect) {
    uint8_t sReg = SREG;
    ticks++;
    SREG = sReg;
}

void yield(uint8_t pid, uint32_t numTicks) {
    cli();
    Tasks[pid].state = WAIT;
    Tasks[pid].wait_ticks = ticks + numTicks;
    Tasks[pid].tick_roll = (Tasks[pid].wait_ticks < ticks) ? 1 : 0;
    sei();
}

void taskOne(uint8_t pid) {
    LED = (LED ^ LED1_MASK);
    yield(pid, 25);
}

void taskTwo(uint8_t pid) {
    LED = (LED ^ LED0_MASK);
#ifdef GREEDY_TASK
    uint32_t wait_until = ticks + 98 + pid;
    while (ticks != wait_until) { asm("nop"); };
#else
    yield(pid, 100);
#endif
}

void taskThree(uint8_t pid) {
    BCD = (count_h << BCD_OFFSET) | (1 << BCD_L_MSD);
    BCD = (count_l << BCD_OFFSET) | (1 << BCD_L_LSD);
    BCD = BCD_BLANK;

    count_l++;
    if (count_l >= 10) {
        count_l = 0;
        count_h++;
    }
    if (count_h >= 10) count_h = 0;

    yield(pid, 50);
}
