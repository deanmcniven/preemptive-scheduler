#ifndef _DISPLAY_H_
#define _DISPLAY_H_ 1

#include <avr/io.h>

#define BCD         PORTD
#define BCD_0       0x00
#define BCD_1       0x10
#define BCD_2       0x20
#define BCD_3       0x30
#define BCD_4       0x40
#define BCD_5       0x50
#define BCD_6       0x60
#define BCD_7       0x70
#define BCD_8       0x80
#define BCD_9       0x90
#define BCD_BLANK   0xF0
#define BCD_L_MSD   3
#define BCD_L_LSD   2
#define BCD_OFFSET  4

#define LED         PORTB
#define LED1        1
#define LED0        0
#define LED1_MASK   0x02
#define LED0_MASK   0x01

void setup_display(void);

#endif
