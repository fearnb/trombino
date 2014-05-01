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
int port = 8888;
byte ip[] ={192, 168, 1, 177};
byte outIp[] ={192, 168, 1, 72};
byte mac[] = {  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
OSCClient client;
boolean noteOffSent = true;


int slideSensor = 0;
float slideReading = 0;
int mouthSensor = 5;
int mouthReading = 0;
int buttonState = 0;
int buttonPin = 5;
int speaker = 3;
float note = 0;
//tolerance for wind sensor
int lower = 500;
//samples for debounce
int toSum = 3;

void setup(){
  Ethernet.begin(mac,ip);
  pinMode(speaker, OUTPUT); // Set speaker as output
  pinMode(buttonPin, INPUT);
}

void loop(){
  mouthReading = 0;
  //check several readings and average
  for (int i = 0; i < toSum; i++){
    mouthReading += analogRead(mouthSensor);
  }
  mouthReading = mouthReading/toSum;
  buttonState = digitalRead(buttonPin);
  
  slideReading = analogRead(slideSensor);
  

if(mouthReading > lower){ 
    //send note data
//     outSound();
    outOSC();
    noteOffSent = false;
  } else {
    //send note off
    if (noteOffSent == false){
      sendOff();
      noteOffSent = true;
    }
//    noTone(speaker);
  }
  
  delay(50);
}

float floatMap(float input, float inLow, float inHigh, float outLow, float outHigh){
  float output = (input - inLow) * (outHigh - outLow) / (inHigh - inLow) + outLow;
  return output;
}

void outSound(){
  if (buttonState==LOW){
    note = floatMap(slideReading, 1022, 890, 116.54, 82.41);
    tone(speaker, (note*2));
  }else{
    note = floatMap(slideReading, 1022, 890, 174.61, 123.47);
      tone(speaker, (note*2));
  }
}

void outOSC(){
  OSCMessage message;
  message.setAddress(outIp, port);
  message.beginMessage("/mouth");
  message.addArgInt32((int)mouthReading);
  client.send(&message);
  message.flush();
  
  if(buttonState==LOW){
    note = floatMap(slideReading, 1022, 890, 116.54, 82.41);
    message.setAddress(outIp, port);
    message.beginMessage("/note");
    message.addArgFloat((float)note);
    client.send(&message);
    message.flush();
  }else{
    note = floatMap(slideReading, 1022, 890, 174.61, 123.47);
    message.setAddress(outIp, port);
    message.beginMessage("/note");
    message.addArgFloat((float)note);
    client.send(&message);
    message.flush();
  }
}

void sendOff(){
  OSCMessage message;
  message.setAddress(outIp, port);
  message.beginMessage("/mouth");
  message.addArgInt32(0);
  client.send(&message);
  message.flush();
  if(buttonState==LOW){
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
