//-----------------------------------------------------------------------------
// F320_Blinky.c
//-----------------------------------------------------------------------------
// Copyright (C) 2008 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program flashes the green LED on the C8051F32x target board about
// five times a second using the interrupt handler for Timer2.
//
//
// How To Test:
//
// 1) Download code to a 'F32x target board
// 2) Ensure that pins 5 and 6 are shorted together on the J3 header
// 3) Run the program.  If the LED flashes, the program is working
//
//
// Target:         C8051F32x
// Tool chain:     Keil C51 7.50 / Keil EVAL C51 / SDCC 2.7.0
// Command Line:   None
//
// Release 1.1 / 10 MAR 2002 (TP)
//    -Updated Format
//
// Release 1.0 / 06 NOV 2002 (HF)
//    -Initial Revision

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include "compiler_defs.h"
#include "C8051F320_defs.h"            // SFR declarations

//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------

#define SYSCLK       24500000 / 8      // SYSCLK frequency in Hz

SBIT(LED, SFR_P2, 2);                  // LED='1' means ON
SBIT(SW2, SFR_P2, 0);                  // SW2='0' means switch pressed

//-----------------------------------------------------------------------------
// Function PROTOTYPES
//-----------------------------------------------------------------------------

void SYSCLK_Init (void);
void PORT_Init (void);
void Timer3_Init (int counts);

INTERRUPT_PROTO(Timer3_ISR, INTERRUPT_TIMER3);

int glob_counter=1;

//-----------------------------------------------------------------------------
// MAIN Routine
//-----------------------------------------------------------------------------
void main (void)
{
   PCA0MD &= ~0x40;                    // Disable watchdog timer

   SYSCLK_Init ();                     // Initialize system clock to
                                       // 12/8=1.5 MHz



  PORT_Init ();  		// Initialize crossbar and GPIO


   Timer3_blink (1);     // Init Timer2 to generate
                                       // interrupts at a 10Hz rate.

   EA = 1;                             // Enable global interrupts

   while (1){};                          // Spin forever
}

//-----------------------------------------------------------------------------
// SYSCLK_Init
//-----------------------------------------------------------------------------
//
// Return Value: none
// Parameters:   none
//
// This routine initializes the system clock to use the internal 24.5MHz / 8
// oscillator as its clock source.  Also enables missing clock detector reset.
//
//-----------------------------------------------------------------------------
void SYSCLK_Init (void)
{
   OSCICN = 0x80;                      // Configure internal oscillator for
                                       // its lowest frequency
   //RSTSRC = 0x04;                      // Enable missing clock detector
}

//-----------------------------------------------------------------------------
// PORT_Init
//-----------------------------------------------------------------------------
//
// Return Value: none
// Parameters:   none
//
// Configure the Crossbar and GPIO ports.
//
// P2.2 - LED (push-pull)
// P0.0 - SYSCLK (push-pull)
// All other port pins unused
//
//-----------------------------------------------------------------------------
void PORT_Init (void)
{
   P0MDOUT |= 0x01;
   P2MDOUT |= 0x04;                    // Enable LED as a push-pull output

   XBR0 = 0x08;                        // SYSCLK routed to pin - No digital peripherals selected
   XBR1 = 0x40;                        // Enable crossbar and weak pull-ups
}

//-----------------------------------------------------------------------------
// Timer2_Init
//-----------------------------------------------------------------------------
//
// Return Value: none
// Parameters:   none
//
// Configure Timer2 to 16-bit auto-reload and generate an interrupt at
// interval specified by <counts> using SYSCLK/48 as its time base.
//
//-----------------------------------------------------------------------------
void Timer3_Init (secs)
{
   TMR3CN = 0x00;                      // Stop Timer; Clear TF;
                                       // use SYSCLK/12 as timebase

   T3XCLK=0x00;   // clck based on system clock/8*8=187500 signals per second

   TMR3L=0x1b; // setting the high to  187500/4=46875 clicks
   TMR3H=0xb7;


   ET3 = 1;                            // Enable Timer interrupts
   TR3 = 1;                            // Start Timer

   
}

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Timer2_ISR
//-----------------------------------------------------------------------------
//
// This routine changes the state of the LED whenever Timer2 overflows.
//
//-----------------------------------------------------------------------------
INTERRUPT(Timer3_ISR, INTERRUPT_TIMER3)
{
   TF3H = 0;

   if(glob_counter==2*secs)
   {
	   LED=!LED;

	   glob_counter=1;
   }


   glob_counter+=1;								   // Change state of LED
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
