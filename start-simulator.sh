#!/bin/bash -e

echo "Starting simulator on port 8282 ..."
simulavr --device atmega168 --cpufrequency 16000000 --gdbserver -p 8282 --file preempt-sched.elf
