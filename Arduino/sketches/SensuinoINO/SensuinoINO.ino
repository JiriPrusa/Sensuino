//strtok(), strcmp(), and atoi() 

#include <DS1307RTC.h> 
#include <EEPROM.h>
#include <Time.h>  
#include <Wire.h>
#include <SdFat.h>
#include "EEPROMAnything.h"
#include <OneWire.h>
//#include <PString.h>
#include <Streaming.h>

#define MAXTIMINGS 85
#define DHT11 11
#define DHT22 22

#define MAX_SENSOR_DISTANCE 500 // Maximum sensor distance can be as high as 500cm, no reason to wait for ping longer than sound takes to travel this distance and back.
#define US_ROUNDTRIP_IN 146     // Microseconds (uS) it takes sound to travel round-trip 1 inch (2 inches total), uses integer to save compiled code space.
#define US_ROUNDTRIP_CM 57      // Microseconds (uS) it takes sound to travel round-trip 1cm (2cm total), uses integer to save compiled code space.

// Probably shoudln't change these values unless you really know what you're doing.
#define NO_ECHO 0               // Value returned if there's no ping echo within the specified MAX_SENSOR_DISTANCE or max_cm_distance.
#define MAX_SENSOR_DELAY 18000  // Maximum uS it takes for sensor to start the ping (SRF06 is the highest measured, just under 18ms).
#define ECHO_TIMER_FREQ 24      // Frequency to check for a ping echo (every 24uS is about 0.4cm accuracy).
#define PING_MEDIAN_DELAY 29    // Millisecond delay between pings in the ping_median method.

// Conversion from uS to distance (round result to nearest cm or inch).
#define NewPingConvert(echoTime, conversionFactor) (max((echoTime + conversionFactor / 2) / conversionFactor, (echoTime ? 1 : 0)))

#define TIME_MSG_LEN  11 

#define MENU_NORMAL	0
#define MENU_HIGHLIGHT 1
#define PIXEL_OFF 0
#define PIXEL_ON  1
#define PIXEL_XOR 2
#define SPI_MOSI PORTB3
#define SPI_SCK PORTB5
#define LCD_DC  PORTB2

#define SPI_CS  PORTB1   //old versio B0
#define SPI_CS_PORT  PORTB
#define SPI_CS_DDR  DDRB
#define LCD_RST PORTB4
#define SPI_SS  PORTB1   //old version B0
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
#define Buttons A3
#define SD_CS 8  //old version
#define CE_PIN A3  
#define IO_PIN  A4    
#define SCLK_PIN A5
/////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////PINS///////////////////////////////////////////
#define ANpin1 A0
#define ANpin2 A1
#define ANpin3 A2
#define SEpin_A A4
#define SEpin_B A5
#define DIGpin1_A 0
#define DIGpin1_B 1
#define DIGpin2_A 2
#define DIGpin2_B 5
#define DIGpin3_A 3
#define DIGpin3_B 4
#define DIGpin4_A 6
#define DIGpin4_B 7
/////////////////////////////////////////////////////////////////////////////////
#define KeyUp    30
#define KeyDown  40
#define KeySel   50
#define KeyBack  60
#define KeyInv   70 //invalid key value

#define AnalogMenuLength 4
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
      0x00, 0x06, 0x09, 0x09, 0x06 ,  // Degree symbol
      0x7C, 0x42, 0x41, 0x41, 0x7F // SD card symbol 
      // 0x03, 0x03, 0x7E, 0x42, 0x66 // Degree of celsius
      // 0xEE, 0x26, 0x40, 0x2E, 0xE6  //ppm
       
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
prog_char string_17[] PROGMEM =   "Water Temp"; //17
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
// string_17 //soil moisture
};

PROGMEM const char *DigitalMenuTable[] = {
 string_2, //"Digital"
 string_30, // "N/A"
 string_13, // "DHT11"
 string_14, // "DHT22"
 string_15, // "EC"
 string_16, // "distance"
 string_17 // "water temp"
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

boolean showStatus; //0 MainMenu; 1 Display values

boolean returning; 
uint8_t level;
uint8_t path[5];
byte whichkey;
uint8_t offset;

uint8_t delayReadable;
uint8_t delayType;

char endLine='\n';

unsigned char cursor_row; /* 0-5 */
unsigned char cursor_col; /* 0-83 */

boolean firstRun;
boolean SDwrite;
boolean serialStatus;
boolean conected;

SdFat sd; 
SdFile file;

time_t pctime;

//ping library
uint8_t _triggerBit;
uint8_t _echoBit;
volatile uint8_t *_triggerOutput;
volatile uint8_t *_triggerMode;
volatile uint8_t *_echoInput;
unsigned int _maxEchoTime;
unsigned long _max_time;

//DHT library
uint8_t data[6];
uint8_t _type, _count;



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////SETUP///////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup()                    // run once, when the sketch starts
{

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

///pinMode(SD_CS, OUTPUT); //8 bytes in this :]

LCDinit();

//Wire.begin();

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


if(!serialStatus){
  if(sd.begin(SD_CS, SPI_HALF_SPEED)){
    if(file.open("DATA.TXT", O_CREAT | O_APPEND | O_RDWR)){
    char space='\t';
    //if(file.print(hour()))
    file << now() << space << D2val << space << D3val << space << D4val << space << A1val << space << A2val << space << A3val << space << I2Cval << endLine ;
    SDwrite=true;
   // file.sync();
    file.close();
    }
  }else SDwrite=false;
}else{
   if(conected) sendValues();
}


writeVal();

for(int i=0; i<(delayTime/1000); i++){
if(firstRun) break;
delay(1000);

if((KeyScan()==KeyBack)&&!conected)
{
  showStatus=true;
  MainMenu();
}

if(KeyScan()==KeySel)
{
  startSerial();
}

}

} 
///////////////////////////////////////LOOP END//////////////////////////////////////////////////////////////////////////////////////
void startSerial(){
  serialStatus=!serialStatus;
gotoXY( 43,5 ); 
  if(serialStatus){
Serial.begin(9600);
writeChar('S');
}else{
Serial.end();
conected=false;
writeChar(' ');
}
  
while(KeyScan()==KeySel){
delay(100);
}

}

void sendValues(){
Serial.write('V');
//Serial.println(D1val);
Serial << D2val << endLine << D3val << endLine << D4val << endLine << A1val << endLine << A2val << endLine << A3val << endLine << I2Cval << endLine;
sendTime();
}

void writeVal(){
/*Print value buffers on LCD*/
 clear();
 printAtLCD(A3val, 0,0 );

 printAtLCD(A2val, 0,1 );

 printAtLCD(A1val, 0,2 );

 printAtLCD(I2Cval, 0,3 );
 
 printAtLCD(D4val, RIGHT_COLUM,0 ); 

 printAtLCD(D3val, RIGHT_COLUM,1 ); 
 
 printAtLCD(D2val, RIGHT_COLUM,2 ); 
 
 printAtLCD(D1val, RIGHT_COLUM,3 ); 


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
if(serialStatus){
writeChar('S');
} else{
if(SDwrite) writeChar(124);
else writeChar(' ');
}


/*
printNumber(day());
writeChar('.');
printNumber(month());
writeChar('.');

gotoXY( 50,4 );
printNumber(year()-(year()/1000)*1000);       
*/
}

char val[]="N/A";

void getDigitalValue(char *where, uint8_t digitalPin, byte type){
switch(type){
case 1: strcpy(where, val); break;
case 2: readDHT(where, digitalPin, DHT11); break;
case 3: readDHT(where, digitalPin, DHT22); break;
case 4: readEC(where, digitalPin); break;
case 5: readSR04(where, digitalPin);break;
case 6: readDS18B20(where, digitalPINS[digitalPin][0]); break;
//case 6: readSoilHumDig(where, digitalPin);break;
}
}

void getAnalogValue(char *where, uint8_t pin, byte type){
switch(type){
case 1: strcpy(where, val);  break;
case 2: readPH(where, pin); break;
case 3: readCO2(where, pin); break;
//case 4: readSoilHumidityAnal(where, pin); break;
}
}

void getI2CValue(char *where, uint8_t pin, byte type){
switch(type){
case 1: strcpy(where, val);  break;
case 2: BH1750read(where,  0x23); break; 
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

//boolean backButtonReleased;
void MainMenu(){
clear();
while(KeyScan()==KeyBack){
delay(100);
}
level=0;
do{
switch(makeMenu(MainMenuTable, MainMenuLength,1 )){

case 0: clear(); showStatus=false; break;
case 1: ConectMenu(); break;
case 2: CalibMenu(); break;
case 3: DelayMenu(); break;
case 4: SetTimeMenu(); break;
}
returning=false;
}while(showStatus);
//triggered when leaving menu
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
           
           if(!(delayType==1)) delayType+=1;
          }
          else newDelay++;
          break;
       case KeySel:
     
       switch(delayType){
       case 0: delayTime=newDelay*1000;break;
       case 1: delayTime=newDelay*MICROSINMINUTE;break;
      // case 2: delayTime=newDelay*MICROSINHOUR; break;
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

void setDelayTime(unsigned long newDelay){
delayTime=newDelay;
assignDelay(); 
EEPROM_writeAnything(200, delayTime);
}
////////////////////////BUTTON HANDLE FUNCTIONS//////////////////////////////////
char KeyScan() {
   int which, which2, diff,retVal;
   which = analogRead(Buttons);
   delay(15);
   which2 = analogRead(Buttons);
   retVal = KeyInv;
     
     diff = abs(which - which2);
      if (diff < 12) {
      if (which > 10 && which < 300) retVal =  KeyUp;
      if (which > 300  && which < 600) retVal =  KeyDown;
      if (which > 600 && which < 900) retVal =  KeySel;
      if (which > 900 && which < 1030) retVal =  KeyBack;
   }

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
//      if (x > LCDCOLMAX - 1) x = LCDCOLMAX - 1 ; // ensure within limits
//      if (y > LCDROWMAX - 1) y = LCDROWMAX - 1 ; // ensure within limits

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
initDHT(digitalPINS[digitalPin][1],digitalPINS[digitalPin][0] , type);
if (read()) {
strcpy(where, dtostrf(readTemperature(),1,1,tmp));
strcat(where,"/");
strcat(where, dtostrf(readHumidity(),1,0,tmp));
}else strcpy(where, "0/0");

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

initPins(enable_pin, pin );
unsigned long freqhigh = 0;
unsigned long freqlow =0;
unsigned long frequency=0;

_maxEchoTime=3000;
*_triggerMode |= _triggerBit; // set trigger pin as output
*_triggerOutput |= _triggerBit; // Set trigger pin high

//if(highDuration()>_maxEchoTime){
for(unsigned int j=0; j<FREQSAMPLES; j++){
freqhigh+= highDuration();
freqlow+= lowDuration();
}
frequency=100000/((freqhigh + freqlow)/ FREQSAMPLES);
//} else frequency=0;
*_triggerOutput &= ~_triggerBit; // Set the trigger pin low

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
 /*  if ( !ds.search(addr)) {
   //   Serial.print("No more addresses.\n");
      ds.reset_search();
      return;
  }
  
  
  ds.reset();
  ds.select(addr);
  ds.write(0x44,1);         // start conversion, with parasite power on at the end

  delay(1000);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.

   present =ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad

*/
  //Reset, skip ROM and start temperature conversion
  
  ds.reset();
  ds.write(0xcc); //SKIPROM
  ds.write(0x44); //CONVERTTEMP
  //Wait until conversion is complete
  delay(1000); //  while(!ds.read_bit());
  //Reset, skip ROM and send command to read Scratchpad
  ds.reset();
  ds.write(0xcc); //SKIPROM
  ds.write(0xBE); //RSCRATCHPAD

  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
  }

  byte MSB = data[1];
  byte LSB = data[0];

  float tempRead = ((MSB << 8) | LSB); //using two's compliment
 tempRead/=16;
 strcpy(where,dtostrf(tempRead,1,1,tmp));
  
 
  /*  
  LowByte = data[0];
  HighByte = data[1];
  TReading = (HighByte << 8) + LowByte;
  SignBit = TReading & 0x8000;  // test most sig bit
 // if (SignBit) // negative
 // {
//    TReading = (TReading ^ 0xffff) + 1; // 2's comp
 // }
  Tc_100 = (6 * TReading) + TReading / 4;    // multiply by (100 * 0.0625) or 6.25

  Whole = Tc_100 / 100;  // separate off the whole and fractional portions
  Fract = Tc_100 % 100;


//  if (SignBit) // If its negative
//  {
//    // Serial.print("-");
//    strcpy(where,"-");
//  }
   strcpy(where,dtostrf(Whole,1,0,tmp));
   strcat(where,".");
  //Serial.print(Whole);
  //Serial.print(".");
  if (Fract < 10)
  {
   strcat(where,"0"); // Serial.print("0");
  }
  strcat(where,dtostrf(Fract,1,0,tmp));
  //Serial.print(Fract);
*/
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
//pinMode(digitalPINS[digitalPin][0],INPUT);
//pinMode(digitalPINS[digitalPin][1],OUTPUT);
//NewPing sonar(digitalPINS[digitalPin][1], digitalPINS[digitalPin][0], MAX_DISTANCE); //
initPing(digitalPINS[digitalPin][1], digitalPINS[digitalPin][0], MAX_DISTANCE);
unsigned int uS = ping();
strcpy(where,dtostrf((uS / US_ROUNDTRIP_CM),1,0,tmp));
strcat(where," cm");
}


void readCO2(char *where, uint8_t pin){
float value=((double)getVoltage(pin)*5000)/1024;
strcpy(where,dtostrf(value,1,0,tmp));
strcat(where,"ppm");
}



void BH1750read(char *where, int address){
int i=0;
  byte buff[2];
  int val=0;
//BH1750_Init(address);
 Wire.beginTransmission(address);
  Wire.write(0x10);//1lx reolution 120ms
//  Wire.endTransmission();

 
//  Wire.beginTransmission(address);
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
//itoa(val,where, 10);
strcpy(where, dtostrf(val,1,0,tmp));
strcat(where," lx");
}

//////////////////////SUPPORT FUNCTIONS//////////////////////////////////////////

char getDelayChar(){
switch(delayType){
case 0:return 's';
case 1:return 'm';
//case 2:return 'h';
}
}

void assignDelay(){
  
   if (!(delayTime%MICROSINMINUTE)) {
            delayReadable=delayTime/MICROSINMINUTE;
             delayType=1;
       }
   else {
     delayReadable=delayTime/1000;
             delayType=0;
           }
           
}


	
		//boolean ping_wait_timer();
		
		//static void timer_setup();
		//static void timer_ms_cntdwn();

void initPing(uint8_t trigger_pin, uint8_t echo_pin, int max_cm_distance) {
	initPins(trigger_pin, echo_pin);

	_maxEchoTime = min(max_cm_distance, MAX_SENSOR_DISTANCE) * US_ROUNDTRIP_CM + (US_ROUNDTRIP_CM / 2); // Calculate the maximum distance in uS.


	*_triggerMode |= _triggerBit; // Set trigger pin to output.

}

void initPins (uint8_t trigger_pin, uint8_t echo_pin){
        _triggerBit = digitalPinToBitMask(trigger_pin); // Get the port register bitmask for the trigger pin.
	_echoBit = digitalPinToBitMask(echo_pin);       // Get the port register bitmask for the echo pin.

	_triggerOutput = portOutputRegister(digitalPinToPort(trigger_pin)); // Get the output port register for the trigger pin.
	_echoInput = portInputRegister(digitalPinToPort(echo_pin));         // Get the input port register for the echo pin.

	_triggerMode = (uint8_t *) portModeRegister(digitalPinToPort(trigger_pin)); // Get the port mode register for the trigger pin.
}

//Calculate duration of HIGH pulse
unsigned int ping() {
        *_triggerOutput &= ~_triggerBit; // Set the trigger pin low, should already be low, but this will make sure it is.
	delayMicroseconds(4);            // Wait for pin to go low, testing shows it needs 4uS to work every time.
	*_triggerOutput |= _triggerBit;  // Set trigger pin high, this tells the sensor to send out a ping.
	delayMicroseconds(10);           // Wait long enough for the sensor to realize the trigger pin is high. Sensor specs say to wait 10uS.
	*_triggerOutput &= ~_triggerBit; // Set trigger pin back to low. 
	return highDuration();
}

unsigned int highDuration(){
 if (!highStarted()) return NO_ECHO;                // Trigger a ping, if it returns false, return NO_ECHO to the calling function.
while (*_echoInput & _echoBit)                      // Wait for the ping echo.
		if (micros() > _max_time) return NO_ECHO;       // Stop the loop and return NO_ECHO (false) if we're beyond the set maximum distance.
return (micros() - (_max_time - _maxEchoTime) - 5); // Calculate ping time, 5uS of overhead.
}

unsigned int lowDuration(){
   if (!lowStarted()) return NO_ECHO;   
while (!(*_echoInput & _echoBit))                      // Wait for the ping echo.
		if (micros() > _max_time) return NO_ECHO;       // Stop the loop and return NO_ECHO (false) if we're beyond the set maximum distance.
return (micros() - (_max_time - _maxEchoTime) - 5); // Calculate ping time, 5uS of overhead.
}


boolean highStarted() {
	_max_time =  micros() + MAX_SENSOR_DELAY;                  // Set a timeout for the ping to trigger.
	while (*_echoInput & _echoBit && micros() <= _max_time) {} // Wait for echo pin to clear.
	while (!(*_echoInput & _echoBit))                          // Wait for ping to start.
		if (micros() > _max_time) return false;                // Something went wrong, abort.
	_max_time = micros() + _maxEchoTime; // Ping started, set the timeout.
	return true;                         // Ping started successfully.
}

boolean lowStarted() {
	_max_time =  micros() + MAX_SENSOR_DELAY;                  // Set a timeout for the ping to trigger.
	while (!(*_echoInput & _echoBit) && micros() <= _max_time) {} // Wait for echo pin to clear.
	while (*_echoInput & _echoBit)                          // Wait for ping to start.
		if (micros() > _max_time) return false;                // Something went wrong, abort.
	_max_time = micros() + _maxEchoTime; // Ping started, set the timeout.
	return true;                         // Ping started successfully.
}


void initDHT(uint8_t trigPin, uint8_t echoPin, uint8_t type) {
initPins(trigPin,echoPin);
  _type = type;
  _count = 15;

}
/*
void beginDHT() {
  // set up the pins!
  pinMode(_pin, INPUT);
  digitalWrite(_pin, HIGH);

}
*/

float readTemperature() {
  float f;

 // if (read()) {
    switch (_type) {
    case DHT11:
      f = data[2];
    return f;
    case DHT22:
      f = data[2] & 0x7F;
      f *= 256;
      f += data[3];
      f /= 10;
      if (data[2] & 0x80)
      f *= -1;

      return f;
    }
 // }

  return NAN;
}

float readHumidity() {
  float f;
 // if (read()) {
    switch (_type) {
    case DHT11:
      f = data[0];
      return f;
    case DHT22:

      f = data[0];
      f *= 256;
      f += data[1];
      f /= 10;
      return f;
    }
  //}
  return NAN;
}

boolean read() {
  uint8_t laststate = _triggerBit;
  uint8_t counter = 0;
  uint8_t j = 0, i;
 
  *_triggerMode |= _triggerBit;
  // pull the pin high and wait 250 milliseconds
  *_triggerOutput |= _triggerBit;  // Set trigger pin high //digitalWrite(_pin, HIGH);
  delay(250);

  data[0] = data[1] = data[2] = data[3] = data[4] = 0;
  
  // now pull it low for ~20 milliseconds
  //pinMode(_pin, OUTPUT);
  *_triggerOutput &= ~_triggerBit; // Set the trigger pin low //digitalWrite(_pin, LOW);
  delay(20);
  cli();
  *_triggerOutput |= _triggerBit;  // Set trigger pin high  //digitalWrite(_pin, HIGH);
  delayMicroseconds(40);
  *_triggerMode &= ~_triggerBit; //pinMode(_pin, INPUT);

  // read in timings
  for ( i=0; i< MAXTIMINGS; i++) {
    counter = 0;
    while ((*_echoInput & _triggerBit) == laststate) {
      counter++;
      delayMicroseconds(1);
      if (counter == 255) {
        break;
      }
    }
   

    laststate=(*_echoInput & _triggerBit);
  
  
    if (counter == 255) break;

    // ignore first 3 transitions
    if ((i >= 4) && (i%2 == 0)) {
      // shove each bit into the storage bytes
      data[j/8] <<= 1;
      if (counter > _count)
        data[j/8] |= 1;
      j++;
    }

  }

  sei();
  
  // check we read 40 bits and that the checksum matches
  if ((j >= 40) &&
      (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) ) {
    return true;
  }
  

  return false;

}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////SERIAL COMUNICATION/////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void sendAnalogType(){
Serial << 'T';
Serial << 'A';
for(int i=1; i<4; i++){
Serial << analogTypeArray[i] << endLine;
}
Serial << I2CTypeArray[1] << endLine;;
//Serial.write('Z');
}

void sendDigitalType(){
Serial << 'T';
Serial << 'D';
for(int i=1; i<5; i++){
Serial << digitalTypeArray[i] << endLine;
}
//Serial.write('Z');
}

float constantA;
float constantB;

void sendPHConst(){
Serial << 'C';
Serial << 'P';
for(int i=0; i<3; i++){
EEPROM_readAnything(i*8, constantA); 
EEPROM_readAnything(i*8+4, constantB);
Serial << constantA << endLine;
Serial << constantB << endLine;
}
}

void sendECConst(){
Serial << 'C';
Serial << 'E';
for(int i=1; i<4; i++){
EEPROM_readAnything(i*8+24, constantA); 
EEPROM_readAnything(i*8+28, constantB);
Serial << constantA << endLine;
Serial << constantB << endLine;
}
}

void sendTime(){
Serial << 'A';  
Serial << now() << endLine;
}

void sendDelay(){
Serial << 'D';  
Serial << delayTime << endLine;
}

void serialEvent(){

while (Serial.available() > 0){
char inByte=Serial.read();

/* 
if((char)inByte=='A'){
sendTime();
}
*/


if((char)inByte=='B'){
delay(500);
int c;
while(Serial.available()<TIME_MSG_LEN-1){
delay(50);
//Serial.println(Serial.available());
}
pctime = 0;
for(int i=0; i < TIME_MSG_LEN-1; i++){  //TIME_MSG_LEN -1
c = Serial.read();

if( c >= 0 && c <= 9){ 
pctime = (10 * pctime) + (c) ;// convert digits to a number  
  }
}  
setTime(pctime);
//Serial.println(pctime);
}

if((char)inByte=='D'){
//unsigned long cislo=Serial.parseInt();
//Serial.write('H'); 
//Serial.println(cislo);
setDelayTime(Serial.parseInt());
sendDelay();
}

if((char)inByte=='Y'){
Serial << 'Y';
conected=true;
sendAnalogType();
sendDigitalType();
sendPHConst();
sendECConst();
sendDelay();
}

if((char)inByte=='X'){
conected=false;
}


if((char)inByte=='T'){
while(Serial.available() < 3){}
char director=(char)Serial.read();
  if(director=='A'){  
  analogTypeArray[Serial.read()]=Serial.read(); 
  sendAnalogType();
  EEPROM_writeAnything(170, analogTypeArray);
}
if(director=='D'){
digitalTypeArray[Serial.read()]=Serial.read();
sendDigitalType();  
EEPROM_writeAnything(180, digitalTypeArray);
}
if(director=='I'){
I2CTypeArray[Serial.read()]=Serial.read(); 
sendAnalogType();
EEPROM_writeAnything(190, I2CTypeArray);
}  
}


if((char)inByte=='M'){
delay(10);
int pin=Serial.read();  
clear();
PrintLCD_P(2, 10, 3, MainMenuTable);
do
{
Serial << 'M' << pin << getFrequency(digitalPINS[pin][0],digitalPINS[pin][1]) << endLine;
delay(100);
 
}while(!((char)Serial.read()=='W'));
 
}

if((char)inByte=='N'){
delay(10);
int pin=Serial.read();  
clear();
PrintLCD_P(2, 10, 3, MainMenuTable);
do
{
Serial << 'N' << pin << PHclear(pin) << endLine;
delay(100);
 
}while(!((char)Serial.read()=='W'));
 
}

/*Incoming new EC constants*/
if((char)inByte=='U'){
delay(30);
setECConstants(Serial.read(),Serial.parseFloat(),Serial.parseFloat());
sendECConst();
}

/*Incoming new pH constants*/
if((char)inByte=='V'){
delay(30);
setPHConstants(Serial.read(),Serial.parseFloat(),Serial.parseFloat());
sendPHConst();
}

}
}


