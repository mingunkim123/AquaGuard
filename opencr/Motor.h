#pragma once
#include <Arduino.h>
#include <Dynamixel2Arduino.h>
#include "config.h"

class Motor {
public:
  Motor(HardwareSerial& dxlSer, int dirPin, uint32_t baud, uint8_t id, uint8_t opMode);
  void begin();
  void moveToReady();
  void moveToSmash();
  void torqueOn(bool on);

private:
  HardwareSerial& _dxlSer;
  Dynamixel2Arduino _dxl;
  int _dirPin;
  uint32_t _baud;
  uint8_t _id;
  uint8_t _mode;
};
