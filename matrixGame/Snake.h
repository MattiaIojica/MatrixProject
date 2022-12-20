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
  int lastTailX;
  int lastTailY;

public:
  Snake();
  ~Snake();

  int getLenght();
  void move(int, int);
  void grow();
  void draw(LedControl &);
  bool isColliding(int[], int);
  bool isOnSegment(int, int);
};

// Initialize the snake with a single segment at the origin (0, 0)
Snake::Snake() {
  head = new SnakeSegment{ 0, 0, nullptr, nullptr };
  tail = new SnakeSegment{ 0, 1, head, nullptr };
  lastTailX = 0;
  lastTailY = 0;
  head->next = tail;
  length = 2;
}


// Delete all of the segments in the snake
Snake::~Snake() {
  SnakeSegment *current = head;
  while (current != nullptr) {
    SnakeSegment *next = current->next;
    delete current;
    current = next;
  }
}

int Snake::getLenght() {
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

// Add a new segment to the end of the snake
void Snake::grow() {
  tail->next = new SnakeSegment{ tail->x, tail->y, tail, nullptr };
  tail = tail->next;
  length++;
}



// Check if the head of the snake is colliding with any of the other segments
bool Snake::isColliding(int obstacle[], int obstacleSize) {
  SnakeSegment *current = head->next;
  while (current != nullptr) {
    if (current->x == head->x && current->y == head->y) {
      return true;
    }
    current = current->next;
  }

  for (int i = 0; i < 2 * obstacleSize; i += 2) {
    if (head->x == obstacle[i] && head->y == obstacle[i + 1]) {
      return true;
    }
  }

  return false;
}

// Iterate through the segments of the snake and check if any of them have the given coordinates
bool Snake::isOnSegment(int x, int y) {
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

  // Turns off the led of the last Tail
  ledControl.setLed(0, lastTailX, lastTailY, false);
  
  // Clears the display
  // ledControl.clearDisplay(0);

  // Iterate through the segments of the snake and draw each one on the LCD
  SnakeSegment *current = head;
  while (current != nullptr) {
    ledControl.setLed(0, current->x, current->y, true);
    current = current->next;
  }

  // Modifies the variable for the last tail to the current tail
  lastTailX = tail->x;
  lastTailY = tail->y;
}
