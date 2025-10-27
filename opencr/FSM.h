#pragma once
#include <Arduino.h>
#include "types.h"
#include "Motor.h"
#include "Sensors.h"
#include "Attitude.h"
#include "Comm.h"
#include "config.h"

class FSM {
public:
  FSM(Motor& m, Sensors& s, Attitude& a, Comm& c);
  void tick(); // 50ms 주기 (내부 delay 포함)

private:
  Motor& _motor;
  Sensors& _sensors;
  Attitude& _att;
  Comm& _comm;

  SystemState _state = STATE_IDLE;
  bool   _movingSmash = false;
  uint32_t _triggerStartMs = 0;
  uint32_t _smashEndMs = 0;

  void sendSensorReport_500ms();
  void handleHeartbeatTimeout();
};
