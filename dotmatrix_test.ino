#include "LedControl.h"

#define B1 5 /*(D1) Button 1 */
#define B2 4 /*(D2) Button 2 */
#define buzzer 16

LedControl lc = LedControl(12, 15, 13, 1);

unsigned long delaytime = 1000;

int rnd;
int roud;
int rounds;
int score;
int abrand;
int timer;

void setup() {
  Serial.begin(9600);
  lc.shutdown(0, false);
  lc.setIntensity(0, 8);
  lc.clearDisplay(0);
  pinMode(B1, INPUT_PULLUP);
  pinMode(B2, INPUT_PULLUP);
  randomSeed(analogRead(0));
  pinMode(buzzer, OUTPUT);
}

void displayW() {
  byte W[8] = {0x82, 0x82, 0x82, 0x92, 0x92, 0xAA, 0xC6, 0x82};
  lc.setRow(0, 0, W[0]);
  lc.setRow(0, 1, W[1]);
  lc.setRow(0, 2, W[2]);
  lc.setRow(0, 3, W[3]);
  lc.setRow(0, 4, W[4]);
  lc.setRow(0, 5, W[5]);
  lc.setRow(0, 6, W[6]);
  lc.setRow(0, 7, W[7]);
  delay(10);
}

void displayM() {
  byte M[8] = {0x82, 0xC6, 0xAA, 0x92, 0x92, 0x82, 0x82, 0x82};
  lc.setRow(0, 0, M[0]);
  lc.setRow(0, 1, M[1]);
  lc.setRow(0, 2, M[2]);
  lc.setRow(0, 3, M[3]);
  lc.setRow(0, 4, M[4]);
  lc.setRow(0, 5, M[5]);
  lc.setRow(0, 6, M[6]);
  lc.setRow(0, 7, M[7]);
  delay(10);
}

void game() {
    int b1State = digitalRead(B1);
  int b2State = digitalRead(B2);
  rnd = random(0, 2);
  Serial.println(rnd);
  
  if (rnd == 0) {
    displayM();
    // Wait for button input
    while (b1State == HIGH && b2State == HIGH) {
      b1State = digitalRead(B1);
      b2State = digitalRead(B2);
      delay(10);
    }

    if (b1State == LOW) {
      Serial.println("yippie");
      delay(50);
    } else if (b2State == LOW) {
      Serial.println("not yippie");
      delay(50);
    }
  }

  if (rnd == 1) {
    displayW();
    // Wait for button input
    while (b1State == HIGH && b2State == HIGH) {
      b1State = digitalRead(B1);
      b2State = digitalRead(B2);
      delay(10);
    }

    if (b1State == LOW) {
      Serial.println("not yippie");
      delay(50);
    }
    else if (b2State == LOW) {
      Serial.println("yippie");
      delay(50);
    }
  }
  
  while (b1State == LOW || b2State == LOW) {
    b1State = digitalRead(B1);
    b2State = digitalRead(B2);
    delay(10);
  }
}
void loop() {
  timer++;
  if (timer == 10){
  tone(buzzer, 780, 2000); // Send 1KHz sound signal...
    while(rounds !=9){
    game();
    rounds++;
    }
    timer = 0;
    rounds = 0;
    lc.clearDisplay(0);
  } else {
    delay(1000);
  }
  }
