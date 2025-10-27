#include "FSM.h"

FSM::FSM(Motor& m, Sensors& s, Attitude& a, Comm& c)
: _motor(m), _sensors(s), _att(a), _comm(c) {}

void FSM::sendSensorReport_500ms() {
  static uint32_t lastMs = 0;
  uint32_t now = millis();
  if (now - lastMs < 500) return;
  lastMs = now;

  // payload: state(1) | tilt_ok(1) | roll(2, x10) | pitch(2, x10) | dist(2, x10)
  uint8_t p[8]; uint8_t i=0;
  p[i++] = (uint8_t)_state;
  bool ok = _att.tiltOK();
  p[i++] = ok ? 1 : 0;
  int16_t r = (int16_t)(_att.rollDeg()  * 10.0f);
  int16_t pch = (int16_t)(_att.pitchDeg() * 10.0f);
  int16_t d = (int16_t)(_sensors.ultraFilteredCM() * 10.0f);
  p[i++] = (uint8_t)(r >> 8);   p[i++] = (uint8_t)(r & 0xFF);
  p[i++] = (uint8_t)(pch >> 8); p[i++] = (uint8_t)(pch & 0xFF);
  p[i++] = (uint8_t)(d >> 8);   p[i++] = (uint8_t)(d & 0xFF);
  _comm.sendPacket(SENSOR_REPORT, p, i);
}

void FSM::handleHeartbeatTimeout() {
  if (millis() - _comm.lastHeartbeatMs > HEARTBEAT_TIMEOUT_MS) {
    Serial.println("!!! PI HB TIMEOUT !!! -> to IDLE");
    if (_state != STATE_IDLE || _movingSmash) _motor.moveToReady();
    _state = STATE_IDLE;
    _triggerStartMs = 0;
    _movingSmash = false;
    _comm.lastCmd = NO_COMMAND;
    _comm.lastHeartbeatMs = millis(); // 재타임아웃 방지
  }
}

void FSM::tick() {
  // 타이밍 고정
  static uint32_t lastTick = 0;
  uint32_t now = millis();
  if (now - lastTick < FSM_TICK_MS) { delay(1); return; }
  lastTick = now;

  // 주기 처리
  _att.update();
  handleHeartbeatTimeout();
  sendSensorReport_500ms();

  // 센서 판정
  bool touch_triggered = _sensors.touchTriggered();
  bool lr_triggered    = _sensors.lastResortTriggered();
  bool opt_detected    = _sensors.optDetected();
  bool ultra_detected  = _sensors.ultraDetected();
  bool submerged       = (opt_detected && ultra_detected);
  bool trigger_cond    = touch_triggered || submerged || lr_triggered;

  // 수신 명령 스냅샷
  PacketType cmd = _comm.lastCmd;

  switch (_state) {
    case STATE_IDLE: {
      if (trigger_cond) {
        _state = STATE_TRIGGERED;
        _triggerStartMs = now;

        uint8_t reason = 0;
        if (lr_triggered) reason = REASON_LAST_RESORT;
        else if (touch_triggered) reason = REASON_MANUAL;
        else if (submerged) reason = REASON_AUTO;
        _comm.sendPacket(EVENT_DETECTED, &reason, 1);
        Serial.println("[FSM] IDLE -> TRIGGERED");
      } else if (cmd == SMASH_NOW) {
        Serial.print("[FSM] SMASH_NOW in IDLE, tilt check...");
        if (_att.tiltOK()) {
          _state = STATE_SMASHING;
          _motor.moveToSmash();
          _smashEndMs = now + SMASH_PULSE_MS;
          _movingSmash = true;
          Serial.println("OK -> SMASHING");
        } else {
          Serial.println("VETO");
        }
        _comm.lastCmd = NO_COMMAND;
      }
    } break;

    case STATE_TRIGGERED: {
      if (cmd == CANCEL) {
        _state = STATE_IDLE;
        if (_movingSmash) { _motor.moveToReady(); _movingSmash = false; }
        _triggerStartMs = 0;
        _comm.lastCmd = NO_COMMAND;
        Serial.println("[FSM] CANCEL -> IDLE");
        break;
      }

      if (!trigger_cond) {
        _state = STATE_IDLE;
        _triggerStartMs = 0;
        Serial.println("[FSM] Trigger Lost -> IDLE");
        break;
      }

      if ((now - _triggerStartMs >= TRIGGER_DELAY_MS) || (cmd == SMASH_NOW)) {
        Serial.print("[FSM] Delay or SMASH_NOW, tilt check...");
        if (_att.tiltOK()) {
          _state = STATE_SMASHING;
          if (!_movingSmash) { _motor.moveToSmash(); _movingSmash = true; }
          _smashEndMs = now + SMASH_PULSE_MS;
          Serial.println("OK -> SMASHING");
        } else {
          Serial.println("VETO (stay TRIGGERED)");
        }
        _comm.lastCmd = NO_COMMAND;
      }
    } break;

    case STATE_SMASHING: {
      if (cmd == CANCEL) {
        _state = STATE_IDLE;
        _motor.moveToReady();
        _movingSmash = false;
        _triggerStartMs = 0;
        _comm.lastCmd = NO_COMMAND;
        Serial.println("[FSM] CANCEL during SMASH -> IDLE");
        break;
      }

      if (now >= _smashEndMs) {
        _state = STATE_IDLE;
        _motor.moveToReady();
        _movingSmash = false;
        _triggerStartMs = 0;
        _comm.sendPacket(ACTION_DONE, nullptr, 0);
        Serial.println("[FSM] Smash done -> IDLE (ActionDone)");
      } else if (!_att.tiltOK()) {
        _state = STATE_IDLE;
        _motor.moveToReady();
        _movingSmash = false;
        _triggerStartMs = 0;
        _comm.lastCmd = NO_COMMAND;
        Serial.println("!!! Tilt fail during SMASH -> IDLE");
      }
    } break;

    default:
      Serial.println("!!! Unknown state -> IDLE");
      _state = STATE_IDLE;
      _motor.moveToReady();
      _movingSmash = false;
      _triggerStartMs = 0;
      _comm.lastCmd = NO_COMMAND;
      break;
  }

  delay(1); // 약간의 양보
}
