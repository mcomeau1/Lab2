// ******************************************************************************************* //
// Include file for PIC24FJ64GA002 microcontroller. This include file defines
// MACROS for special function registers (SFR) and control bits within those
// registers.

#include "p24fj64ga002.h"
#include <stdio.h>
#include "lcd.h"


// ******************************************************************************************* //
// Configuration bits for CONFIG1 settings.
//
// Make sure "Configuration Bits set in code." option is checked in MPLAB.
//
// These settings are appropriate for debugging the PIC microcontroller. If you need to
// program the PIC for standalone operation, change the COE_ON option to COE_OFF.

_CONFIG1( JTAGEN_OFF & GCP_OFF & GWRP_OFF &
		 BKBUG_ON & COE_ON & ICS_PGx1 &
		 FWDTEN_OFF & WINDIS_OFF & FWPSA_PR128 & WDTPS_PS32768 )

// ******************************************************************************************* //
// Configuration bits for CONFIG2 settings.
// Make sure "Configuration Bits set in code." option is checked in MPLAB.

_CONFIG2( IESO_OFF & SOSCSEL_SOSC & WUTSEL_LEG & FNOSC_PRIPLL & FCKSM_CSDCMD & OSCIOFNC_OFF &
		 IOL1WAY_OFF & I2C1SEL_PRI & POSCMOD_XT )

#define XTFREQ          7372800         	  // On-board Crystal frequency
#define PLLMODE         4               	  // On-chip PLL setting (Fosc)
#define FCY             (XTFREQ*PLLMODE)/2    // Instruction Cycle Frequency (Fosc/2)
// ******************************************************************************************* //

// Varible used to indicate that the current configuration of the keypad has been changed,
// and the KeypadScan() function needs to be called.

volatile int scanKeypad = 0;

// ******************************************************************************************* //

int main(void)
{

        TMR4 = 0;
        PR4 = 50464;           //2s
        PR5 = 1;
        IFS1bits.T5IF = 0;
        IEC1bits.T5IE = 1;
        T4CON = 0x8030;
        T4CONbits.T32 = 1;
	char key;
        //key is used to receive the character or number which is from user input
        int state = 0;
	char code[4][4] = {{ "1234" },
                           { "aaaa" },
                           { "aaaa" },
                           { "aaaa" }};
        //code is used to store 4 different passwords
        char temp[4] = {0, 0, 0, 0};
        //temp is used to store user input, either character or number.
        //comparing code if in correct form(4 digits)
        //store into code if in Set Mode and in correct form(4 digits + '#'
        int i = 0;
        //used in for loop
        int j = 0;
        //used in for loop
        int index = 1;
        //index is used to keep track of code's valid passwords
        //default is 1, because the default password is 1234
        //In Set Mode, if user adds a new correct password, index++
        //Example:
        //  default:        +(4567#):       +(invalid):
        //        1234           1234           1234
        // index->aaaa           4567           4567
        //        aaaa    index->aaaa           aaaa
        //        aaaa           aaaa           aaaa
        int valid = 1;
        //valid is used to check the first 4 bits in temp have any '*' in Set Mode
	int count = 0;
        //count is used to compare temp and code bit by bit in Enter Modeused
        IFS1bits.CNIF = 0;
        IEC1bits.CNIE = 1;
	// TODO: Initialize and configure IOs, LCD (using your code from Lab 1),
	// UART (if desired for debugging), and any other configurations that are needed.

	LCDInitialize();
	KeypadInitialize();
        LCDClear();
	// TODO: Initialize scanKeypad variable.
	while(1)
	{
            switch(state)
            {
            case 0:
                //first state, waiting for the 1st number
                //if '*' go to state 4 which is waiting for next '#' to go to Set Mode
                //if '#' go to state 7 which print bad for 2 sec
                LCDMoveCursor(0,0);
		LCDPrintString("Enter   ");
                LCDMoveCursor(1,0);
                if( scanKeypad == 1 )
                {
                    key = KeypadScan();
                    if( key != -1 )
                    {
                        if(key == '#')
                        {
                            state = 7;
                            //state: print bad for 2 sec
                        }
                        else if(key == '*')
                        {
                            state = 4;
                            //state: waiting for another '*' to go to Set Mode
                        }
                        else
                        {
                            temp[0] = key;
                            //store the 1st number into temp
                            state = 1;
                        }
                        LCDPrintChar(key);
                    }
                scanKeypad = 0;
                }
                break;

            case 1:
                //second state, waiting for the 2nd number
                //if '*' go to state 4 which is waiting for next '#' to go to Set Mode
                //if '#' go to state 7 which print bad for 2 sec
                if( scanKeypad == 1 )
                {
                    key = KeypadScan();
                    if( key != -1 ) {
                        if(key == '#')
                        {
                            state = 7;
                            //state: print bad for 2 sec
                        }
                        else if(key == '*')
                        {
                            state = 4;
                            //state: waiting for another '*' to go to Set Mode
                        }
                        else
                        {
                            temp[1] = key;
                            //store the 2nd number into temp
                            state = 2;
                        }
                         LCDPrintChar(key);
                    }
                scanKeypad = 0;
                }
                break;
            case 2:
                //third state, waiting for the 3rd number
                //if '*' go to state 4 which is waiting for next '#' to go to Set Mode
                //if '#' go to state 7 which print bad for 2 sec
                if( scanKeypad == 1 )
                {
                    key = KeypadScan();
                    if( key != -1 ) {
                        if(key == '#')
                        {
                            state = 7;
                            //state: print bad for 2 sec
                        }
                        else if(key == '*')
                        {
                            state = 4;
                            //state: waiting for another '*' to go to Set Mode
                        }
                        else
                        {
                            temp[2] = key;
                            state = 3;
                         }
                       LCDPrintChar(key);
                    }
                scanKeypad = 0;
                }
                break;

             case 3:
                //4th state, waiting for the 4th number
                //if '*' go to state 4 which is waiting for next '#' to go to Set Mode
                //if '#' go to state 7 which print bad for 2 sec
                if( scanKeypad == 1 )
                {
                    key = KeypadScan();
                    if( key != -1 ) {
                        if(key == '#')
                        {
                            state = 7;
                            //state: print bad for 2 sec
                        }
                        else if(key == '*')
                        {
                            state = 4;
                            //state: waiting for another '*' to go to Set Mode
                        }
                        else
                        {

                            temp[3] = key;
                            state = 5;
                        }
                        LCDPrintChar(key);
                    }
                    scanKeypad = 0;
                }
                break;

              case 4:
                  //User Mode <-> state 4 -> Set Mode
                  //in user mode if entered '*', state changes to 4
                  //if user press '*' again, go to set mode
                  //else print bad for 2 sec then go back to state 0
                  if( scanKeypad == 1 )
                  {
                    key = KeypadScan();
                    if( key != -1 ) {
                        if(key != '*')
                        {
                            state = 7;
                            //state: print bad for 2 sec
                        }
                        else
                        {
                            state = 8;
                            //Set Mode
                        }
                        LCDPrintChar(key);
                    }
                  scanKeypad = 0;
                  }
                  break;

              case 5:
                  //state to compare temp(user input) with code(passwords) bit by bit
                  for (i = 0; i < 4; i++)
                  {
                      count = 0;
                      for (j = 0; j < 4; j++)
                      {
                          if (temp[j] == code[i][j])
                          {
                              count++;
                          }
                      }
                      if(count == 4)
                      {
                          state = 6;
                          //state: print good for 2 sec
                      }
                   }
                  if(state != 6)
                  {
                      state = 7;
                      //state: print bad for 2 sec
                  }
                   break;

                case 6:
                    LCDClear();
                    LCDMoveCursor(0,0);
                    LCDPrintString("Good    ");
                    TMR4 = 0;
                    TMR5 = 0;
                    while(IFS1bits.T5IF == 0);  //2 sec
                    LCDClear();
                    state = 0;
                    break;

                case 7:
                    LCDClear();
                    LCDMoveCursor(0,0);
                    LCDPrintString("Bad     ");
                    LCDPrintString(count);
                    TMR4 = 0;
                    TMR5 = 0;
                    while(IFS1bits.T5IF == 0);  //2 sec
                    LCDClear();
                    state = 0;
                    break;

                case 8:
                    //state: print Set Mode on top
                    //       go to state 9 automatically
                    LCDClear();
                    LCDMoveCursor(0,0);
                    LCDPrintString("Set Mode");
                    LCDMoveCursor(1,0);
                    state = 9;
                    //state: let user print newpasswords
                    break;

                case 9:
                    //state: let user print newpasswords
                    //       will not change state until user press '#'
                    i = 0;
                    while(key != '#')
                    {
                        if( scanKeypad == 1 )
                        {
                            key = KeypadScan();
                            if( key != -1 ) {
                                LCDPrintChar(key);
                                if(i<4)
                                {
                                    temp[i] = key;
                                }
                                //only receive first 4 digits or characters
                                i++;
                            }
                        }
                        scanKeypad = 0;
                    }
                    LCDMoveCursor(0,0);
                    state = 10;
                    break;

                case 10:
                    //state: check user inputs(store in temp) bit by bit
                    if(i > 5)
                    {
                        valid = -1;
                    }
                    //check if user inputs more than 4 digits or characters(Ex: 12345#)
                    for(i = 0; i < 4; i++)
                    {
                        if(temp[i] < '0' || temp[i] > '9')
                        {
                            valid = -1;
                        }
                        //check if user inputs has characters
                    }
                    state = 11;
                    break;

                case 11:
                    //determine whether user input is valid or invalid(by variable valid)
                    if(valid == -1)
                    {
                        LCDPrintString("Invalid ");
                    }
                    else
                    {
                        LCDPrintString("Valid   ");
                        for(i = 0; i < 4; i++)
                        {
                            code[index][i] = temp [i];
                        }
                        //store new password into the code
                        index++;
                    }
                    TMR4 = 0;
                    TMR5 = 0;
                    while(IFS1bits.T5IF == 0);
                    LCDClear();
                    state = 0;
                    break;
            }

	}
	return 0;
}

// ******************************************************************************************* //
// Defines an interrupt service routine that will execute whenever any enable
// input change notifcation is detected.
//
//     In place of _ISR and _ISRFAST, we can directy use __attribute__((interrupt))
//     to inform the compiler that this function is an interrupt.
//
//     _CNInterrupt is a macro for specifying the interrupt for input change
//     notification.
//
// The functionality defined in an interrupt should be a minimal as possible
// to ensure additional interrupts can be processed.
void __attribute__((interrupt)) _CNInterrupt(void)
{
	// TODO: Clear interrupt flag
	IFS1bits.CNIF = 0;
        scanKeypad = 1;

	// TODO: Detect if *any* key of the keypad is *pressed*, and update scanKeypad
	// variable to indicate keypad scanning process must be executed.
}

// ******************************************************************************************* //

void __attribute__((interrupt,auto_psv)) _T5Interrupt(void){
    IFS1bits.T5IF = 0;
    TMR4 = 0;
    TMR5 = 0;
}