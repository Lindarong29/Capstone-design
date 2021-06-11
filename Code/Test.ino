#define no_dust 0.35 // 초기값

#include <Stepper.h>
const int stepvalue=2048;
Stepper stepper(stepvalue, 8, 6, 7, 5); // stepper객체 설정
// 연결 핀 : IN4, IN2, IN3, IN1 

int v_led=4; // 미세먼지 검출 led D4

int state=0, now_state=0; // 창문이 닫혀있으면 0, 열려있으면 1

void setup() 
{
  pinMode(v_led, OUTPUT);
  stepper.setSpeed(15); // 모터 속도
  Serial.begin(9600); // 시리얼 통신 시작
}

void loop() 
{
  digitalWrite(v_led, LOW);
  delayMicroseconds(280);
  float vo_value = analogRead(A5); // 미세먼지 데이터 읽기 (A5)
  delayMicroseconds(40);
  digitalWrite(v_led, HIGH);
  delayMicroseconds(9680);

  float dust = get_dust_density(get_voltage(vo_value)); // 아날로그 값을 미세먼지 농도로 변환
  int rain_value = analogRead(A4); // 빗방울 데이터 읽기 (A4)

  Serial.print("rain - ");
  Serial.print(rain_value);
  Serial.print(", dust - ");
  Serial.print(dust);
  
  if(dust > 400 || rain_value < 500) state=1; // 하나라도 감지 되는 순간, state에 1대입
  
  if(now_state==1) { // 창문이 열려 있을때만 - 5초간 데이터 측정
    float dust_sum=0;
    int rain_sum=0;
    
    for(int i=0; i<10; i++) { // 10번 데이터 측정
      rain_value = analogRead(A0);
      rain_sum += rain_value; // 0.5동안 읽어온 데이터를 10번 저장

      digitalWrite(v_led, LOW);
      delayMicroseconds(280);
      float vo_value = analogRead(A5); // 미세먼지 데이터 읽기
      delayMicroseconds(40);
      digitalWrite(v_led, HIGH);
      delayMicroseconds(9680);
      float dust = get_dust_density(get_voltage(vo_value));
      dust_sum += dust;
      
      delay(500);
    }
    rain_sum/=10; // 평균값 구하기
    dust_sum/=10; 
    if(rain_sum > 900 && dust_sum < 200) { // 5초가 측정한 데이터가 기준에 맞으면 state에 0대입
      state=0;
      Serial.println("The weather is fine for 5 seconds");
    }
  }

  if(state != now_state) { // 상태가 바뀌었을 때
    if(state==1) { // 창문을 열기 위해서 시계 방향으로 회전
      stepper.step(stepvalue);
      Serial.println("open");
    }
    else { // 창문을 닫기 위해서 반시계 방향으로 회전
      stepper.step(-stepvalue);
      Serial.println("close");
    }
    delay(1000);
  }
  now_state = state;
  delay(500);
}

float get_voltage(float value)
{
 // 아날로그 값을 전압 값으로 바꿈
 float V= value * 5.0 / 1024; 
 return V;
}

float get_dust_density(float voltage)
{
 // 데이터 시트에 있는 미세 먼지 농도(ug) 공식 기준
 float dust=(voltage-no_dust) / 0.005;
 return dust;
}