#include <avr/interrupt.h>
#include <DS1307RTC.h> 
#include <DHT.h>
#include <EEPROM.h>
#include <Time.h>  
#include <NewPing.h>
#include <Wire.h>

#include "EEPROMAnything.h"
#include "avr_bmp.h"

#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters).
#define PH_SAMPLES 50
#define FREQSAMPLES 5

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

uint8_t digitalPINS[4][2]={
{DIGpin1_A, DIGpin1_B},
{DIGpin2_A, DIGpin2_B},
{DIGpin3_A, DIGpin3_B},
{DIGpin4_A, DIGpin4_B}
};

uint8_t analogPINS[3]={ANpin1,ANpin2,ANpin3};

int BH1750address = 0x23;

char A1val[5];
char A2val[5];
char A3val[5];

char D1val[5];
char D2val[5];
char D3val[5];
char D4val[5];
char I2Cval[5];

char tmp[5];

byte analogTypeArray[4];
byte digitalTypeArray[5];
byte I2CTypeArray[2];

float ECconst[4][2];
float PHconst[3][2];

int delayTime;

boolean conected;

void setup()                    // run once, when the sketch starts
{

if(I2CTypeArray[1]==3)setSyncProvider(RTC.get); 

EEPROM_readAnything(0, PHconst); 
EEPROM_readAnything(24, ECconst); //up to 56
EEPROM_readAnything(170, analogTypeArray);
EEPROM_readAnything(180, digitalTypeArray);
EEPROM_readAnything(190, I2CTypeArray);
EEPROM_readAnything(200, delayTime);


Wire.begin();
Serial.begin(9600);

delay(1000);
}

void loop(){
 
getAnalogValue(&A1val[0], 0, analogTypeArray[1]);
getAnalogValue(&A2val[0], 1, analogTypeArray[2]);
getAnalogValue(&A3val[0], 2, analogTypeArray[3]);

//getDigitalValue(&D1val[0], 0, digitalTypeArray[1]);
getDigitalValue(&D2val[0], 1, digitalTypeArray[2]);
getDigitalValue(&D3val[0], 2, digitalTypeArray[3]);
getDigitalValue(&D4val[0], 3, digitalTypeArray[4]);

getI2CValue(&I2Cval[0], SEpin_A, I2CTypeArray[1]);

if(conected) sendValues();

delay(5000); 
}

char val[]="N/A";

void getDigitalValue(char *where, uint8_t digitalPin, byte type){
switch(type){
case 0: strcpy(where, val); break;
case 1: readDHT(where, digitalPin, DHT11); break;
case 2: readDHT(where, digitalPin, DHT22); break;
case 3: readEC(where, digitalPin); break;
case 4: readSR04(where, digitalPin);break;
case 5: readSoilHumDig(where, digitalPin);break;
}
}

void getAnalogValue(char *where, uint8_t pin, byte type){
switch(type){
case 0: strcpy(where, val);  break;
case 1: readPH(where, pin); break;
case 2: readCO2(where, pin); break;
case 3: readSoilHumidityAnal(where, pin); break;
}
}

void getI2CValue(char *where, uint8_t pin, byte type){
switch(type){
case 0: strcpy(where, val);  break;
case 1: BH1750read(where, BH1750address); break; 
case 2: strcpy(where, "time");  break; // ???????????????????????????????????????????????????????????????
}
}



void setECConstants(uint8_t which, float a, float b)
{
EEPROM_writeAnything(which*8,  a);
EEPROM_writeAnything(which*8+4,  b);
}

void setPHConstants(uint8_t which, float a, float b)
{
EEPROM_writeAnything(which*8+24, a);
EEPROM_writeAnything(which*8+28, b);
}

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
strcpy(where,"EC ");
float ECf;
pinMode(digitalPINS[digitalPin][0], OUTPUT);
pinMode(digitalPINS[digitalPin][1], INPUT);
//ECf=(getFrequency(digitalPINS[digitalPin][0],digitalPINS[digitalPin][1])/ECconst[digitalPin][0])+ECconst[digitalPin][1];// prvni nuly ????????????????????????????????
ECf=(getFrequency(digitalPINS[digitalPin][0],digitalPINS[digitalPin][1])/constantA)+constantB;
if(ECf<0 || ECf>20) strcat(where,"NC");
else {
strcat(where,dtostrf(ECf,1,1,tmp));
}

}

long getFrequency(uint8_t enable_pin, uint8_t pin) {
//Serial.println(pin);
long freqhigh = 0;
long freqlow =0;
long frequency=0;
digitalWrite(enable_pin, HIGH);
if(pulseIn(pin, HIGH)){
for(unsigned int j=0; j<FREQSAMPLES; j++){
freqhigh+= pulseIn(pin, HIGH);
freqlow+= pulseIn(pin, LOW);
}
//frequency = 1000000 / ( (freqhigh / FREQSAMPLES) + (freqlow / FREQSAMPLES) );
frequency=(freqhigh / FREQSAMPLES) + (freqlow / FREQSAMPLES);
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
//pH=(PHclear(pin)/PHconst[pin][0])+PHconst[pin][1];
strcat(where,dtostrf(pH,1,1,tmp));
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
NewPing sonar(digitalPINS[digitalPin][1], digitalPINS[digitalPin][0], MAX_DISTANCE);
unsigned int uS = sonar.ping();
strcpy(where,dtostrf((uS / US_ROUNDTRIP_CM),1,0,tmp));
//strcat(where," cm");
}


void readSoilHumDig(char *where, uint8_t digitalPin){
if (digitalRead(digitalPINS[digitalPin][1])) strcpy(where,"s");
else strcpy(where,"p");
}

void readCO2(char *where, uint8_t pin){
strcpy(where,dtostrf(getVoltage(pin),1,1,tmp));
}

void readSoilHumidityAnal(char *where, uint8_t pin){
strcpy(where,dtostrf(getVoltage(pin),1,1,tmp));
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
sprintf(where,"%i", val);
//strcat(where," lx");
}
 
void BH1750_Init(uint8_t address)
{
  Wire.beginTransmission(address);
  Wire.write(0x10);//1lx reolution 120ms
  Wire.endTransmission();
}

boolean checkStatus(){
return digitalRead(5);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////SERIAL COMUNICATION/////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void sendAnalogType(){
Serial.write('T');
Serial.write('A');
for(int i=1; i<4; i++){
Serial.println(analogTypeArray[i]);
}
Serial.println(I2CTypeArray[1]);
Serial.write('Z');
}

void sendDigitalType(){
Serial.write('T');
Serial.write('D');
for(int i=1; i<5; i++){
Serial.println(digitalTypeArray[i]);
}
Serial.write('Z');
}

void sendPHConst(){
Serial.write('C');
Serial.write('P');
for(int i=0; i<3; i++){
Serial.println(PHconst[i][0]);
Serial.println(PHconst[i][1]);
}
}

void sendECConst(){
Serial.write('C');
Serial.write('E');
for(int i=0; i<4; i++){
Serial.println(ECconst[i][0]);
Serial.println(ECconst[i][1]);
}
}

void sendValues(){
Serial.write('V');
//Serial.println(D1val);
Serial.println(D2val);
Serial.println(D3val);
Serial.println(D4val);
Serial.println(A1val);
Serial.println(A2val);
Serial.println(A3val);
Serial.println(I2Cval);
}


void serialEvent(){

while (Serial.available() > 0){
char inByte=Serial.read();
 
/*
if((char)inByte=='B'){
delay(500);
int c;
while(Serial.available()<TIME_MSG_LEN-1){
delay(50);
Serial.println(Serial.available());
}
pctime = 0;
for(int i=0; i < TIME_MSG_LEN-1; i++){  //TIME_MSG_LEN -1
c = Serial.read();

if( c >= 0 && c <= 9){ 
pctime = (10 * pctime) + (c) ;// convert digits to a number  
  }
}  
setTime(pctime);
Serial.println(pctime);
}
*/
if((char)inByte=='Y'){
Serial.write('Y');
conected=true;
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




/*

if((char)inByte=='S'){
delay(10);
  if(Serial.read()=='A'){  
    switch(Serial.read()){
    case 1: Serial.print(A1val);break;
    case 2:Serial.print(A2val);break;
    case 3:Serial.print(A3val);break;
    case 4:Serial.print(I2Cval);break;
    }
}else{
    switch(Serial.read()){
    case 1: Serial.print(D1val);break;
    case 2:Serial.print(D2val);break;
    case 3:Serial.print(D3val);break;
    case 4:Serial.print(D4val);break;
    }
    }
}


if((char)inByte=='V'){
delay(10);
switch(Serial.read()){
case 1:ECa=Serial.parseFloat();break;
case 2:ECb=Serial.parseFloat();break;
case 3:PHa=Serial.parseFloat();break;
case 4:PHb=Serial.parseFloat();break;
}
}
if((char)inByte=='W'){
delay(10);
switch(Serial.read()){
case 1:sendECpure();break;
case 2:sendPHpure();break;
}
  
}
*/
 
}
}






