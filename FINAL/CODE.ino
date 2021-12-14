//최종코드//

#include <Stepper.h>
#include <SoftwareSerial.h> 
#include <Wire.h>

#define no_dust 0.35 //먼지 없을 때 초기값


int BTstate=0; // BT로 인한 창문 현재 상태 (0 = open / 1 = close)
int state=0; // Sensor로 인한 창문 현재 상태
int pre_state=0; //이전 상태
int data;

//블루투스
int BT_RXD = 4; // 아두이노측 수신부 RXD 2번핀 (HC-06측 송신부 TXD와 연결)
int BT_TXD = 3; // 아두이노측 송신부 TXD 3번핀 (HC-06측 수신부 RXD와 연결)
String input = "";
SoftwareSerial bluetooth(BT_RXD, BT_TXD);   // 소프트웨어 시리얼 bluetooth 객체 선언



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
const int stepsPerRevolution=2048;
Stepper myStepper (stepsPerRevolution,11,9,10,8);  // 스텝모터 연결
// 연결 핀 : IN4, IN2, IN3, IN1

void manualMode_open();
void manualMode_close();
void autoMode();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); //시리얼 통신 시작
  bluetooth.begin(9600);  // 블루투스 통신 시작
  myStepper.setSpeed(10); //모터 속도
  pinMode(v_led,OUTPUT);
  pinMode(vo,INPUT);
}

void loop() {

  if(bluetooth.available()>0) {

    data=bluetooth.read();
    
    // close(c) 입력이 들어오고 보호창이 내려간 상태면, 보호창 올려
    if (data == 'c' && state == 0) {
      manualMode_close();
    }
    // open(o) 입력이 들어오고 보호창이 올라간 상태면, 보호창 내려
    else if (data == 'o' && state == 1) {
      manualMode_open();
    }

    // auto(a) 입력이 들어오면 센서측정해서 보호창 구동
    else if (data == 'a') {
      Serial.println ("Sensor_Mode");
      bluetooth.println ("Sensor_Mode");
      autoMode(); 
    }
  }
}



void manualMode_open(){
  myStepper.step(stepsPerRevolution*3); //motor 돌아가는 코드
  Serial.println ("BT-window open/"); //비교 전
  Serial.print ("state : ");
  Serial.println (state);

  bluetooth.println ("BT-window open/"); //비교 전
  bluetooth.print ("state : ");
  bluetooth.println (state);
  
  state=0; // 열린 상태로 변경
  pre_state = state;
}


void manualMode_close(){
  myStepper.step(-stepsPerRevolution*3); //motor 돌아가는 코드
  Serial.println ("BT-window close/"); 
  Serial.print ("state : ");
  Serial.println (state);

  bluetooth.println ("BT-window close/"); //비교 전
  bluetooth.print ("state : ");
  bluetooth.println (state);
  
  state=1; //닫힌 상태로 변경
  pre_state = state;
}


void autoMode(){
    while(data == 'a'){
 
    //센서값 측정
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

    delay(100);
    
    
  //Rain 측정
    rain_value = analogRead(A4); //빗방울 센서 READ (A4)
    rain_sum += rain_value; //측정한 빗물센서 값 저장

    delay(100);
 
 }
  
  
   //10번 측정한 값 평균
  dust_avg = dust_sum/10;
  rain_avg = rain_sum/10; //평균값
  
  
  // 센서값 출력
  Serial.print ("dust_avg : ");
  Serial.println (dust_avg);
  
  Serial.print ("rain_avg : ");
  Serial.println (rain_avg);

  bluetooth.print ("dust_avg : ");
  bluetooth.println (dust_avg);
  
  bluetooth.print ("rain_avg : ");
  bluetooth.println (rain_avg);

  
  //state 변경
  if(dust_avg<200 && rain_avg>800) { //기준 이하면 
    // 빗물센서에 빗물 있으면 값이 떨어짐.
    state = 0; //Open상태
    Serial.println ("Weather is good :)");
    bluetooth.println ("Weather is good :)");
  }

  else { //기준 이상이면
    state = 1; //close
    Serial.println ("Weather is bad :(");
    bluetooth.println ("Weather is bad :(");
  }
  


  
 //pre_state 와 state 비교하여 모터 동작
  if(state != pre_state) {
      if (state==1) { //pre_state=0, state=1
       

        myStepper.step(-stepsPerRevolution*3);
        Serial.println ("Close");
        bluetooth.println ("Close");
        delay (500);
        
      
      }
    
       else if(state==0) { //pre_state=1, state=0
        
        myStepper.step(stepsPerRevolution*3);
        Serial.println ("Open");
        bluetooth.println ("Open");
        delay (500);
     
       }
  }
  
  else {
    // state same. nothing
  }
   
  delay (1000);

  //모터 구동 끝난 후 현재 상태 업데이트
  pre_state = state;
  delay (500);
    
    
  Serial.println ("END");
  Serial.println ("-----------------------");

  bluetooth.println ("END");
  bluetooth.println ("-----------------------");
  
  //센서값 초기화
  dust_sum = 0;
  rain_sum = 0;

  delay (5000);
  delay (5000);

  if(bluetooth.available()>0) 
    {
      int data;
      data = bluetooth.read();
      if(data=='s'){
        Serial.println ("Sensor stop");
        bluetooth.println ("Sensor stop");
        break;
      }
    }
 
  }
}
