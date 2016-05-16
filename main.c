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

void setupHardware(void);
void yield(uint8_t pid, uint32_t numTicks);
void taskOne(uint8_t);

#define NUM_TASKS 1
volatile task_t Tasks[NUM_TASKS] = {
    {
        taskOne,
        RUNNABLE,
        0,
        0
    }
};

volatile uint32_t ticks = 0;

int main()
{
    cli();
    setupHardware();
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

void setupHardware()
{
    //Setup Ticks generation
    ASSR = 0x00;    //Timer2: Internal Clock
    TCCR2A = 0x02;  //CTC Mode
    TCCR2B = 0x07;  //Prescale: 1024
    OCR2A = 0xEB;   //Count: 235 (~15ms)
    TIMSK2 = 0x02;  //Enable Compare Interrupt

    //Led on PB5 output
    DDRB = 1 << DDB5;
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
    if (Tasks[pid].wait_ticks < ticks) Tasks[pid].tick_roll = 1;
    else Tasks[pid].tick_roll = 0;
    sei();
}

void taskOne(uint8_t pid) {
    PORTB = (PORTB && 0x10) ? (0 << PORTB5) : (1 << PORTB5);
    yield(pid, 66);
}
