#include <EEPROM.h>
#include "EEPROMAnything.h"

byte analogTypeArray[]={0,1,1,1};
byte digitalTypeArray[]={0,1,1,1,1};
byte I2CTypeArray[]={0,1};

float PHconst[][2]= {{1,1},{1,1},{1,1}};
float ECconst[][2]={{1,1},{1,1},{1,1},{1,1}}; 

unsigned long delayTime=1000;

void setup() {
EEPROM_writeAnything(0, PHconst); 
EEPROM_writeAnything(24, ECconst); //up =24+(4*2)*4=56 next can begin on 56
EEPROM_writeAnything(170,analogTypeArray);
EEPROM_writeAnything(180, digitalTypeArray);
EEPROM_writeAnything(190, I2CTypeArray);
EEPROM_writeAnything(200, delayTime);

Serial.begin(9600);
Serial.println("Writing successfuly done!");
}

void loop() {

delay(500);
}

