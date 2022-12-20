#include "Arduino.h"

// Joystick class with basics methods
class Joystick {
public:
  Joystick(int xPin, int yPin, int swPin);
  void begin();
  int readX();
  int readY();
  bool readSw();
  const int minThreshold = 300;
  const int maxThreshold = 800;

public:
  bool joyMovedY;

private:
  int xPin;
  int yPin;
  int swPin;
};

Joystick::Joystick(int xPin, int yPin, int swPin) {
  this->xPin = xPin;
  this->yPin = yPin;
  this->swPin = swPin;
  joyMovedY = false;

  pinMode(xPin, INPUT);
  pinMode(yPin, INPUT);
  pinMode(swPin, INPUT);
}

int Joystick::readX() {
  return analogRead(xPin);
}

int Joystick::readY() {
  return analogRead(yPin);
}


bool Joystick::readSw() {
  return digitalRead(swPin);
}

