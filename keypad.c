// ******************************************************************************************* //

#include "p24fj64ga002.h"
#include "keypad.h"
#include <string.h>

// ******************************************************************************************* //

void KeypadInitialize() {
    TRISAbits.TRISA0 = 1;
    TRISAbits.TRISA1 = 1;
    TRISBbits.TRISB11 = 1;
    TRISBbits.TRISB2 = 0;
    TRISBbits.TRISB3 = 0;
    TRISBbits.TRISB5 = 0;
    TRISBbits.TRISB8 = 0;

    AD1PCFGbits.PCFG0 = 1;
    AD1PCFGbits.PCFG1 = 1;

    ODCBbits.ODB2 = 1;
    ODCBbits.ODB3 = 1;
    ODCBbits.ODB5 = 1;
    ODCBbits.ODB8 = 1;

    CNPU1bits.CN3PUE = 1;
    CNPU1bits.CN2PUE = 1;
    CNPU1bits.CN15PUE = 1;

    CNEN1bits.CN3IE = 1;
    CNEN1bits.CN2IE = 1;
    CNEN1bits.CN15IE = 1;

 
}

// ******************************************************************************************* //

char KeypadScan() {
	char key = -1;
        int cnt = 0;

	// TODO: Implement the keypad scanning procedure to detect if exactly one button of the
	// keypad is pressed. The function should return:
	//
	//      -1         : Return -1 if no keys are pressed.
	//      '0' - '9'  : Return the ASCII character '0' to '9' if one of the
	//                   numeric (0 - 9) keys are pressed.
	//      '#'        : Return the ASCII character '#' if the # key is pressed.
	//      '*'        : Return the ASCII character '*' if the * key is pressed.
	//       -1        : Return -1 if more than one key is pressed simultaneously.
	// Notes:
	//        1. Only valid inputs should be allowed by the user such that all invalid inputs
	//           are ignored until a valid input is detected.
	//        2. The user must release all keys of the keypad before the following key press
	//           is processed. This is to prevent invalid keypress from being processed if the
	//           users presses multiple keys simultaneously.
	//
        while(key == -1)
        {
            LATBbits.LATB2 = 0;
            LATBbits.LATB3 = 1;
            LATBbits.LATB5 = 1;
            LATBbits.LATB8 = 1;
            if(LATBbits.LATB2 == 0 && PORTAbits.RA0 == 0)
            {
                key = '1';
                cnt = cnt + 1;
            }
            if(LATBbits.LATB2 == 0 && PORTAbits.RA1 == 0)
            {
                key = '2';
                cnt = cnt + 1;
            }
            if(LATBbits.LATB2 == 0 && PORTBbits.RB11 == 0)
            {
                key = '3';
                cnt = cnt + 1;
            }

            LATBbits.LATB2 = 1;
            LATBbits.LATB3 = 0;
            LATBbits.LATB5 = 1;
            LATBbits.LATB8 = 1;
            if(LATBbits.LATB3 == 0 && PORTAbits.RA0 == 0)
            {
                key = '4';
                cnt = cnt + 1;
            }
            if(LATBbits.LATB3 == 0 && PORTAbits.RA1 == 0)
            {
                key = '5';
                cnt = cnt + 1;
            }
            if(LATBbits.LATB3 == 0 && PORTBbits.RB11 == 0)
            {
                key = '6';
                cnt = cnt + 1;
            }

            LATBbits.LATB2 = 1;
            LATBbits.LATB3 = 1;
            LATBbits.LATB5 = 0;
            LATBbits.LATB8 = 1;
            if(LATBbits.LATB5 == 0 && PORTAbits.RA0 == 0)
            {
                key = '7';
                cnt = cnt + 1;
            }
            if(LATBbits.LATB5 == 0 && PORTAbits.RA1 == 0)
            {
                key = '8';
                cnt = cnt + 1;
            }
            if(LATBbits.LATB5 == 0 && PORTBbits.RB11 == 0)
            {
                key = '9';
                cnt = cnt + 1;
            }

            LATBbits.LATB2 = 1;
            LATBbits.LATB3 = 1;
            LATBbits.LATB5 = 1;
            LATBbits.LATB8 = 0;
            if(LATBbits.LATB8 == 0 && PORTAbits.RA0 == 0)
            {
                key = '*';
                cnt = cnt + 1;
            }
            if(LATBbits.LATB8 == 0 && PORTAbits.RA1 == 0)
            {
                key = '0';
                cnt = cnt + 1;
            }
            if(LATBbits.LATB8 == 0 && PORTBbits.RB11 == 0)
            {
                key = '#';
                cnt = cnt + 1;
            }
        }
        LATBbits.LATB2 = 0;
        LATBbits.LATB3 = 0;
        LATBbits.LATB5 = 0;
        LATBbits.LATB8 = 0;
        if(cnt >= 2)
        {
            key = -1;
        }
	return key;
}


// ******************************************************************************************* //