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
        TRISBbits.TRISB5    = 0;
        TRISBbits.TRISB10   = 0;
        TRISBbits.TRISB11   = 0;

        //Set output high initially.
        LATB = (LATB & 0xF0FF) | 0x0000;
        //-------------------------------------------------------------------
        // 1111 0001 1111 1111 //Why not 0xF0FF? to put output 8 also "0"?
        //-------------------------------------------------------------------

        //Default pins to digital where neccessary.
        AD1PCFGbits.PCFG4   = 1; //RB2

        //-------------------------------------------------------------------

        //Enable Open Drain Configuration for neccessary pins.
        ODCBbits.ODB8       = 1;
        ODCBbits.ODB5       = 1;
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

        int i = 0;
        int j = 0;
        int numRows = 4;
        char keys[4][3] = {
                            {'1','2','3'},
                            {'4','5','6'},
                            {'7','8','9'},
                            {'#','0','*'}
                            };

        int erro = 0;

        long groundedBits = 0xFEFF; //(In order to begin in RB8 (or change the wires fisically))

        // The outer loop will represent the four rows of the keypad
        for (i = 0; i < numRows; i++) {

            // When the row of the keypad is grounded pressing
            // a button in that row will pull the input pin
            // from high to low, thus indicating a button press.
            //
            // To ground the row I've created a "long" variable
            // named groundedBits which has a 1 for every bit
            // accept for the pin that we want grounded. I've
            // initialized groundedBits to 1111 0111 1111 1111
            // so that RB11 is grounded. at the end of each loop
            // the bits will be shifted to the right so that RB10,
            // RB5, and RB8 will be grounded in that order.
            //
            // You can't just "or" grounded bits with LATB though
            // without causing changes to the other PORTB pins.
            // To fixed this I applied masks to both LATB and
            // grounded bits so that only the desired pins are
            // changed.
            LATB = (LATB & 0xF0FF) | (groundedBits & 0x0F00);

            // If we are scanning the first column we want
            // to know if RB2 equals 0. If so we know that
            // the button on this row and this column was
            // pressed. We will return that key which is found
            // in the keys array.
            if(PORTBbits.RB2 == 0 && key != -1) {
                erro = 1;
            }
            if(PORTBbits.RB2 == 0 && key == -1) {
                key == keys[i][0];
            }
            if(PORTBbits.RB6 == 0 && key != -1) {
                erro = 1;
            }
            if(PORTBbits.RB6 == 0 && key == -1) {
                key == keys[i][1];
            }
            if(PORTBbits.RB7 == 0 && key != -1) {
                erro = 1;
            }
            if(PORTBbits.RB7 == 0 && key == -1) {
                key == keys[i][2];
            }
        }
            
        // Shift grounded bits to the left so that
        // the next row will be scanned in the next loop.
        groundedBits = groundedBits << 1;
        
        if(erro == 0){
            return key;
        }
        if(erro == 1){
            key = -1;
            return key;
        }
}

// ******************************************************************************************* //