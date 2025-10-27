#include "Motor.h"

Motor::Motor(HardwareSerial& dxlSer, int dirPin, uint32_t baud, uint8_t id, uint8_t opMode)
  : _dxlSer(dxlSer), _dxl(dxlSer, dirPin), _dirPin(dirPin), _baud(baud), _id(id), _mode(opMode) {}

void Motor::begin() {
  _dxlSer.begin(_baud);
  _dxl.begin(_baud);
  _dxl.setPortProtocolVersion(1.0);
  _dxl.torqueOff(_id);
  _dxl.setOperatingMode(_id, _mode);
  _dxl.torqueOn(_id);
}

void Motor::moveToReady() {
  _dxl.setGoalPosition(_id, DXL_POS_READY, UNIT_DEGREE);
}

void Motor::moveToSmash() {
  _dxl.setGoalPosition(_id, DXL_POS_SMASH, UNIT_DEGREE);
}

void Motor::torqueOn(bool on) {
  if (on) _dxl.torqueOn(_id);
  else    _dxl.torqueOff(_id);
}
