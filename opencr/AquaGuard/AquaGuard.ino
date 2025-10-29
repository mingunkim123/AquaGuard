#include "config.h"
#include "types.h"
#include "Comm.h"
#include "Motor.h"
#include "Sensors.h"
#include "Attitude.h"
#include "FSM.h"

Comm      comm(RPI_SERIAL, 115200);

// --- Motor 객체 5개 생성 (config.h의 설정값 사용) ---
Motor motor1(DXL_SERIAL, DXL_DIR_PIN, DXL_BAUD, DXL_ID_1, DXL_MODE, DXL_POS_READY_1, DXL_POS_SMASH_1);
Motor motor2(DXL_SERIAL, DXL_DIR_PIN, DXL_BAUD, DXL_ID_2, DXL_MODE, DXL_POS_READY_2, DXL_POS_SMASH_2);
Motor motor3(DXL_SERIAL, DXL_DIR_PIN, DXL_BAUD, DXL_ID_3, DXL_MODE, DXL_POS_READY_3, DXL_POS_SMASH_3);
Motor motor4(DXL_SERIAL, DXL_DIR_PIN, DXL_BAUD, DXL_ID_4, DXL_MODE, DXL_POS_READY_4, DXL_POS_SMASH_4);
Motor motor5(DXL_SERIAL, DXL_DIR_PIN, DXL_BAUD, DXL_ID_5, DXL_MODE, DXL_POS_READY_5, DXL_POS_SMASH_5);

// --- 나머지 객체 생성 ---
Sensors   sensors(PIN_TOUCH_1, PIN_TOUCH_2, PIN_OPT,
                  PIN_TRIG, PIN_ECHO, PIN_TOUCH_WATER,
                  ULTRA_TH_CM);
Attitude  attitude(TILT_ALLOW_DEG);

// --- FSM 생성자에 모터 5개 전달 ---
FSM       fsm(motor1, motor2, motor3, motor4, motor5, 
              sensors, attitude, comm);

void setup() {
  Serial.begin(115200);
  comm.begin();
  
  // --- 모터 5개 모두 초기화 및 준비 위치로 ---
  motor1.begin();
  motor2.begin();
  motor3.begin();
  motor4.begin();
  motor5.begin();
  
  sensors.begin();
  attitude.begin(20.0f); // 20Hz

  // --- 5개 모터 모두 준비 위치로 ---
  motor1.moveToReady();
  motor2.moveToReady();
  motor3.moveToReady();
  motor4.moveToReady();
  motor5.moveToReady();
  
  Serial.println("=== AquaGuard (Modular, 5 DXL) Initialized ===");
}

void loop() {
  comm.poll();
  fsm.tick(); // 상태머신 50ms 주기 실행 (내부 delay 포함)
}
