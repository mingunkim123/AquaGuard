#pragma once
#include <Arduino.h>
#include <Dynamixel2Arduino.h>
#include "config.h"

class Motor {
public:
  // 생성자에 준비/파쇄 위치(각도) 추가
  Motor(HardwareSerial& dxlSer, int dirPin, uint32_t baud, 
        uint8_t id, uint8_t opMode, float posReady, float posSmash);
  
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

  // 모터별 고유 위치값 저장
  float _posReady;
  float _posSmash;
};










