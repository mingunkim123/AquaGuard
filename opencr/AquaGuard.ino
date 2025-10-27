#include "config.h"
#include "types.h"
#include "Comm.h"
#include "Motor.h"
#include "Sensors.h"
#include "Attitude.h"
#include "FSM.h"

Comm      comm(RPI_SERIAL, 115200);
Motor     motor(DXL_SERIAL, DXL_DIR_PIN, DXL_BAUD, DXL_ID, DXL_MODE);
Sensors   sensors(PIN_TOUCH_1, PIN_TOUCH_2, PIN_OPT,
                  PIN_TRIG, PIN_ECHO, PIN_TOUCH_WATER,
                  ULTRA_TH_CM);
Attitude  attitude(TILT_ALLOW_DEG);
FSM       fsm(motor, sensors, attitude, comm);

void setup() {
  Serial.begin(115200);                 // PC 디버깅
  comm.begin();                         // 라즈베리파이 통신
  motor.begin();                        // 다이나믹셀 준비
  sensors.begin();                      // 센서 핀셋업
  attitude.begin(20.0f);                // 20Hz 업데이트 기준
  motor.moveToReady();                  // 기본 위치
  Serial.println("=== AquaGuard (Modular) Initialized ===");
}

void loop() {
  comm.poll();          // 수신 패킷 처리 + 하트비트 타임스탬프 갱신
  fsm.tick();           // 상태머신 50ms 주기 실행 (내부 delay 포함)
}
