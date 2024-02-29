/* io.c
   This file written 2024 by Marcus Jansson and Hampus Berglund

   For copyright and licensing, see file COPYING */

#include <stdint.h>
#include <pic32mx.h>
#include "mipslab.h"

int getsw(void)
{
    int switchStatus = (PORTD & 0x0F00) >> 8;
    return switchStatus;
}

int getbtns(void)
{
    int buttons1 = ((PORTD >> 5) & 0x7) << 1;
    int buttons2 = ((PORTF >> 1) & 0x1);
    return buttons1 + buttons2;
}
