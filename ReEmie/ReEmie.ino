#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
BlynkTimer timer;

//TEMP SENSOR dht11
//#include <dht.h>
//#define DHT11_PIN D4
//dht DHT;

// TEMP SENSOR dht22
#include "DHT.h"
#define DHTPIN D4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

//SERVO MOTOR
 #include <Servo.h>
Servo myservo;

//RTC
#include <TimeLib.h> 
#include <WidgetRTC.h> 
WidgetRTC rtc;

WidgetLED led1(V2);
WidgetLED led2(V6);
WidgetTerminal terminal1(V7);
WidgetTerminal terminal2(V8);



float temp;
float humidity;
int button = 0;
int ReEmie = 0;
int servo;

unsigned long startMillis;
unsigned long currentMillis;
const unsigned long period = 1000; 
int hourRTC;
int minuteRTC;
int secondRTC;
String currentTime;
String OP = " Window Open" ; 
String CL = " Window CLosed" ; 
String notifyWO;
String notifyWC;
int windowStatus = 0;
int fanStatus = 0;

int setHour = 7;
int setTemp = 100;

int red = D6;
int green = D7;
int blue = D5;
int blinkEye;


char auth[] = "FIBvkiCRxGrFJ3jReC0V7xeWL_xqJEzK"; //ReEmie

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "";
char pass[] = "";



void setup()
{
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);
  digitalWrite(green, HIGH); blinkEye = 1;
  
  terminal1.clear();
  terminal2.clear();
  Serial.begin(9600);
  Serial.println ("....Activating ReEmie........");
  Blynk.begin(auth, ssid, pass);
  //Blynk.virtualWrite(V4, LOW);
  //Blynk.virtualWrite(V3, LOW);
  timer.setInterval(10000L, tempSensor);
  timer.setInterval(1000L, buttonControl);
  timer.setInterval(1000L, fanControl);
  timer.setInterval(2000L, servoControl);
  timer.setInterval(1000L, timeRTCmethod);
  timer.setInterval(1000L, windowControl);
  timer.setInterval(1000L, serialMOnitor);
  timer.setInterval(5000L, terminalPrint);
  timer.setInterval(3000L, blinkEyes);
  
  servo = 2; // MAKE SURE THAT WINDOW IS INITIALLY CLOSE
  fanStatus =2;
  rtc.begin();
  startMillis = millis();
  dht.begin(); //dht22
  Blynk.virtualWrite(V4,LOW);
}

BLYNK_WRITE(V4) {
   ReEmie = param.asInt();
}
BLYNK_WRITE(V3) {
   button = param.asInt();
}
BLYNK_WRITE(V5) {
   servo = param.asInt();
}
BLYNK_WRITE(V7){
 setHour = param.asInt();
}
BLYNK_WRITE(V8){
 setTemp = param.asInt();
}

void tempSensor(){
  //float chk = DHT.read11(DHT11_PIN);
  //temp = DHT.temperature;
  //humidity = DHT.humidity;

  temp = dht.readTemperature();
  humidity = dht.readHumidity();
  
  delay(2000);

  Blynk.virtualWrite(V0, temp);
  Blynk.virtualWrite(V1, humidity);
  }

void serialMOnitor(){
  Serial.print("B");
  Serial.print(button);
  
  Serial.print(" Temperature = ");
  Serial.print(temp); //DHT.temperature for dht11
  Serial.print(" bE");
  Serial.println(blinkEye);
  Serial.print("R");
  Serial.print(ReEmie);
  Serial.print(" Humidity = ");
  Serial.print(humidity); //DHT.humidity for dht11
  Serial.print(" s");
  Serial.print(servo);
  Serial.print("  ");
  Serial.print(currentTime);
  Serial.print("  f");
  Serial.print(fanStatus);
  Serial.print("  w");
  Serial.print(windowStatus);
  Serial.print("  setHour=");
  Serial.print(setHour);
  Serial.print("  setTemp=");
  Serial.println(setTemp);
  }

void terminalPrint(){
    terminal1.clear();
    terminal1.print("Window Open in: ");
    terminal1.print(setHour);
    terminal1.println(":00");
    terminal1.flush();

    terminal2.clear();
    terminal2.print("Fan Turn ON: ");
    terminal2.print(setTemp);
    terminal2.println("°C");
    terminal2.flush();
    
  }

void blinkEyes(){
    if (blinkEye == 0 && secondRTC >0 && secondRTC <5 | secondRTC >10 && secondRTC <15 |secondRTC >20 && secondRTC <25 
          |secondRTC >30 && secondRTC <35 |secondRTC >40 && secondRTC <45 |secondRTC >50 && secondRTC <55){
      digitalWrite(green, LOW);
      digitalWrite(blue, LOW);
      delay(300);
      digitalWrite(green,HIGH);        
      } else if (blinkEye == 1 && secondRTC >0 && secondRTC <5 | secondRTC >10 && secondRTC <15 |secondRTC >20 && secondRTC <25 
          |secondRTC >30 && secondRTC <35 |secondRTC >40 && secondRTC <45 |secondRTC >50 && secondRTC <55){
      digitalWrite(blue, LOW);
      digitalWrite(green, LOW);
      delay(300);
      digitalWrite(blue,HIGH);  
      }
  }
  


void fanON(){
  Blynk.virtualWrite(V3, HIGH);
   digitalWrite (D8, HIGH);
    led1.on();
  digitalWrite(blue, LOW);
  digitalWrite(green,HIGH); blinkEye = 0;
  }
void fanOFF(){
  Blynk.virtualWrite(V3, LOW);
  digitalWrite (D8, LOW);
    led1.off();
  digitalWrite(blue, LOW);
  digitalWrite(green,HIGH); blinkEye = 0;
  }

void fanControl(){
     if (temp >= setTemp && ReEmie ==1 && fanStatus==0 ){
    fanON(); }
    else if (temp >= setTemp && ReEmie ==0 && fanStatus==0){
    fanOFF();
    fanStatus =0;} 
    else if (temp < setTemp && ReEmie ==0 && fanStatus==0){
    fanOFF();
    fanStatus =0; }
    else if (temp < setTemp && ReEmie ==1 && fanStatus==0){
    fanOFF();
    fanStatus =0; }
}

void buttonControl(){
  if (button == 1 && temp < setTemp && ReEmie == 0 ){
  fanON();
  button =2;
  fanStatus =1;  
  } else if (button == 0 && temp < setTemp && ReEmie == 0  ){
  fanOFF();
  button =2;
  fanStatus =0;
  }  else if (button == 1 && temp < setTemp && ReEmie == 1 ){
  fanON();
  button =2;
  fanStatus =1;  //
  } else if (button == 0 && temp < setTemp && ReEmie == 1  ){
  fanOFF();
  button =2;
  fanStatus =0;
  } else if (button == 1 && temp >=setTemp && ReEmie ==0  ){
  fanON();
  button =2;
  fanStatus =2;
  } else if (button == 0 && temp >=setTemp && ReEmie ==0   ){
  fanOFF();
  button =2;
  fanStatus =0;
  } else if (button == 1 && temp >=setTemp && ReEmie ==1  ){
  fanON();
  button =2;
  fanStatus =0;
  } else if (button == 0 && temp >=setTemp && ReEmie ==1   ){
  fanOFF();
  button =2;
  fanStatus =2;
  } 
}

void openServo() {
  digitalWrite(blue, LOW);
  digitalWrite(green,HIGH); blinkEye = 0;
  myservo.attach(D1);
  myservo.write(171); //Counter  - Open
  delay(6000);
  myservo.detach();
  led2.on();
  servo = 2;
  Blynk.notify(notifyWO);
  Blynk.virtualWrite(V5, HIGH);
  windowStatus = 1;

  }
void closeServo() { 
  digitalWrite(blue, LOW);
  digitalWrite(green,HIGH); blinkEye = 0;
  myservo.attach(D1);
  myservo.write(0); //clock  - CLose
  delay(6000);
  myservo.detach();
  led2.off();
  servo = 2;
  Blynk.notify(notifyWC);
  Blynk.virtualWrite(V5, LOW); 
  windowStatus = 0;
  }
  
void servoControl(){
  if (servo == 1 && windowStatus == 0){
    openServo();      
    delay(1000);
     
      
  } else if (servo == 0 && windowStatus ==1){
    closeServo();       
    delay(1000);
    
    }
}

void windowControl(){
    if (hourRTC == setHour && minuteRTC == 0 && secondRTC < 5  && windowStatus == 0){
    openServo();
    Blynk.notify(notifyWO);
    delay(1000);
    }else if (hourRTC == 18 && minuteRTC == 0 && secondRTC < 5 && windowStatus == 1 ){
    closeServo();
    Blynk.notify(notifyWC);
    delay(1000);
    } 
}


void timeRTCmethod (){
  currentMillis = millis();                                                       /* Keep counting time for display refresh */
  if(currentMillis - startMillis > period)                                        /* For every 1 second, run the set of code*/
  {
     hourRTC = hour() ;
     minuteRTC = minute(); 
     secondRTC = second(); 
     
     currentTime = String(hour()) + ":" + minute() + ":" + second();
     
    notifyWO = currentTime + OP ;
    notifyWC = currentTime + CL ;   
  }
}


void loop(){
  Blynk.run();
  timer.run();
  delay(1000);
}
