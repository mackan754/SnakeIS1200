/* mipslabwork.c

   This file written 2015 by F Lundevall
   Updated 2017-04-21 by F Lundevall

   This file should be changed by YOU! So you must
   add comment(s) here with your name(s) and date(s):

   This file modified 2017-04-31 by Ture Teknolog

   For copyright and licensing, see file COPYING */

#include <stdint.h>  /* Declarations of uint_32 and the like */
#include <pic32mx.h> /* Declarations of system-specific addresses etc */
#include "mipslab.h" /* Declatations for these labs */

/* Interrupt Service Routine */
void user_isr(void)
{
  return;
}

/* Lab-specific initialization goes here */
void labinit(void)
{
  TRISD |= 0xFFE0; // Ställer in bit 11-5 som inputs.

  int prescaling = 0b111;
  int start = 0b1;
  int source = 0b0;
  int mode = 0b0;
  int period = 31250; // Timer period = Clock/(prescaler*frequency) Timer Period= 80 000 000/256*10

  T2CON = 0 | start << 15 | mode << 3 | prescaling << 4 | source << 1;
  PR2 = period;
}