#include "Motor.h"

// 생성자에서 posReady, posSmash 값을 받아 내부 변수에 저장
Motor::Motor(HardwareSerial& dxlSer, int dirPin, uint32_t baud, 
             uint8_t id, uint8_t opMode, float posReady, float posSmash)
  : _dxlSer(dxlSer), _dxl(dxlSer, dirPin), _dirPin(dirPin), _baud(baud), 
    _id(id), _mode(opMode), _posReady(posReady), _posSmash(posSmash) {}

void Motor::begin() {
  _dxlSer.begin(_baud);
  _dxl.begin(_baud);
  _dxl.setPortProtocolVersion(1.0);
  _dxl.torqueOff(_id);
  _dxl.setOperatingMode(_id, _mode);
  _dxl.torqueOn(_id);
}

void Motor::moveToReady() {
  // config.h의 DXL_POS_READY 대신 내부 변수 _posReady 사용
  _dxl.setGoalPosition(_id, _posReady, UNIT_DEGREE);
}

void Motor::moveToSmash() {
  // config.h의 DXL_POS_SMASH 대신 내부 변수 _posSmash 사용
  _dxl.setGoalPosition(_id, _posSmash, UNIT_DEGREE);
}

void Motor::torqueOn(bool on) {
  if (on) _dxl.torqueOn(_id);
  else    _dxl.torqueOff(_id);
}






