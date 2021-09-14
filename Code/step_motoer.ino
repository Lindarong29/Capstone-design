#include <Stepper.h>

const int stepvalue=2048;

Stepper stepper(stepvalue, 8, 7, 6, 5); // stepper객체 설정
// 연결 핀 : IN4, IN2, IN3, IN1

void setup () {
  stepper.setSpeed(15); // 모터 속도
}

void loop () {
  stepper.step(stepvalue);
  Serial.println("open");
  delay (1000);
  
  stepper.step(-stepvalue);
  Serial.println("close");
  delay (1000);
}
