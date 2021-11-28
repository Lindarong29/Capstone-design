//함수로 만들어 + 블투 + 모드 0 1 2

#include <Wire.h>
#include <HuemonelabKit.h>
#include <SoftwareSerial.h>     // 소프트웨어 시리얼 라이브러리 불러옴

#define BT_RXD 4
#define BT_TXD 3
SoftwareSerial bluetooth(BT_RXD, BT_TXD);

#define no_dust 0.6 //먼지 없을 때 초기값

int state=0; //현재 상태 (0 = open / 1 = close)
int pre_state=0; //이전 상태

//미세먼지 Dust
int vo = A5; //input핀 - A5
int v_led = 2; //미세먼지 내부 led - D2
float vo_value = 0; //센서가 Read 한 Analog value (0~1023)
float Voltage = 0; //Analog -> Voltage 변환 값
float dust_density = 0; //미세먼지 농도 값
float dust_sum = 0; //미세먼지 값 저장 (10번 측정)
float dust_avg = 0; // (dust_avg/10)

//Rain
float rain_value = 0; //빗방울 데이터
float rain_sum = 0; //빗방울 값 저장 (10번)
float rain_avg = 0; //평균값

//Step motor
Stepper stepper(8,9,10,11);

static int MOTOR_CASE = 0; // 모터제동 bool

void Dust();
void Rain();
void State_change();
void State_case();
void Motor_run();


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); //시리얼 통신 시작
  bluetooth.begin(9600);
  pinMode(v_led,OUTPUT);
  pinMode(vo,INPUT);
  stepper.setSpeed(10); //모터 속도
  stepper.setDir(1);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(bluetooth.available()){
    char val = bluetooth.read();
    Serial.write(bluetooth.read());
    Serial.println (val);
      if (val == '0'){ //센서 동작모드
         Dust();
         Rain();
  
         dust_avg = dust_sum/10;
         rain_avg = rain_sum/10; //평균값
  
         Serial.print ("dust_avg : ");
         Serial.println (dust_avg);
  
         Serial.print ("rain_avg : ");
         Serial.println (rain_avg);

         State_change();
         State_case();

         Motor_run();
  
         pre_state = state;
         delay (500);

         Serial.println ("END");
         Serial.println ("-----------------------");
         dust_sum = 0;
         rain_sum = 0;

         delay (10000);
      }
      
      else if (val == '1'){ //사용자 동작모드 (OPEN)
       state=0;
       MOTOR_CASE=4;
       Motor_run();
         pre_state = state;
         delay (500);

         Serial.println ("END");
         Serial.println ("-----------------------");
      }
      
      else if (val == '2'){ //사용자 동작모드 (CLOSE)
       state=1;
       MOTOR_CASE=5;
       Motor_run();
         pre_state = state;
         delay (500);

         Serial.println ("END");
         Serial.println ("-----------------------");
      }
  }
}

//--------------------------------------------------

void Dust() { //미세먼지 측정
    for(int i=0; i<11; i++) { //10번 측정
      digitalWrite(v_led, LOW); // 미세먼지 센서 내의 LED ON
      delayMicroseconds(280); // 280us Delay
      vo_value = analogRead(A5); // 미세먼지 측정 값 READ (Analog)
      delayMicroseconds(40); // 40us Delay
      digitalWrite(v_led, HIGH); // 미세먼지 센서 내의 LED OFF
      delayMicroseconds(9680);  // 9680us Delay

      Voltage = vo_value * 5.0 / 1023.0 ; // 아날로그 값을 전압으로 변환
      dust_density = (Voltage - no_dust) / 0.005;  // 전압 이용하여 미세먼지 농도 계산
  
      dust_sum += dust_density; //측정한 미세먼지 값 저장

      delay(100);
    }
}

void Rain() { //빗물 측정
    for(int i=0; i<11; i++) { //10번 측정
      rain_value = analogRead(A4); //빗방울 센서 READ (A4)
      rain_sum += rain_value; //측정한 빗물센서 값 저장

      delay(100);
    }
}

void State_change(){
  //state 변경
  if(dust_avg<400 && rain_avg>400) { //기준 이하면 
    // 빗물센서에 빗물 있으면 값이 떨어짐.
    state = 0; //Open상태
    Serial.println ("Weather is good :)");
  }

  else if (dust_avg>400 && rain_avg<400) { //기준 이상이면
    state = 1; //close
    Serial.println ("Weather is bad :(");
  }
}

void State_case(){
   //pre_state 와 state 비교하여 동작
  if(state != pre_state) {
      if (state==1) { //pre_state=0, state=1
        MOTOR_CASE=1;
      }
    
     else if (state==0){ //pre_state=1, state=0
        MOTOR_CASE=2;
     }
  }
  
  else if (state == pre_state) {
    // state same
        MOTOR_CASE=3;
  }
   
  delay (1000);
}

void Motor_run(){
  switch(MOTOR_CASE) {
    case 1:
        stepper.move(1080); //보호창 올리기 위해 모터 회전
        Serial.println ("Close");
        delay (500);
    break;

    case 2:
        stepper.move(-1080); //보호창 내리기 위해 모터 회전
        Serial.println ("Open");
        delay (500);
    break;

    case 3:
        stepper.move(0); //모터 회전 없음
    break;

    case 4: //블루투스로 열었을 때
      if (state ==0 && pre_state ==1){
        stepper.move(-1080); //보호창 내리기 위해 모터 회전
        Serial.println ("BT Open");
        delay (500);
      }
      else {
        MOTOR_CASE=3;
      }
    break;

    case 5: //블루투스로 닫았을 때
      if (state ==1 && pre_state==0){
        stepper.move(1080); //보호창 올리기 위해 모터 회전
        Serial.println ("BT Close");
        delay (500);
      }
      else {
        MOTOR_CASE=3;
      }
    break;

    default:
    break;
  }
}
