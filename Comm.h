#pragma once
#include <Arduino.h>
#include "types.h"

// Stream 기반: USBSerial(Serial)과 HardwareSerial 모두 지원
class Comm {
public:
  Comm(Stream& ser, uint32_t baud);
  void begin();                       // 포트 오픈 X (Serial.begin은 스케치에서 수행)
  void poll();                        // 수신 처리
  void sendPacket(PacketType type, const uint8_t* payload, uint8_t len);

  volatile uint32_t lastHeartbeatMs = 0;   // 마지막 HB 수신 시간
  volatile PacketType lastCmd = NO_COMMAND;

private:
  Stream& _ser;
  uint32_t _baud;

  enum ParseState { WAIT_START, WAIT_TYPE, WAIT_LEN, WAIT_PAYLOAD, WAIT_CRC1, WAIT_CRC2 };
  ParseState _st = WAIT_START;

  uint8_t  _rxBuf[64];
  uint8_t  _idx = 0;
  uint8_t  _typ = 0;
  uint8_t  _len = 0;
};
