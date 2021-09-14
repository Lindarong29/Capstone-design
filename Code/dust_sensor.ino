
#include <Wire.h>

#define no_dust 0.35 //먼지 없을 때 초기값

int state=0; //현재 상태
int pre_state=0; //이전 상태

//미세먼지
int vo = A5; //input핀 - A5
int v_led = 2; //미세먼지 내부 led - D2
float vo_value = 0; //센서가 Read 한 Analog value (0~1023)
float Voltage = 0; //Analog -> Voltage 변환 값
float dust_density = 0; //미세먼지 농도 값
float dust_sum = 0; //미세먼지 값 저장 (10번 측정)
int dust_avg = 0; //

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); //시리얼 통신 시작
  pinMode(v_led,OUTPUT);
  pinMode(vo,INPUT);
}


void loop() {
  // put your main code here, to run repeatedly:

  
  for(int i=0; i<11; i++) { //10번 측정
    
  //미세먼지 측정
    digitalWrite(v_led, LOW); // 미세먼지 센서 내의 LED ON
    delayMicroseconds(280); // 280us Delay
    vo_value = analogRead(A5); // 미세먼지 측정 값 READ (Analog)
    delayMicroseconds(40); // 40us Delay
    digitalWrite(v_led, HIGH); // 미세먼지 센서 내의 LED OFF
    delayMicroseconds(9680);  // 9680us Delay

    Voltage = vo_value * 5.0 / 1023.0 ; // 아날로그 값을 전압으로 변환
    dust_density = (Voltage - no_dust) / 0.005;  // 전압 이용하여 미세먼지 농도 계산

    dust_sum += dust_density; //측정한 미세먼지 값 저장

    delay(500);
  }
  
  
  //10번 측정한 값 평균 구함
 
  dust_avg = dust_sum/10;

  if(dust_sum <200) { //기준에 맞으면 state=0
    state = 0;
    Serial.println("Weather is good for 5s");
  }

  if(state != pre_state) { //상태가 바뀌면 동작
    if(state==1){ //보호창 올리기 위해 모터 시계방향 회전
      stepper.step(stepvalue);
      Serial.println("close");
    }
    else{ //보호창 내리기 위해 모터 반시계방향 회전
      stepper.step(-stepvalue);
      Serial.println("close");
    }
    delay(1000);
    state = pre_state; //측정값은 이제 과거의 값이 됨!
    delay(500);
  }


}
