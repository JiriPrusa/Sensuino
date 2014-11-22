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


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////SERIAL COMUNICATION/////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
#define TIME_MSG_LEN 11 

void sendAnalogType(){
Serial << 'T';
Serial << 'A';
for(int i=1; i<4; i++){
Serial << analogTypeArray[i] << endLine;
}
Serial << I2CTypeArray[1] << endLine;;
}

void sendDigitalType(){
Serial << 'T';
Serial << 'D';
for(int i=1; i<5; i++){
Serial << digitalTypeArray[i] << endLine;
}
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

/*Incoming new Time*/
if((char)inByte=='B'){
delay(500);
int c;
while(Serial.available()<TIME_MSG_LEN-1){
delay(50);
}
pctime = 0;
for(int i=0; i < TIME_MSG_LEN-1; i++){  //TIME_MSG_LEN -1
c = Serial.read();

if( c >= 0 && c <= 9){ 
pctime = (10 * pctime) + (c) ;// convert digits to a number  
  }
}  
setTime(pctime);
}

/*Incoming new delay*/
if((char)inByte=='D'){
setDelayTime(Serial.parseInt());
sendDelay();
}

/*Hand shake*/
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

/*Incoming retyping*/
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

/*Print frequency readings from pin on serial*/
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

/*Print voltage readings from pin on serial*/
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


