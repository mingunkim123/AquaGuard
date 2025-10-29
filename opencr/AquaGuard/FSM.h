// FSM.h
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
  FSM(Motor& m1, Motor& m2, Motor& m3, Motor& m4, Motor& m5,
      Sensors& s, Attitude& a, Comm& c);
  void tick();

private:
  Motor& _motor1; Motor& _motor2; Motor& _motor3; Motor& _motor4; Motor& _motor5;
  Sensors& _sensors; Attitude& _att; Comm& _comm;

  SystemState _state = STATE_IDLE;
  uint32_t _triggerStartMs = 0;

  // 스매시 단계 타이머
  uint32_t _smashStepTimerMs = 0;

  void sendSensorReport_500ms(); // Pi가 온라인일 때만 전송

  // 그룹 시퀀스(2,4)→(3,5)→(1)
  inline void startSeq_step1(uint32_t now) {
    _state = STATE_SMASHING_STEP1;
    _motor2.moveToSmash();
    _motor4.moveToSmash();
    _smashStepTimerMs = now + SMASH_GROUP_GAP_MS;
  }
  inline void step2(uint32_t now) {
    _state = STATE_SMASHING_STEP2;
    _motor3.moveToSmash();
    _motor5.moveToSmash();
    _smashStepTimerMs = now + SMASH_GROUP_GAP_MS;
  }
  inline void step3(uint32_t now) {
    _state = STATE_SMASHING_STEP3;
    _motor1.moveToSmash();
    _smashStepTimerMs = now + SMASH_FINAL_PULSE_MS;
  }
};




