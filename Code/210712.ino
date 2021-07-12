//0712센서 측정 -> 시리얼출력까지

#include <Wire.h>

#define no_dust 0.35 //먼지 없을 때 초기값

//미세먼지
int vo = A5; //OUT핀 - A5
int v_led = 2; //미세먼지 내부 led - D2

float vo_value = 0; //센서가 Read 한 Analog value (0~1023)
float Voltage = 0; //Analog -> Voltage 변환 값
float dust_density = 0; //미세먼지 농도 값

//빗방울
int rain_value = 0 ; //빗방울 데이터

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); //시리얼 통신 시작
  pinMode(v_led,OUTPUT);
  pinMode(vo,INPUT);
}


void loop() {
  // put your main code here, to run repeatedly:

  //미세먼지 측정
  digitalWrite(v_led, LOW); // 미세먼지 센서 내의 LED ON
  delayMicroseconds(280); // 280us Delay
  vo_value = analogRead(A5); // 미세먼지 측정 값 READ (Analog)
  delayMicroseconds(40); // 40us Delay
  digitalWrite(v_led, HIGH); // 미세먼지 센서 내의 LED OFF
  delayMicroseconds(9680);  // 9680us Delay

  Voltage = vo_value * 5.0 / 1023.0 ; // 아날로그 값을 전압으로 변환
  dust_density = (Voltage - no_dust) / 0.005;  // 전압 이용하여 미세먼지 농도 계산


  //빗방울센서 측정
  rain_value = analogRead(A4); //빗방울 센서 READ (A4)


  //시리얼 출력
  Serial.print("Dust Density : ");
  Serial.print(dust_density);
  Serial.print(", rain : ");
  Serial.print(rain_value);

  

  
}
