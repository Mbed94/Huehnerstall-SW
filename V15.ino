// Zeit immer über Konsole mit dem entsprechenden Programm "RTC per Konsole einstellen" einstellen

//ACHTUNG!!
//Wenn Winterbetrieb VOR 12 Uhr mittags (da ist Freigabewert 10) verlassen wird und erst NACH 12 Uhr mittags wieder eingeschaltet wird, 
//muss das System NEUGESTARTET werden!!
//-->Dies setzt die ABENDLICHT-Freigabe auf AKTIV (Freigabewert ist dann durch Setup-Routine == 0).



// Beschreibung Sommerbetrieb
/*
Klappe geht auf, wenn Helligkeit = 13+Hysterese erreicht ist
Abendlicht geht an, 15 Minuten nachdem Schwelle == 357
Klappe geht zu, wenn 60 Minuten vergangen sind, seit Helligkeit == 357
Abendlicht wird runtergedimmt, wenn Klappe geschlossen wird. Dimmen dauert 6 Minuten, minütliches Dimmen
*/

// Beschreibung Winterbetrieb
/*
Licht schaltet um 4Uhr frühs an, dimmt dabei über 10Min hoch
Klappe geht um 7 Uhr morgens auf
Licht geht bei Helligkeit > 356 morgens aus
Licht geht bei Helligkeit < 357 abends an
Klappe geht bei Helligkeit < 13 abends und weiteren 30 Minuten zu
Licht wird ab 18Uhr über 15 Minuten runtergedimmt, wenn vorher dunkel genug, damit es eingeschalten wurde
*/

int j, Monat, Minute, tag, h, s, z=0, u, Helligkeit, zaehler, ManuAuto, verz, zaehlfreigabe, TimerMorgensAus, LichtErkannt;
int HochManu, RunterManu, gueltig, Klappe_Auf, Klappe_Zu, q;
int Not_Aus, stoerung, Licht, dz, hochgedimmt, SoWi, Licht_aktiv, Abend_Licht;
long int timer, var; 
double spg, spgv, temp, summe=0, schnitt, Hellsumm;
unsigned long previousMillis=0, nl;

#define WZ 1000 // Anzeigedauer für Display-Werte UND Timer-Basistakt!!
#define SCHALTSCHWELLE 356 // Helligkeit für Klappe auf
#define LICHTSTUNDE 4 // Stunde und Minute, wann Licht MORGENS eingeschaltet werden soll (Winterbetrieb)
#define LICHTMINUTE 0 // Darf max. "49" sein, wegen Anschaltdimmung
#define LICHTAUSSCHWELLE 355 // Helligkeit, bei der Licht im Winterbetrieb MORGENS ausgeschaltet werden soll (Winterbetrieb)
#define STUNDE 6 // Stunde und Minute, wann Tür MORGENS aufgehen soll (Winterbetrieb)
#define MINUTE 55
#define SOMMERAUFSTUNDE 6 // Stunde und Minute wann Tür MORGENS aufgehen soll (Sommerbetrieb)
#define SOMMERAUFMINUTE 55
#define ABENDLICHTSCHWELLE 340 // Helligkeit, UNTER der ABENDS das Licht im Stall angemacht werden soll
#define ABENDAUSSTUNDE 18 //Stunde und Minute, wann ABENDS das Licht im Stall wieder ausgeschaltet werden soll (Start von Dimmung)
#define ABENDAUSMINUTE 0 // Darf max. "44" sein, wegen Abschaltdimmung
#define NACHLAUFVERZ 1*470000 // entspricht 1s Nachlauf für Klappe
#define LAV 30 // Morgendliche Ausschaltverzögerung gegen Störlichter in 1s - Einheiten ("30" entspricht 30s)

#include "LiquidCrystal.h"
#include "Wire.h"
#include "RTClib.h"
#pragma GCC optimize ("O0") // Compiler-Optimierungsgrad auf 0 stellen, damit Nachlaufverzögerung (for-Schleife) korrekt arbeitet

RTC_DS1307 rtc;

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);


void setup() {
  Hellsumm=0;
  lcd.begin(16, 2);
  Serial.begin(9600);

  #ifdef AVR
  Wire.begin();
#else
  Wire1.begin();
#endif
  rtc.begin();
  
 
  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
 
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2019, 9, 30, 8, 5, 0));
  }

  stoerung = 0;
  hochgedimmt = 0;
  Licht_aktiv = 0;
  Abend_Licht = 0;
  var = 0;
  zaehlfreigabe = 0;
  verz = 0;
  TimerMorgensAus = 0;
  LichtErkannt = 0;
  
  pinMode(A8, INPUT); // Helligkeits-Sensor
  pinMode(A9, INPUT); // Temperatur-Sensor
  pinMode(A10, INPUT); // Batterie-Spannung
  pinMode(6, OUTPUT); //Transistor-Schaltpegel
  pinMode(22, INPUT); //Lichtschalter einlesen
  pinMode(23, OUTPUT); //IN3 an Brücke
  pinMode(24, INPUT); //Auto-Manu einlesen
  pinMode(25, OUTPUT); //IN4 an Brücke
  pinMode(26, INPUT); //Sommer/Winter einlesen
  pinMode(27, OUTPUT); //MotorSignal an Brücke
  pinMode(28, INPUT); //Hoch einlesen
  pinMode(30, INPUT); //Runter einlesen
  pinMode(32, INPUT); //Reed unten
  pinMode(34, INPUT); //Reed oben
  pinMode(36, INPUT); //Reed Not-Halt
  pinMode(46, OUTPUT); //Display-Helligkeit 
  
}

void loop() {

Zeitholen();

unsigned long currentMillis = millis();

if(currentMillis - previousMillis >= WZ){ // Zähler für LCD-Anzeigen
  if(z>11){
    z=0;
  }
  previousMillis = currentMillis;
  z++;
  
  if(zaehlfreigabe == 1){
  verz++;
  }

  if(LichtErkannt == 1){
  TimerMorgensAus++;
  }

  lcd.clear();
  
}

  if(z==1){
  Starttext();
  }

  if(z==2){
  Datum();
  }

  if(z==3){
  Uhrzeit();
  }

  if(z==4){
  Batteriespannung();
  }
  
  if(z==5){
  Temperaturanzeige();
  }

  if(z==6){
  HelligkeitLesen();
  }

  if(z==7){
  Stoerung();
  }

  if(z==8){
  KlappeAuf();
  }

  if(z==9){
  KlappeZu();
  }

  if(z==10){
  NotAus();
  }

  if(z==11){
  Zaehlfr();  
  }

  if(z==12){
  Verzoeg();
  }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Ab hier beginnt die eigentliche Programmschleife

Licht = digitalRead(22);
ManuAuto = digitalRead(24);
SoWi = digitalRead(26);

if(Licht == HIGH && hochgedimmt == 0){
  for(dz=0; dz<255; dz++){
  analogWrite(6, dz);
  analogWrite(46, dz);
  
  //Dimmverzögerung
  for(nl=0; nl<2800; nl++){
      ;; 
      }
  }
  hochgedimmt = 1;
}

if(Licht == LOW && hochgedimmt == 1){
  for(dz=255; dz>0; dz--){
  analogWrite(6, dz);
  analogWrite(46, dz);
   //Dimmverzögerung
   for(nl=0; nl<2800; nl++){
      ;; 
      }
  }
  hochgedimmt = 0;
  
  // Verzögerung nach Runterdimmen, 0.5s
  for(nl=0; nl<235000; nl++){
      ;; 
      }
  digitalWrite(46, LOW);
  digitalWrite(6, LOW);
}
////////////////////////////////////////////////////////////////////////////////

  if(ManuAuto == LOW){ //Falls Mode-Betriebsschalter auf Auto (LOW), führe normale Routine aus
  
  Klappe_Auf = digitalRead(34); //Endschalterzustände einlesen
  Klappe_Zu = digitalRead(32);

  Helligkeit = analogRead(A8); //Helligkeit einlesen
  Helligkeit = 1023-Helligkeit; //Helligkeit umrechnen (invertieren)

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
//AUFMACHEN SOMMER
  if(SoWi == LOW){

  Klappe_Zu = digitalRead(32);
  if ((Klappe_Auf == 0) && (h==SOMMERAUFSTUNDE) && (Minute==SOMMERAUFMINUTE)){ //Falls hell, Klappe nicht offen und mind. 4 Uhr frühs, aber max. 10 Uhr frühs
  lcd.clear();
  Not_Aus = digitalRead(36);
  timer = 0;

  while (Klappe_Auf == 0 && Not_Aus == 0 && timer < 1550 && stoerung == 0 ) //Solange Klappe nicht oben angekommen (Klappe_Auf == 0) und NotAus nicht ausgelöst und Zeitabbruch nicht erfüllt
  {
   timer++;
   Serial.println(timer);
   Not_Aus = digitalRead(36);
   Klappe_Auf = digitalRead(34); //Momentanen Klappe-Offen-Zustand einlesen
   Linkslauf(); //Klappe aufmachen
   lcd.setCursor(2, 0);
   lcd.print("Hochfahren");
   lcd.setCursor(0, 1);
   lcd.print("                 ");

   if (timer > 1549){
    stoerung = 1;
    }
  }
  
  MotorAus(); //Motor aus, wenn Öffnungsschalter erreicht
  }
  }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//AUFMACHEN WINTER
  else{
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if((Licht_aktiv == 0) && (h == LICHTSTUNDE) && (Minute >= LICHTMINUTE)){ // Falls Licht aus und Anschaltzeit erreicht, dimme Licht über 10 Minuten hoch

     if(Minute == LICHTMINUTE){
      analogWrite(6, 15);
     }

      if(Minute == LICHTMINUTE+2){
      analogWrite(6, 30);
     }

      if(Minute == LICHTMINUTE+4){
      analogWrite(6, 50);
     }

     if(Minute == LICHTMINUTE+6){
      analogWrite(6, 80);
     }

     if(Minute == LICHTMINUTE+8){
      analogWrite(6, 160);
     }

     if(Minute == LICHTMINUTE+10){
      digitalWrite(6, HIGH);
      Licht_aktiv = 1;
     }

}
    //Ausschaltverzögerung für kuzzeitige Störlichter (Auto, Scheinwerfer Beck etc.)
    if(Helligkeit > LICHTAUSSCHWELLE)
    {
      LichtErkannt = 1;
    }

    else{ 
    LichtErkannt = 0;
    TimerMorgensAus = 0;
    }

    if((Licht_aktiv == 1) && (Helligkeit > LICHTAUSSCHWELLE) && (Licht == 0) && (TimerMorgensAus >= LAV)){ // Falls Licht an und Ausschaltschwelle erreicht (gültig), schalte Licht aus
      digitalWrite(6, LOW);
      Licht_aktiv = 0;
    }
    

   Klappe_Zu = digitalRead(32);
    
   if((h == STUNDE) && (Minute == MINUTE) && (Klappe_Auf == 0)){ //Falls Öffnungs-Uhrzeit erreicht und Klappe nicht offen
      
   lcd.clear();
   timer = 0;
   while (Klappe_Auf == 0 && Not_Aus == 0 && timer < 1550 && stoerung == 0 ){
   timer++;
   Serial.println(timer);
   Not_Aus = digitalRead(36);
   Klappe_Auf = digitalRead(34); //Momentanen Klappe-Offen-Zustand einlesen
   Linkslauf(); //Klappe aufmachen
   lcd.setCursor(2, 0);
   lcd.print("Hochfahren");
   lcd.setCursor(0, 1);
   lcd.print("                 ");

  
   if (timer > 1500){
    stoerung = 1;
    //Serial.println(stoerung);
    
    }
      }
     MotorAus();
    }
    
  }
    
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//ZUMACHEN SOMMER
  if(SoWi == LOW){ 

  if(h==12){ //Ab 12 Uhr mittag das Abendlicht nur 1x freischalten pro Tag
      Abend_Licht = 0;
    }

    if(Helligkeit < ABENDLICHTSCHWELLE && Abend_Licht==0){ //15 Minuten nach Abendlichtschwelle, mind. 12 Uhr und Abendlicht aus, schalte Licht ein
      digitalWrite(6, HIGH);
      Abend_Licht = 1;
    }


    DimmenPruefen(); //Prüfe, ob gedimmt werden muss (Wenn Licht bereits 25 Minuten an war, jede Minute, über 6 Minuten)



  if((Helligkeit < ABENDLICHTSCHWELLE) && Klappe_Zu == 0 && h>11){ //Wenn Abendlicht angeht + Klappe nicht geschlossen und Zeit zwischen 12 Uhr mittags und 00 Uhr nachts

  zaehlfreigabe = 1; //Starte verz, wenn Abendlichtschwelle erreicht (=356)
  analogWrite(46, 250);
  Klappe_Zu = digitalRead(32);
  timer = 0;
  
  while (Klappe_Zu == 0 && timer < 1550 && stoerung == 0 && verz>3000) // 19 Minuten NACH "Abendlicht aus" (verz = 2100), dann, solange Klappe nicht unten angekommen, runterfahren
  {
   timer++;
   Serial.println(timer);
   Klappe_Zu = digitalRead(32);
   Rechtslauf(); // Klappe zumachen
   lcd.setCursor(1, 0);
   lcd.print("Herunterfahren");
   lcd.setCursor(0, 1);
   lcd.print("                 ");

    if (timer > 1549){
    stoerung = 1;
    }
  }
  
  //NACHLAUFVERZÖGERUNG
      for(nl=0; nl<NACHLAUFVERZ; nl++){
      ;; 
      }
      
  MotorAus(); //Motor aus
  }
  
  if(verz > 3060){ // Falls 51min verstrichen (1 Min nach Klappe zu), setze beide Variablen zurück
  zaehlfreigabe = 0;
  verz = 0;
  analogWrite(46, 0);
  }
   
}


  //ZUMACHEN WINTER (Zeitsperre für Lichtschaltung bis 12 Uhr mittags, dann aktiv von 12Uhr mittags bis 00Uhr nachts!!!)
  else{
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//ABENDLICHT

    if(h==12){ //Ab 12 Uhr mittag das Abendlicht nur 1x freischalten pro Tag
      Abend_Licht = 0;
    }

    if(Helligkeit<ABENDLICHTSCHWELLE && Abend_Licht==0){ //Falls Abendlichtschwelle erreicht, mind. 12 Uhr, und Abendlicht aus, schalte Licht ein
      digitalWrite(6, HIGH);
      Abend_Licht = 1;
    }


    if(h==ABENDAUSSTUNDE && Minute >= ABENDAUSMINUTE && Abend_Licht==1){ //Falls Licht an und Ausschaltzeit erreicht, Licht (ab 18 Uhr über 15 Minuten) runterdimmen (15 Minuten lang)

        if( Minute == ABENDAUSMINUTE+1){
        analogWrite(6, 238);
        }
        
        if( Minute == ABENDAUSMINUTE+2){
        analogWrite(6, 221);
        }
        
        if( Minute == ABENDAUSMINUTE+3){
        analogWrite(6, 204);
        }
        
        if( Minute == ABENDAUSMINUTE+4){
        analogWrite(6, 187);
        }
        
        if( Minute == ABENDAUSMINUTE+5){
        analogWrite(6, 170);
        }
   
        if( Minute == ABENDAUSMINUTE+6){
        analogWrite(6, 153);
        }

        if( Minute == ABENDAUSMINUTE+7){
        analogWrite(6, 136);
        }

        if( Minute == ABENDAUSMINUTE+8){
        analogWrite(6, 119);
        }

        if( Minute == ABENDAUSMINUTE+9){
        analogWrite(6, 102);
        }

        if( Minute == ABENDAUSMINUTE+10){
        analogWrite(6, 85);
        }

        if( Minute == ABENDAUSMINUTE+11){
        analogWrite(6, 68);
        }

        if( Minute == ABENDAUSMINUTE+12){
        analogWrite(6, 30);
        }

        if( Minute == ABENDAUSMINUTE+13){
        analogWrite(6, 20);
        }

        if( Minute == ABENDAUSMINUTE+14){
        analogWrite(6, 10);
        }
   
        if( Minute == ABENDAUSMINUTE+15){
        digitalWrite(6, LOW);
        Abend_Licht=10;
   }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  if((Helligkeit<ABENDLICHTSCHWELLE) && Klappe_Zu == 0 && h>11){ //Falls dunkel + Klappe nicht geschlossen und Zeit zwischen 12 Uhr mittags und 00 Uhr nachts

  zaehlfreigabe = 1;

  Klappe_Zu = digitalRead(32);
  timer = 0;
  
  while (Klappe_Zu == 0 && timer < 1550 && stoerung == 0 && verz>3599) //Wenn 60 Minuten nach Abendlicht-Anschalten um (verz = 3600), dann solange Klappe nicht unten angekommen (Klappe_Zu == 0) 
  {
    timer++;
    Serial.println(timer);
 
   
   Klappe_Zu = digitalRead(32);
   Rechtslauf(); // Klappe zumachen
   lcd.setCursor(1, 0);
   lcd.print("Herunterfahren");
   lcd.setCursor(0, 1);
   lcd.print("                 ");

   
    if (timer > 1500){
    stoerung = 1;
    }
  }
  
      // NACHLAUFVERZÖGERUNG
      for(nl=0; nl<NACHLAUFVERZ; nl++){
      ;; 
      }
      
  MotorAus(); //Motor aus
  }
  
  if(verz > 3605){ // Falls 1 Stunde verstrichen, setze beide Variablen zurück
  zaehlfreigabe = 0;
  verz = 0;
  }
 }
}
  
  else{ //Falls Mode-Betriebsschalter auf Manuell (HIGH), führe manuelle Routine aus
    
    stoerung = 0; //Rücksetzung des Timers zur Notabschaltung
    RunterManu = digitalRead(30);
    HochManu = digitalRead(28);
 
      Klappe_Auf = digitalRead(34);
     
      while (HochManu == HIGH && RunterManu == LOW && Klappe_Auf == LOW){ //Hochfahren
      HochManu = digitalRead(28);
      Klappe_Auf = digitalRead(34);
      Linkslauf();
      }
      MotorAus();
      
    HochManu = digitalRead(28);
    Klappe_Auf = digitalRead(34);
      
      Klappe_Zu = digitalRead(32);
     
      while (RunterManu == HIGH && HochManu == LOW && Klappe_Zu == LOW){ //Runterfahren
      RunterManu = digitalRead(30);
      Klappe_Zu = digitalRead(32);
      Rechtslauf();
      }

      // NACHLAUFVERZÖGERUNG
      for(nl=0; nl<NACHLAUFVERZ; nl++){
      ;; 
      }
      
      MotorAus();
      
      RunterManu = digitalRead(30);
      Klappe_Zu = digitalRead(32);
  }   
}
