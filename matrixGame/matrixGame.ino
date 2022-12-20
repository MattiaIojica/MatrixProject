#include <LiquidCrystal.h>
#include "Menu.h"


void setup() {

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // pinMode(buzzerPin, OUTPUT);

  // Print a welcome message to the LCD.
  welcomeMenu();

  // Read the values saved in EEPROM
  readValuesFromEEPROM();


  // Display the Menu on the LCD
  updateLCD();

  Serial.begin(9600);
}


void loop() {
  //calls the function behind the game logic
  game();
}



 