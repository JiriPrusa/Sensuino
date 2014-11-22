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



#include <DS1307RTC.h> 
#include <EEPROM.h>
#include <Time.h>  
#include <Wire.h>
#include <SdFat.h>
#include <EEPROMAnything.h>
#include <OneWire.h>
#include <Streaming.h>

#include "Sensuino.h"
#include "LCD.h"
#include "Menu.h"
#include "Sensors.h"
#include "Serial.h"

#define RIGHT_COLUM 42
/////////////////////////////////////////////////////////////////////////////////
boolean SDwrite;
boolean serialStatus;

SdFat sd; 
SdFile file;

char val[]="N/A";
///////////////////////////////////////////////SETUP///////////////////////////////////////////////////////////////////////////////////
void setup() {                  // run once, when the sketch starts
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

Wire.begin();

delay(1000);
}

///////////////////////////////////////////////////LOOP////////////////////////////////////////////////////////////////////////////////
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

if(!serialStatus){ //if Serial Communication is OFF
  if(sd.begin(SD_CS, SPI_HALF_SPEED)){ //if SD card is IN
    if(file.open("data.log", O_CREAT | O_APPEND | O_RDWR)){
    char space='\t';
    //if(file.print(hour()))
    file << now() << space << D2val << space << D3val << space << D4val << space << A1val << space << A2val << space << A3val << space << I2Cval << endLine ;
    SDwrite=true;
   // file.sync();
    file.close();
    }
  }else SDwrite=false;
    }else if(conected) sendValues(); //If Serial has began and get "hand shake" from SensuinoApl
     
writeVal(); //print values on LCD

for(int i=0; i<(delayTime/1000); i++){
  if(firstRun) break; //check if just return from MENU
  delay(1000);
  if((KeyScan()==KeyBack)&&!conected){ //jump to MENU if KeyBack is hold
    showStatus=true;
    MainMenu();
    }

  if(KeyScan()==KeySel){ //Start or stop serial communication if KeySel is hold
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

void getDigitalValue(char *where, uint8_t digitalPin, byte type){
switch(type){
  case 1: strcpy(where, val); break;
  case 2: readDHT(where, digitalPin, DHT11); break;
  case 3: readDHT(where, digitalPin, DHT22); break;
  case 4: readEC(where, digitalPin); break;
  case 5: readSR04(where, digitalPin);break;
  case 6: readDS18B20(where, digitalPINS[digitalPin][0]); break;
    }
}

void getAnalogValue(char *where, uint8_t pin, byte type){
switch(type){
  case 1: strcpy(where, val);  break;
  case 2: readPH(where, pin); break;
  case 3: readCO2(where, pin); break;
    }
}

void getI2CValue(char *where, uint8_t pin, byte type){
switch(type){
  case 1: strcpy(where, val);  break;
  case 2: BH1750read(where,  0x23); break; 
  case 3: strcpy(where, "time");  break; 
    }
}

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

