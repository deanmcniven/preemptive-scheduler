#include "preempt_sched.h"

//Array of process control blocks (Add one for idle process)
static process_t pcbs[MAX_NUM_PROCESS + 1];
static uint8_t next_process = 0;
static process_t *process_ll_head;
process_t *current_process;

static uint8_t nop_process_stack[PROCESS_STACK_SIZE];
static void nop_process(void) { while (1) {} }


static uint8_t isr_level = 0;
void isr_enter(void) { isr_level++; }
void isr_exit(void) {
    isr_level--;
    schedule();
}

#ifdef SEMAPHORE
static semaphore_t semaphores[MAX_SEMAPHORES+1];
static uint8_t next_semaphore = 0;

semaphore_t *semaphore_init(int8_t value) {
    semaphore_t *s = &semaphores[next_semaphore++];
    s->value = value;
    return s;
}

void semaphore_post(semaphore_t *semaphore) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        process_t *process;
        semaphore->value++;

        process = process_ll_head;
        while (process)
        {
            if (process->state == WAIT && process->status_pointer == semaphore)
                break; //this is the task to be restored
            process = (void*)process->next;
        }

        process->state = RUNNABLE;
        schedule();
    }
}

void semaphore_pend(semaphore_t *semaphore) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        int8_t val = semaphore->value--; //val is value before decrement

        if (val <= 0) {
            //we need to wait on the semaphore
            current_process->status_pointer = semaphore;
            current_process->state = WAIT;

            schedule();
        }
    }
}
#endif

void scheduler_init() {
    add_process(&nop_process, &nop_process_stack[PROCESS_STACK_SIZE - 1]);
}

void add_process(process_fn_t process, void *stack_ptr) {
    uint8_t *stack = stack_ptr;
    process_t *pcb;

    //Store PC
    //Note: count backwards as adding to stack goes up in memory
    stack[0] = (uint16_t)process & 0xFF;
    stack[-1] = (uint16_t)process >> 8;

    //Init 32 Registers
    int8_t reg_ctr;
    for(reg_ctr = -2; reg_ctr > -34; reg_ctr--) {
        stack[reg_ctr] = 0;
    }

    //Init SREG
    stack[-34] = 0x80;

    //Setup PCB
    pcb = &pcbs[next_process++];
    pcb->sp = stack - 35;
    pcb->state = RUNNABLE;
    if (process_ll_head) {
        pcb->next = (void*)process_ll_head;
    }

    //Add to head of task list
    process_ll_head = pcb;
}

void schedule() {
    if (isr_level) return;

    process_t *process = process_ll_head;
    while (process->state != RUNNABLE) process = (void*)process->next;

    if (process != current_process) {
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
            process_dispatch(process);
        }
    }
}

void process_dispatch(process_t *process) {
    //Note: Calling this fn already placed the PC to the stack
    __asm__ __volatile__ (
            "push   r31 \n\t"
            "push   r30 \n\t"
            "push   r29 \n\t"
            "push   r28 \n\t"
            "push   r27 \n\t"
            "push   r26 \n\t"
            "push   r25 \n\t"
            "push   r24 \n\t"
            "push   r23 \n\t"
            "push   r22 \n\t"
            "push   r21 \n\t"
            "push   r20 \n\t"
            "push   r19 \n\t"
            "push   r18 \n\t"
            "push   r17 \n\t"
            "push   r16 \n\t"
            "push   r15 \n\t"
            "push   r14 \n\t"
            "push   r13 \n\t"
            "push   r12 \n\t"
            "push   r11 \n\t"
            "push   r10 \n\t"
            "push   r9 \n\t"
            "push   r8 \n\t"
            "push   r7 \n\t"
            "push   r6 \n\t"
            "push   r5 \n\t"
            "push   r4 \n\t"
            "push   r3 \n\t"
            "push   r2 \n\t"
            "push   r1 \n\t"
            "push   r0 \n\t"
            "in     r0, %[_SREG_] \n\t" //Save SREG
            "push   r0 \n\t"
            "lds    r26, current_process \n\t"
            "lds    r27, current_process+1 \n\t"
            "sbiw   r26, 0 \n\t"
            "breq   1f \n\t" //null check, skip next section
            "in     r0, %[_SPL_] \n\t"
            "st     X+, r0 \n\t"
            "in     r0, %[_SPH_] \n\t"
            "st     X+, r0 \n\t"
            "1:" //begin dispatching
            "mov    r26, %A[_next_process_] \n\t"
            "mov    r27, %B[_next_process_] \n\t"
            "sts    current_process, r26 \n\t" //set current process
            "sts    current_process+1, r27 \n\t"
            "ld     r0, X+ \n\t" //load stack pointer
            "out    %[_SPL_], r0 \n\t"
            "ld     r0, X+ \n\t"
            "out    %[_SPH_], r0 \n\t"
            "pop    r31 \n\t" //status into r31: andi requires register above 15
            "bst    r31, %[_I_] \n\t" //we don't want to enable interrupts just yet, so store the interrupt status in T
            "bld    r31, %[_T_] \n\t" //T flag is on the call clobber list and tasks are only blocked as a result of a function call
            "andi   r31, %[_nI_MASK_] \n\t" //I is now stored in T, so clear I
            "out    %[_SREG_], r31 \n\t"
            "pop    r0 \n\t"
            "pop    r1 \n\t"
            "pop    r2 \n\t"
            "pop    r3 \n\t"
            "pop    r4 \n\t"
            "pop    r5 \n\t"
            "pop    r6 \n\t"
            "pop    r7 \n\t"
            "pop    r8 \n\t"
            "pop    r9 \n\t"
            "pop    r10 \n\t"
            "pop    r11 \n\t"
            "pop    r12 \n\t"
            "pop    r13 \n\t"
            "pop    r14 \n\t"
            "pop    r15 \n\t"
            "pop    r16 \n\t"
            "pop    r17 \n\t"
            "pop    r18 \n\t"
            "pop    r19 \n\t"
            "pop    r20 \n\t"
            "pop    r21 \n\t"
            "pop    r22 \n\t"
            "pop    r23 \n\t"
            "pop    r24 \n\t"
            "pop    r25 \n\t"
            "pop    r26 \n\t"
            "pop    r27 \n\t"
            "pop    r28 \n\t"
            "pop    r29 \n\t"
            "pop    r30 \n\t"
            "pop    r31 \n\t"
            "brtc   2f \n\t" //if the T flag is clear, do the non-interrupt enable return
            "reti \n\t"
            "2: \n\t"
            "ret \n\t"
            "" ::
            [_SREG_] "i" _SFR_IO_ADDR(SREG),
            [_I_] "i" SREG_I,
            [_T_] "i" SREG_T,
            [_nI_MASK_] "i" (~(1 << SREG_I)),
            [_SPL_] "i" _SFR_IO_ADDR(SPL),
            [_SPH_] "i" _SFR_IO_ADDR(SPH),
            [_next_process_] "r" (process));
}
