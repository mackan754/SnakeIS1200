#include <stdint.h>
#include <pic32mx.h>
#include "mipslab.h"

int getsw(void) {
    int switchStatus = (PORTD >> 8) & 0xF; // Shift right by 8 positions and mask with 0x000F to isolate the 4 bits.
    return switchStatus;
}

int getbtns(void) {
    // Mask to isolate bits 7 through 5 (for BTN4 to BTN2) and shift them to the right.
    int buttonStatus = (PORTD >> 5) & 0x7; // Shift right by 5 positions and mask with 0x0007 to isolate the 3 bits.
    return buttonStatus;
}