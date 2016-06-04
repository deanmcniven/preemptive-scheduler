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

static semaphore_t *sem_task_one;
static uint8_t task_one_stack[PROCESS_STACK_SIZE];
void task_one(void);

static semaphore_t *sem_task_two;
static uint8_t task_two_stack[PROCESS_STACK_SIZE];
void task_two(void);

static semaphore_t *sem_task_three;
static uint8_t task_three_stack[PROCESS_STACK_SIZE];
void task_three(void);


int main()
{
    cli();
    setup_timer();
    setup_display();
    scheduler_init();

    sem_task_one = semaphore_init(0);
    sem_task_two = semaphore_init(1);
    sem_task_three = semaphore_init(2);

    add_process(&task_three, &task_three_stack[PROCESS_STACK_SIZE - 1]);
    add_process(&task_two, &task_two_stack[PROCESS_STACK_SIZE - 1]);
    add_process(&task_one, &task_one_stack[PROCESS_STACK_SIZE - 1]);
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
    isr_enter();
    semaphore_post(sem_task_one);
    semaphore_post(sem_task_two);
    semaphore_post(sem_task_three);
    isr_exit();
}

void task_one() {
    while (1) {
        semaphore_pend(sem_task_one);
        LED = (LED ^ LED1_MASK);
    }
}

void task_two() {
    while (1) {
        semaphore_pend(sem_task_two);
        LED = (LED ^ LED0_MASK);
    }
}

void task_three() {
    uint8_t count_h = 0;
    uint8_t count_l = 0;

    while (1) {
        semaphore_pend(sem_task_three);

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
