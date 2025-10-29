#pragma once
#include <Arduino.h>
#include <Dynamixel2Arduino.h>

// ==== 모터/다이나믹셀 (5개 모터용) ====
#define DXL_SERIAL       Serial3
static const int      DXL_DIR_PIN    = 84;
static const uint32_t DXL_BAUD       = 57600;
static const uint8_t  DXL_MODE       = OP_POSITION;

// --- 모터 ID ---
static const uint8_t DXL_ID_1 = 1;
static const uint8_t DXL_ID_2 = 2;
static const uint8_t DXL_ID_3 = 3;
static const uint8_t DXL_ID_4 = 4;
static const uint8_t DXL_ID_5 = 5;

// --- 모터별 준비/파쇄 위치 (도, UNIT_DEGREE) ---

// [config.h 파일]

// --- 모터별 준비/파쇄 위치 (도, UNIT_DEGREE) ---
// (예시 값이므로 실제 로봇에 맞게 수정하세요)

// 1번 모터
static const float DXL_POS_READY_1 = 0.0f;
static const float DXL_POS_SMASH_1 = 120.0f;

// 2번 모터 (정방향 +60도)
static const float DXL_POS_READY_2 = 360.0f;
static const float DXL_POS_SMASH_2 = 180.0f;

// 3번 모터 (정방향 +60도)
static const float DXL_POS_READY_3 = 340.0f;
static const float DXL_POS_SMASH_3 = 300.0f;

// 4번 모터 (반대방향 -60도)
static const float DXL_POS_READY_4 = 0.0f;
static const float DXL_POS_SMASH_4 = 180.0f; // <- 수정된 부분

// 5번 모터 (반대방향 -60도)
static const float DXL_POS_READY_5 = 0.0f;
static const float DXL_POS_SMASH_5 = 40.0f; // <- 수정된 부분

// ==== FSM 주기 및 순차 제어 ====
static const uint16_t FSM_TICK_MS = 50; // 20Hz
static const unsigned long TRIGGER_DELAY_MS = 500; // 트리거 지연 필터

// --- 파쇄 시퀀스 타이밍 ---
// (예: STEP1 실행 후 STEP2까지 300ms 대기)
static const uint32_t SMASH_GROUP_GAP_MS = 1500;
// (예: 마지막 STEP3 실행 후 복귀까지 800ms 대기)
static const uint32_t SMASH_FINAL_PULSE_MS = 800; 


// (이하 센서, 통신, IMU 설정은 기존과 동일)
// ==== 센서 핀 (보드에 맞게 필요시 조정) ====
static const int PIN_TOUCH_1 = 51;
static const int PIN_TOUCH_2 = 63;
static const int PIN_OPT     = 59;  // 광학센서: HIGH 감지 가정
static const int PIN_TRIG    = 55;
static const int PIN_ECHO    = 61;
static const int PIN_TOUCH_WATER = 57;

// ==== 초음파 임계 ====
static const float ULTRA_TH_CM = 15.0f;

// ==== 통신 ====
#define RPI_SERIAL Serial
static const uint32_t HEARTBEAT_TIMEOUT_MS = 3000; // ms

// ==== IMU/기울기 ====
static const float TILT_ALLOW_DEG = 30.0f; // ±30deg 허용





