#include <avr/interrupt.h>
#include <DS1307RTC.h> 
#include <DHT.h>
#include <EEPROM.h>
#include <Time.h>  
#include <NewPing.h>
#include <Wire.h>
#include <SdFat.h>
#include "EEPROMAnything.h"
#include "avr_bmp.h"
#include <OneWire.h>
#include <PString.h>

#define MENU_NORMAL	0
#define MENU_HIGHLIGHT 1
#define PIXEL_OFF 0
#define PIXEL_ON  1
#define PIXEL_XOR 2
#define SPI_MOSI PORTB3
#define SPI_SCK PORTB5
#define LCD_DC  PORTB2
#define LCD_BL  PORTD7	
#define SPI_CS  PORTB2
#define SPI_CS_PORT  PORTB
#define SPI_CS_DDR  DDRB
#define LCD_RST PORTB4
#define SPI_SS  PORTB2
#define USE_GRAPHIC
#define USE_BITMAP


////////////////////////////LCD//////////////////////////////////////////////////
#define SPI_INIT		SPCR = 0x51
#define LCDENABLE		SPI_CS_PORT &= ~(1<<SPI_CS)	// Enable LCD
#define LCDDISABLE	SPI_CS_PORT |= (1<<SPI_CS)	// disable LCD
#define LCDCMDMODE	PORTB &= ~(1<<LCD_DC)	// Set LCD in command mode
#define LCDDATAMODE	PORTB |= (1<<LCD_DC)	// Set LCD to Data mode

#define LCDCOLMAX	84
#define LCDROWMAX	6
#define LCDPIXELROWMAX	48
/////////////////////////////////////////////////////////////////////////////////

#define MenuHideButton 2  //Show/Hide menu button pin
#define BOUNCE_DURATION 300 // Bounce duration for Show/Hide menu button

#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters).
#define PH_SAMPLES 100  // number of pH samples taken by one reading
#define FREQSAMPLES 1000 //number of freq samples taken by one EC reading

#define MICROSINHOUR 1440000 //used for delay conversion
#define MICROSINMINUTE 60000 //used for delay conversion

#define PHTYPE 1
#define ECTYPE 2

#define RIGHT_COLUM 42

#define MAXPROGMEM_WORDLENGTH 12

#define PHINDEX 2
#define ECINDEX 4

///////////////////////////////////BUTTONS///////////////////////////////////////
#define Buttons A2
#define SD_CS 10
#define CE_PIN A3  
#define IO_PIN  A4    
#define SCLK_PIN A5
/////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////PINS///////////////////////////////////////////
#define ANpin1 A3
#define ANpin2 A1
#define ANpin3 A0
#define SEpin_A A4
#define SEpin_B A5
#define DIGpin1_A 0
#define DIGpin1_B 1
#define DIGpin2_A 3
#define DIGpin2_B 4
#define DIGpin3_A 7
#define DIGpin3_B 6
#define DIGpin4_A 9
#define DIGpin4_B 8
/////////////////////////////////////////////////////////////////////////////////
#define KeyUp    30
#define KeyDown  40
#define KeySel   50
#define KeyBack  60
#define KeyInv   70 //invalid key value

#define AnalogMenuLength 5
#define DigitalMenuLength 7
#define MainMenuLength 5
#define ConectMenuLength 4
#define I2CMenuLength 4

#define MenuItemStructureLength 5
#define SetTimeMenuLength 6

#define SucesfullyDone string_22

#define CALSTEP 0.1 //Step in calibration menu

static const unsigned char smallFont[] PROGMEM =
{
      0x00, 0x00, 0x00, 0x00, 0x00 ,   // sp
      0x00, 0x00, 0x2f, 0x00, 0x00 ,   // !
      0x00, 0x07, 0x00, 0x07, 0x00 ,   // "
      0x14, 0x7f, 0x14, 0x7f, 0x14 ,   // #
      0x24, 0x2a, 0x7f, 0x2a, 0x12 ,   // $
      0x61, 0x66, 0x08, 0x33, 0x43 ,   // %
      0x36, 0x49, 0x55, 0x22, 0x50 ,   // &
      0x00, 0x05, 0x03, 0x00, 0x00 ,   // '
      0x00, 0x1c, 0x22, 0x41, 0x00 ,   // (
      0x00, 0x41, 0x22, 0x1c, 0x00 ,   // )
      0x14, 0x08, 0x3E, 0x08, 0x14 ,   // *
      0x08, 0x08, 0x3E, 0x08, 0x08 ,   // +
      0x00, 0x00, 0x50, 0x30, 0x00 ,   // ,
      0x10, 0x10, 0x10, 0x10, 0x10 ,   // -
      0x00, 0x60, 0x60, 0x00, 0x00 ,   // .
      0x20, 0x10, 0x08, 0x04, 0x02 ,   // /
      0x3E, 0x51, 0x49, 0x45, 0x3E ,   // 0
      0x00, 0x42, 0x7F, 0x40, 0x00 ,   // 1
      0x42, 0x61, 0x51, 0x49, 0x46 ,   // 2
      0x21, 0x41, 0x45, 0x4B, 0x31 ,   // 3
      0x18, 0x14, 0x12, 0x7F, 0x10 ,   // 4
      0x27, 0x45, 0x45, 0x45, 0x39 ,   // 5
      0x3C, 0x4A, 0x49, 0x49, 0x30 ,   // 6
      0x01, 0x71, 0x09, 0x05, 0x03 ,   // 7
      0x36, 0x49, 0x49, 0x49, 0x36 ,   // 8
      0x06, 0x49, 0x49, 0x29, 0x1E ,   // 9
      0x00, 0x36, 0x36, 0x00, 0x00 ,   // :
      0x00, 0x56, 0x36, 0x00, 0x00 ,   // ;
      0x08, 0x14, 0x22, 0x41, 0x00 ,   // <
      0x14, 0x14, 0x14, 0x14, 0x14 ,   // =
      0x00, 0x41, 0x22, 0x14, 0x08 ,   // >
      0x02, 0x01, 0x51, 0x09, 0x06 ,   // ?
      0x32, 0x49, 0x59, 0x51, 0x3E ,   // @
      0x7E, 0x11, 0x11, 0x11, 0x7E ,   // A
      0x7F, 0x49, 0x49, 0x49, 0x36 ,   // B
      0x3E, 0x41, 0x41, 0x41, 0x22 ,   // C
      0x7F, 0x41, 0x41, 0x22, 0x1C ,   // D
      0x7F, 0x49, 0x49, 0x49, 0x41 ,   // E
      0x7F, 0x09, 0x09, 0x09, 0x01 ,   // F
      0x3E, 0x41, 0x49, 0x49, 0x7A ,   // G
      0x7F, 0x08, 0x08, 0x08, 0x7F ,   // H
      0x00, 0x41, 0x7F, 0x41, 0x00 ,   // I
      0x20, 0x40, 0x41, 0x3F, 0x01 ,   // J
      0x7F, 0x08, 0x14, 0x22, 0x41 ,   // K
      0x7F, 0x40, 0x40, 0x40, 0x40 ,   // L
      0x7F, 0x02, 0x0C, 0x02, 0x7F ,   // M
      0x7F, 0x04, 0x08, 0x10, 0x7F ,   // N
      0x3E, 0x41, 0x41, 0x41, 0x3E ,   // O
      0x7F, 0x09, 0x09, 0x09, 0x06 ,   // P
      0x3E, 0x41, 0x51, 0x21, 0x5E ,   // Q
      0x7F, 0x09, 0x19, 0x29, 0x46 ,   // R
      0x46, 0x49, 0x49, 0x49, 0x31 ,   // S
      0x01, 0x01, 0x7F, 0x01, 0x01 ,   // T
      0x3F, 0x40, 0x40, 0x40, 0x3F ,   // U
      0x1F, 0x20, 0x40, 0x20, 0x1F ,   // V
      0x3F, 0x40, 0x38, 0x40, 0x3F ,   // W
      0x63, 0x14, 0x08, 0x14, 0x63 ,   // X
      0x07, 0x08, 0x70, 0x08, 0x07 ,   // Y
      0x61, 0x51, 0x49, 0x45, 0x43 ,   // Z
      0x00, 0x7F, 0x41, 0x41, 0x00 ,   // [
      0x55, 0x2A, 0x55, 0x2A, 0x55 ,   // checker pattern
      0x00, 0x41, 0x41, 0x7F, 0x00 ,   // ]
      0x04, 0x02, 0x01, 0x02, 0x04 ,   // ^
      0x40, 0x40, 0x40, 0x40, 0x40 ,   // _
      0x00, 0x01, 0x02, 0x04, 0x00 ,   // '
      0x20, 0x54, 0x54, 0x54, 0x78 ,   // a
      0x7F, 0x48, 0x44, 0x44, 0x38 ,   // b
      0x38, 0x44, 0x44, 0x44, 0x20 ,   // c
      0x38, 0x44, 0x44, 0x48, 0x7F ,   // d
      0x38, 0x54, 0x54, 0x54, 0x18 ,   // e
      0x08, 0x7E, 0x09, 0x01, 0x02 ,   // f
      0x0C, 0x52, 0x52, 0x52, 0x3E ,   // g
      0x7F, 0x08, 0x04, 0x04, 0x78 ,   // h
      0x00, 0x44, 0x7D, 0x40, 0x00 ,   // i
      0x20, 0x40, 0x44, 0x3D, 0x00 ,   // j
      0x7F, 0x10, 0x28, 0x44, 0x00 ,   // k
      0x00, 0x41, 0x7F, 0x40, 0x00 ,   // l
      0x7C, 0x04, 0x18, 0x04, 0x78 ,   // m
      0x7C, 0x08, 0x04, 0x04, 0x78 ,   // n
      0x38, 0x44, 0x44, 0x44, 0x38 ,   // o
      0x7C, 0x14, 0x14, 0x14, 0x08 ,   // p
      0x08, 0x14, 0x14, 0x18, 0x7C ,   // q
      0x7C, 0x08, 0x04, 0x04, 0x08 ,   // r
      0x48, 0x54, 0x54, 0x54, 0x20 ,   // s
      0x04, 0x3F, 0x44, 0x40, 0x20 ,   // t
      0x3C, 0x40, 0x40, 0x20, 0x7C ,   // u
      0x1C, 0x20, 0x40, 0x20, 0x1C ,   // v
      0x3C, 0x40, 0x30, 0x40, 0x3C ,   // w
      0x44, 0x28, 0x10, 0x28, 0x44 ,   // x
      0x0C, 0x50, 0x50, 0x50, 0x3C ,   // y
      0x44, 0x64, 0x54, 0x4C, 0x44 ,   // z
      0x00, 0x06, 0x09, 0x09, 0x06     // Degree symbol
};

prog_char string_0[] PROGMEM =    "CONECTION";  
prog_char string_1[] PROGMEM =    "Analog";  
prog_char string_2[] PROGMEM =    "Digital";  
prog_char string_3[] PROGMEM =    "I2C";  
prog_char string_4[] PROGMEM =    "CALIBRATION";  
prog_char string_5[] PROGMEM =    "EC";  
prog_char string_6[] PROGMEM =    "pH"; 
prog_char string_7[] PROGMEM =    "1";  
prog_char string_8[] PROGMEM =    "2";  
prog_char string_9[] PROGMEM =    "3";  
prog_char string_10[] PROGMEM =   "4";  
prog_char string_11[] PROGMEM =   "DELAY"; 
prog_char string_12[] PROGMEM =   "SET TIME";  //13
prog_char string_13[] PROGMEM =   "DHT11";  //14
prog_char string_14[] PROGMEM =   "DHT22";  //15
prog_char string_15[] PROGMEM =   "EC";
prog_char string_16[] PROGMEM =   "Ultra sonic"; //17
prog_char string_17[] PROGMEM =   "Soil Digit"; //17
prog_char string_18[] PROGMEM =   "pH"; //17
prog_char string_19[] PROGMEM =   "CO2"; //17
prog_char string_20[] PROGMEM =   "DS1307"; //17
prog_char string_21[] PROGMEM =   "light"; //17
prog_char string_22[] PROGMEM =   "reading..."; //47
prog_char string_23[] PROGMEM =   "MAIN MENU"; //17
prog_char string_24[] PROGMEM =   "ROK"; //17
prog_char string_25[] PROGMEM =   "MESIC"; //17
prog_char string_26[] PROGMEM =   "DEN"; //17
prog_char string_27[] PROGMEM =   "HODINA"; //17
prog_char string_28[] PROGMEM =   "MINUTA"; //17
prog_char string_29[] PROGMEM =   "PIN";
prog_char string_30[] PROGMEM =   "N/A";
prog_char string_31[] PROGMEM =   "Done...";
prog_char string_32[] PROGMEM =   "LOW";  
prog_char string_33[] PROGMEM =   "HIGH";
prog_char string_34[] PROGMEM =   "STANDART";
prog_char string_35[] PROGMEM =   "GET";

PROGMEM const char *MainMenuTable[] = {
  string_23, // "MAIN MENU"
  string_0, // "CONECTION"
  string_4, // "CALIBRATION"
  string_11, // "DELAY"         
  string_12 // "SET TIME"
};

PROGMEM const char *ConectMenuTable[] = {
  string_0, // "CONECTION"
  string_1, // "Analog"
  string_2, // "Digital"
  string_3 // "I2C"
};

PROGMEM const char *AnalogMenuTable[] = {
 string_1, // "Analog"
 string_30, // N/A
 string_6, // pH
 string_19, // CO2
 string_17 //soil moisture
};

PROGMEM const char *DigitalMenuTable[] = {
 string_2, //"Digital"
 string_30, // "N/A"
 string_13, // "DHT11"
 string_14, // "DHT22"
 string_15, // "EC"
 string_16, // "distance"
 string_17, // "soil moisture"
};

PROGMEM const char *I2CMenuTable[] = {
 string_3, /// "I2C"
 string_30, // "N/A"
 string_21, // "DS1307"
 string_20, // "light"
};


PROGMEM const char *PinConectMenuTable[] = {
 string_29, // "PIN"
 string_7, //  "1"
 string_8, // "2"
 string_9, // "3"
 string_10 //  "4"
};

PROGMEM const char *SetTimeMenuTable[] = {
 string_12, // "SET TIME"
 string_24, // "ROK"
 string_25, // "MESIC"
 string_26, // "DEN"
 string_27, // "HODINA"
 string_28 // "MINUTA"
};

PROGMEM const char *AnyStringTable[]={
 string_4, // "CALIBRATION"
 string_11, // "DELAY"
 string_31, // "Done..."
 string_32, // "LOW"
 string_33, // "HIGH"
 string_34, // "STANDART"
 string_35 // "GET"
};

uint8_t digitalPINS[4][2]={
{DIGpin1_A, DIGpin1_B},
{DIGpin2_A, DIGpin2_B},
{DIGpin3_A, DIGpin3_B},
{DIGpin4_A, DIGpin4_B}
};

uint8_t analogPINS[3]={ANpin1,ANpin2,ANpin3};

int BH1750address = 0x23;

///////////////////////////////VALUE BUFFERS/////////////////////////////////////
char A1val[9];
char A2val[9];
char A3val[9];

char D1val[9];
char D2val[9];
char D3val[9];
char D4val[9];
char I2Cval[9];
/////////////////////////////////////////////////////////////////////////////////
char tmp[5]; // Buffer for dtostrf
////////////////////////////TYPE ARRAYS//////////////////////////////////////////
/*Values in this arrays defined type of sensor connected on concrete pin. Array[1]-corespond to 1st pin, so 0 position is not used*/
/*Analog: 1 - nothing
          2 - pH
          3 - CO2
          4 - Soil Humidity
 Digital: 1 - nothing
          2 - DHT11
          3 - DHT22
          4 - EC
          5 - SR04 (Ulrasound distance sensor) 
          6 - Soil Humidity
 I2C:     1 - nothing
          2 - BH1750 (light intesity)
          3 - DS1307 (RTC)  */

byte analogTypeArray[4];
byte digitalTypeArray[5];
byte I2CTypeArray[2];
/////////////////////////////////////////////////////////////////////////////////

unsigned long delayTime; //Time before the main loop run again

volatile boolean outputFormat; //0 LCD; 1 SD card
volatile boolean showStatus; //0 MainMenu; 1 Display values

volatile unsigned long bounceTime=0;

boolean returning; 
uint8_t level;
uint8_t path[5];
byte whichkey;
uint8_t offset;

uint8_t delayReadable;
uint8_t delayType;

unsigned char cursor_row; /* 0-5 */
unsigned char cursor_col; /* 0-83 */

boolean firstRun;

SdFat sd; 
SdFile file;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////SETUP///////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup()                    // run once, when the sketch starts
{
outputFormat=checkStatus();
/* Read values stored in EEPROM*/
EEPROM_readAnything(170, analogTypeArray);
EEPROM_readAnything(180, digitalTypeArray);
EEPROM_readAnything(190, I2CTypeArray);
EEPROM_readAnything(200, delayTime);

if(I2CTypeArray[1]==3){  //Check if RTC is set to be connected
setSyncProvider(RTC.get); 
if(timeStatus()!= timeSet) RTC.set(now()); // Get RTC run if it is not running
}

assignDelay(); //classifed delat and assign as s/min/hr

if(outputFormat){
  /*SD card initialization*/
pinMode(SD_CS, OUTPUT);
sd.begin(SD_CS, SPI_HALF_SPEED);
}else{
  /*LCD initialization*/
  LCDinit();
  attachInterrupt(0,show, RISING); // Interrupt for Show/Hide menu button
  }

Wire.begin();

delay(1000);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////LOOP////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop(){
firstRun=false; //set firsRun false after get there after returnig from Menu

/*Call of sensor reading function getValue(Value Buffer, PIN, Type of sensor)*/ 
getAnalogValue(&A1val[0], 0, analogTypeArray[1]);
getAnalogValue(&A2val[0], 1, analogTypeArray[2]);
getAnalogValue(&A3val[0], 2, analogTypeArray[3]);

getDigitalValue(&D1val[0], 0, digitalTypeArray[1]);
getDigitalValue(&D2val[0], 1, digitalTypeArray[2]);
getDigitalValue(&D3val[0], 2, digitalTypeArray[3]);
getDigitalValue(&D4val[0], 3, digitalTypeArray[4]);

getI2CValue(&I2Cval[0], SEpin_A, I2CTypeArray[1]);

if(!outputFormat){
if(showStatus) MainMenu();
writeVal();
}
else{
 /*this will append readed values to BENCH.TXT file on SD card*/
 if(file.open("DATA.TXT", O_CREAT | O_APPEND | O_RDWR)){
 char space='\t';
 file.print(hour());
 file.print(':');
 file.print(minute());  
 file.print(space);
 file.print(D1val);
 file.print(space);
 file.print(D2val);
 file.print(space);
 file.print(D3val);
 file.print(space);
 file.print(D4val);
 file.print(space);
 file.print(A1val);
 file.print(space);
 file.print(A2val);
 file.print(space);
 file.print(A3val);
 file.print(space);
 file.println(I2Cval);
 file.sync();
 file.close();
}

/*
ofstream sdout("APPEND.TXT", ios::out | ios::app);
sdout << "line ";
sdout.close();
*/
}
/*sequenced delay - so you wont wait longer then 1 sec if you want to show menu */
for(int i=0; i<(delayTime/1000); i++){
if(firstRun) break;
delay(1000);
if(!outputFormat && showStatus) MainMenu();
}

} 
///////////////////////////////////////LOOP END//////////////////////////////////////////////////////////////////////////////////////

void writeVal(){
/*Print value buffers on LCD*/
 clear();
 gotoXY( 0,0 );
 writeString(A3val);
 gotoXY( 0,1 );
 writeString(A2val);
 gotoXY( 0,2 );
 writeString(A1val);
 gotoXY( 0,3 );
 writeString(I2Cval);
 gotoXY( 0,4 );
  
 gotoXY( RIGHT_COLUM,0 );
 writeString(D4val);
 gotoXY( RIGHT_COLUM,1 );
 writeString(D3val);
 gotoXY( RIGHT_COLUM,2 );
 writeString(D2val);
 gotoXY( RIGHT_COLUM,3 );
 writeString(D1val);

/*Print delay*/ 
PrintLCD_P(3, 0, 4,MainMenuTable);
printAtLCD(delayReadable,38,4);
gotoXY( 50,4 );
writeChar(getDelayChar()); 

/*Print time*/ 
gotoXY( 0,5 );
if(hour()<10){
writeChar('0');  
printNumber(hour());
} else printNumber(hour());
gotoXY( 13,5 );
writeChar(':'); 
gotoXY( 17,5 );
if(minute()<10){
writeChar('0');  
printNumber(minute());
}else printNumber(minute());

gotoXY( 43,5 );
printNumber(day());
writeChar('.');
printNumber(month());
writeChar('.');
/*
gotoXY( 50,4 );
printNumber(year()-(year()/1000)*1000);       
*/
}

/*function called after Show/Hide menu button interrupt*/
void show(){
if (abs(millis() - bounceTime) > BOUNCE_DURATION)  
{
showStatus=true;
clear(); 
bounceTime = millis();  // set whatever bounce time in ms is appropriate
 }
}

char val[]="N/A";

void getDigitalValue(char *where, uint8_t digitalPin, byte type){
switch(type){
case 1: strcpy(where, val); break;
case 2: readDHT(where, digitalPin, DHT11); break;
case 3: readDHT(where, digitalPin, DHT22); break;
case 4: readEC(where, digitalPin); break;
case 5: readSR04(where, digitalPin);break;
case 6: readSoilHumDig(where, digitalPin);break;
}
}

void getAnalogValue(char *where, uint8_t pin, byte type){
switch(type){
case 1: strcpy(where, val);  break;
case 2: readPH(where, pin); break;
case 3: readCO2(where, pin); break;
case 4: readSoilHumidityAnal(where, pin); break;
}
}

void getI2CValue(char *where, uint8_t pin, byte type){
switch(type){
case 1: strcpy(where, val);  break;
case 2: BH1750read(where, BH1750address); break; 
case 3: strcpy(where, "time");  break; 
}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////LCD MENU//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*Display done... on LCD and wait for 500ms*/
void done(){
clear();
PrintLCD_P(2, 0 , 0,  AnyStringTable ); 
returning=true;
delay(500);
};

void MainMenu(){
detachInterrupt(0); 
level=0;
do{
switch(makeMenu(MainMenuTable, MainMenuLength,1 )){
case 0: clear(); attachInterrupt(0,show, RISING); showStatus=false; break;
case 1: ConectMenu(); break;
case 2: CalibMenu(); break;
case 3: DelayMenu(); break;
case 4: SetTimeMenu(); break;
}
returning=false;
}while(showStatus);
firstRun=true;
}

void ConectMenu(){
level=1;
offset=makeMenu(ConectMenuTable, ConectMenuLength,1 );
if(offset==0) MainMenu();
else{
path[level]=offset;
PinConectMenu();
}
};

void PinConectMenu(){
level=2;
uint8_t PinConectMenuLength;
switch(path[1]){
case 1: PinConectMenuLength=4;break;
case 2: PinConectMenuLength=5;break;
case 3: PinConectMenuLength=2;break;
}
offset=makeMenu(PinConectMenuTable, PinConectMenuLength,1);
if(offset==0) ConectMenu();
else{
path[level]=offset;
TypeMenu();
}
};

void TypeMenu(){
level=3;
switch(path[1]){
case 1: offset=makeMenu(AnalogMenuTable ,AnalogMenuLength, analogTypeArray[path[2]]); break;
case 2: offset=makeMenu(DigitalMenuTable ,DigitalMenuLength, digitalTypeArray[path[2]]); break;
case 3: offset=makeMenu(I2CMenuTable, I2CMenuLength, I2CTypeArray[path[2]]); break;
}
if(offset==0) ConectMenu();
else{
path[level]=offset;
Assign();
done();
}
};

void Assign(){
switch(path[1]){
case 1: analogTypeArray[path[2]]=path[3]; EEPROM_writeAnything(170,analogTypeArray); break;  
case 2: digitalTypeArray[path[2]]=path[3];EEPROM_writeAnything(180, digitalTypeArray); break;
case 3: I2CTypeArray[path[2]]=path[3]; EEPROM_writeAnything(190, I2CTypeArray);
if(I2CTypeArray[1]==3){ setSyncProvider(RTC.get); 
if(timeStatus()!= timeSet) RTC.set(now());
}
break;
}
};

void CalibMenu() {
  uint8_t i=0;
  uint8_t type;
  uint8_t maxIndex=(sizeof(analogTypeArray)+sizeof(digitalTypeArray));
  PrintLCD_P(0, 0, 0, AnyStringTable);
  do{
    i++;
    }while(!(clibrateableInTypeArray(i)|| i==maxIndex));
          type=checkAndPrintType(i);
          if (type==0) {
          clear();
          printAtLCD("nothing to calibrate",4,0);
         delay(3000);
         returning=true; 
        }
  while (showStatus && !returning) {
    PrintLCD_P(0, 0, 0, AnyStringTable); 
    whichkey = PollKey();
    switch(whichkey) {
       case KeyDown:
          do{
          i--;
          if (i==0) i=maxIndex;
          }while(!clibrateableInTypeArray(i));
          type=checkAndPrintType(i);
          
          break;
       case KeyUp:
         do{
          i++;
          if (i==maxIndex) i=0;
          }while(!clibrateableInTypeArray(i));
           type=checkAndPrintType(i);
          break;
       case KeySel:
          if(type==PHTYPE)CalibFirstStep(i-1, type);
          else CalibFirstStep(i-sizeof(analogTypeArray)-1, type);
          break;
      case KeyBack:
      returning=true;
      break;
      }
   } 
};

uint8_t checkAndPrintType(uint8_t i){
  clear();  
  if( i<sizeof(analogTypeArray) && analogTypeArray[i]==PHINDEX) {
     printAtLCD("pH",0,2);
     printNumber(i);
     return PHTYPE;
          }
          else if(digitalTypeArray[(i-sizeof(analogTypeArray))]==ECINDEX && i-sizeof(analogTypeArray)>0){
                printAtLCD("EC",0,2);
                printNumber((i-sizeof(analogTypeArray)));
                return ECTYPE;
          } 
          else {
          return 0;
          }
};

boolean clibrateableInTypeArray(uint8_t index){
return (((index-sizeof(analogTypeArray))>0 && digitalTypeArray[(index-sizeof(analogTypeArray))]==ECINDEX) || (index<sizeof(analogTypeArray) && analogTypeArray[index]==PHINDEX));
}
 
void CalibFirstStep(uint8_t pin, char type) {
float lowStandart;
  if(type==ECTYPE) lowStandart=1.4;
  else lowStandart=4;
  do {
    //delay(1000);
    clear();
    PrintLCD_P(0,0,0, AnyStringTable); //
    PrintLCD_P(3,0,1, AnyStringTable); //
   printAtLCD(dtostrf(lowStandart,1,1,tmp),0,2);
    whichkey = PollKey();
    switch(whichkey) {
       case KeyDown:lowStandart-=CALSTEP;break;
       case KeyUp:lowStandart+=CALSTEP;break;
       case KeySel:CalibSecondStep(pin, type, lowStandart);break;
       case KeyBack:CalibMenu();break;
    }
  } while (showStatus && !returning);
};

void CalibSecondStep(uint8_t pin, char type, float lowStandart) {
 float highStandart;
  if(type==ECTYPE) highStandart=4.2;
  else highStandart=7;
  do {
    //delay(1000);
   clear();
    PrintLCD_P(0,0,0, AnyStringTable); //????????????????????????????????
    PrintLCD_P(4,0,1, AnyStringTable); //"      Main Menu     "
   printAtLCD(dtostrf(highStandart,1,1,tmp),0,2);
  
    whichkey = PollKey();
    switch(whichkey) {
       case KeyDown:highStandart-=CALSTEP;break;
       case KeyUp:highStandart+=CALSTEP;break;
       case KeySel:CalibThirdStep(pin, type, lowStandart, highStandart);break;
       case KeyBack:CalibMenu();break;
    }
   
  } while (showStatus && !returning);
};

void CalibThirdStep(uint8_t pin, char type, float lowStandart, float highStandart ) {
 clearLine(1);
  PrintLCD_P(0,0,0, AnyStringTable); 
  PrintLCD_P(6,0,1, AnyStringTable); 
  PrintLCD_P(3,20,1, AnyStringTable); 
  unsigned long lowValue;
    do {
    if(type==ECTYPE) lowValue=getFrequency(digitalPINS[pin][0],digitalPINS[pin][1]);
    else lowValue=PHclear(pin);
  // clear();
 clearLine(2);
  printAtLCD(lowValue,0,2);
  whichkey = PollKey();

} while (whichkey!=KeySel);
   clear();
 delay(1500);
 CalibFourthStep(pin, type, lowStandart, highStandart, lowValue);
};

void CalibFourthStep(uint8_t pin, char type, float lowStandart, float highStandart, unsigned long  lowValue) {
  PrintLCD_P(0,0,0, AnyStringTable); 
  PrintLCD_P(6,0,1, AnyStringTable); 
  PrintLCD_P(4,20,1, AnyStringTable); 
  unsigned long highValue;
  do {
    if(type==ECTYPE) highValue=getFrequency(digitalPINS[pin][0],digitalPINS[pin][1]);
    else highValue=PHclear(pin);
    clearLine(2);
    printAtLCD(highValue,0,2);
    whichkey = PollKey();     
  } while (whichkey!=KeySel);
 if(type==ECTYPE){
 setECConstants(pin, ((highValue-lowValue)/(highStandart-lowStandart)),(highStandart-(highValue/((highValue-lowValue)/(highStandart-lowStandart)))));
 
 } else setPHConstants(pin, ((highValue-lowValue)/(highStandart-lowStandart)),(highStandart-(highValue/((highValue-lowValue)/(highStandart-lowStandart)))));
done();
};

void DelayMenu(){
   clear();
   PrintLCD_P(1,0,0, AnyStringTable); 
   uint8_t DownLimit=1;
   uint8_t upLimit=58; 
   uint8_t newDelay;
   newDelay=delayReadable;

   do {
    clearLine(2);
    printAtLCD(newDelay,0,2);
    gotoXY( 25,2 );
    writeChar(getDelayChar()); 
    whichkey = PollKey();
    switch(whichkey) {
       case KeyDown:
         if (newDelay < DownLimit){
           newDelay=upLimit;
           if(!(delayType==0)) delayType-=1;
         } 
          else newDelay--;
          break;
       case KeyUp:
          if (newDelay > upLimit){
           newDelay=DownLimit;
           
           if(!(delayType==2)) delayType+=1;
          }
          else newDelay++;
          break;
       case KeySel:
     
       switch(delayType){
       case 0: delayTime=newDelay*1000;break;
       case 1: delayTime=newDelay*MICROSINMINUTE;break;
       case 2: delayTime=newDelay*MICROSINHOUR; break;
       }
      assignDelay(); 
      EEPROM_writeAnything(200, delayTime);
          done();
          break;
       case KeyBack:
       MainMenu();
       break;
    }
   
  } while (showStatus && !returning);
};

void SetTimeMenu(){
level=1;
offset=(makeMenu(SetTimeMenuTable,SetTimeMenuLength,1));
if(offset==0) returning=true;
else{
path[level]=offset;
SubTimeMenu();
}
};

void SubTimeMenu() {
 tmElements_t tm;
 breakTime(now(), tm); 
   int  time;
   int  upLimit;
   int  DownLimit;
  switch (path[1]){
  case 1:
  time=tmYearToCalendar(tm.Year);
  upLimit=2099;
  DownLimit=2000;break;
  case 2:
  time=tm.Month;
  upLimit=12;
  DownLimit=1;break;
  case 3:
  time=tm.Day;
  upLimit=31;
  DownLimit=1;break;
  case 4:
  time=tm.Hour;
  upLimit=23;
  DownLimit=0;break;
  case 5:
  time=tm.Minute;
  upLimit=59;
  DownLimit=0;break;
  }
  do {
    //delay(1000);
   /* lcd().clear();*/
    clearLine(5);
    //PrintLCD_P(path[1] ,0,0 ,SetTimeMenuTable ); 
    printAtLCD(time,0,5);
    whichkey = PollKey();
    switch(whichkey) {
       case KeyDown:
         if (time < DownLimit+1)time=upLimit;
          else time--;
          break;
       case KeyUp:
          if (time > upLimit-1) time=DownLimit;
          else time++;
          break;
       case KeySel:
       switch(path[1]){
      case 1:  tm.Year=CalendarYrToTm(time); break;
      case 2:   tm.Month=time; break; 
      case 3:   tm.Day=time; break; 
      case 4:  tm.Hour=time; break;
      case 5: tm.Minute=time; break;
       }   
          setTime(makeTime(tm)); 
          if(I2CTypeArray[1]==3) RTC.write(tm);
         done();
         returning=false;
         SetTimeMenu();
          break;
    case KeyBack:SetTimeMenu();break;
    }
   
  } while (showStatus && !returning);
  
};

int makeMenu(const char *itemTable[], uint8_t length, uint8_t presetOffset){
uint8_t innerOffset=presetOffset;
  do {
    clear(); 
    PrintLCD_P(0, 0, 0, itemTable);
  //  printAtLCD("pin", 35, 0);
// lcd().print(path[2]); //"      Main Menu     ", itemTable[offset]??????????????????????  to tu ma byt
    //Serial.println(offset);
    
    PrintLCD_P(innerOffset, 0, 2, itemTable);
    
    whichkey = PollKey();
    switch(whichkey) {
       case KeyDown:
         if (innerOffset < 2) innerOffset= length-1;
          else innerOffset--;
          break;
       case KeyUp:
          if (innerOffset > length-2) innerOffset=1;
          else innerOffset++;
          break;
       case KeySel:
          return innerOffset;
          break;
       case KeyBack:
          return 0;
          break;
    }
   
  } while (showStatus);
};

///////////////////////////////////////////////LCD MENU END///////////////////////////////////////////////////////////////////////////

void setECConstants(uint8_t which, float a, float b){
EEPROM_writeAnything(which*8+24,  a);
EEPROM_writeAnything(which*8+28,  b);
}

void setPHConstants(uint8_t which, float a, float b)
{
EEPROM_writeAnything(which*8, a);
EEPROM_writeAnything(which*8+4, b);
}

////////////////////////BUTTON HANDLE FUNCTIONS//////////////////////////////////
char KeyScan() {
   int which, which2, diff,retVal;
   which = analogRead(Buttons);
   ////Serial.println(which);
   delay(15);
   which2 = analogRead(Buttons);
   retVal = KeyInv;
   
   if(digitalRead(MenuHideButton)) retVal=KeyBack;
   else{
     diff = abs(which - which2);
      if (diff < 12) {
      if (which > 850 && which < 1024) retVal =  KeySel;
      if (which > 250  && which < 410) retVal =  KeyDown;
      if (which > 430 && which < 570) retVal =  KeyUp;
   }
   }
  ////Serial.println(retVal);
   return retVal;
}

//repeatedly calls KeyScan until a valid key press occurs
char PollKey() {
  char Whichkey;
    do {
     Whichkey = KeyScan();
     delay(20);
  } while ((Whichkey==KeyInv)&&showStatus);
  delay(200);
  return Whichkey;
}

////////////////////////////LCD FUNCTOINS////////////////////////////////////////
void PrintLCD_P(int which, char x, char y,  const char *menuName[]) {
   char buffer[MAXPROGMEM_WORDLENGTH];
   strcpy_P(buffer, (char*)pgm_read_word(&(menuName[which])));
  printAtLCD(buffer, x, y);
 
}

void printAtLCD(char *inStr, uint8_t x, uint8_t y){
gotoXY(x,y);
writeString(inStr);
};

void printAtLCD(const int inInt, uint8_t x, uint8_t y){
gotoXY(x,y);
printNumber(inInt);
};


void LCDinit(){
    DDRB |= (1<<LCD_DC)|(1<<LCD_RST)|(1<<SPI_MOSI)|(1<<SPI_SCK)|(1<<SPI_SS);
    DDRD |= (1<<LCD_BL);
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
    PORTD |= (1<<LCD_BL);  	// turn on backlight

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

void clear() {
gotoXY (0,0);  	//start with (0,0) home position
for(int i=0; i< LCDROWMAX; i++) {
clearLine(i);	
}  
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

void gotoXY(unsigned char x, unsigned char y) {
      if (x > LCDCOLMAX - 1) x = LCDCOLMAX - 1 ; // ensure within limits
      if (y > LCDROWMAX - 1) y = LCDROWMAX - 1 ; // ensure within limits

	writeCommand (0x80 | x);   //column
	writeCommand (0x40 | y);   //row

	cursor_row = y;
	cursor_col = x;
}

void writeString(char *string){
     while (*string)
	 writeChar(*string++);
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

////////////////////////////SENSOR READING FUNCTIONS/////////////////////////////
void readDHT(char* where, uint8_t digitalPin, uint8_t type){
DHT dht(digitalPINS[digitalPin][1], type);
dht.begin();
strcpy(where, dtostrf(dht.readTemperature(),1,1,tmp));
strcat(where,"/");
strcat(where, dtostrf(dht.readHumidity(),1,0,tmp));
}

void readEC(char *where, uint8_t digitalPin){
float constantA;
float constantB;
EEPROM_readAnything(digitalPin*8+24, constantA); 
EEPROM_readAnything(digitalPin*8+28, constantB);
strcpy(where,"EC:");
float ECf;
ECf=getFrequency(digitalPINS[digitalPin][0],digitalPINS[digitalPin][1]);
if(ECf) ECf=(ECf/constantA)+constantB;
//ECf=(getFrequency(digitalPINS[digitalPin][0],digitalPINS[digitalPin][1])/constantA)+constantB;
if(ECf<0 || ECf>20) strcat(where,"bad");
else strcat(where,dtostrf(ECf,1,1,tmp));

}


unsigned long getFrequency(uint8_t enable_pin, uint8_t pin) {

pinMode(enable_pin, OUTPUT);
pinMode(pin, INPUT);

unsigned long freqhigh = 0;
unsigned long freqlow =0;
unsigned long frequency=0;
digitalWrite(enable_pin, HIGH);
if(pulseIn(pin, HIGH, 3000)){
for(unsigned int j=0; j<FREQSAMPLES; j++){
freqhigh+= pulseIn(pin, HIGH);
freqlow+= pulseIn(pin, LOW);
}
frequency=100000/((freqhigh + freqlow)/ FREQSAMPLES);
}
digitalWrite(enable_pin, LOW);

return frequency;
}

void readPH(char *where, uint8_t pin){
float constantA;
float constantB;
float pH;
EEPROM_readAnything(pin*8, constantA); 
EEPROM_readAnything(pin*8+4, constantB);
strcpy(where,"pH:");
pH=(PHclear(pin)/constantA)+constantB;
if(pH>14 || pH<0) strcat(where, "cal");
else strcat(where,dtostrf(pH,1,1,tmp));
}

void readDS18B20(char *where, uint8_t pin){
OneWire ds(pin); 
int HighByte, LowByte, TReading, SignBit, Tc_100, Whole, Fract;
  byte i;
  byte present = 0;
  byte data[12];
  byte addr[8];
    if ( !ds.search(addr)) {
   //   Serial.print("No more addresses.\n");
      ds.reset_search();
      return;
  }
  
  ds.reset();
  ds.select(addr);
  ds.write(0x44,1);         // start conversion, with parasite power on at the end

  delay(1000);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.

  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad


  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
  }
    
  LowByte = data[0];
  HighByte = data[1];
  TReading = (HighByte << 8) + LowByte;
  SignBit = TReading & 0x8000;  // test most sig bit
  if (SignBit) // negative
  {
    TReading = (TReading ^ 0xffff) + 1; // 2's comp
  }
  Tc_100 = (6 * TReading) + TReading / 4;    // multiply by (100 * 0.0625) or 6.25

  Whole = Tc_100 / 100;  // separate off the whole and fractional portions
  Fract = Tc_100 % 100;


  if (SignBit) // If its negative
  {
    // Serial.print("-");
  }
  //Serial.print(Whole);
  //Serial.print(".");
  if (Fract < 10)
  {
    // Serial.print("0");
  }
  //Serial.print(Fract);

  
  
  
}


float PHclear(uint8_t pin){
float sensorValue=0;
for(int i=0;i<PH_SAMPLES;i++){
  sensorValue+=getVoltage(pin);
  delay(10);
  }
  sensorValue/=PH_SAMPLES;
  return sensorValue;
}

int getVoltage(uint8_t pin){
return analogRead(analogPINS[pin]);
}

void readSR04(char *where, uint8_t digitalPin){
pinMode(digitalPINS[digitalPin][0],INPUT);
pinMode(digitalPINS[digitalPin][1],OUTPUT);
NewPing sonar(digitalPINS[digitalPin][1], digitalPINS[digitalPin][0], MAX_DISTANCE); //
unsigned int uS = sonar.ping();
strcpy(where,dtostrf((uS / US_ROUNDTRIP_CM),1,0,tmp));
strcat(where," cm");
}


void readSoilHumDig(char *where, uint8_t digitalPin){
  pinMode(digitalPINS[digitalPin][0],INPUT);
 // pinMode(digitalPINS[digitalPin][1],OUTPUT);
//  digitalWrite(digitalPINS[digitalPin][1],HIGH);

if (digitalRead(digitalPINS[digitalPin][0])) strcpy(where,"DRY");
else strcpy(where,"WET");
//digitalWrite(digitalPINS[digitalPin][1],LOW);
}

void readCO2(char *where, uint8_t pin){
float value=((double)getVoltage(pin)*5000)/1024;
strcpy(where,dtostrf(value,1,0,tmp));
}

void readSoilHumidityAnal(char *where, uint8_t pin){
strcpy(where,dtostrf(getVoltage(pin),1,0,tmp));
}

void BH1750read(char *where, int address){
int i=0;
  byte buff[2];
  int val=0;
BH1750_Init(address);
 
  Wire.beginTransmission(address);
  Wire.requestFrom(address, 2);
  while(Wire.available()) //
  {
    buff[i] = Wire.read();  // receive one byte
    i++;
  }
  Wire.endTransmission();  
 if(i==2)
  {
    val=((buff[0]<<8)|buff[1])/1.2;
   }
PString(where, 9, val);
strcat(where," lx");
}
 
void BH1750_Init(uint8_t address)
{
  Wire.beginTransmission(address);
  Wire.write(0x10);//1lx reolution 120ms
  Wire.endTransmission();
}

//////////////////////SUPPORT FUNCTIONS//////////////////////////////////////////
boolean checkStatus(){
return digitalRead(5);
}

char getDelayChar(){
switch(delayType){
case 0:return 's';
case 1:return 'm';
case 2:return 'h';
}
}

void assignDelay(){
    if(!(delayTime%MICROSINHOUR)) {
   delayReadable=delayTime/MICROSINHOUR;
   delayType=2;
   }
   else {if (!(delayTime%MICROSINMINUTE)) {
            delayReadable=delayTime/MICROSINMINUTE;
             delayType=1;
       }
   else {
     delayReadable=delayTime/1000;
             delayType=0;
           }
         }  
}


