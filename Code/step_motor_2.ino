#include <Stepper.h>

const int stepsPerRevolution = 2048;

Stepper myStepper(stepsPerRevolution, 8,7,6,5);

void setup() {
  myStepper.setSpeed(15); 
}

void loop() {
  myStepper.step(stepsPerRevolution);
  Serial.println("open");
  delay(1000);

  myStepper.step(-stepsPerRevolution);
  Serial.println("close");
  delay(1000);
}
