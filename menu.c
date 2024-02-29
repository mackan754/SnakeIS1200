/* menu.c
   This file written 2024 by Marcus Jansson and Hampus Berglund

   For copyright and licensing, see file COPYING */

#include <stdint.h>  /* Declarations of uint_32 and the like */
#include <pic32mx.h> /* Declarations of system-specific addresses etc */
#include "mipslab.h" /* Declarations for these labs */
int gamemode = 0;

/*
Huvudmeny för spelet.
Här kan du välja lätt eller svår svårighetsgrad.
*/

void menu(void)
{
    display_string(0, "    Snake    ");
    display_string(1, "    1.Easy");
    display_string(2, "    2.Hard");
    display_update();

    int sw = getsw();

    if (sw & 0x08) // Första switchen
    {
        gamemode = 0;
        gameloop();
    }
    else if (sw & 0x04) // Andra switchen
    {
        gamemode = 1;
        gameloop();
    }
}