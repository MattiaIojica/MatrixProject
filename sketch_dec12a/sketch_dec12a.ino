#include <LiquidCrystal.h>
#include "Menu.h"


void setup() {

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // pinMode(buzzerPin, OUTPUT);

  // Print a message to the LCD.
  welcomeMenu();

  updateLCD();

  Serial.begin(9600);
}


void loop() {

  game();

}



 