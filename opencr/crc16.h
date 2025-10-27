#pragma once
#include <Arduino.h>

uint16_t crc_ccitt_ffff(const uint8_t* data, size_t len);
