#include <EEPROM.h>

// updates the HighScore
void updateHighScore(int score, char letters[]) {

  // finds the position of the current score
  int position = -1;
  for(int i = 0; i < 5; i++){
    if(EEPROM.read(i) < score){
      position = i;
      break;
    }
  }

  // shifts the Scores and names from the Leaderboard 
  for(int i = 4; i > position && position != -1; i--)
  {
    EEPROM.update(i,  EEPROM.read(i - 1));
    for(int j = 0; j < 3; j++){
      EEPROM.update(7 + 3 * i + j,  EEPROM.read(7 + 3 * i + j - 3));
    }
  }


  if(position != -1){
    EEPROM.update(position, score);
    for(int i = 0; i < 3; i++)
    {
      EEPROM.update(7 + i + 3 * position, letters[i]);
    }
  }
}

// updates the difficulty
void updateDifficulty(int diff)
{
  EEPROM.update(5, diff);
}

// reads the difficulty
int readDifficulty(){
  return EEPROM.read(5);
}

//updates the brightness
void updateBrightness(int br)
{
  EEPROM.update(6, br);
}

//reads the Brightness
int readBrightness(){
  return EEPROM.read(6);
}

// resets the values from the Leaderboars
void resetValues(){
  for(int i = 0; i < 5; i++)
  {
   EEPROM.update(i, 0);
  }
}

// resets the letters from the Leaderboars
void resetLetters(){
  for(int player = 0; player < 5; player++)
  {
    for(int i = 0; i < 3; i++)
    {
      EEPROM.update(7 + 3 * player + i, 65 + i);
    }
  }
}