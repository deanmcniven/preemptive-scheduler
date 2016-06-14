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

static uint8_t task_one_stack[PROCESS_STACK_SIZE];
void task_one(void);

static uint8_t task_two_stack[PROCESS_STACK_SIZE];
void task_two(void);

static uint8_t task_three_stack[PROCESS_STACK_SIZE];
void task_three(void);

volatile uint32_t ticks = 0;

int main()
{
    cli();
    setup_timer();
    setup_display();

    add_process(&task_three, &task_three_stack[PROCESS_STACK_SIZE - 1]);
    add_process(&task_two, &task_two_stack[PROCESS_STACK_SIZE - 1]);
    add_process(&task_one, &task_one_stack[PROCESS_STACK_SIZE - 1]);
    scheduler_init();
    sei();

    schedule();

    while (1) { /* Should never be here */ }
    return 0;
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
    uint8_t sreg = SREG;
    //isr_enter();
    ticks++;
    SREG = sreg;
    //isr_exit();
    process_t *process = (current_process) ? (void*)current_process->next : process_ll_head;
    while (process->state != RUNNABLE) process = (void*)process->next;

    if (process != current_process) {
        process_dispatch(process);
    }
}

void task_one() {
    while (1) {
        LED = (LED ^ LED1_MASK);

        uint32_t wait_until = ticks + 25;
        while (ticks != wait_until) {
            __asm__ __volatile__ ("nop \n\t");
        };
    }
}

void task_two() {
    while (1) {
        LED = (LED ^ LED0_MASK);

        uint32_t wait_until = ticks + 100;
        while (ticks != wait_until) {
            __asm__ __volatile__ ("nop \n\t");
        };
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

        uint32_t wait_until = ticks + 50;
        while (ticks != wait_until) {
            __asm__ __volatile__ ("nop \n\t");
        };
    }
}
