# include <Wire.h>

int rain_value = 0; //빗방울 데이터
int rain_sum = 0; //빗방울 값 저장 (10번)
int rain_avg = 0; //ㅍㅕㅇㄱㅠㄴㄱㅏㅂㅅ
int pre_state = 0;
int state = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); //시리얼 통신 시작

}

void loop() {
  // put your main code here, to run repeatedly:

  for(int i=0; i<11; i++) { //10번 측정
    rain_value = analogRead(A4); //빗방울 센서 READ (A4)
    rain_sum += rain_value; //측정한 빗물센서 값 저장
  }

  Serial.print ("rain_sum : ");
  Serial.print (rain_sum);

  rain_avg = rain_sum/10; //평균값
  Serial.print ("rain_sum/10 = rain_avg : "); //avg
  Serial.print (rain_avg);


//state 변경
  if(rain_avg < 900) { //기준 이하면
    state = 0 //Open상태
    Serial.print ("Weather is good for 5s");
  }

  else { //기준 이상이면
    state = 1
  }
  
  Serial.print ("---------before----------"); //비교 전
  Serial.print ("state : ");
  Serial.print (state);
  
  Serial.print ("pre_state : ");
  Serial.print (pre_state);
  
  Serial.print ("-------------------------");

  
//pre_state 와 state 비교하여 동작
  if(state != pre_state) {
      if (state==1) { //pre_state=0, state=1
        Serial.print ("Motor Right"); //보호창 올리기 위해 모터 회전
        Serial.print ("Close");
      }
    
     else { //pre_state=1, state=0
        Serial.print ("Motor Left"); //보호창 내리기 위해 모터 회전
        Serial.print ("Open");
      }
  
  else {
    // state same. nothing
  }
    
    
  delay (1000);
  pre_state = state;
  delay (500);
    
  Serial.print ("---------after---------"); //비교 후
  Serial.print ("state : ");
  Serial.print (state);
  
  Serial.print ("pre_state : ");
  Serial.print (pre_state);
  Serial.print ("-----------------------");
  
  Serial.print ("END");
  }
  
}
