/*
  Sensuin v2.0.1 - November 2014
  www.sensuino.net

  An Open Source Arduino based project.

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

//A part for LCD display handling. Copied from nokia_3110_lcd library of A. Lindsay.


#include "CharTable.h"

////////////////////////////LCD FUNCTOINS////////////////////////////////////////
////////////////////////////LCD//////////////////////////////////////////////////
#define SPI_INIT		SPCR = 0x51
#define LCDENABLE		SPI_CS_PORT &= ~(1<<SPI_CS)	// Enable LCD
#define LCDDISABLE	SPI_CS_PORT |= (1<<SPI_CS)	// disable LCD
#define LCDCMDMODE	PORTB &= ~(1<<LCD_DC)	// Set LCD in command mode
#define LCDDATAMODE	PORTB |= (1<<LCD_DC)	// Set LCD to Data mode

#define LCDCOLMAX	84
#define LCDROWMAX	6
#define LCDPIXELROWMAX	48
#define MAXPROGMEM_WORDLENGTH 12
/////////////////////////////////////////////////////////////////////////////////
#define SPI_CS PORTB1  
#define SPI_CS_PORT PORTB
#define SPI_CS_DDR DDRB
#define LCD_RST PORTB4
#define LCD_DC PORTB2
#define SPI_SS PORTB1   
#define USE_GRAPHIC
#define USE_BITMAP
#define SPI_MOSI PORTB3
#define SPI_SCK PORTB5

unsigned char cursor_row; /* 0-5 */
unsigned char cursor_col; /* 0-83 */

void writeCommand(unsigned char command ) {
    LCDENABLE;
    LCDCMDMODE;
    SPDR = command;		// Send data to display controller

    while (!(SPSR & 0x80));   	// Wait until Tx register empty

    LCDDISABLE;
}

void writeData(unsigned char data ) {
    LCDENABLE;
    LCDDATAMODE;
    SPDR = data;		// Send data	

    while (!(SPSR & 0x80));	// Wait until Tx register empty   

    LCDDISABLE;
}


void gotoXY(unsigned char x, unsigned char y) {
//      if (x > LCDCOLMAX - 1) x = LCDCOLMAX - 1 ; // ensure within limits
//      if (y > LCDROWMAX - 1) y = LCDROWMAX - 1 ; // ensure within limits

	writeCommand (0x80 | x);   //column
	writeCommand (0x40 | y);   //row

	cursor_row = y;
	cursor_col = x;
}

void writeChar(unsigned char ch) {
unsigned char lcd_buffer[LCDROWMAX][LCDCOLMAX];	
  
  unsigned char j;
  

      if (cursor_col > LCDCOLMAX - 6) cursor_col = LCDCOLMAX - 6; // ensure space is available for the character
      if (cursor_row > LCDROWMAX - 1) cursor_row = LCDROWMAX - 1; // ensure space is available for the character
	lcd_buffer[cursor_row][cursor_col] = 0x00;
	for(j=0; j<5; j++) {
		lcd_buffer[cursor_row][cursor_col + j] =  pgm_read_byte(&(smallFont [(ch-32)*5 + j] ));
        }

	lcd_buffer[cursor_row][cursor_col + 5] = 0x00;

	for(j=0; j< 6; j++) {
		writeData(lcd_buffer[cursor_row][cursor_col++]);
		
		if (cursor_col >= LCDCOLMAX)
		{
			cursor_col=0;
			cursor_row++;
			if (cursor_row >= LCDROWMAX) cursor_row=0;
		}
	}

}

void writeString(char *string){
     while (*string)
	 writeChar(*string++);
}

void printNumber(unsigned long n) {
  char buf[8 * sizeof(long) + 1]; // Assumes 8-bit chars plus zero byte.
  char *str = &buf[sizeof(buf) - 1];

  *str = '\0';

  // prevent crash if called with base == 1
 
  do {
    unsigned long m = n;
    n /= 10;
    char c = m - 10 * n;
    *--str = c < 10 ? c + '0' : c + 'A' - 10;
  } while(n);

  writeString(str);
}

void clearLine(uint8_t line) {
//unsigned char lcd_buffer[LCDROWMAX][LCDCOLMAX];	
        gotoXY (0,line);  	//start with (0,0) home position

	
		for(int j=0; j< LCDCOLMAX; j++) {
			writeData( 0x00 );

			//lcd_buffer[i][j] = 0x00;

		}
	
      gotoXY (0,0);	//bring the XY position back to (0,0)
}

void clear() {
gotoXY (0,0);  	//start with (0,0) home position
for(int i=0; i< LCDROWMAX; i++) {
clearLine(i);	
}  
}

void printAtLCD(char *inStr, uint8_t x, uint8_t y){
gotoXY(x,y);
writeString(inStr);
};

void printAtLCD(const int inInt, uint8_t x, uint8_t y){
gotoXY(x,y);
printNumber(inInt);
};

void PrintLCD_P(int which, char x, char y,  const char *menuName[]) {
   char buffer[MAXPROGMEM_WORDLENGTH];
   strcpy_P(buffer, (char*)pgm_read_word(&(menuName[which])));
  printAtLCD(buffer, x, y);
 
}





void LCDinit(){
    DDRB |= (1<<LCD_DC)|(1<<LCD_RST)|(1<<SPI_MOSI)|(1<<SPI_SCK)|(1<<SPI_SS);

    SPI_CS_DDR |= (1<<SPI_CS);
    //
    // even if we don't use SPI_SS for enabling the SPI device it must be high whilst
    // changing SPCR. This code assumes that if it is re-used it is only by another SPI device
    // and thus it will always be high whilst that device is not in use, hence it is safe to set high here
    // we would need to set it high on every LCDENABLE otherwise.
    //
    PORTB |= (1<<SPI_SS);
    LCDDISABLE;
    PORTB &= ~(1<<LCD_RST);
  
  	
   delayMicroseconds(1);

    PORTB |= (1<<LCD_RST);  
    SPI_INIT;   			// enable SPI master, fosc/16 = 1MH
   

    writeCommand(0x21);		// LCD Extended Commands
    //writeCommand(0xe0); 	// Set LCD Vop (Contrast)
    writeCommand(0xC0); 	// Set LCD Vop (Contrast) **NEW**
    //writeCommand(0x04);  	// Set temp coefficient
    writeCommand(0x06);  	// Set temp coefficient   **NEW**
    writeCommand(0x13);		// LCD bias mode1:48
    writeCommand(0x20);		// LCD Standard Commands, Horizontal addressing mode
    writeCommand(0x0c);		// LCD in normal mode
    clear();
}

