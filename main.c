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

void setup_timer(void);
void task_one(void);
void task_two(void);
void task_three(void);

#define NUM_TASKS 3
volatile task_t tasks[NUM_TASKS] = {
    {
        task_one,
        RUNNABLE,
        0, 0
    }, {
        task_two,
        RUNNABLE,
        0, 0
    }, {
        task_three,
        RUNNABLE,
        0, 0
    }
};

volatile uint8_t current_task = 0;
volatile uint32_t ticks = 0;

int main()
{
    cli();
    setup_timer();
    setup_display();
    sei();

    while (1) {
        //Should never be here
    }
}

void setup_timer()
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

    (*tasks[current_task].entry)();
    current_task++;
    if (current_task >= NUM_TASKS) current_task = 0;

    SREG = sReg;
}

void task_one() {
    while (1) {
        LED = (LED ^ LED1_MASK);
    }
}

void task_two() {
    while (1) {
        LED = (LED ^ LED0_MASK);
    }
}

void task_three() {
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
