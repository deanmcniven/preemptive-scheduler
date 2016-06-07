#Preemptive Scheduler

Simple PoC Pre-emptive scheduler for an Atmel atMega168a
Pre-emption code based this tutorial: http://kevincuzner.com/2015/12/31/writing-a-preemptive-task-scheduler-for-avr/


#Display Shield
Custom shield with 2 LEDS and 2 7-Segent Displays driven by HEF4543B BCD-to-7Seg Drivers.

Logic 1 Illuminates
LED1 = PORTB1
LED0 = PORTB0

BCD Common address bus = PORTD7-4
BCD LatchEn MSD = PORTD3
BCD LatchEn LSD = PORTD2

#Attaching to simulator
Run avr-gdb
Then run:
- file preempt-sched.elf
- target remote localhost:8282
- load
- break main
- continue

#Introducting Delays
Run 'make clean all PFLAGS=-DGREEDY_TASK' to add delays into a task to demonstrate benefits of a preemptive system.
