#include <HuemonelabKit.h>

Stepper stepper(8,9,10,11);

void setup() {
  Serial.begin(9600);
  stepper.setSpeed(10);
  stepper.setDir(1);
  }

void loop() {
  stepper.move(1080);
  Serial.println("open");
  delay(1000);

  
  stepper.move(-1080);
  Serial.println("close");
  delay(1000);
}
