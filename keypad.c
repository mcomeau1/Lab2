// ******************************************************************************************* //

#include "p24fj64ga002.h"
#include "keypad.h"

// ******************************************************************************************* //

void KeypadInitialize() {
	
	// TODO: Configure IOs and Change Notificaiton interrupt for keypad scanning. This 
	// configuration should ensure that if any key is pressed, a change notification interrupt 
	// will be generated.

        //Configure data direction
        TRISBbits.TRISB2    = 1;
        TRISBbits.TRISB6    = 1;
        TRISBbits.TRISB7    = 1;
        TRISBbits.TRISB8    = 0;
        TRISBbits.TRISB9    = 0;
        TRISBbits.TRISB10   = 0;
        TRISBbits.TRISB11   = 0;

        //Default pins to digital where neccessary.
        AD1PCFGbits.PCFG2   = 1;

        //Enable Open Drain Configuration for neccessary pins.
        ODCBbits.ODB8       = 1;
        ODCBbits.ODB9       = 1;
        ODCBbits.ODB10      = 1;
        ODCBbits.ODB11      = 1;

        //Enable the change notification for inputs
        CNEN1bits.CN6IE     = 1;
        CNEN2bits.CN24IE    = 1;
        CNEN2bits.CN27IE    = 1;

        //Enable the internal pullup resistor for inputs
        CNPU1bits.CN6PUE    = 1;
        CNPU2bits.CN24PUE   = 1;
        CNPU2bits.CN27PUE   = 1;
}

// ******************************************************************************************* //

char KeypadScan() {
	char key = -1;
	
	
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
	return key;
}

// ******************************************************************************************* //
