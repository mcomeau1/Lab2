// ******************************************************************************************* //
// Include file for PIC24FJ64GA002 microcontroller. This include file defines
// MACROS for special function registers (SFR) and control bits within those
// registers.

#include "p24fj64ga002.h"
#include <stdio.h>
#include "lcd.h"
#include "keypad.h"

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

// ******************************************************************************************* //

// Varible used to indicate that the current configuration of the keypad has been changed,
// and the KeypadScan() function needs to be called.

volatile char scanKeypad;

// ******************************************************************************************* //
// Type definition used in main for a switch statement
typedef enum stateTypeEnum {
    EnterAndWait,
    PrintCharacter,
    CheckPassword,
    PrintGood,
    PrintBad,
    Delay,
    FirstStar,
    EnterProgramMode,
    VerifyNumDigits,
    PrintInvalid,
    PrintValid
} stateType;

volatile stateType state = EnterAndWait;

// ******************************************************************************************* //

int main(void)
{
	char key;                                       //Holds the key pressed or -1
        char pwdDB[4][4] = {{'1','2','3','4'},          //Password Database and Default pw
                            {NULL, NULL, NULL, NULL},
                            {NULL, NULL, NULL, NULL},
                            {NULL, NULL, NULL, NULL}
                            };
        char userPwd[4] = {NULL, NULL, NULL, NULL};     //Holds the password the user
        int i = 0;                                      //Loop variable
        int j = 0;                                      //Need two loop variables for pwdDB
        int match = 0;                                  //1 -> pwd found 0 -> pwd not found
        int numPwds = 0;                                //Tracks the number of passwords
        int digitCount = 0;                             //Counts the number of digits entered
	
	// TODO: Initialize and configure IOs, LCD (using your code from Lab 1), 
	// UART (if desired for debugging), and any other configurations that are needed.
	
	LCDInitialize();
	KeypadInitialize();
	
	// TODO: Initialize scanKeypad variable.
        scanKeypad = 0;
	
	while(1)
	{
		// TODO: Once you create the correct keypad driver (Part 1 of the lab assignment), write
		// the C program that use both the keypad and LCD drivers to implement the 4-digit password system.
                IFS1bits.CNIF = 0;
                IEC1bits.CNIE = 1;

                switch (state) {
                    case EnterAndWait:
                        LCDMoveCursor(0,0);
                        LCDPrintString("Enter");
                        if ((key != -1) && (key != '#') && (key != '*')) {
                            userPwd[digitCount] = key;
                            LCDMoveCursor(1,digitCount);
                            state = PrintCharacter;
                        }
                        else if (key == "#") {
                            state = PrintBad;
                        }
                        else if (key == "*") {
                            state = FirstStar;
                        }
                        break;
                    case PrintCharacter:
                        LCDPrintChar(key);
                        digitCount++;
                        if (digitCount < 3) {
                            state = EnterAndWait;
                        }
                        else {
                            digitCount = 0;
                            state = CheckPassword;
                        }
                        break;
                    case CheckPassword:
                        for (i = 0; i < numPwds; i++) {
                            for(j = 0; j < 4; j++) {
                                if (pwdDB[i][j] != userPwd[j]) {
                                    match = 0;
                                }
                                else {
                                    match = 1;
                                }
                            }
                        }
                        if (match == 1) {
                            state = PrintGood;
                        }
                        else {
                            state = PrintBad;
                        }
                        break;
                    case PrintGood:
                        LCDClear();
                        LCDMoveCursor(0,0);
                        LCDPrintString("Good");
                        state = Delay;
                        break;
                    case PrintBad:
                        break;
                    case Delay:
                        break;
                    case FirstStar:
                        break;
                    case EnterProgramMode:
                        break;
                    case VerifyNumDigits:
                        break;
                    case PrintInvalid:
                        break;
                    case PrintValid:
                        break;
                }

		if( scanKeypad == 1 ) {
			key = KeypadScan();
			if( key != -1 ) {
				LCDMoveCursor(0,0);		
				LCDPrintChar(key);
			}
			scanKeypad = 0;
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
	
	// TODO: Detect if *any* key of the keypad is *pressed*, update scanKeypad
	// variable to indicate keypad scanning process must be executed.
        if ((PORTBbits.RB2 == 0) || (PORTBbits.RB6 == 0) || (PORTBbits.RB7 == 0)) {
            scanKeypad = 1;
        }
}

// ******************************************************************************************* //
