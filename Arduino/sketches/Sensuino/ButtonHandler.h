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

//A part of code handling buttons. Most of the code is based on work of Collin80
//originaly published on Arduino forum: http://forum.arduino.cc/index.php/topic,41450.0.html

////////////////////////BUTTON HANDLE FUNCTIONS//////////////////////////////////
#define KeyUp    30
#define KeyDown  40
#define KeySel   50
#define KeyBack  60
#define KeyInv   70 //invalid key value

byte whichkey;

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

