/*Status board test for Trombino
Test LEDs, audio, pot and switch
Digital
0: RX
1: TX
2:
3:
4: SD
5: Button - mode switch
6: Button - Pitch switch
7: Button - Pitch switch
8: Button - Pitch switch
9: Audio
10: Ethernet
11: Ethernet
12: Ethernet
13: Ethernet

Analog
0: Wind Sensor
1: Slide Potentiometer
2: Volume Potentiometer
3: LED (Digital 17)
4: LED (Digital 18)
5: LED (Digital 19)
*/

/*Analog pins*/
int wind = 0;
int slide = 1;
int pot = 2;
int led1 = 17;
int led2 = 18;
int led3 = 19;

/*Digital pins*/
int modeSwitch = 5;
int button1 = 6;
int button2 = 7;
int button3 = 8;
int audio = 9;

/*Variables*/

int windRead = 0;
int slideRead = 0;
int potRead = 0;
int modeSwitchState = 0;
int button1State = 0;
int button2State = 0;
int button3State = 0;

void setup(){
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(modeSwitch, INPUT);
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  pinMode(button3, INPUT);
  pinMode(audio, OUTPUT);
  Serial.begin(9600);
}

void loop(){
  /*Output to LEDs*/
  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
  digitalWrite(led3, LOW);
  
  /*Output audio*/
  tone(audio, 440);
  
  /*Read variables*/
  windRead = analogRead(wind);
  slideRead = analogRead(slide);
  potRead = analogRead(pot);
  modeSwitchState = digitalRead(modeSwitch);
  button1State = digitalRead(button1);
  button2State = digitalRead(button2);
  button3State = digitalRead(button3);
  
  /*Pring variables*/
  Serial.print("Wind read = ");
  Serial.println(windRead);
//  Serial.print("Slide read = ");
//  Serial.println(slideRead);
//  Serial.print("Pot read = ");
//  Serial.println(potRead);
//  Serial.print("Mode button = ");
//  Serial.println(modeSwitchState);
//  Serial.print("Button 1 = ");
//  Serial.println(button1State);
//  Serial.print("Button 2 = ");
//  Serial.println(button2State);
//  Serial.print("Button 3 = ");
//  Serial.println(button3State);
  delay(50);
}
