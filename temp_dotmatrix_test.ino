#include "LedControl.h"


// pin 12 (D6) is connected to the DataIn 
// pin 15 (D7) is connected to the CS
// pin 13 (D8) is connected to CLK
#define B1 5 /*(D1) Button 1 */
#define B2 4 /*(D2) Button 2 */

LedControl lc=LedControl(12,15,13,1);

unsigned long delaytime=1000;

void setup() {
  Serial.begin(9600);
  lc.shutdown(0,false);
  lc.setIntensity(0,8);
  lc.clearDisplay(0);
  pinMode(B1,INPUT_PULLUP);
}

void displayW() {
  byte W[8]={0x82,0x82,0x82,0x92,0x92,0xAA,0xC6,0x82};
  lc.setRow(0,0,W[0]);
  lc.setRow(0,1,W[1]);
  lc.setRow(0,2,W[2]);
  lc.setRow(0,3,W[3]);
  lc.setRow(0,4,W[4]);
  lc.setRow(0,5,W[5]);
  lc.setRow(0,6,W[6]);
  lc.setRow(0,7,W[7]);
}
void displayM() {
  byte M[8]={0x82,0xC6,0xAA,0x92,0x92,0x82,0x82,0x82};
  lc.setRow(0,0,M[0]);
  lc.setRow(0,1,M[1]);
  lc.setRow(0,2,M[2]);
  lc.setRow(0,3,M[3]);
  lc.setRow(0,4,M[4]);
  lc.setRow(0,5,M[5]);
  lc.setRow(0,6,M[6]);
  lc.setRow(0,7,M[7]);
}
void loop() {
  int b1State = digitalRead(B1);
  Serial.println(b1State);
  delay(1000);
}
