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
void taskOne(void);
void taskTwo(void);
void taskThree(void);

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

volatile uint8_t currentTask = 0;
volatile uint32_t ticks = 0;

int main()
{
    cli();
    setupTimer();
    setupDisplay();
    sei();

    while (1) {
        //Should never be here
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

    (*Tasks[currentTask].entry)();
    currentTask++;
    if (currentTask >= NUM_TASKS) currentTask = 0;

    SREG = sReg;
}

void taskOne() {
    while (1) {
        LED = (LED ^ LED1_MASK);
    }
}

void taskTwo() {
    while (1) {
        LED = (LED ^ LED0_MASK);
    }
}

void taskThree() {
    uint8_t count_h = 0;
    uint8_t count_l = 0;

    while (1) {
        BCD = (count_h << BCD_OFFSET) | (1 << BCD_L_MSD);
        BCD = (count_l << BCD_OFFSET) | (1 << BCD_L_LSD);
        BCD = BCD_BLANK;

        count_l++;
        if (count_l >= 10) {
            count_l = 0;
            count_h++;
        }
        if (count_h >= 10) count_h = 0;
    }
}
