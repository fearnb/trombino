/*Trombino by Fearn Bishop
---------------------
Trombino using wind sensor.
---------------------
Project website: http://www.trombino.co.uk
Contact: podling@ipodling.co.uk
Project Github: https://github.com/iPodling/trombino
Project hackaday.io: http://hackaday.io/project/1010
License: GNU General Public License v3
Library dependencies:
  Mozzi: http://sensorium.github.io/Mozzi/
  ArdOSC: http://recotana.com/recotanablog/closet

Pins:
Digital
5: Button - mode switch
6: Button - Pitch switch
7: Button - Pitch switch
8: Button - Pitch switch
9: Audio

Analog
0: Wind Sensor
1: Slide Potentiometer
2: Volume Potentiometer
3: LED (Digital 17)
4: LED (Digital 18)
5: LED (Digital 19)

Log: 1st of May 2014
  Abilities:
    -Send OSC over UDP (mouth measurment and note devised by mappings)
    -Tone a mapped note 
  To add:
    -Send OSC of individual parts
    -Send OSC in MIDI format (to be translated by OSCulator)
    -Nicer audio (refactor for Mozzi use)
    -Add in a couple more octaves
*/

#include <SPI.h>        
#include <Ethernet.h>
#include <ArdOSC.h>
#include <toneAC.h>

int port = 8888;
byte ip[] ={192, 168, 1, 177};
byte outIp[] ={192, 168, 1, 72}; //destination IP here
byte mac[] = {  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
OSCClient client;
boolean noteOffSent = true;

/*Analog pins*/
int windSensor = 0;
int slideSensor = 1;
int pot = 2;
int led1 = 17;
int led2 = 18;
int led3 = 19;

/*Digital pins*/
int modePin = 5;
int button1 = 6;
int button2 = 7;
int button3 = 8;
int speaker = 9;

/*Readings*/
int windReading = 0;
float slideReading = 0;
int potReading = 0;
int modeState = 0;
int button1State = 0;
int button2State = 0;
int button3State = 0;

float note = 0; //variable to hold the mapped note
int lower = 600; //tolerance for wind sensor
int toSum = 3;//samples for debounce
int prevModeState = 0;
int mode = 0;
int MIDINote = 0;
int lastMIDINote = 0;
int volume = 0;

void setup(){
  Ethernet.begin(mac,ip); //begin ethernet connection
  /*Set outputs*/
  pinMode(speaker, OUTPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  /*Set inputs*/
  pinMode(modePin, INPUT);
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  pinMode(button3, INPUT);
}

void loop(){  
  /*Check wind sensor*/
  windReading = 0;
  for (int i = 0; i < toSum; i++){  //check several readings and average
    windReading += analogRead(windSensor);
  }
  windReading = windReading/toSum;
  
  /*Check for mode change*/ 
   modeState = digitalRead(modePin);
   if(modeState!= prevModeState){ //button state has changed
      if(modeState == HIGH){ //if pressed
       mode++;
        if(mode > 3){
        mode = 0;
        }
     }
     prevModeState = modeState; //set previous state for next comparisson
   }
  
  slideReading = analogRead(slideSensor);
  potReading = analogRead(pot);
  button1State = digitalRead(button1);
  button2State = digitalRead(button2);
  button3State = digitalRead(button3);    
  
  /*Set LEDs*/
  if(mode == 0){
    digitalWrite(led1, HIGH);
    digitalWrite(led2, HIGH);
    digitalWrite(led3, HIGH);
  }else if(mode == 1){
    digitalWrite(led1, LOW);
    digitalWrite(led2, LOW);
    digitalWrite(led3, HIGH);
  }else if(mode == 2){
    digitalWrite(led1, HIGH);
    digitalWrite(led2, LOW);
    digitalWrite(led3, LOW);
  }else if(mode == 3){
    digitalWrite(led1, LOW);
    digitalWrite(led2, HIGH);
    digitalWrite(led3, LOW);
  }

if (mode == 0){ //mode for sending raw data over OSC
  sendOSCRaw();
}
if(windReading > lower){ //wind sensor reading over tolerance
    if(mode == 1){ //make a noice
    outSound();
    }else if (mode == 2){ //send mapped OSC data
    outOSCMapped();
    }else if (mode == 3){
      //send midi here
        MIDINote = calculateMIDI();
        if(MIDINote != lastMIDINote){ //slide has moved, cancel last note before playing current
          sendOSCMIDIOff();
          lastMIDINote = MIDINote;
        }
        sendOSCMIDI();
    }
    noteOffSent = false; //mark that a note off will be required
  } else { //wind sensor below tolerance
    noToneAC(); //make sure no note is played
    if (noteOffSent == false){ //if a note off is required
      if(mode == 2){ //send OSCMapped note off
      sendOffMapped();
      }else if(mode == 3){
        //send midi off
        sendOSCMIDIOff();
      }
      noteOffSent = true; //mark that note off is no longer required
    }
  }
  
  delay(50);
}

/*function for mapping floats*/
float floatMap(float input, float inLow, float inHigh, float outLow, float outHigh){
  float output = (input - inLow) * (outHigh - outLow) / (inHigh - inLow) + outLow;
  return output;
}

/*Make a noise!*/
void outSound(){
    volume = map(potReading, 0, 1023,0, 10);
  if(button1State == LOW && button2State == LOW && button3State == LOW){
    note = floatMap(slideReading, 1022, 890, 116.54, 82.41);
    toneAC((note*2), volume);
  } else if(button1State == HIGH){
      note = floatMap(slideReading, 1022, 890, 174.61, 123.47);
      toneAC((note*2), volume);
  }else if(button2State == HIGH){
      note = floatMap(slideReading, 1022, 890, 233.08, 185.00);
      toneAC((note*2), volume);
  }else if(button3State == HIGH){
      note = floatMap(slideReading, 1022, 890, 349.23, 246.94);
      toneAC((note*2), volume);
  }
}

/*Sned the mapped note over OSC*/
void outOSCMapped(){
  OSCMessage message; //make OSC message variable
  
  message.setAddress(outIp, port);
  message.beginMessage("/wind");
  message.addArgInt32((int)windReading);
  client.send(&message);
  message.flush();
  
  if(button1State==LOW && button2State == LOW && button3State == LOW){ //no buttons
    note = floatMap(slideReading, 1022, 890, 116.54, 82.41);
    message.setAddress(outIp, port);
    message.beginMessage("/note");
    message.addArgFloat((float)note);
    client.send(&message);
    message.flush();
  }else if(button1State == HIGH){
    note = floatMap(slideReading, 1022, 890, 174.61, 123.47);
    message.setAddress(outIp, port);
    message.beginMessage("/note");
    message.addArgFloat((float)note);
    client.send(&message);
    message.flush();
  }else if(button2State == HIGH){
    note = floatMap(slideReading, 1022, 890, 233.08, 185.00);
    message.setAddress(outIp, port);
    message.beginMessage("/note");
    message.addArgFloat((float)note);
    client.send(&message);
    message.flush();
  }else if(button3State == HIGH){
    note = floatMap(slideReading, 1022, 890, 349.23, 246.94);
    message.setAddress(outIp, port);
    message.beginMessage("/note");
    message.addArgFloat((float)note);
    client.send(&message);
    message.flush();
  }
}

/*Send note off for OSC*/
void sendOffMapped(){
  OSCMessage message;
  message.setAddress(outIp, port);
  message.beginMessage("/wind");
  message.addArgInt32(0);
  client.send(&message);
  message.flush();
  if(button1State==LOW){
    note = floatMap(slideReading, 1022, 890, 116.54, 82.41);
      message.setAddress(outIp, port);
      message.beginMessage("/note");
      message.addArgFloat(note);
      client.send(&message);
      message.flush();
  }else{
    note = floatMap(slideReading, 1022, 890, 174.61, 123.47);
      message.setAddress(outIp, port);
      message.beginMessage("/note");
      message.addArgFloat(note);
      client.send(&message);
      message.flush();
  }
}

/*Send unmapped readings over OSC*/
void sendOSCRaw(){
  /*Wind sensor*/
  OSCMessage message;
  message.setAddress(outIp, port);
  message.beginMessage("/wind");
  message.addArgInt32(windReading);
  client.send(&message);
  message.flush();
  /*Slide*/
  message.setAddress(outIp, port);
  message.beginMessage("/slide");
  message.addArgInt32(slideReading);
  client.send(&message);
  message.flush();
  /*Potentiometer*/
  message.setAddress(outIp, port);
  message.beginMessage("/pot");
  message.addArgInt32(potReading);
  client.send(&message);
  message.flush();
  /*Buttons*/
  message.setAddress(outIp, port);
  message.beginMessage("/button1");
  message.addArgInt32(button1State);
  client.send(&message);
  message.flush();
  message.setAddress(outIp, port);
  message.beginMessage("/button2");
  message.addArgInt32(button2State);
  client.send(&message);
  message.flush();
  message.setAddress(outIp, port);
  message.beginMessage("/button3");
  message.addArgInt32(button3State);
  client.send(&message);
  message.flush();
}

int calculateMIDI(){
  int MIDI = 0;
  if (button1State == LOW && button2State == LOW && button3State == LOW){
     MIDI = map(slideReading, 1022, 890, 46, 40);
  }else if (button1State == HIGH){
     MIDI = map(slideReading, 1022, 890, 53, 47);
  }else if (button2State == HIGH){
    MIDI = map(slideReading, 1022, 890, 58, 44);    
  }else if (button3State == HIGH){
    MIDI = map(slideReading, 1022, 890, 65, 59);
}
  return MIDI;
}

void sendOSCMIDI(){
  OSCMessage message;
  message.setAddress(outIp, port);
  message.beginMessage("/tromb/1/note");
  message.addArgInt32(MIDINote);
  int velo = map(windReading, 600, 700, 0, 127);
  message.addArgInt32(velo);
  message.addArgInt32(1);
  client.send(&message);
  message.flush();
}

void sendOSCMIDIOff(){
  OSCMessage message;
  message.setAddress(outIp, port);
  message.beginMessage("/tromb/1/note");
  message.addArgInt32(lastMIDINote);
  message.addArgInt32(0);
  message.addArgInt32(0);
  client.send(&message);
  message.flush();
}
