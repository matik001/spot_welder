#include <LiquidCrystal.h>

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const int BUTTON = 6;
const int BUZZER = 7;
const int OUT = 8;
const int TIME_CONTROL = A0;
const int START_OPTOTRIAK = 9;
const int CHANGE_TIME_PIN = 13;

unsigned long MIN_WELDING_TIME = 1;
unsigned long MAX_WELDING_TIME = 100;
unsigned long weldingTime = 60;
unsigned long IGNORE_TIME = 500; /// ignoruje sekunde po przerwaniu, bo zdarza sie ze przy puszczaniu odbije
unsigned long START_DELAY = 500;
bool isActive = false;
bool clicked = false;
bool active = false;
unsigned long startTime = 0;
unsigned long finishTime = 0;
unsigned long ignoreTime = 0;


void updateMessage(){
   lcd.setCursor(0, 0);
   lcd.print("Czas zgrzewania");
   lcd.setCursor(0, 1);
   lcd.print(String(weldingTime)+" ms ");
}
long calculateWatingTime(){
  long res = MIN_WELDING_TIME+(analogRead(TIME_CONTROL)/1000.0)*(MAX_WELDING_TIME - MIN_WELDING_TIME);
  if(res > MAX_WELDING_TIME)
    res = MAX_WELDING_TIME;
  if(res < MIN_WELDING_TIME) /// na wszelki wypadek
    res = MIN_WELDING_TIME;
  return res;
}
void forceUpdateWeldingTime(){
  weldingTime = calculateWatingTime();
  updateMessage();
}
void updateWeldingTime(){
  bool isChangingTime = digitalRead(CHANGE_TIME_PIN);
  if(isChangingTime){
    forceUpdateWeldingTime();
  }
}
void setup() {
//   Serial.begin(9600);
   lcd.begin(16, 2);
   pinMode(BUTTON, INPUT);
   pinMode(TIME_CONTROL, INPUT); 
   pinMode(CHANGE_TIME_PIN, INPUT);
   pinMode(BUZZER, OUTPUT);
   pinMode(OUT, OUTPUT);
   pinMode(START_OPTOTRIAK , OUTPUT);
   
   digitalWrite(START_OPTOTRIAK, LOW);
   forceUpdateWeldingTime();
}


void loop() {
  unsigned long currentTime = millis();

  updateWeldingTime();
  
  if(currentTime < ignoreTime || currentTime < startTime){
    return;
  }
  bool newClicked = digitalRead(BUTTON);
  if(newClicked && !clicked){
    startTime = currentTime+START_DELAY;
    finishTime = startTime+weldingTime;
//    Serial.println("klikam");
    active = false;
  }
  else if(newClicked && clicked){
    active = currentTime >= startTime && currentTime <= finishTime;
  }
  else if(!newClicked){
    active = false;
    if(clicked){
      ignoreTime = currentTime+IGNORE_TIME;
//      Serial.println("puszczam"); 
    }
  }
  
  clicked = newClicked;
  if(active){
    digitalWrite(START_OPTOTRIAK, HIGH);
    digitalWrite(BUZZER, HIGH);
    digitalWrite(OUT, HIGH);
  }
  else{
    digitalWrite(START_OPTOTRIAK, LOW);
    digitalWrite(OUT, LOW);
    noTone(BUZZER); 
  }
  
}
