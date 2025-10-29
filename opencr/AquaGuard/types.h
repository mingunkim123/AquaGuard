#pragma once
#include <Arduino.h>

// (PacketType, EventReason은 기존과 동일)
enum PacketType : uint8_t {
  SENSOR_REPORT   = 0xA1,
  EVENT_DETECTED  = 0xA2,
  HEARTBEAT       = 0xB1,
  SMASH_NOW       = 0xC1,
  CANCEL          = 0xC2,
  ACTION_DONE     = 0xD1,
  NO_COMMAND      = 0x00
};

enum EventReason : uint8_t {
  REASON_AUTO        = 1,
  REASON_MANUAL      = 2,
  REASON_LAST_RESORT = 3
};

// --- FSM 상태 변경 ---
// 시스템을 영구적으로 멈추는 STATE_DONE 추가
enum SystemState : uint8_t {
  STATE_IDLE = 0,
  STATE_TRIGGERED,
  STATE_SMASHING_STEP1,
  STATE_SMASHING_STEP2,
  STATE_SMASHING_STEP3,
  STATE_DONE            // <-- ★ COOLDOWN 대신 DONE으로 수정
};
