//BLYNK
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
BlynkTimer timer;

//TEMP Sensor
#include "DHT.h"
#define DHTPIN D4
#define DHTTYPE DHT12   // DHT 11
DHT dht(DHTPIN, DHTTYPE);

//ULTRASONIC
#define trigPin D5
#define echoPin D6
#define pirPin D1

//RTC
#include <TimeLib.h> 
#include <WidgetRTC.h> 

WidgetRTC rtc;
WidgetLCD lcd(V2);
WidgetBridge bridge1(V1);


// Define variables:
long duration;
int distance;
int lightSensor;
String light;
int t = 0;
float h = 0;
int pir = 0;
String prox;

unsigned long startMillis;
unsigned long currentMillis;
const unsigned long period = 1000; 
String currentTime;
String currentDate;
int hourRTC;
int minuteRTC;
int secondRTC;

String MD = " Motion Detected!" ; 
String LO = " Lamp ON!" ; 
String DO = " Door Open! " ;
String SI = " Someone is Inside! " ;  
String notifyMD;
String notifyLO;
String notifyDO;
String notifySI;

int RoOmie;
int IRSensor;
int IRValue;
int key;

int red = D3;
int green = D2;
int blue = D4;
int blinkEye;

char auth[] = "z3oCfA_Bd4hvpkKps97YBh3rZdQEn5hz";
char ssid[] = "";
char pass[] = "";
 
void setup() {
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(3000L, serialPrint);
  timer.setInterval(1500L, project);
  timer.setInterval(1000L, ultrasonic);
  timer.setInterval(1000L, lightSense);
  //timer.setInterval(2200L, tempSense);
  timer.setInterval(1000L, pirSense);
  timer.setInterval(2000L, timeRTCmethod);
  //timer.setInterval(10000L, terminalPrint);
  timer.setInterval(100L, IRSense);
  timer.setInterval(3000L, blinkEyes);

  // Define inputs and outputs:
  pinMode(D0,INPUT); // IR Sensor0
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(pirPin, INPUT);
  Serial.begin(9600);
  Serial.println("BIENVENIDO!!!");
  //dht.begin();
  rtc.begin();

  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);
  digitalWrite(green, HIGH);
  
//setSyncInterval(1);                                           /* Synchronise or read time from the Blynk Server every 1 second */
 //while (Blynk.connect() == false ) {}                          /* If the Blynk Server not yet connected to nodeMCU, keep waiting here */
 // setSyncInterval(10*60);                                       /* After successful login, change Synchornise Time reading for every 10 minute (Do not need to always check for the time)*/ 
startMillis = millis(); 
}


BLYNK_CONNECTED() {
  // Place the AuthToken of the second hardware here
  bridge1.setAuthToken("FIBvkiCRxGrFJ3jReC0V7xeWL_xqJEzK"); 
  
}

BLYNK_WRITE(V6)   
{
   RoOmie = param.asInt();
}
void serialPrint(){
  // Print the distance and lightSensor on the Serial Monitor (Ctrl+Shift+M):
  Serial.print(currentDate);
  Serial.print(" ");
  Serial.print(currentTime);
  Serial.print(" LightSensor = ");
  Serial.print(lightSensor);
  Serial.print(" Distance = ");
  Serial.print(distance);
  Serial.print(" cm ");
  //Serial.print(" Temperature: ");
  //Serial.print(t,2);
  //Serial.print("°C ");
  //Serial.print(" Humidity: ");
  //Serial.print(h,2);
  //Serial.print("% ");
  Serial.println(prox);
  Serial.print(" RoOmie:");
  Serial.print(RoOmie);
  Serial.print(" IRvalue:");
  Serial.print(IRValue);
  Serial.print(" Key:");
  Serial.println(key);
  }

/*void terminalPrint(){
  terminal.print(" LightSensor = ");
  terminal.print(lightSensor);
  terminal.print(" Distance = ");
  terminal.print(distance);
  terminal.print(" cm");
  terminal.flush();
  }*/
  
void ultrasonic() {
  // Clear the trigPin by setting it LOW:
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  // Trigger the sensor by setting the trigPin high for 10 microseconds:
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Read the echoPin, pulseIn() returns the duration (length of the pulse) in microseconds:
  duration = pulseIn(echoPin, HIGH);
  // Calculate the distance:
  distance = duration * 0.034 / 2;
  Blynk.virtualWrite(V0, distance);

  if (distance  < 10 ){
    key = 1;
    RoOmie = 1;
   Blynk.virtualWrite(V6, HIGH);
      } else{   
    key = 0;
    //RoOmie = 1;
    //Blynk.virtualWrite(V6, HIGH);
    }
      
}

void blinkEyes(){
    if (blinkEye == 0 && secondRTC >0 && secondRTC <5 | secondRTC >10 && secondRTC <15 |secondRTC >20 && secondRTC <25 
          |secondRTC >30 && secondRTC <35 |secondRTC >40 && secondRTC <45 |secondRTC >50 && secondRTC <55){
      digitalWrite(blue, LOW);
      digitalWrite(green, LOW);
      delay(300);
      digitalWrite(green,HIGH);        
      } 
  }

void lightSense(){
     //Read Photoresistor and set light description
 lightSensor = analogRead(A0);
 Blynk.virtualWrite(V3, lightSensor);
        
        /////MAIN LIGHT////
         //DAY
   if (hourRTC < 18 && hourRTC > 6   && lightSensor > 40 && lightSensor < 57){
    light = "LIGHTS ON";
    } else if ( lightSensor < 20){
    light = "LIGHTS OFF";}
        //NIGHT
    if (hourRTC > 17 | hourRTC < 7 && lightSensor > 29 ){
    light = "LIGHTS ON";
    } else if (lightSensor < 30 ){  //lightSensor > 10 && 
    light = "LIGHTS OFF";
    }

        //LAMP LIGHT STATUS
    if (lightSensor >81 && lightSensor <99){
    Blynk.virtualWrite(V7, HIGH);}
    else if (lightSensor >26 && lightSensor <34){
    Blynk.virtualWrite(V7, HIGH);} 
    else Blynk.virtualWrite(V7, LOW);
    
}

/*void tempSense(){
    t = dht.readTemperature();
    h = dht.readHumidity();
    Blynk.virtualWrite(V4, t);
    Blynk.virtualWrite(V5, h);
}*/

void pirSense () {
   pir = digitalRead(pirPin);
   if (pir == HIGH){
        prox = "ALERT ";
        lcd.print(11,0, prox);
      } else prox = "..... ";
        lcd.print(11,0, prox);

}

void timeRTCmethod (){
  currentMillis = millis();                                                       /* Keep counting time for display refresh */
  if(currentMillis - startMillis > period)                                        /* For every 1 second, run the set of code*/
  {
     currentTime = String(hour()) + ":" + minute() + ":" + second();        /* Define "currentTime" by combining hour, minute and second */
     currentDate = String(day()) + "/" + month() + "/" + year() + " ";            /* Define "currentDate" by combining day, month, and year */
     
      hourRTC = hour();
      minuteRTC = minute(); 
      secondRTC = second(); 
      
    notifyMD = currentDate + currentTime + MD ;
    notifyLO = currentDate + currentTime + LO ;
    notifyDO = currentDate + currentTime + DO; //+ distance ;
    notifySI = currentDate + currentTime + SI ;
    
    Blynk.virtualWrite(V9, currentTime);                                          /* Send Time parameters to Virtual Pin V1 on Blynk App */
    Blynk.virtualWrite(V10, currentDate);                                          /* Send Date parameters to Virtual Pin V2 on Blynk App */
                                                          /* Reset time for the next counting cycle */
  }
}

void IRSense (){
  IRSensor = digitalRead(D0);

   if(IRSensor==LOW)  { 
    // If no signal print collision detected
    
    IRValue = 1; //Collision detected
    lcd.clear();
    lcd.print(0,1, "DOOR OPENED");
    lcd.print(0,0, light );
    lcd.print(11,0, prox);
   }
   else 
  {
     // If signal detected print collision detected
     IRValue = 0; //Collision not detected
    lcd.clear();
    lcd.print(0,1, "DOOR CLOSED");
    lcd.print(0,0, light );
    lcd.print(11,0, prox);
  }
 
}



void project(){

    if ( IRValue == 1 && lightSensor < 11  && RoOmie == 1 && pir == LOW){// | key ==0){
      digitalWrite(D7, HIGH);
      Blynk.notify (notifyDO);
      
      lcd.clear();
      lcd.print(0,1, "DOOR OPENED");
      lcd.print(0,0, light);
      lcd.print(11,0, prox);
      delay(8000);    
      digitalWrite(D7, LOW);
      pir == LOW;  
      
    }
    else if(pir == HIGH && lightSensor < 11 && RoOmie == 1 && IRValue == 0){// | key ==0){
        digitalWrite(D7, HIGH);
        Blynk.notify (notifyMD);
        
        lcd.clear();
        lcd.print(0,0, light);
        lcd.print(11,0, prox);
        delay(8000); 
        digitalWrite(D7, LOW);
        pir == LOW;
        
     }
    else if (IRValue == 1 && lightSensor >10  && RoOmie == 1 && key ==0 && pir == LOW){
      Blynk.notify (notifyDO);}
    else if (pir == HIGH && IRValue ==1 && lightSensor > 10 && RoOmie ==1 && key ==0){
      Blynk.notify (notifySI); }
     else if (pir == HIGH && IRValue ==0 && lightSensor > 910  && RoOmie ==1 && key ==0){
      Blynk.notify (notifyMD);}

    delay(2000);   
}
void loop(){
  Blynk.run();
  timer.run();
}
