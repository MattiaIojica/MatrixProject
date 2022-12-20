#include "Arduino.h"
#include "Game.h"
#include "EEPROMMemory.h"
#include "Buzzer.h"

// LCD Variables
const byte RS = 9;
const byte enable = 8;
const byte d4 = 7;
const byte d5 = 6;
const byte d6 = 5;
const byte d7 = 4;

// the state of the game {0 - Main Menu, 1 - Into the section of the menu}
bool gameState = 0;

// Main menu
const int menuSize = 5;
const String menu[menuSize] = { "START GAME", "HIGHSCORE", "SETTINGS", "ABOUT", "HOW TO PLAY" };
int menuPosition = 0;

// Setting Menu
const int numSettingsItems = 4;
const String settingsItems[numSettingsItems] = {"DIFFICULTY", "MATR BRTNSS", "SOUNDS", "RESET HIGHSCORE"};

// Array for the Player Name
char letters[3] = {'A', 'B', 'C'};

// Constructor of the LCD
LiquidCrystal lcd(RS, enable, d4, d5, d6, d7);


//
bool displayMenuOnce = false;

// Variables for the Joystick's button
const int shortPressDuration = 50;
const int longPressDuration = 1500;
unsigned long lastDebounceTime = 0;
unsigned int debounceDelay = 50;
unsigned long pressedTime = 0;
unsigned long releasedTime = 0;

// sounds on / off
bool sounds = 1;

// The state of the switch
bool lastSwState = LOW;
byte switchState = HIGH;

// variable for the About Menu 
int textPosition = 0;
int lastTextPosition = -1;

// last Score of the game
int lastScore = -1;

// variable for the About Menu 
int topPlayer = 0;
int lastTopPlayer = -1;

// 
bool nameSelected = false;


void welcomeMenu();
void gameMenu();
void highScoreMenu();
void settingsMenu();
void aboutMenu();
void howToPlayMenu();
void buttonChangeState();
void updateLCD();
void GameOverMenu();
void WinMenu();
void reset();
void checkGameState();
void writeName();
void resetHIGHSCORE();
void readValuesFromEEPROM();

// Display the Welcome Message
void welcomeMenu() {
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Welcome to the");
  lcd.setCursor(3, 1);
  lcd.print("Snake game");

  delay(1200);

  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Get ready to");
  lcd.setCursor(6, 1);
  lcd.print("PLAY!");

  delay(1500);
}


// based on the menuPosition, calls the right function
void checkGameState()
{
  gameState = 1;
  switch(menuPosition)
  {
    case 0:
      if(!nameSelected)
      {
        writeName();
        nameSelected = true;
      }

      gameMenu();
      break;

    case 1:
      highScoreMenu();
      break;

    case 2:
      settingsMenu();
      break;

    case 3:
      aboutMenu();
      break;

    case 4:
      howToPlayMenu();
      break;

    default:
      break;
  }

}

// The movement in the Main Menu
void menuMovement(){
  int yValue = joystick.readX();

  if (yValue < joystick.minThreshold && !joystick.joyMovedY) {
    
    // Still in the main menu
    if(gameState == 0)
    {
      menuPosition = max(menuPosition - 1, 0);

      scrollSound();
    }
    else
    // In a Section of the Menu
    if(gameState == 1)
    {
      topPlayer = max(topPlayer - 1, 0);
      textPosition = max(textPosition - 1, 0);
    }

    joystick.joyMovedY = true;
    if(gameState == 0)
      updateLCD();
  }
  else
  if (yValue > joystick.maxThreshold && !joystick.joyMovedY) {
    // Still in the main menu
    if(gameState == 0){
      menuPosition = min(menuPosition + 1, menuSize - 1);

      scrollSound();
    }
    else
    // In a Section of the Menu// In a Section of the Menu
    if(gameState == 1)
    {
      textPosition = min(textPosition + 1, 1);
      topPlayer = min(topPlayer + 1, 4);
    }
    
    joystick.joyMovedY = true;

    // updates the LCD if in Main Menu
    if(gameState == 0)
      updateLCD();
  }
  else
  // if the joystick is neutral resets the joymoved variable
  if (yValue >= joystick.minThreshold && yValue <= joystick.maxThreshold) {
    joystick.joyMovedY = false;
  }
}


void gameMenu(){
  // Check if the game is over
  if(gameOver == true)
  {
    if(!displayMenuOnce){
      // Check if the player won
      if(snake->getLenght() == 64 - (obstacleSize * (difficulty % 2 + 1))){
        WinMenu();
      }
      else{
        GameOverMenu();
      }
    }

    return;
  }

  // Calls the Game Loop
  gameLoop();


  // Display on the LCD the time left
  lcd.setCursor(2, 1);
  lcd.print("TIME LEFT: ");
  lcd.print(timeToEat - (millis() - timeTillDeath) / 1000);


  // Updates the score only it has increased
  if(lastScore != score)
  {
    lcd.clear();
    lcd.setCursor(5, 0);
    lcd.print("SCORE:");
    lcd.print(score);
    
    lastScore = score;
  }
}


void aboutMenu(){
  // Updates only if a Joystick movement was made
  if(lastTextPosition == textPosition)
    return;
  
  lcd.clear();

  if (textPosition == 0) {
    lcd.setCursor(5, 0);
    lcd.print("ABOUT");
    lcd.setCursor(1, 1);
    lcd.print("Name: Dot Game");
  }
  else
  if (textPosition == 1) {
    lcd.setCursor(4, 0);
    lcd.print("Creator:");
    lcd.setCursor(1, 1);
    lcd.print("Iojica Mattia");
  }
  lastTextPosition = textPosition;
}

void settingsMenu(){
  
  int currentSetting = 0;

  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("SETTINGS");

  while (true) {
      lcd.setCursor((16 - settingsItems[currentSetting].length()) / 2, 1);
      lcd.print(settingsItems[currentSetting]);
      lcd.print(":");

      switch (currentSetting) {
          case 0:
          lcd.print(difficulty);
          break;

          case 1:
          lcd.print(matrixBrightness);
          break;

          case 2:
          lcd.print(sounds);
          break;

          default:
          break;
    }

      // Read the joystick position
      int yValue = joystick.readX();
      int xValue = joystick.readY();

      // Check if the joystick is being moved left or right
      if(xValue > joystick.maxThreshold&& !joystick.joyMovedY)
      {
        currentSetting = max(currentSetting - 1, 0);
        lcd.clear();
        lcd.setCursor(4, 0);
        lcd.print("SETTINGS");
        joystick.joyMovedY = true;
      }
      else
      if(xValue < joystick.minThreshold && !joystick.joyMovedY)
      {
        currentSetting = min(currentSetting + 1, numSettingsItems - 1);
        lcd.clear();
        lcd.setCursor(4, 0);
        lcd.print("SETTINGS");
        joystick.joyMovedY = true;
      }
      else
      if (xValue >= joystick.minThreshold && xValue <= joystick.maxThreshold) {
          joystick.joyMovedY = false;
      }
      
      
        // Check if the joystick is being moved up or down
        // updates the variables based on the current setting
        if (yValue > joystick.maxThreshold) {
          
          if(currentSetting == 0){
            difficulty = max(difficulty - 1, 1);
            updateDifficulty(difficulty);
          }
          else
            if(currentSetting == 1){
              matrixBrightness = max(matrixBrightness - 1, 1);
              updateBrightness(matrixBrightness);
            }
            else
            if(currentSetting == 2){
              sounds = 0;
            }

          delay(300); 
        } else if (yValue < joystick.minThreshold) {
            if(currentSetting == 0){
              difficulty = min(difficulty + 1, 2);
              updateDifficulty(difficulty);
            }
            else
            if(currentSetting == 1){
              matrixBrightness = min(matrixBrightness + 1, 15);
              updateBrightness(matrixBrightness);
            }
            else
            if(currentSetting == 2){
              sounds = 1;
            }


            delay(300); 
          }
      
      // checks if the joystick button was pressed
      if(joystick.readSw() == LOW)
      {
        if(currentSetting == 3)
        { 
          resetHIGHSCORE();
        }

        //wait until the button is released
        while(joystick.readSw() == LOW)
        {

        }
        reset();
        break;
      }
        
  }
}

void highScoreMenu(){
  // Updates only if a Joystick movement was made 
  if(lastTopPlayer == topPlayer)
    return;

  // Displays the score and the Name of the current selected player

  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("Name: ");
  for(int i = 0; i < 3; i++)
  {
    lcd.print(char(EEPROM.read(7 + 3 * topPlayer + i)));
  }

  lcd.setCursor(3, 1);
  lcd.print("Score: ");
  lcd.print(EEPROM.read(topPlayer));
  lastTopPlayer = topPlayer;
}

// Return the number of digits of a given number
int getNumberOfDigits(int x)
{
  int nr = 0;

  while(x != 0)
  {
    nr++;
    x /= 10;
  }

  return nr;
}

// Displays a Game Over Message and the Score
void GameOverMenu(){

  updateHighScore(score, letters);
  

  displayMenuOnce = true;
  lc.clearDisplay(0);
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("GAME OVER");
  lcd.setCursor(4, 1);
  lcd.print("SCORE: ");
  lcd.print(score);
}

// Displays a Winning Message and the Score
void WinMenu(){
  updateHighScore(score, letters);
  displayMenuOnce = true;
  lc.clearDisplay(0);
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("GOOD JOB!");
  lcd.setCursor(4, 1);
  lcd.print("SCORE:");
  lcd.print(score);
}

// The HowToPlay Menu
void howToPlayMenu(){
  if(displayMenuOnce)
    return;
  
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("EAT THE");
  lcd.setCursor(2, 1);
  lcd.print("BLINKING DOT");


  displayMenuOnce = true;
}

// checks if the button was pressed
void buttonChangeState() {
  bool swState = joystick.readSw();

  if (swState != lastSwState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (swState != switchState) {
      switchState = swState;
      if (switchState == LOW) {
        pressedTime = millis();
      }
      else {
        releasedTime = millis();
      }

      long pressDuration = releasedTime - pressedTime;
      // short press
      if (pressDuration > shortPressDuration) {
        // long press
        if(pressDuration > longPressDuration)
        {
          if(gameOver == true){ 
            reset();
            return;
          }
        }

        if(gameState == 0)
        {
          gameState = 1;
          return;
        }
        
        if(gameState == 1)
        {
          if(menuPosition == 0){
            //let's say that is an accidental press during the game
            if(gameOver == false)
            {
              return;
            }
            else
            {
              doOnce = false;
              gameOver = false;
              displayMenuOnce = false;
              checkGameState();
              lastScore = -1;
              resetGame();
              return;
            }
          }

          reset();
        }

      }
    }
  }
  
  lastSwState = swState;
}

// Game Logic function
void game(){
  
  buttonChangeState();

  if(gameState == 0) //main menu state
    menuMovement();
  else
    if(gameState == 1)
    {
      checkGameState();
      if(menuPosition)
      {
        menuMovement();
      }
    }
}


// Reset Function
void reset(){
  lc.clearDisplay(0);
  nameSelected = false;
  doOnce = false;
  displayMenuOnce = false;
  gameOver = false;
  gameState = 0;
  menuPosition = 0;
  updateLCD();
  lastTopPlayer = -1;
  lastTextPosition = -1;
  topPlayer = 0;
  textPosition = 0;
}

// Displays the Main Menu
void updateLCD(){
  lcd.clear();


  if(menuPosition == 0)
  {
      lcd.setCursor(6, 0);
      lcd.print("Menu");

      lcd.setCursor(0, 1);
      lcd.print(">  ");-
      lcd.print(menu[menuPosition]);
  }
  else
  if(menuPosition < 4)
  {
      lcd.setCursor(0, 0);
      lcd.print(">");

      lcd.setCursor((16 - menu[menuPosition].length()) / 2, 0);
      lcd.print(menu[menuPosition]);

      lcd.setCursor((16 - menu[menuPosition + 1].length()) / 2, 1);
      lcd.print(menu[menuPosition + 1]);
  }
  else
  if(menuPosition == menuSize - 1)
  {
      lcd.setCursor((16 - menu[menuPosition - 1].length()) / 2, 0);
      lcd.print(menu[menuPosition - 1]);

      lcd.setCursor(0, 1);
      lcd.print(">");
      lcd.setCursor((16 - menu[menuPosition].length()) / 2, 1);
      lcd.print(menu[menuPosition]);
  }
}

// Write name function
void writeName() {
    // Initialize the current letter index
    int currentLetter = 0;

    // Clear the LCD
    lcd.clear();


    lcd.setCursor(2, 0);
    lcd.print("PLAYER NAME:");

    while (true) {
        for(int i = 0; i < 3; i++)
        {
          lcd.setCursor(5 + 2 * i, 1);
          lcd.print(letters[i]);
        }

        lcd.setCursor(5 + 2 * currentLetter, 1);

        // Read the joystick position
        int yValue = joystick.readX();
        int xValue = joystick.readY();

        // Check if the joystick is being moved left or right
        if(xValue > joystick.maxThreshold)
        {
          delay(500); 
          currentLetter = max(currentLetter - 1, 0);
        }
        else
        if(xValue < joystick.minThreshold)
        {
          currentLetter = min(currentLetter + 1, 2);
          delay(500); 
        }
        else
        // Check if the joystick is being moved up or down
          if (yValue > joystick.maxThreshold) {
             letters[currentLetter] = max(letters[currentLetter] - 1, 'A');
              delay(300); 
          } else if (yValue < joystick.minThreshold) {
              letters[currentLetter] = min(letters[currentLetter] + 1, 'Z');
              delay(300); 
          }
        
        // Check if the button was pressed
        if(joystick.readSw() == LOW)
        {
          break;
        }
          
    }
}

// Reads the values from the EEPROM
void readValuesFromEEPROM(){
  difficulty = readDifficulty();
  matrixBrightness = readBrightness();
}


// Resets the Leaderboard
void resetHIGHSCORE(){
  resetValues();
  resetLetters();
}
