#include <stdint.h>  /* Declarations of uint_32 and the like */
#include <pic32mx.h> /* Declarations of system-specific addresses etc */
#include "mipslab.h" /* Declarations for these labs */

void menu(void) 
{
 int sw = getsw();

    if (sw & 0x01) // If the first switch is ON
    {
        gameloop();
    }
    else if (sw & 0x02) // If the second switch is ON
    {
        //gameloopHard();
    }
    else if (sw & 0x04) // If the third switch is ON
    {
        //gameloopAI();
    }   
}
