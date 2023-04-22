#include "LedControl.h"

int randomnum;
bool worm;
int selection = 2;

LedControl lc=LedControl(12,11,10,1);

unsigned long d=1000;

void setup() {
  Serial.begin(9600);
  lc.shutdown(0,false);
  lc.setIntensity(0,8);
  lc.clearDisplay(0);
  randomSeed(analogRead(0));
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
}

void loop() {
  randomnum = random(0,1000);
  if (randomnum % 2 == 0) {
    worm = 1;
  }
  else {
    worm = 0;
  }
  // Serial.println(worm);
  if (worm == 1) {
    displayW();
  }
  else if (worm == 0) {
    displayM();
  }
  delay(d);
  if (digitalRead(2)== LOW){
    selection = 1;
  }
  else if (digitalRead(3)== LOW){
    selection = 0;
  }
  else {
    selection = 2;
  }
  Serial.println(selection);
}

void displayW() {
  lc.clearDisplay(0);
  byte W[8]={0x82,0x82,0x82,0x92,0x92,0xAA,0xC6,0x82};
  lc.setRow(0,0,W[0]);
  lc.setRow(0,1,W[1]);
  lc.setRow(0,2,W[2]);
  lc.setRow(0,3,W[3]);
  lc.setRow(0,4,W[4]);
  lc.setRow(0,5,W[5]);
  lc.setRow(0,6,W[6]);
  lc.setRow(0,7,W[7]);
  Serial.println("Displaying W");
}

void displayM() {
  lc.clearDisplay(0);
  byte M[8]={0x82,0xC6,0xAA,0x92,0x92,0x82,0x82,0x82};
  lc.setRow(0,0,M[0]);
  lc.setRow(0,1,M[1]);
  lc.setRow(0,2,M[2]);
  lc.setRow(0,3,M[3]);
  lc.setRow(0,4,M[4]);
  lc.setRow(0,5,M[5]);
  lc.setRow(0,6,M[6]);
  lc.setRow(0,7,M[7]);
  Serial.println("Displaying M");
}
