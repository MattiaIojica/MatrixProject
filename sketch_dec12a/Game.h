#include "HardwareSerial.h"
#include <LedControl.h>
#include "JoyStick.h"
#include "Snake.h"



// Create an instance of the joystick
Joystick joystick = Joystick(A4, A5, 0);


// constants for the LED matrix pins
const int dinPin = 12;
const int clockPin = 11;
const int loadPin = 10;

// delay between movements in milliseconds
int snakeSpeed;  

// initialize the LED matrix
LedControl lc = LedControl(dinPin, clockPin, loadPin, 1);

const int obstacleSize = 6;
int obstacle[obstacleSize * 2] = {1, 1, 1, 2, 4, 3, 5, 3, 3, 7, 3, 6};

// const int obstacleBlinkInterval = 100;
// unsigned long lastObstacleBlink;
bool obstacleBlink = true;

// Define constants for the joystick directions
const int UP = 0;
const int DOWN = 1;
const int LEFT = 2;
const int RIGHT = 3;

// Create a linked list to store the snake segments
SnakeSegment* head = NULL;

// Define the starting position and direction for the snake
int snake_x = 0;
int snake_y = 0;
int snake_direction;

// score
int score = 0;

int matrixSize = 8;

// food coordinates
int foodX;
int foodY;

const int blinkInterval = 250;
unsigned long lastBlink;
bool blinkFood = false;

bool gameOver;
bool doOnce = false;

int difficulty = 1;

unsigned long timeTillDeath;

void initializeSnake();
void generateFood();
void moveSnake();
void drawBoard();
void foodBlink();
bool onObstacles();

Snake *snake;

void gameSetup() {
  // joystick.begin();

  // initialize the LED matrix
  lc.shutdown(0, false);
  lc.setIntensity(0, 8);
  lc.clearDisplay(0);

  // initialize the snake and food
  initializeSnake();
  timeTillDeath = millis();
}

void gameLoop() {
  if(!doOnce)
  {
    gameSetup();
    doOnce = true;
  }

  // check joystick input
  int xVal = joystick.readX();
  int yVal = joystick.readY();
  int swVal = joystick.readSw();

  // set the direction based on the joystick input
  if (xVal < joystick.minThreshold && snake_direction != RIGHT) {
    // move left
    snake_direction = LEFT;
  } else if (xVal > joystick.maxThreshold  && snake_direction != LEFT) {
    // move right
    snake_direction = RIGHT;
  } else if (yVal < joystick.minThreshold  && snake_direction != UP) {
    // move down
    snake_direction = DOWN;
  } else if (yVal > joystick.maxThreshold  && snake_direction != DOWN) {
    // move up
    snake_direction = UP;
  }

  // move the snake
  moveSnake();

  // check if the snake has eaten the food
  if (snake_x == foodX && snake_y == foodY) {
    // increase the snake's length
    score += difficulty;

    snake->grow();
    snakeSpeed -= 5;

    // generate new food
    generateFood();
  }

  // draw the snake and food
  drawBoard();

  // delay before next movement
  delay(snakeSpeed);
  
  if((millis() - timeTillDeath) / 1000 > 7)
  {
    gameOver = true;
  }

  if(snake->getLenght() == 64 - (obstacleSize * (difficulty % 2 + 1)))
    gameOver == true;

  if(difficulty == 2)
  {
    if(snake->isColliding(obstacle, obstacleSize)){
      gameOver = true;
    }
  }

}


// initialize the snake to the middle of the board
void initializeSnake() {
  if(snake != nullptr)
  {
    delete snake;
  }

  generateFood();
  snake = new Snake();
  score = 0;
  snake_direction = DOWN;
  snake_x = 0;
  snake_y = 0;

  if(difficulty == 1)
  {
    snakeSpeed = 400;
  }
  else
  if(difficulty == 2)
  {
    snakeSpeed = 250;
  }
}

// place the food at a random location on the board
void generateFood() {
  // Generate new food position
  foodX = rand() % matrixSize;
  foodY = rand() % matrixSize;

  while ((snake_x == foodX && snake_y == foodY) || snake->isOnSegment(foodX, foodY) || onObstacles()) {
    foodX = rand() % matrixSize;
    foodY = rand() % matrixSize;
  }
  timeTillDeath = millis(); 

}


// move the snake in the current direction
void moveSnake() {
  // update the snake's position
  int x = snake_x;
  int y = snake_y;
  if (snake_direction == UP) {
    y = (y - 1 + matrixSize) % matrixSize;
  } else if (snake_direction == DOWN) {
    y = (y + 1) % matrixSize;
  } else if (snake_direction == LEFT) {
    x = (x - 1 + matrixSize) % matrixSize;
  } else if (snake_direction == RIGHT) {
    x = (x + 1) % matrixSize;
  }

  snake_x = x;
  snake_y = y;
  snake->move(x, y);
}

// draw the snake and food on the LED matrix
void drawBoard() {

  
  snake->draw(lc);

  if (millis() - lastBlink > blinkInterval) {
    foodBlink();
    lastBlink = millis();
  }
  
  // if(difficulty == 2)
  // {
  //   if (millis() - lastObstacleBlink > obstacleBlinkInterval) {
  //     obstacleBlink = !obstacleBlink;
  //     lastObstacleBlink = millis();
  //   }
  // }

  lc.setLed(0, foodX, foodY, blinkFood);
  
  if(difficulty == 2){
    for(int i = 0; i < 2 * obstacleSize; i += 2)
    {
      lc.setLed(0, obstacle[i], obstacle[i + 1], obstacleBlink);
    }
  }
}

void resetGame() {
  // reset the snake and food
  initializeSnake();

  // clear the LED matrix
  lc.clearDisplay(0);
}

void foodBlink(){
  blinkFood = !blinkFood;
}

bool onObstacles(){

  for(int i = 0; i < 2 * obstacleSize; i += 2)
  {
    if(foodX == obstacle[i] && foodY == obstacle[i + 1]){
      return true;
    }
  }

  return false;
}





