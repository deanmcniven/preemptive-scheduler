#!/bin/bash -e

echo "Starting AVR-GDB and connecting to simulator ..."
avr-gdb --command=gdb_startup.cmd
