/*
 *  Phone -> Arduino
 *  스마트폰 -> 아두이노 제어
 *  
 *  스위치 모드 이용하여 보호창 작동
 */

// 스텝모터 동작 부분 수정 필요. 일단 모터 동작 확인 뒤에 수정하기


#include <SoftwareSerial.h>     // 소프트웨어 시리얼 라이브러리 불러옴
 
int BT_RXD = 2; // 아두이노측 수신부 RXD는 2번핀(HC-06측 송신부 TXD와 연결)
int BT_TXD = 3; // 아두이노측 송신부 TXD는 3번핀(HC-06측 수신부 RXD와 연결)

Stepper stepper (stepvalue, 8,7,6,5);  // 스텝모터 연결
const int stepvalue=2048;

String input = "";

SoftwareSerial bluetooth(BT_RXD, BT_TXD);   // 소프트웨어 시리얼 bluetooth 객체 선언
String get_string(String input);     // 데이터 수신 함수 선언


void setup(){
  Serial.begin(9600);     // 시리얼 통신 시작
  bluetooth.begin(9600);  // 블루투스 통신 시작
  stepper.setSpeed(15); //모터 속도
}
 
void loop(){
  input = get_string(input);     // 스마트폰으로부터 문자열을 가져옴(입력받지 않은 경우 입력 문자열 input을 그대로 유지)
   
  // input은 1이 올리고 0이 내리라는 의미
  
  /* 여기선 보호창 올라가 있는 상태가 1이고, 내려간 상태가 0이라고 했는데 
  이전 코드에서 어떻게 했는지 기억이 안나!!확인하고 바꿔야 해!
  state 를 통일시킵시다 ~_~ */
  
  // 만약 1의 입력이 들어오고 보호창이 내려간 상태면, 보호창 올려 (input을 int로 변환함)
  if (input.toInt() == 1 && state == 0) {
    stepper.step(stepvalue);
  }
  
  // 만약 0의 입력이 들어오고 보호창이 올라간 상태면, 보호창 내려
  else if (input.toInt() == 0 && state == 1) {
    stepper.step(-stepvalue);
  }
 
}


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
