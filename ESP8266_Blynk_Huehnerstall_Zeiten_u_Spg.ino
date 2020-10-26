#include <Blynk.h>
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h><br>
#include <TimeLib.h>

#include <WidgetRTC.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "qSbwLiqx8Lrl4iHSqCs6n87XaV7NbFVU"; //Hühnerstall-Token!
 
// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Kackschneider_1";
char pass[] = "guenterbrigitte19030302";

int eingang1, eingang2, y, z;
double spg, spg1, Licht;
 
BlynkTimer timer;
WidgetRTC rtc;
WidgetLED led1(V1);
WidgetLED led2(V2);
WidgetLED led3(V6);


String currentTime1, currentTime2;



void Einlesen()
{
  eingang1=digitalRead(16);
  if (eingang1 == HIGH) {
    led1.on();
    
     if(z==0){
    clockDisplay1();
    z++;
    }
    
  } 
  else {
    led1.off();
    z=0;
  }


  eingang2=digitalRead(5);
  if (eingang2 == HIGH) {
    led2.on();

    if(y==0){
    clockDisplay2();
    y++;
    }
  } 
  
  else {
    led2.off();
    y=0;
  }

  Spannung();

  Licht=digitalRead(14);
  if (Licht==HIGH){
    led3.on();
  }
  
  else{
    led3.off();
  }
}


 
void setup()
{
  // Debug console
  Serial.begin(9600);
 //pinMode(LED, OUTPUT);
  Blynk.begin(auth, ssid, pass);
 
  // Begin synchronizing time
  rtc.begin();
  
  // Display digital clock every 10 seconds
  timer.setInterval(1000, Einlesen);

  int z=0;
  int y=0;
}
 
void loop()
{
  Blynk.run();
  timer.run();
}

void clockDisplay1()
{
  // You can call hour(), minute(), ... at any time
  // Please see Time library examples for details
 
  currentTime1 = String(hour()) + ":" + minute() + ":" + second();
  
  // Send time to the App
  Blynk.virtualWrite(V3, currentTime1);
  
}

void clockDisplay2()
{
  // You can call hour(), minute(), ... at any time
  // Please see Time library examples for details
 
  currentTime2 = String(hour()) + ":" + minute() + ":" + second();
  
  // Send time to the App
  Blynk.virtualWrite(V4, currentTime2);
  
}

void Spannung(){
  spg = analogRead(A0);
  spg1 = spg/66.4;
  Blynk.virtualWrite(V5, spg1);
}
