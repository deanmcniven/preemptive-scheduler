#include "display.h"

void setup_display() {
   DDRB = (1 << LED1) | (1 << LED0);
   DDRD = 0xFC;

   BCD = BCD_BLANK | (1 << BCD_L_MSD) | (1 << BCD_L_LSD);
   BCD = (0 << BCD_L_MSD) | (0 << BCD_L_LSD);
}
