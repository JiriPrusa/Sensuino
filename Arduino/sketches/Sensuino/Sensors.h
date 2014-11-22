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

//A part for handling sensors. The code contains parts of 
//DHT library written by Adafruit Industries
//NewPing library written by Tim Eckel


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

#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters).
#define PH_SAMPLES 100  // number of pH samples taken by one reading
#define FREQSAMPLES 500 //number of freq samples taken by one EC reading

#define MAXTIMINGS 85

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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////RAW SENSOR DATA MINERS///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////SUPPORT FUNCTIONS////////////////////////////////////////////////////////////////////
/*Pin initiation -used by: DHT, EC, Ping */
void initPins (uint8_t trigger_pin, uint8_t echo_pin){
        _triggerBit = digitalPinToBitMask(trigger_pin); // Get the port register bitmask for the trigger pin.
	_echoBit = digitalPinToBitMask(echo_pin);       // Get the port register bitmask for the echo pin.

	_triggerOutput = portOutputRegister(digitalPinToPort(trigger_pin)); // Get the output port register for the trigger pin.
	_echoInput = portInputRegister(digitalPinToPort(echo_pin));         // Get the input port register for the echo pin.

	_triggerMode = (uint8_t *) portModeRegister(digitalPinToPort(trigger_pin)); // Get the port mode register for the trigger pin.
}

/*Ping initialization*/
void initPing(uint8_t trigger_pin, uint8_t echo_pin, int max_cm_distance) {
	initPins(trigger_pin, echo_pin);

	_maxEchoTime = min(max_cm_distance, MAX_SENSOR_DISTANCE) * US_ROUNDTRIP_CM + (US_ROUNDTRIP_CM / 2); // Calculate the maximum distance in uS.


	*_triggerMode |= _triggerBit; // Set trigger pin to output.

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

unsigned int highDuration(){
 if (!highStarted()) return NO_ECHO;                // Trigger a ping, if it returns false, return NO_ECHO to the calling function.
while (*_echoInput & _echoBit)                      // Wait for the ping echo.
		if (micros() > _max_time) return NO_ECHO;       // Stop the loop and return NO_ECHO (false) if we're beyond the set maximum distance.
return (micros() - (_max_time - _maxEchoTime) - 5); // Calculate ping time, 5uS of overhead.
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

unsigned int lowDuration(){
   if (!lowStarted()) return NO_ECHO;   
while (!(*_echoInput & _echoBit))                      // Wait for the ping echo.
		if (micros() > _max_time) return NO_ECHO;       // Stop the loop and return NO_ECHO (false) if we're beyond the set maximum distance.
return (micros() - (_max_time - _maxEchoTime) - 5); // Calculate ping time, 5uS of overhead.
}
///////////////////////////////////////////////////////////////SUPPORT FUNCTIONS END///////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////DHT////////////////////////////////////////////////////////////////////////////
void initDHT(uint8_t trigPin, uint8_t echoPin, uint8_t type) {
initPins(trigPin,echoPin);
  _type = type;
  _count = 15;

}

/*Read temperature from DHT type sensor*/
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

/*Read humidity from DHT type sensor*/
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

////////////////////////////////////////////////////////////////////////////DHT END////////////////////////////////////////////////////////////////////////

int getVoltage(uint8_t pin){
return analogRead(analogPINS[pin]);
}


unsigned long getFrequency(uint8_t enable_pin, uint8_t pin) {
initPins(enable_pin, pin );
unsigned long freqhigh = 0;
unsigned long freqlow =0;
unsigned long frequency=0;
_maxEchoTime=3000;
*_triggerMode |= _triggerBit; // set enable pin as output
*_triggerOutput |= _triggerBit; // Set enable pin high
if(highDuration()<2000 && highDuration()>1){
for(unsigned int j=0; j<FREQSAMPLES; j++){
freqhigh+= highDuration();
freqlow+= lowDuration();
}
frequency=100000/((freqhigh + freqlow)/ FREQSAMPLES);
} else frequency=0;
*_triggerOutput &= ~_triggerBit; // Set the enable pin low
return frequency;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////RAW SENSOR DATA MINERS///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
if(ECf<0 || ECf>20) strcat(where,"bad");
else strcat(where,dtostrf(ECf,1,1,tmp));

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



void readSR04(char *where, uint8_t digitalPin){
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


