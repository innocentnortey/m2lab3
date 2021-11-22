
#include "IRremote.h"
#include "IR.h"
#include <Wire.h>
#include <DS3231.h>
#include <LiquidCrystal.h>
#define ENABLE 5
#define DIRA 3
#define DIRB 4

int pinPush = 2;
int state = 0;
bool buttonPushed = false;
String dir = "C";
String fanSpeed = "FULL";
int speedDef[4]  = {0,127, 191, 255};
int speedState = 1;

LiquidCrystal lcd(7, 8, 9, 10, 11, 12);
DS3231 clock;
RTCDateTime dt;

IRrecv irrecv(13);     // create instance of 'irrecv'
decode_results results;      // create instance of 'decode_results'

void setup() {
  lcd.begin(16, 2);
  Serial.begin(9600);
  Serial.println("IR Receiver Button Decode");
  irrecv.enableIRIn();
  clock.begin();
  clock.setDateTime(__DATE__, __TIME__);
  pinMode(ENABLE,OUTPUT);
  pinMode(DIRA,OUTPUT);
  pinMode(DIRB,OUTPUT);
  pinMode(pinPush, INPUT_PULLUP);
  digitalWrite(DIRA,HIGH); //one way
  digitalWrite(DIRB,LOW);
  digitalWrite(ENABLE,HIGH); // enable on
  dir = "C";
}

void loop()
{ 
  dt = clock.getDateTime();
  int tmpValue;
  bool pushButton = !digitalRead(pinPush);
  if(pushButton && !(buttonPushed)){
    buttonPushed = true;
    state++;
    digitalWrite(DIRA,LOW);  //reverse
    digitalWrite(DIRB,HIGH);
    dir = "CC";
    if(state>1){
      state = 0;
      digitalWrite(DIRA,HIGH); //one way
      digitalWrite(DIRB,LOW);
      dir = "C";
    }
  }
  if(!pushButton){
    buttonPushed = false;
  }
  
  if (irrecv.decode(&results)) // have we received an IR signal?
  {
    for (int i = 0; i < 23; i++)
    {
      if ((keyValue[i] == results.value) && (i<KEY_NUM))
      {
        Serial.println(keyBuf[i]);
        tmpValue = results.value;

        if(String(keyBuf[i]).compareTo("UP")==0){
          speedState++;
        }
        if(String(keyBuf[i]).compareTo("DOWN")==0){
          speedState--;
        }
      }
      else if(REPEAT==i)
      {
        results.value = tmpValue;
      }
    }
    irrecv.resume(); // receive the next value
  }


  if(speedState>3){
    speedState = 0;  
  }
  if(speedState<0){
    speedState = 3;  
  }  
  switch(speedState){
    case 0:
      fanSpeed = "0";
      break;
    case 1:
      fanSpeed = "1/2";
      break;
    case 2:
      fanSpeed = "3/4";
      break;
    case 3:
      fanSpeed = "FULL";
      break;
  }
  analogWrite(ENABLE,speedDef[speedState]); // enable on
  lcd.setCursor(0,0);
  lcd.print(String(dt.hour) + ":"  + String(dt.minute) + ":" + String(dt.second) + " Dir:" + dir);
  lcd.setCursor(0,1);
  lcd.print("Speed:" + fanSpeed);
  delay(100);
  lcd.clear();
  
}
