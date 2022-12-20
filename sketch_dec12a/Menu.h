
#include "Game.h"
#include "EEPROMMemory.h"
#include "Buzzer.h"

const byte RS = 9;
const byte enable = 8;
const byte d4 = 7;
const byte d5 = 6;
const byte d6 = 5;
const byte d7 = 4;

bool gameState = 0;

const int menuSize = 5;
const String menu[menuSize] = { "START GAME", "HIGHSCORE", "SETTINGS", "ABOUT", "HOW TO PLAY" };
int menuPosition = 0;

LiquidCrystal lcd(RS, enable, d4, d5, d6, d7);

bool displayMenuOnce = false;
const int shortPressDuration = 50;
const int longPressDuration = 1500;
unsigned long lastDebounceTime = 0;
unsigned int debounceDelay = 50;
unsigned long pressedTime = 0;
unsigned long releasedTime = 0;

bool lastSwState = LOW;
byte switchState = HIGH;

int textPosition = 0;
int lastTextPosition = -1;

int lastScore = -1;

int lastDifficulty = -1;

int topPlayer = 0;
int lastTopPlayer = -1;


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

void welcomeMenu() {

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Welcome to the");
  lcd.setCursor(0, 1);
  lcd.print("Snake game");

  delay(1200);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Get ready to");
  lcd.setCursor(0, 1);
  lcd.print("play!");

  delay(1500);
}




void checkGameState()
{
  gameState = 1;
  switch(menuPosition)
  {
    case 0:
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

void menuMovement(){
  int yValue = joystick.readX();

  // Serial.println(gameState);
  if (yValue < joystick.minThreshold && !joystick.joyMovedY) {
      Serial.println("down");
      Serial.println(gameState);
    if(gameState == 0)
    {
      menuPosition = max(menuPosition - 1, 0);
      Serial.println("sound0");
      scrollSound();
    }
    else
    if(gameState == 1)
    {
      topPlayer = max(topPlayer - 1, 0);
      textPosition = max(textPosition - 1, 0);
      updateDifficulty(min(EEPROM.read(5) + 1, 2));
    }

    joystick.joyMovedY = true;
    if(gameState == 0)
      updateLCD();
  }
  else
  if (yValue > joystick.maxThreshold && !joystick.joyMovedY) {
    Serial.println("up");
    if(gameState == 0){
      menuPosition = min(menuPosition + 1, menuSize - 1);
    Serial.println("sound1");
      scrollSound();
    }
    else
    if(gameState == 1)
    {
      textPosition = min(textPosition + 1, 1);
      topPlayer = min(topPlayer + 1, 4);
      updateDifficulty(max(EEPROM.read(5) - 1, 1));
    }
    
    joystick.joyMovedY = true;
    if(gameState == 0)
      updateLCD();
  }
  else
  if (yValue >= joystick.minThreshold && yValue <= joystick.maxThreshold) {
    joystick.joyMovedY = false;
  }
}


void gameMenu(){
  if(gameOver == true)
  {
    if(!displayMenuOnce){
      if(snake->getLenght() == 64 - (obstacleSize * (difficulty % 2 + 1))){
        WinMenu();
      }
      else{
        GameOverMenu();
      }
    }

    return;
  }

  gameLoop();

  lcd.setCursor(2, 1);
  lcd.print("TIME LEFT: ");
  lcd.print(7 - (millis() - timeTillDeath) / 1000);


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
  if(lastTextPosition == textPosition)
    return;
  
  lcd.clear();

  if (textPosition == 0) {
    lcd.setCursor(6, 0);
    lcd.print("ABOUT");
    lcd.setCursor(0, 1);
    lcd.print("Name: Dot Game");
  }
  else
  if (textPosition == 1) {
    lcd.setCursor(5, 0);
    lcd.print("Creator:");
    lcd.setCursor(1, 1);
    lcd.print("Iojica Mattia");
  }
  lastTextPosition = textPosition;
}

void settingsMenu(){
  if(lastDifficulty == EEPROM.read(5))
    return;

  lcd.clear();
  lcd.setCursor(1, 1);
  lcd.print("Difficulty: ");

  lcd.print(EEPROM.read(5));
  lastDifficulty = EEPROM.read(5);
}

void highScoreMenu(){

  if(lastTopPlayer == topPlayer)
    return;

  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("Name: ");

  lcd.setCursor(3, 1);
  lcd.print("Score: ");
  lcd.print(EEPROM.read(topPlayer));
  lastTopPlayer = topPlayer;
}


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

void GameOverMenu(){

  updateHighScore(score);
  

  displayMenuOnce = true;
  lc.clearDisplay(0);
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("GAME OVER");
  lcd.setCursor(4, 1);
  lcd.print("SCORE:");
  lcd.print(score);
}

void WinMenu(){
  updateHighScore(score);
  displayMenuOnce = true;
  lc.clearDisplay(0);
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("GOOD JOB!");
  lcd.setCursor(4, 1);
  lcd.print("SCORE:");
  lcd.print(score);
}

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
      if (pressDuration > shortPressDuration) {
        

        if(pressDuration > longPressDuration)
        {
          Serial.println("long press");
          reset();
          return;
        }
        Serial.println("short press");
        if(gameState == 0)
        {
          Serial.println("-------------");
          gameState = 1; //intra in functia afisata
          return;
        }
        
        if(gameState == 1)
        {
          Serial.println(gameState);
          if(menuPosition == 0){
            if(gameOver == false) //let's say that is an accidental press during the game
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

void game(){

  difficulty = EEPROM.read(5);
  
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

void reset(){
  doOnce = false;
  displayMenuOnce = false;
  Serial.println("reset");
  gameState = 0;
  menuPosition = 0;
  updateLCD();
  lastTopPlayer = -1;
  lastTextPosition = -1;
  topPlayer = 0;
  textPosition = 0;
  lastDifficulty = 0;
}


void updateLCD(){
  lcd.clear();

  // Serial.println(menuPosition);

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