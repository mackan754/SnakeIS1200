#include <stdint.h>  /* Declarations of uint_32 and the like */
#include <pic32mx.h> /* Declarations of system-specific addresses etc */
#include "mipslab.h" /* Declarations for these labs */
int gamemode = 0;

void aiMenu(void);

void menu(void)
{
    display_string(0, "    Snake    ");
    display_string(1, "    1.Easy");
    display_string(2, "    2.Hard");
    //display_string(3, "    3.AI");
    display_update();

    int sw = getsw();

    if (sw & 0x08) // If the first switch is ON
    {
        gamemode = 0;
        gameloop();
    }
    else if (sw & 0x04) // If the second switch is ON
    {
        gamemode = 1;
        gameloop();
    }
    else if (sw & 0x02)
    {
        //aiMenu();
    }
}

/*
void aiMenu(void)
{
    display_string(0, "    AI-Menu    ");
    display_string(1, "    1.Easy");
    display_string(2, "    2.Hard");
    display_string(3, "");
    display_update();

    while (1)
    {

    }
}
*/
