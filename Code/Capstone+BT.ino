
#include <Wire.h>
// #include <Stepper.h>
#include <HuemonelabKit.h>
#include <SoftwareSerial.h>     // 소프트웨어 시리얼 라이브러리 불러옴

#define no_dust 0.35 //먼지 없을 때 초기값

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
// 연결 핀 : IN4, IN2, IN3, IN1

//Bluetooth
int BT_RXD = 2; // 아두이노측 수신부 RXD는 2번핀(HC-06측 송신부 TXD와 연결)
int BT_TXD = 3; // 아두이노측 송신부 TXD는 3번핀(HC-06측 수신부 RXD와 연결)
String input = "";
SoftwareSerial bluetooth(BT_RXD, BT_TXD);   // 소프트웨어 시리얼 bluetooth 객체 선언
String get_string(String input);     // 데이터 수신 함수 선언

//------------------------------------------------------------------------------

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); //시리얼 통신 시작
  bluetooth.begin(9600);  // 블루투스 통신 시작
  pinMode(v_led,OUTPUT);
  pinMode(vo,INPUT);
  stepper.setSpeed(10); //모터 속도
  stepper.setDir(1);
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

    delay(100);
    
    
  //Rain 측정
    rain_value = analogRead(A4); //빗방울 센서 READ (A4)
    rain_sum += rain_value; //측정한 빗물센서 값 저장

    delay(100);
 
 }
  
  
   //10번 측정한 값 평균
  dust_avg = dust_sum/10;
  rain_avg = rain_sum/10; //평균값
  
  
  // TEST
  Serial.print ("dust_avg : ");
  Serial.println (dust_avg);
  
  Serial.print ("rain_avg : ");
  Serial.println (rain_avg);

  
  //state 변경
  if(dust_avg<400 && rain_avg>400) { //기준 이하면 
    // 빗물센서에 빗물 있으면 값이 떨어짐.
    state = 0; //Open상태
    Serial.println ("Weather is good for 5s");
  }

  else { //기준 이상이면
    state = 1; //close
    Serial.println ("Weather is bad for 5s");
  }
  
  
  
 //pre_state 와 state 비교하여 동작
  if(state != pre_state) {
      if (state==1) { //pre_state=0, state=1

        stepper.move(-1080); //보호창 올리기 위해 모터 회전
        Serial.println ("Close");
        delay (500);
        
      }
    
     else { //pre_state=1, state=0
        stepper.move(1080); //보호창 내리기 위해 모터 회전
        Serial.println ("Open");
        delay (500);
     }
  }
  
  else {
    // state same. nothing
  }
   
  delay (1000);
  
  pre_state = state;
  delay (500);
    
    
  Serial.println ("END");
  Serial.println ("-----------------------");
  dust_sum = 0;
  rain_sum = 0;

  delay (5000);

  //------------------------------------
  input = get_string(input);     // 스마트폰으로부터 문자열을 가져옴(입력받지 않은 경우 입력 문자열 input을 그대로 유지)
   
  // input은 1이 올리고 0이 내리라는 의미
  
  /* 여기선 보호창 올라가 있는 상태가 1이고, 내려간 상태가 0이라고 했는데 
  이전 코드에서 어떻게 했는지 기억이 안나!!확인하고 바꿔야 해!
  state 를 통일시킵시다 ~_~ */
  
  // 만약 1의 입력이 들어오고 보호창이 내려간 상태면, 보호창 올려 (input을 int로 변환함)
  if (input.toInt() == 1 ) { //&& state == 0
        stepper.move(-1080); //보호창 올리기 위해 모터 회전
        Serial.println ("Close");
        delay (500);
  }
  
  // 만약 0의 입력이 들어오고 보호창이 올라간 상태면, 보호창 내려
  else if (input.toInt() == 0 ) { //&& state == 1
        stepper.move(1080); //보호창 내리기 위해 모터 회전
        Serial.println ("Open");
        delay (500);
  }
 
}


//--------------------------------------------------------


String get_string(String input){
  String inputString = "";             // 아무것도 입력하지 않았을 때 빈 문자열
  while(bluetooth.available()) {      // 블루투스 통신에 데이터가 입력되면(스마트폰 -> 아두이노)
    delay(3);                         // 데이터 수신이 잘 이루어지기 위해 3ms 기다림
    if (bluetooth.available() >0) {   // 수신된 데이터가 있으면
      char c = bluetooth.read();      // 수신된 데이터 1개를 받아 c로 저장(이후에는 그다음걸 받아 c로 저장
      inputString += c;                // 수신된 데이터를 c와 붙임. 
    }
  }
  if(inputString.length() == 0) return input;    // 문자열 길이가 0이면(데이터를 입력받지 않았다면) 기존의 값을 반환  
  else return inputString;             // 데이터를 입력받았다면 해당 문자열을 출력
}
