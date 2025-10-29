#include "Comm.h"
#include "crc16.h"

Comm::Comm(Stream& ser, uint32_t baud) : _ser(ser), _baud(baud) {}

void Comm::begin() {
  // 시리얼 포트 오픈은 스케치에서 수행(Serial.begin(115200))
  lastHeartbeatMs = millis();
}

void Comm::sendPacket(PacketType type, const uint8_t* payload, uint8_t len) {
  if (len > 26) return;
  uint8_t buf[32];
  buf[0] = 0x02; buf[1] = (uint8_t)type; buf[2] = len;
  if (len && payload) memcpy(&buf[3], payload, len);
  uint16_t crc = crc_ccitt_ffff(buf, 3 + len);
  buf[3 + len] = crc & 0xFF; buf[4 + len] = (crc >> 8) & 0xFF;
  _ser.write(buf, 5 + len);
}

void Comm::poll() {
  while (_ser.available() > 0) {
    uint8_t b = _ser.read();
    switch (_st) {
      case WAIT_START:
        if (b == 0x02) { _idx = 0; _rxBuf[_idx++] = b; _st = WAIT_TYPE; }
        break;
      case WAIT_TYPE:
        _rxBuf[_idx++] = b; _typ = b; _st = WAIT_LEN;
        break;
      case WAIT_LEN:
        _rxBuf[_idx++] = b; _len = b;
        if (_len > sizeof(_rxBuf) - 5) { _st = WAIT_START; }
        else _st = (_len == 0) ? WAIT_CRC1 : WAIT_PAYLOAD;
        break;
      case WAIT_PAYLOAD:
        if (_idx < sizeof(_rxBuf)) _rxBuf[_idx++] = b; else { _st = WAIT_START; break; }
        if (_idx >= 3 + _len) _st = WAIT_CRC1;
        break;
      case WAIT_CRC1:
        if (_idx < sizeof(_rxBuf)) _rxBuf[_idx++] = b; else { _st = WAIT_START; break; }
        _st = WAIT_CRC2;
        break;
      case WAIT_CRC2: {
        if (_idx < sizeof(_rxBuf)) _rxBuf[_idx++] = b; else { _st = WAIT_START; break; }
        uint16_t rx_crc = (uint16_t)((_rxBuf[_idx-1] << 8) | _rxBuf[_idx-2]);
        uint16_t calc   = crc_ccitt_ffff(_rxBuf, _idx - 2);
        if (rx_crc == calc) {
          PacketType vt = (PacketType)_typ;
          if (vt == HEARTBEAT) lastHeartbeatMs = millis();
          else if (vt == SMASH_NOW || vt == CANCEL) lastCmd = vt;
        }
        _st = WAIT_START;
      } break;
    }
  }
}
