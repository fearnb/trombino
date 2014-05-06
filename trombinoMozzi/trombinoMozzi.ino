#include <MozziGuts.h>
#include <Oscil.h> // oscillator template
#include <tables/sin2048_int8.h> // sine table for oscillator

int slide = 1;
int pot = 2;
int wind = 0;
int button1 = 6;
int button2 = 7;
int button3 = 8;

Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin(SIN2048_DATA);

int volume;

void setup(){
  startMozzi();
}

void updateControl(){
  int button1State = digitalRead(button1);
  int button2State = digitalRead(button2);
  int button3State = digitalRead(button3);
  
  int windval = mozziAnalogRead(wind);
  if(windval > 600){
    int potval = mozziAnalogRead(pot);

    volume = potval >> 2;
    int note = 0;
    int slideval = mozziAnalogRead(slide);
    if(button1State == LOW && button2State == LOW && button3State == LOW){
    note = map(slideval, 1022, 890, 116, 82);
  } else if(button1State == HIGH){
      note = map(slideval, 1022, 890, 174, 123);
  }else if(button2State == HIGH){
      note = map(slideval, 1022, 890, 233, 185);
  }else if(button3State == HIGH){
      note = map(slideval, 1022, 890, 349, 246);
  }
    aSin.setFreq(note*2);
  } else{
    volume = 0;
  }
}

int updateAudio(){
  return ((int)aSin.next() * volume) >> 8;
}

void loop(){
  audioHook();
}

