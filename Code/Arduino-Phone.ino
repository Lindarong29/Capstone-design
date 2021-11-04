/*
 *  Arduino -> Phone
 *  아두이노 -> 스마트폰 센서값 전달
 *
 *  Window 치면 현재 창문상태
 *  Weather 치면 현재 비 / 미세먼지 상태
 */

#include <SoftwareSerial.h>     // 소프트웨어 시리얼 라이브러리 불러옴
 
int BT_RXD = 2;    // 아두이노측 수신부 RXD는 8번핀(HC-06측 송신부 TXD와 연결)
int BT_TXD = 3;    // 아두이노측 송신부 TXD는 7번핀(HC-06측 수신부 RXD와 연결)
int Rain = A4;     // 빗물센서를 아두이노의 A0에 연결
int vo = A5;       // 미세먼지를 아두이노의 A5에 연결

String input = ""; // 스마트폰으로부터 받을 문자열 변수(현재는 빈 문자열)

SoftwareSerial bluetooth(BT_RXD, BT_TXD);   // 소프트웨어 시리얼 bluetooth 객체 선언
String get_string(String input);

void setup(){
  Serial.begin(9600);     // 시리얼 통신 시작
  bluetooth.begin(9600);  // 블루투스 통신 시작
}
 
void loop(){
  input = get_string(input);          // 스마트폰으로부터 문자열 받음
  
 
  if(input == "Window"){              // 입력받은 문자열이 Window 라면
    bluetooth.print("창문상태 : ");   // 창문의 상태를 출력
    bluetooth.println(state);
    bluetooth.println(" ");
    input = "";                       // 받은 문자열을 빈 문자열로 갱신
  }
  
   if(input == "Weather"){              // 입력받은 문자열이 Weather 라면
     
       //=============센서 기준에 따라 현재의 비/미세먼지 상태===========
        if(rain_avg < 900) {
          String rain_state = X //빗물이 일정 이하면 안 온다는 뜻의 X
        }
        else{
          rain_state = O
        }  
  
        if(dust_avg < 900) {
          String dust_state = GOOD //미세먼지 일정 이하면 날씨 좋다는 뜻의 O
        }
        else{
          dust_state = BAD
        }
      //=================================================================
  
  
    bluetooth.print("비 : ");          // 비 상태를 출력
    bluetooth.println(rain_state);
    bluetooth.print("미세먼지 : ");          // 먼지 상태를 출력
    bluetooth.println(dust_state);
    bluetooth.println(" ");
    input = "";                       // 받은 문자열을 빈 문자열로 갱신
  }
  
}


String get_string(String input){
  String inputString = "";             // 아무것도 입력하지 않았을 때 빈 문자열
  while(bluetooth.available()) {      // 블루투스 통신에 데이터가 입력되면(스마트폰 -> 아두이노)
    delay(3);                         // 데이터 수신이 잘 이루어지기 위해 3ms 기다림
    if (bluetooth.available() >0) {   // 수신된 데이터가 있으면
      char c = bluetooth.read();      // 수신된 데이터 1개를 받아 c로 저장(이후에는 그다음걸 받아 c로 저장
      inputString += c;                // 수신된 데이터를 c와 붙임
    }
  }
  if(inputString.length() == 0) return input;    // 문자열 길이가 0이면(데이터를 입력받지 않았다면) 기존의 값을 반환  
  else return inputString;             // 데이터를 입력받았다면 해당 문자열을 출력
}
