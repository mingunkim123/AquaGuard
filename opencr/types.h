#pragma once
#include <Arduino.h>

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

enum SystemState : uint8_t {
  STATE_IDLE = 0,
  STATE_TRIGGERED,
  STATE_SMASHING
};
