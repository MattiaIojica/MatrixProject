#include <EEPROM.h>


void updateHighScore(int score) {
  int x = -1;
  for(int i = 0; i < 5; i++){
    if(EEPROM.read(i) < score){
      x = i;
      break;
    }
  }

  for(int i = 4; i > x && x != -1; i--)
    EEPROM.update(i,  EEPROM.read(i - 1));


  if(x != -1)
    EEPROM.update(x, score);
}

void updateDifficulty(int diff)
{
  EEPROM.update(5, diff);
}

void resetValues(){
  for(int i = 0; i < 5; i++)
  {
   EEPROM.update(i, 0);
  }
}