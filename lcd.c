// ******************************************************************************************* //

#include "p24fj64ga002.h"

// ******************************************************************************************* //

// LCD Data connected to RB15 -> RB12, RS is connected to RB7, and E is connected to RB6
#define LCD_D   LATB
#define LCD_RS  LATBbits.LATB7
#define LCD_E   LATBbits.LATB6

// ******************************************************************************************* //

// TRIS register mappings for LCD signals
#define LCD_TRIS_D7  TRISBbits.TRISB15
#define LCD_TRIS_D6  TRISBbits.TRISB14
#define LCD_TRIS_D5  TRISBbits.TRISB13
#define LCD_TRIS_D4  TRISBbits.TRISB12
#define LCD_TRIS_RS  TRISBbits.TRISB7
#define LCD_TRIS_E   TRISBbits.TRISB6

// ******************************************************************************************* //

// Defines LCD write types for EnableLCD function that will be assigned to RS LCD input
// used to specific writing control instructions or data characters.
#define LCD_WRITE_DATA    1
#define LCD_WRITE_CONTROL 0

// ******************************************************************************************* //

// This function will use a 16-bit timer (Timer 2) to wait for approximately 0 to the 16000 us
// specfied by the input usDelay.
//
// Function Inputs:
//    unsigned usDelay     : Specifies the requested delay in microsecond

void DelayUs(unsigned int usDelay) {

	// TODO: Use Timer 2 to delay for precisely (as precise as possible) usDelay
	// microseconds provided by the input variable.
	//
	// Hint: Determine the configuration for the PR1 setting that provides for a
	// one microsecond delay, and multiply this by the input variable.
	// Be sure to user integer values only.

        // Clear Timer value (i.e. current tiemr value) to 0
        TMR2 = 0;

        IEC0bits.T2IE = 0;
	IFS0bits.T2IF = 0;

        // Set Timer 1's period value regsiter to value for 1us. (1:1 prescalar).
        PR2 = 15*usDelay;

        // Setup Timer 1 control register (T1CON) to:
        //     TON           = 1     (start timer)
        //     TCKPS1:TCKPS2 = 0    (set timer prescaler to 1:256)
        //     TCS           = 0     (Fosc/2)
        T2CON = 0x8000;

        IFS0bits.T2IF = 0;
        while(IFS0bits.T2IF == 0);
        return;
        
}

// ******************************************************************************************* //

// EnableLCD function enables writing control instructions or data (ASCII characters) to the
// LCD, controlling the LCD_RS and LCD_E signals.
//
// If the commandType is 0 (LCD_WRITE_CONTROL), the 8-bit control instructions must be
// asserted using the D7-D4 inputs. The 8-bit control instructions must be written
// to the LCD using the D7-D4 inputs as two 4-bit writes with the most significant 4-bits
// (nibble) written first. This requires two calls to the EnableLCD() function.
//
// If the commandType is 1 (LCD_WRITE_DATA), the 8-bit ASCII characters written to the LCD
// will be displayed by the LCD at the current cursor location. The 8-bit characters must be
// written to the LCD using the D7-D4 inputs as two 4-bit writes with the most significant
// 4-bits (nibble) written first. This requires two calls to the EnableLCD() function.

void EnableLCD(unsigned char commandType, unsigned usDelay) {

	LCD_RS = commandType; DelayUs(usDelay);
	LCD_E = 1;  DelayUs(usDelay);
	LCD_E = 0;  DelayUs(usDelay);
}

// ******************************************************************************************* //

// TODO: WriteLCD function should write either control instruciton or data characters to the
// LCD display using the 4-bit interface. This function will handled the bit masking and shifting
// to write the individual 4-bit nibbles to the LCD data inputs. Control instructions
// are written if the commandType is 0 (LCD_WRITE_CONTROL), and ASCII characters are written
// if the commandType is 1 (LCD_WRITE_DATA).
//
// Function Inputs:
//    unsigned char word   : 8-bit control instruction or data character to be written
//    unsigned commandType : Specifies the type of write operation, see above
//    unsigned usDelay     : Specifies the write delay required for this operation

void WriteLCD(unsigned char word, unsigned commandType, unsigned usDelay) {

	// TODO: Using bit masking and shift operations, write most significant bits to correct
	// bits of the LCD_D signal (i.e. #define used to map internal name to LATB)
	// and enable the LCD for the correct command.
        LCD_D = (LCD_D & 0x0FFF) | ((word << 8) & 0xF000);
        EnableLCD(commandType, usDelay);

	// TODO: Using bit masking and shift operations, write least significant bits to correct
	// bits of the LCD_D signal (i.e. #define used to map internal name to LATB)
	// and enable the LCD for the correct command.
        LCD_D = (LCD_D & 0x0FFF) | ((word << 12) & 0xF000);
        EnableLCD(commandType, usDelay);
}

// ******************************************************************************************* //

// The initialization routine is utulized to start the LCD operation and configure the
// LCD interface to utilize 4-bit interface. Please reference the data sheet for the LCD
// for further details.

void LCDInitialize(void) {

	// Setup D, RS, and E to be outputs (0).
        LCD_TRIS_D7 = 0;	// D7
	LCD_TRIS_D6 = 0;	// D6
	LCD_TRIS_D5 = 0;	// D5
	LCD_TRIS_D4 = 0;	// D4
	LCD_TRIS_RS = 0;	// RS
	LCD_TRIS_E  = 0;	// E
        
        LCD_D = (LCD_D & 0x0FFF) | 0x0000;
        LCD_RS = 0;
	LCD_E = 0;
	DelayUs(15000);

	// Initilization sequence utilizes specific LCD commands before the general configuration
	// commands can be utilized. The first few initilition commands cannot be done using the
	// WriteLCD function. Additionally, the specific sequence and timing is very important.
        LCD_D = (LCD_D & 0x0FFF) | 0x3000;
	EnableLCD(LCD_WRITE_CONTROL, 4100);
        EnableLCD(LCD_WRITE_CONTROL, 100);
        EnableLCD(LCD_WRITE_CONTROL, 100);

	// Enable 4-bit interface
        // DB5 -> Function Set
        // DB4 -> 4-bit data width)
        //
        //  |    Most Significant   |   Least Significant   |
        //  | DB7 | DB6 | DB5 | DB4 | DB3 | DB2 | DB1 | DB0 |
        //  |  0  |  0  |  1  |  0  |  *  |  *  |  *  |  *  |
        LCD_D = (LCD_D & 0x0FFF) | 0x2000;
        EnableLCD(LCD_WRITE_CONTROL, 40);

	// Function Set (specifies data width, lines, and font.
        // DB5 -> Function Set
        // DB4 -> 4-bit data width
        // DB3 -> Number of lines
        // DB2 -> Font
        //
        //  |    Most Significant   |   Least Significant   |
        //  | DB7 | DB6 | DB5 | DB4 | DB3 | DB2 | DB1 | DB0 |
        //  |  0  |  0  |  1  |  0  |  1  |  0  |  0  |  0  |
        WriteLCD(0x28, LCD_WRITE_CONTROL, 40);

	// 4-bit mode initialization is complete. We can now configure the various LCD
	// options to control how the LCD will function.

	// DB3 -> Display On/Off Control
	// DB2 -> Turn Display (D) Off
        //
        //  |    Most Significant   |   Least Significant   |
        //  | DB7 | DB6 | DB5 | DB4 | DB3 | DB2 | DB1 | DB0 |
        //  |  0  |  0  |  0  |  0  |  1  |  0  |  0  |  0  |
        WriteLCD(0x08, LCD_WRITE_CONTROL, 40);

	// DB0 -> Clear Display
        //
        //  |    Most Significant   |   Least Significant   |
        //  | DB7 | DB6 | DB5 | DB4 | DB3 | DB2 | DB1 | DB0 |
        //  |  0  |  0  |  0  |  0  |  0  |  0  |  0  |  1  |
        WriteLCD(0x01, LCD_WRITE_CONTROL, 1640);

	// DB2 -> Entry Mode Set
	// DB1 -> Increment Display
        // DB0 -> No Shift (i.e. cursor move)
        //
        //  |    Most Significant   |   Least Significant   |
        //  | DB7 | DB6 | DB5 | DB4 | DB3 | DB2 | DB1 | DB0 |
        //  |  0  |  0  |  0  |  0  |  0  |  1  |  1  |  0  |
        WriteLCD(0x06, LCD_WRITE_CONTROL, 40);

	// DB3 -> Display On/Off Control
	// DB2 -> Turn Display (D) On
        // DB1 -> Cursor (C) Off
        // DB0 -> and Blink(B) Off
        //
        //  |    Most Significant   |   Least Significant   |
        //  | DB7 | DB6 | DB5 | DB4 | DB3 | DB2 | DB1 | DB0 |
        //  |  0  |  0  |  0  |  0  |  1  |  1  |  0  |  0  |
        WriteLCD(0x0C, LCD_WRITE_CONTROL, 40);

}

// ******************************************************************************************* //

// TODO: LCDClear sends a clear screen command to the LCD that will both clear the screen
// and return the cursor to the origin (0,0).

void LCDClear(void) {

	// TODO: Write the proper control instruction to clear the screen ensuring
	// the proper delay is utilized.

    	// DB0 -> Clear Display
        //
        //  |    Most Significant   |   Least Significant   |
        //  | DB7 | DB6 | DB5 | DB4 | DB3 | DB2 | DB1 | DB0 |
        //  |  0  |  0  |  0  |  0  |  0  |  0  |  0  |  1  |
        WriteLCD(0x01, LCD_WRITE_CONTROL, 1640);

        // DB1 -> Return Home
        //
        //  |    Most Significant   |   Least Significant   |
        //  | DB7 | DB6 | DB5 | DB4 | DB3 | DB2 | DB1 | DB0 |
        //  |  0  |  0  |  0  |  0  |  0  |  0  |  1  |  0  |
        WriteLCD(0x02, LCD_WRITE_CONTROL, 1640);

}

// ******************************************************************************************* //

// LCDMoveCursor should move the cursor to the specified (x,y) coordinate. Note that
// as the LCD controller is a generic interface for many LCD displays of varying size,
// some (x,y) cooridnates may not be visible by the LCD display.
//
// Function Inputs:
//    unsigned char x : coordinate for LCD row (0 or 1)
//    unsigned char y : coordinate for LCD column (0 to 7)

void LCDMoveCursor(unsigned char x, unsigned char y) {

	// Write the propoer control instruction to move the cursor to the specified
	// (x,y) coordinate. This operation should perform as a single control
	// instruction, i.e. a single call the WriteLCD() function.
        unsigned char address = 0;
        if ((x >= 0) && (x <= 1) && (y >= 0) && (y <= 7)) {
            // 0x80 -> Command Code for Set DD RAM Address
            // 0X40 -> DD RAM address for the begining of the second line.
            //  when multiplied by 0 you get the beining of the fist line.
            address = ((0x40 * x + y) + 0x80);
        }

        // DB7 -> Set DD RAM Address
        //
        //  |    Most Significant   |   Least Significant   |
        //  | DB7 | DB6 | DB5 | DB4 | DB3 | DB2 | DB1 | DB0 |
        //  |  1  |             DD RAM Adress               |
        WriteLCD(address, LCD_WRITE_CONTROL, 40);

}

// ******************************************************************************************* //

// TODO: LCDPrintChar should print a single ASCII character to the LCD diplay at the
// current cursor position.
//
// Function Inputs:
//    char c : ASCII character to write to LCD

void LCDPrintChar(char c) {

	// Write the ASCII character provide as input to the LCD display ensuring
	// the proper delay is utilized.
        WriteLCD(c, LCD_WRITE_DATA, 46);

}

// ******************************************************************************************* //

// LCDPrintString will print a null-terminated string to the LCD at the current cursor
// location.
//
// Notes:
//       1. If the string is longer than the length of a row, the entire string may not
//          appear on the LCD.
//       2. If the string is not null-terminated, random data within the memory pointed to
//          by the pointer will be displayed. This process will only terminate when a null
//          characters if found.

void LCDPrintString(const char* s) {
        // loop through the string printing the char and moving the cursor
    char c = s[0];
    int i = 0;
    while( c != '\0')
    {
        LCDPrintChar(c);
        i++;
        c = s[i];
    }
}

// ******************************************************************************************* //
