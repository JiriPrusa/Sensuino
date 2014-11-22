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
#define SD_CS 8 
#define Buttons A3
 
/////////////////////////////////////////////////////////////////////////////////
#define PHINDEX 2 //index of pH in menu used in  clibrateableInTypeArray(uint8_t) in Menu.h
#define ECINDEX 4 //index of EC in menu used in  clibrateableInTypeArray(uint8_t) in Menu.h

#define CALSTEP 0.1  //Step in calibration menu


#define MICROSINHOUR 1440000 //used for delay conversion
#define MICROSINMINUTE 60000 //used for delay conversion

#define DHT11 11
#define DHT22 22
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
uint8_t digitalPINS[4][2]={
{DIGpin1_A, DIGpin1_B},
{DIGpin2_A, DIGpin2_B},
{DIGpin3_A, DIGpin3_B},
{DIGpin4_A, DIGpin4_B}
};

uint8_t analogPINS[3]={ANpin1,ANpin2,ANpin3};

boolean showStatus; //0 MainMenu; 1 Display values

boolean conected;

uint8_t delayReadable;
uint8_t delayType;

char endLine='\n';
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
/////////////////////////////////////////////////////////////////////////////////
byte analogTypeArray[4];
byte I2CTypeArray[2];
byte digitalTypeArray[5];

time_t pctime;

unsigned long delayTime; //Time before the main loop run again

void startSerial();
void sendValues();
void getDigitalValue(char , uint8_t , byte );
void getAnalogValue(char, uint8_t , byte );
void getI2CValue(char , uint8_t , byte );
void setDelayTime(unsigned long);
unsigned long getFrequency(uint8_t, uint8_t);
float PHclear(uint8_t);
void setECConstants(uint8_t, float, float);
void setPHConstants(uint8_t, float, float);


char getDelayChar();
void assignDelay();



