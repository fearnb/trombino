#include <toneAC.h>

/*Trombino mapping test*/


int slideSensor = 1;
float slideReading = 0;
int mouthSensor = 0;
int mouthReading = 0;
int volumeSensor = 2;
int volumeReading = 0;
int volume = 0;
int buttonState = 0;
int buttonPin = 6;
int button2State = 0;
int button2Pin = 7;
int button3State = 0;
int button3Pin = 8;
int speaker = 9;
float note = 0;
//tolerances for wind sensor
int lower = 600;
int higher = 700;
//samples for debounce
int toSum = 35;

void setup(){
  pinMode(speaker, OUTPUT); // Set speaker as output
  pinMode(buttonPin, INPUT);
}

void loop(){
//  mouthReading = 0;
  //check several readings and average
//  for (int i = 0; i < toSum; i++){
//    mouthReading += analogRead(mouthSensor);
//  }
//  mouthReading = mouthReading/35;
mouthReading = analogRead(mouthSensor);
  buttonState = digitalRead(buttonPin);
  button2State = digitalRead(button2Pin);
  button3State = digitalRead(button3Pin);
  
  slideReading = analogRead(slideSensor);
  volumeReading = analogRead(volumeSensor);
  volume = map(volumeReading, 0, 1023,0, 10);
//  if(mouthReading > lower && mouthReading < higher){ //if > x && < y
//    note = floatMap(slideReading, 1022, 890, 116.54, 82.41);
//    tone(speaker, (note*2));
//  } else if(mouthReading > higher){
//      note = floatMap(slideReading, 1022, 890, 174.61, 123.47);
//      tone(speaker, (note*2));
//  } else {
//    noTone(speaker);
//  }

if(mouthReading > lower && buttonState == LOW && button2State == LOW && button3State == LOW){ //if > x && < y
    note = floatMap(slideReading, 1022, 890, 116.54, 82.41);
    toneAC((note*2), volume);
  } else if(mouthReading > lower && buttonState == HIGH){
      note = floatMap(slideReading, 1022, 890, 174.61, 123.47);
      toneAC((note*2), volume);
  }else if(mouthReading > lower && button2State == HIGH){
      note = floatMap(slideReading, 1022, 890, 233.08, 185.00);
      toneAC((note*2), volume);
  }else if(mouthReading > lower && button3State == HIGH){
      note = floatMap(slideReading, 1022, 890, 349.23, 246.94);
      toneAC((note*2), volume);
  } else {
    noToneAC();
  }
  
  delay(50);
}

float floatMap(float input, float inLow, float inHigh, float outLow, float outHigh){
  float output = (input - inLow) * (outHigh - outLow) / (inHigh - inLow) + outLow;
  return output;
}
