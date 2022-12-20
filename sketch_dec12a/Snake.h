#include "HardwareSerial.h"
#include "LedControl.h"

struct SnakeSegment {
  int x;
  int y;
  SnakeSegment *prev;
  SnakeSegment *next;
};

class Snake {
private:
  SnakeSegment *head;
  SnakeSegment *tail;
  int length;

public:
  Snake();
  ~Snake();

  int getLenght();
  void move(int, int);
  void grow();
  void draw(LedControl&);
  bool isColliding(int[], int);
  bool isOnSegment(int, int);
};

Snake::Snake() {
  // Initialize the snake with a single segment at the origin (0, 0)
  head = new SnakeSegment{0, 0, nullptr, nullptr};
  tail = new SnakeSegment{0, 1, head, nullptr};
  head->next = tail;
  length = 2;
}


Snake::~Snake() {
  // Delete all of the segments in the snake
  SnakeSegment *current = head;
  while (current != nullptr) {
    SnakeSegment *next = current->next;
    delete current;
    current = next;
  }
}

int Snake::getLenght(){
  return length;
}

void Snake::move(int newX, int newY) {

  // Shift all of the other segments down the chain
  SnakeSegment *current = tail;
  while (current->prev != nullptr) {
    current->x = current->prev->x;
    current->y = current->prev->y;
    current = current->prev;
  }
  
  // Update the position of the head segment
  head->x = newX;
  head->y = newY;
}

void Snake::grow() {
   // Add a new segment to the end of the snake
  tail->next = new SnakeSegment{tail->x, tail->y, tail, nullptr};
  tail = tail->next;
  length++;
}



bool Snake::isColliding(int obstacle[], int obstacleSize) {
  // Check if the head of the snake is colliding with any of the other segments
  SnakeSegment *current = head->next;
  while (current != nullptr) {
    if (current->x == head->x && current->y == head->y) {
      return true;
    }
    current = current->next;
  }

  for(int i = 0; i < 2 * obstacleSize; i += 2)
  {
    if(head->x == obstacle[i] && head->y == obstacle[i + 1]){
      return true;
    }
  }

  return false;
}

bool Snake::isOnSegment(int x, int y) {
  // Iterate through the segments of the snake and check if any of them have the given coordinates
  SnakeSegment *current = head;
  while (current != nullptr) {
    if (current->x == x && current->y == y) {
      return true;
    }
    current = current->next;
  }
  return false;
}

void Snake::draw(LedControl &ledControl) {
  Serial.println("-----------------------------");

  ledControl.clearDisplay(0);

  // Iterate through the segments of the snake and draw each one on the LCD
  
  SnakeSegment *current = head;
  while (current != nullptr) {
    Serial.print(current->x);
    Serial.print(" ");
    Serial.println(current->y);
    ledControl.setLed(0, current->x, current->y, true);
    current = current->next;
  }
}
