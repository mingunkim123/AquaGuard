#pragma once
#include <Arduino.h>
#include <IMU.h>
#include <MadgwickAHRS.h>

class Attitude {
public:
  explicit Attitude(float tiltAllowDeg);
  void begin(float updateHz);
  void update();
  bool tiltOK() const;
  float rollDeg() const { return _roll; }
  float pitchDeg() const { return _pitch; }

private:
  cIMU _imu;
  Madgwick _madgwick;
  float _tiltAllow;
  float _roll = 0.f, _pitch = 0.f;
  uint32_t _lastUpdateMs = 0;
  uint16_t _intervalMs = 50;
};
