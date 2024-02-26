#include <stdint.h>
#include <pic32mx.h>
#include "mipslab.h"

int getsw(void)
{
    int switchStatus = (PORTD & 0x0F00) >> 8; // Shift right by 8 positions and mask with 0x000F to isolate the 4 bits.
    return switchStatus;
}

/*
int getbtns(void)
{
    // Mask to isolate bits 7 through 5 (for BTN4 to BTN2) and shift them to the right.
    int buttonStatus = (PORTD >> 5) & 0x7; // Shift right by 5 positions and mask with 0x0007 to isolate the 3 bits.
    return buttonStatus;
}
*/

int getbtns(void)
{
    // Shift by 5 := Extract 5-7| AND with 0x7 := Remove unnecesary bits | Shift by 1 to the LEFT
    int buttons1 = ((PORTD >> 5) & 0x7) << 1;
    // Shift by 1 := Extract 1| AND with 0x1 := Remove unnecesary bits
    int buttons2 = ((PORTF >> 1) & 0x1);
    return buttons1 + buttons2;
}
