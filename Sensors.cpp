#include "Sensors.h"

Sensors::Sensors(int pinTouch1, int pinTouch2, int pinOpt,
                 int pinTrig, int pinEcho, int pinLastResort, float ultraThCm)
: _pT1(pinTouch1), _pT2(pinTouch2), _pOpt(pinOpt),
  _pTrig(pinTrig), _pEcho(pinEcho), _pLR(pinLastResort),
  _ultraTh(ultraThCm), _kf(2,2,0.01) {}

void Sensors::begin() {
  pinMode(_pT1, INPUT);
  pinMode(_pT2, INPUT);
  pinMode(_pOpt, INPUT);   // HIGH 감지 가정
  pinMode(_pTrig, OUTPUT);
  pinMode(_pEcho, INPUT);
  pinMode(_pLR, INPUT);
}

bool Sensors::touchTriggered() {
  return (digitalRead(_pT1)==HIGH) && (digitalRead(_pT2)==HIGH);
}

bool Sensors::lastResortTriggered() {
  return (digitalRead(_pLR) == HIGH);
}

bool Sensors::optDetected() {
  return (digitalRead(_pOpt) == HIGH);
}

float Sensors::measureUltrasonicOnceCM() {
  digitalWrite(_pTrig, LOW); delayMicroseconds(2);
  digitalWrite(_pTrig, HIGH); delayMicroseconds(10);
  digitalWrite(_pTrig, LOW);
  unsigned long dur = pulseIn(_pEcho, HIGH, 25000UL);
  if (dur == 0) return 999.0f;
  return (float)dur / 58.0f;
}

float Sensors::ultraRawCM() {
  return measureUltrasonicOnceCM();
}

float Sensors::ultraFilteredCM() {
  return _kf.updateEstimate(measureUltrasonicOnceCM());
}

bool Sensors::ultraDetected() {
  return ultraFilteredCM() < _ultraTh;
}
