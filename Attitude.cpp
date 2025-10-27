#include "Attitude.h"
#include <math.h>

Attitude::Attitude(float tiltAllowDeg) : _tiltAllow(tiltAllowDeg) {}

void Attitude::begin(float updateHz) {
  _imu.begin();
  _intervalMs = (uint16_t)(1000.0f / updateHz);
  _madgwick.begin(updateHz);
  _lastUpdateMs = millis();
}

void Attitude::update() {
  uint32_t now = millis();
  if (now - _lastUpdateMs < _intervalMs) return;
  _lastUpdateMs = now;

  _imu.update();
  _madgwick.update(_imu.gx, _imu.gy, _imu.gz, _imu.ax, _imu.ay, _imu.az, _imu.mx, _imu.my, _imu.mz);
  _roll  = _madgwick.getRoll();
  _pitch = _madgwick.getPitch();
}

bool Attitude::tiltOK() const {
  return (fabs(_roll) <= _tiltAllow) && (fabs(_pitch) <= _tiltAllow);
}
