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
// Define constants to be used in this program.

// Defines to simply UART's baud rate generator (BRG) regiser
// given the osicllator freqeuncy and PLLMODE.
#define XTFREQ          7372800         	  // On-board Crystal frequency
#define PLLMODE         4               	  // On-chip PLL setting (Fosc)
#define FCY             (XTFREQ*PLLMODE)/2        // Instruction Cycle Frequency (Fosc/2)

#define BAUDRATE        115200
#define BRGVAL          ((FCY/BAUDRATE)/16)-1

// ******************************************************************************************* //

// Varible used to indicate that the current configuration of the keypad has been changed,
// and the KeypadScan() function needs to be called.

volatile char scanKeypad;
volatile long counter;

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
    PrintCharInProgMode,
    VerifyNumDigits,
    PrintInvalid,
    PrintValid
} stateType;

volatile stateType state = EnterAndWait;

// ******************************************************************************************* //

void Delay2s(){
    TMR2 = 0;
    IFS0bits.T3IF = 0;
    T2CONbits.TON = 1;

    while(IFS0bits.T3IF == 0);
    return;
}

int main(void)
{
        TMR2 = 0;
        TMR3 = 0;
        PR2 = 49664;
        PR3 = 1;
        IFS0bits.T3IF = 0;
        IEC0bits.T3IE = 1;
        T2CONbits.T32 = 1;
        T2CONbits.TON = 0;
        T2CONbits.TCKPS0 = 1;
        T2CONbits.TCKPS1 = 1;


	char key;                                       //Holds the key pressed or -1
        char pwdDB[4][4] = {{'1','2','3','4'},          //Password Database and Default pw
                            {NULL, NULL, NULL, NULL},
                            {NULL, NULL, NULL, NULL},
                            {NULL, NULL, NULL, NULL}
                            };
        char userPwd[4] = {NULL, NULL, NULL, NULL};     //Holds the password the user
        int pwdNumDigits = 0;
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

            if (scanKeypad == 1) {

                key = KeypadScan();

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
                            LCDMoveCursor(1,digitCount);
                            LCDPrintChar("#");
                            digitCount = 0;
                            state = PrintBad;
                        }
                        else if (key == "*" && digitCount > 1) {
                            LCDMoveCursor(1,digitCount);
                            LCDPrintChar("#");
                            digitCount = 0;
                            state = PrintBad;
                        }
                        else if (key == "*") {
                            LCDMoveCursor(1,0);
                            LCDPrintChar("*");
                            digitCount++;
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
                        for (i = 0; i < digitCount; i++) {
                            userPwd[i] = NULL;
                        }
                        digitCount = 0;
                        state = Delay;
                        break;
                    case PrintBad:
                        LCDClear();
                        LCDMoveCursor(0,0);
                        LCDPrintString("Bad");
                        for (i = 0; i < digitCount; i++) {
                            userPwd[i] = NULL;
                        }
                        digitCount = 0;
                        state = Delay;
                        break;
                    case Delay:
                        Delay2s();
                        break;
                    case FirstStar:
                        if (key == '*') {
                            LCDMoveCursor(1,digitCount);
                            LCDPrintChar('*');
                            digitCount = 0;
                            state = EnterProgramMode;
                        }
                        else {
                            digitCount = 0;
                            state = PrintBad;
                        }
                        break;
                    case EnterProgramMode:
                        if (key == '#') {
                            digitCount = 0;
                            state = VerifyNumDigits;
                        }
                        if (key == '*') {
                            LCDClear();
                            digitCount = 0;
                            state = PrintInvalid;
                        }
                        else if (key != -1) {
                            userPwd[digitCount] = key;
                            LCDMoveCursor(1,digitCount);
                            state = PrintCharInProgMode;
                        }
                        break;
                    case PrintCharInProgMode:
                        LCDPrintChar(key);
                        digitCount++;
                        if (digitCount < 3) {
                            state = EnterProgramMode;
                        }
                        else {
                            digitCount = 0;
                            state = VerifyNumDigits;
                        }
                        break;
                    case VerifyNumDigits:
                        for (i = 0; i < 4; i++) {
                            if (userPwd[i] == NULL && userPwd != '#' && userPwd != '*') {
                                pwdNumDigits++;
                            }
                        }
                        if (pwdNumDigits == 4) {
                            for (i = 0; i < 4; i++) {
                                if (pwdDB[i][0] != NULL) {
                                    for (j = 0; j < 4; j++) {
                                        pwdDB[i][j] = userPwd[j];
                                    }
                                }
                            }
                            state = PrintValid;
                        }
                        else {
                            state = PrintInvalid;
                        }
                        break;
                    case PrintInvalid:
                        LCDClear();
                        LCDMoveCursor(0,0);
                        LCDPrintString("Invalid");
                        for (i = 0; i < digitCount; i++) {
                            userPwd[i] = NULL;
                        }
                        digitCount = 0;
                        state = Delay;
                        break;
                    case PrintValid:
                        LCDClear();
                        LCDMoveCursor(0,0);
                        LCDPrintString("Valid");
                        for (i = 0; i < digitCount; i++) {
                            userPwd[i] = NULL;
                        }
                        digitCount = 0;
                        state = Delay;
                        break;
                }
            }
            scanKeypad = 0;
	}
	return 0;
}

// ******************************************************************************************* //
// Defines an interrupt service routine that will execute whenever Timer 1's
// count reaches the specfied period value defined within the PR1 register.
//
//     _ISR and _ISRFAST are macros for specifying interrupts that
//     automatically inserts the proper interrupt into the interrupt vector
//     table
//
//     _T1Interrupt is a macro for specifying the interrupt for Timer 1
//
// The functionality defined in an interrupt should be a minimal as possible
// to ensure additional interrupts can be processed.
void _ISR _T3Interrupt(void)
{
	IFS0bits.T3IF = 0;                                          //Clear Timer1 Flag
        counter++;
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
