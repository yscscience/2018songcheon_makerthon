#include <Servo.h> 
#include <Wire.h> 
#include <SoftwareSerial.h> 
#include <LiquidCrystal_I2C.h>     //LiquidCrystal 라이브러리 추가  
LiquidCrystal_I2C lcd(0x3F, 16, 2);  //lcd 객체 선언 
Servo door; //문에 달은 서보모터 
Servo window; //창문 제어용 서보모터 
int pos = 0; //서보모터 각도제어용 변수 
int rainsensor=A0; //빗물감지센서가 입력되는 핀 번호 
int pir=7; //동작감지용 핀번호 

// -----미세먼지-----// 
int indust=A1; //실내 공기질 측정 
int outdust=A2; //실외 공기질 측정 
float invalue=0; //실내 공기질 값 
float outvalue=0; //실외 공기질 값 
// -----미세먼지----- // 

void setup() { 
  pinMode(11,OUTPUT); //LED 
  pinMode(rainsensor,INPUT); //빗물감지 
  pinMode(pir,INPUT);// 동작감지 
 
  //미세먼지// 
  pinMode(indust,INPUT); //미세먼지 
  pinMode(outdust,INPUT); //미세먼지 
  pinMode(4,OUTPUT);//in dust 
  pinMode(8,OUTPUT);//out dust 
  //미세먼지// 

  pinMode(1,OUTPUT); 
  pinMode(0,OUTPUT); 
  //1세트 

  pinMode(13,OUTPUT); 
  pinMode(12,OUTPUT); 
  //2세트 

  pinMode(10,OUTPUT); 
  door.attach(6); 
  window.attach(5); 
  //for 서보모터, 창문과 문(차례대로) 
 
  lcd.init(); 
  lcd.backlight();  
  //LCD초기화 
 
  Serial.begin(9600); 
  pinMode(9,INPUT_PULLUP); //플로팅 현상 막기위함
} 
 
 
 
void loop() { 
  /* 
  LED(); 
  FanOn(); 
  delay(5000); 
  시현시에만 사용 
  */ 

  TimeScore(); //5분 카운트 함수 
  WindowOpen(); //창문 열기 
  LEDO(); //불끄기 
  Door(); //교실문 잠그기 
  //FanShut(); 시현시에만 사용 
  unsigned long entireTime=millis(); //40분 측정위해 사용 
  while(1){ 
    if(Rainfall()==1){ 
      WindowShut(); 
    } //비가오면 창문닫기 
    if(Pir()== HIGH){ 
      LED(); 
    } //중간에 반에 들릴때를 대비해서 사람이 들어오면 LED켜기ㅠ 
    else{ 
        LEDO(); 
    } 
    if(digitalRead(9)==LOW){ 
      LCD(); 
    } 
    unsigned long time1=millis(); 
    Serial.println(time1-entireTime); 
    if(time1-entireTime>=2400000){ //40분 
      break; 
    } //40분이 되면 이 반복문 탈출 
  } 
  WindowShut(); //창문닫기 
  FanOn(); //선풍기, 온/냉방기 켜기 
  Door1(); //문 열기 
  LED(); //LED 켜기 
  /*  
   *  lcd.setCursor(2,0); 
   *  lcd.print("Fin"); 
   *  delay(1000000000); 
   *  시연시에만 사용 
   */ 
 
} 
 
void TimeScore(){ //5분 카운트 함수 
  unsigned long time1=millis(); 
 while(1){
    if(Pir() == HIGH){ 
        time1=millis(); 
    } //만약 중간에 사람이 들어오면 다시 5분 카운트 
    if(digitalRead(9)==LOW){ 
        LCD(); 
    } //안과 밖의 공기질 표시 
    unsigned long time2=millis(); 
    if(time2-time1>=300000){ 
        break; 
    } //5분이 지나면 함수 종료 
   }  
} 
 
void WindowShut(){ //서보모터 이용한 창문 닫기 
  for(pos = 180; pos > 90; pos -= 10)   
  {                                   
    window.write(pos);               
    delay(15);                        
  } 
} 
void WindowOpen(){ //서보모터 이용한 창문 열기  
  if(Rainfall()==0){ //비가 오지 않을 경우 
    for(pos = 90; pos <180; pos += 10)   
    {                                   
      window.write(pos);               
      delay(15);                     
   } 
  } 
} 
void FanShut(){ //냉/난방기 전원 공급 끄기 
  digitalWrite(10,LOW); 
} 
void FanOn(){ //냉/난방기 전원 공급 
  digitalWrite(10,HIGH); 
} 
void LED(){//LED켜기 
  digitalWrite(11,HIGH); 
} 
void LEDO(){ //LED끄기  
  digitalWrite(11,LOW); 
} 
void Door(){ //문열기 
  for(pos = 0; pos < 90; pos += 10)   
  {                                   
    door.write(pos);               
    delay(15);                     
  } 
} 
void Door1(){ //문 닫기  
  for(pos = 90; pos >0; pos -= 10)   
  {                                  
    door.write(pos);               
    delay(15);                
  } 
} 
int Rainfall(){ //비가 내리는지 여부 
  int result=0; 
  int rain;   
    if(Serial.available()){ 
      rain=analogRead(rainsensor); 
      delay(10); 
      if(rain >= 130){ 
        result =1; //비가 오면 1을 반환 
      } 
      else{ 
        result=0; //비가 안오면 0을 반환 
      }
    } 
  return result; 
} 
int InAirPollute(){ //실내 공기질 측정 
  digitalWrite(4,LOW); 
  delayMicroseconds(280); 
  invalue=analogRead(indust); 
  delayMicroseconds(40); 
  digitalWrite(4,HIGH); 
  delayMicroseconds(9680); 
  Serial.println(invalue); 
} 
int OutAirPollute(){ //실내 공기질 측정 
  digitalWrite(8,LOW); 
  delayMicroseconds(280); 
  outvalue=analogRead(outdust); 
  delayMicroseconds(40); 
  digitalWrite(8,HIGH); 
  delayMicroseconds(9680); 
  Serial.println(outvalue); 
}
int Pir(){   //동작감지 여부 반환 감지되면 HIGH 
  int val=digitalRead(pir); 
  return val; 
} 
void LCD(){ //안과 밖의 공기질 측정후 LCD표기 
  InAirPollute(); 
  OutAirPollute();
 
  lcd.setCursor(2,0); //LCD에서 2,0의 좌표값에 안쪽 공기질 표시 
  lcd.print(" IN: "); 
  lcd.print(invalue); 
  lcd.setCursor(2,1); //LCD에서 2,1의 좌표값에 안쪽 공기질 표시 
  lcd.print("OUT: "); 
  lcd.print(outvalue); 
  delay(10000); 
  lcd.clear();    
}; 
