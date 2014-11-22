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



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////LCD MENU//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "ButtonHandler.h"

#define AnalogMenuLength 4
#define DigitalMenuLength 7
#define MainMenuLength 5
#define ConectMenuLength 4
#define I2CMenuLength 4

#define PHTYPE 1
#define ECTYPE 2

#define MenuItemStructureLength 5
#define SetTimeMenuLength 6


char tmp[5]; // Buffer for dtostrf

boolean returning; 
uint8_t level;
uint8_t path[5];
uint8_t offset;
boolean firstRun;

void ConectMenu(), PinConectMenu(), TypeMenu();
void CalibMenu(), CalibFirstStep(uint8_t, char), CalibSecondStep(uint8_t, char , float), CalibThirdStep(uint8_t, char , float , float  ), CalibFourthStep(uint8_t , char , float , float , unsigned long  ); 
void DelayMenu();
void SetTimeMenu(), SubTimeMenu() ;  
/*Display done... on LCD and wait for 500ms*/
void done(){
clear();
PrintLCD_P(2, 0 , 0,  AnyStringTable ); 
returning=true;
delay(500);
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


int makeMenu(const char *itemTable[], uint8_t length, uint8_t presetOffset){
uint8_t innerOffset=presetOffset;
  do {
    clear(); 
    PrintLCD_P(0, 0, 0, itemTable);

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
};

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

boolean clibrateableInTypeArray(uint8_t index){
return (((index-sizeof(analogTypeArray))>0 && digitalTypeArray[(index-sizeof(analogTypeArray))]==ECINDEX) || (index<sizeof(analogTypeArray) && analogTypeArray[index]==PHINDEX));
}

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




 
void CalibFirstStep(uint8_t pin, char type) {
float lowStandart;
  if(type==ECTYPE) lowStandart=1.4;
  else lowStandart=4;
  do {

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

   clear();
    PrintLCD_P(0,0,0, AnyStringTable); 
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
   uint16_t newDelay;
   newDelay=delayReadable;

   do {
    clearLine(2);
    printAtLCD(newDelay,0,2);
    gotoXY( 25,2 );
    writeChar(getDelayChar()); 
    whichkey = PollKey();
    switch(whichkey) {
       case KeyDown:
         if (newDelay <= DownLimit){
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
       case 0: delayTime=newDelay*1000; break;
       case 1: delayTime=newDelay*MICROSINMINUTE; break;
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

    clearLine(5);

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




///////////////////////////////////////////////LCD MENU END///////////////////////////////////////////////////////////////////////////
