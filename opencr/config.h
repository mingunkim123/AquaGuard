#pragma once
#include <Arduino.h>
#include <Dynamixel2Arduino.h>

// ==== 모터/다이나믹셀 ====
#define DXL_SERIAL       Serial3
static const uint8_t  DXL_ID         = 1;
static const int      DXL_DIR_PIN    = 84;
static const uint32_t DXL_BAUD       = 57600;
static const uint8_t  DXL_MODE       = OP_POSITION;
static const float    DXL_POS_READY  = 0.0f;
static const float    DXL_POS_SMASH  = 60.0f;
static const uint32_t SMASH_PULSE_MS = 800;

// ==== 센서 핀 (보드에 맞게 필요시 조정) ====
static const int PIN_TOUCH_1 = 51;
static const int PIN_TOUCH_2 = 53;
static const int PIN_OPT     = 61;  // 광학센서: HIGH 감지 가정
static const int PIN_TRIG    = 55;
static const int PIN_ECHO    = 57;
static const int PIN_TOUCH_WATER = 59;

// ==== 초음파 임계 ====
static const float ULTRA_TH_CM = 15.0f;

// ==== 통신 ====
#define RPI_SERIAL Serial
static const uint32_t HEARTBEAT_TIMEOUT_MS = 3000; // ms

// ==== IMU/기울기 ====
static const float TILT_ALLOW_DEG = 30.0f; // ±30deg 허용

// ==== FSM 주기 ====
static const unsigned long TRIGGER_DELAY_MS = 500; // 시간 지연 필터 (ms)
static const uint16_t FSM_TICK_MS = 50; // 20Hz
