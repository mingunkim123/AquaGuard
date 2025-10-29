// FSM.cpp
#include "FSM.h"
#include "config.h"

FSM::FSM(Motor& m1, Motor& m2, Motor& m3, Motor& m4, Motor& m5,
         Sensors& s, Attitude& a, Comm& c)
: _motor1(m1), _motor2(m2), _motor3(m3), _motor4(m4), _motor5(m5),
  _sensors(s), _att(a), _comm(c) {}

void FSM::sendSensorReport_500ms() {
  static uint32_t lastMs = 0;
  uint32_t now = millis();
  if (now - lastMs < 500) return;
  lastMs = now;

  // Pi 온라인일 때만 전송
  if (now - _comm.lastHeartbeatMs > HEARTBEAT_TIMEOUT_MS) return;

  uint8_t p[8]; uint8_t i=0;
  p[i++] = (uint8_t)_state;
  bool ok = _att.tiltOK();
  p[i++] = ok ? 1 : 0;
  int16_t r   = (int16_t)(_att.rollDeg()      * 10.0f);
  int16_t pch = (int16_t)(_att.pitchDeg()     * 10.0f);
  int16_t d   = (int16_t)(_sensors.ultraFilteredCM() * 10.0f);
  p[i++] = (uint8_t)(r >> 8);   p[i++] = (uint8_t)(r & 0xFF);
  p[i++] = (uint8_t)(pch >> 8); p[i++] = (uint8_t)(pch & 0xFF);
  p[i++] = (uint8_t)(d >> 8);   p[i++] = (uint8_t)(d & 0xFF);
  _comm.sendPacket(SENSOR_REPORT, p, i);
}

void FSM::tick() {
  static uint32_t lastTick = 0;
  uint32_t now = millis();
  if (now - lastTick < FSM_TICK_MS) { delay(1); return; }
  lastTick = now;

  // 1) 센싱/자세는 항상 업데이트 (Pi 유무와 무관)
  _att.update();

  // 2) Pi 온라인 여부
  const bool pi_online = (now - _comm.lastHeartbeatMs <= HEARTBEAT_TIMEOUT_MS);

  // 3) 센서 보고는 Pi 온라인일 때만
  sendSensorReport_500ms();

  // 4) 센서 판정(항상 수행)
  const bool touch_triggered = _sensors.touchTriggered();
  const bool lr_triggered    = _sensors.lastResortTriggered();
  const bool opt_detected    = _sensors.optDetected();
  const bool ultra_detected  = _sensors.ultraDetected();
  const bool submerged       = (opt_detected && ultra_detected);
  const bool trigger_cond    = touch_triggered || submerged || lr_triggered;

  // 5) Pi 명령: 오프라인이면 무시
  PacketType cmd = pi_online ? _comm.lastCmd : NO_COMMAND;
  if (pi_online) _comm.lastCmd = NO_COMMAND;  // 한 번 소비

  switch (_state) {
    case STATE_IDLE: {
      if (trigger_cond) {
        _state = STATE_TRIGGERED;
        _triggerStartMs = now;
        if (pi_online) {
          uint8_t reason = lr_triggered ? REASON_LAST_RESORT :
                           touch_triggered ? REASON_MANUAL : REASON_AUTO;
          _comm.sendPacket(EVENT_DETECTED, &reason, 1);
        }
      } else if (cmd == SMASH_NOW) {
        if (_att.tiltOK()) startSeq_step1(now);
      }
    } break;

    case STATE_TRIGGERED: {
      if (cmd == CANCEL && pi_online) {
        _state = STATE_IDLE;
        _triggerStartMs = 0;
        break;
      }
      if (!trigger_cond) {
        _state = STATE_IDLE;
        _triggerStartMs = 0;
        break;
      }
      if ((now - _triggerStartMs >= TRIGGER_DELAY_MS) || (cmd == SMASH_NOW)) {
        if (_att.tiltOK()) startSeq_step1(now);
      }
    } break;

    case STATE_SMASHING_STEP1: {
      if (now >= _smashStepTimerMs) step2(now);
    } break;

    case STATE_SMASHING_STEP2: {
      if (now >= _smashStepTimerMs) step3(now);
    } break;

    case STATE_SMASHING_STEP3: {
      if (now >= _smashStepTimerMs) {
        // 끝난 뒤 복귀 정책: 필요 시 여기서 moveToReady() 호출
        // 예) _motor1.moveToReady(); ... (전부)
        if (pi_online) _comm.sendPacket(ACTION_DONE, nullptr, 0);
        _state = STATE_IDLE;  // or STATE_DONE (원하는 정책대로)
      }
    } break;

    case STATE_DONE:
    default:
      // 필요 없으면 쓰지 않아도 됨
      break;
  }

  delay(1);
}





