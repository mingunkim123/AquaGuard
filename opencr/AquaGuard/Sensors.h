#pragma once
#include <Arduino.h>
#include <SimpleKalmanFilter.h>

class Sensors {
public:
  Sensors(int pinTouch1, int pinTouch2, int pinOpt,
          int pinTrig, int pinEcho, int pinLastResort, float ultraThCm);
  void begin();

  // 측정/판정
  bool touchTriggered();
  bool lastResortTriggered();
  bool optDetected();
  float ultraRawCM();
  float ultraFilteredCM();
  bool ultraDetected();

private:
  int _pT1, _pT2, _pOpt, _pTrig, _pEcho, _pLR;
  float _ultraTh;
  SimpleKalmanFilter _kf;

  float measureUltrasonicOnceCM();
};
