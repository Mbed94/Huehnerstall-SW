void Zeitholen(void){
  DateTime now = rtc.now();
 
  tag = now.day();
  //Serial.print("Tag: ");
  //Serial.println(t);

  Monat = now.month();
  //Serial.print("Monat: ");
  //Serial.println(Monat);

  j = now.year();
  //Serial.print("Jahr: ");
  //Serial.println(j);

  h = now.hour();
  //Serial.print("Stunde: ");
  //Serial.println(h);

  Minute = now.minute();
  //Serial.print("Minute: ");
  //Serial.println(Minute);

  s = now.second();
  //Serial.print("Sekunde: ");
  //Serial.println(s);
  }




 void Uhrzeit(void){
  
  lcd.setCursor(0, 0);
  lcd.print("    Uhrzeit");
  lcd.setCursor(4, 1);

  if(h<10){
  lcd.setCursor(4, 1);
  lcd.print("0");
  lcd.setCursor(5, 1);
  lcd.print(h);
  }
  
  else{
  lcd.setCursor(4, 1);
  lcd.print(h);
  }
  
  
  lcd.setCursor(6, 1);
  lcd.print(":");

  if(Minute<10){
  lcd.setCursor(7, 1);
  lcd.print("0");
  lcd.setCursor(8, 1);
  lcd.print(Minute);
  }
  
  else{
  lcd.setCursor(7, 1);
  lcd.print(Minute);
  }

 
  lcd.setCursor(9, 1);
  lcd.print(":");

if(s<10){
  lcd.setCursor(10, 1);
  lcd.print("0");
  lcd.setCursor(11, 1);
  lcd.print(s);
  lcd.setCursor(12, 1);
  lcd.print("");
  lcd.setCursor(13, 1);
  lcd.print("");
  lcd.setCursor(14, 1);
  lcd.print("");
  lcd.setCursor(15, 1);
  lcd.print("");
  
}

else{
  lcd.setCursor(10, 1);
  lcd.print(s);
  }

  
}




void Datum(void){
  lcd.setCursor(5, 0);
  lcd.print("Datum");
  lcd.setCursor(3, 1);
  lcd.print(tag);
  lcd.setCursor(5, 1);
  lcd.print("/");
  lcd.setCursor(6, 1);
  lcd.print(Monat);
  lcd.setCursor(8, 1);
  lcd.print("/");
  lcd.setCursor(9, 1);
  lcd.print(j);
  

}


void Batteriespannung(void){
  spg = analogRead(A10);
  spg = spg/65;
  lcd.setCursor(4, 0);
  lcd.print("Spannung");
  lcd.setCursor(4, 1);
  lcd.print(spg);
  lcd.setCursor(10, 1);
  lcd.print("V");
 
}

void HelligkeitLesen(void){

  for (q=0; q<1000; q++){
  Helligkeit = analogRead(A8);
  Hellsumm = Hellsumm + Helligkeit;
  }
  Hellsumm=Hellsumm/1000;
  Helligkeit = 1023-(int)Hellsumm;
  lcd.setCursor(0, 0);
  lcd.print("Licht (max 1023)");
  lcd.setCursor(5,1);
  
  lcd.print(Helligkeit);
  Hellsumm=0;
  
  if (Helligkeit < 1000){
  lcd.setCursor(8,1);
  lcd.print("         ");    
  }

  if (Helligkeit < 100){
  lcd.setCursor(7,1);
  lcd.print("         ");    
  }
  
  if (Helligkeit < 10){
  lcd.setCursor(6,1);
  lcd.print("          ");    
  }
  
}

void Stoerung(void){
  lcd.setCursor(0,0);
  lcd.print("Strg./Timerfhl.:");
  lcd.setCursor(8,1);
  lcd.print(stoerung);
}

void KlappeAuf(){
  lcd.setCursor(0,0);
  lcd.print("  Klappe auf: ");
  lcd.setCursor(8,1);
  lcd.print(Klappe_Auf);
}

void KlappeZu(){
  lcd.setCursor(0,0);
  lcd.print("  Klappe zu: ");
  lcd.setCursor(8,1);
  lcd.print(Klappe_Zu);
}

void NotAus(){
  lcd.setCursor(0,0);
  lcd.print("  Not-Aus: ");
  lcd.setCursor(8,1);
  lcd.print(Not_Aus);
}




void Zaehlfr(){
  lcd.setCursor(0,0);
  lcd.print("Zaehlfreigabe: ");
  lcd.setCursor(8,1);
  lcd.print(zaehlfreigabe);
}

void Verzoeg(){
  lcd.setCursor(0,0);
  lcd.print("Verzoegerung: ");
  lcd.setCursor(7,1);
  lcd.print(verz);
}




void Rechtslauf(void)
{
 digitalWrite(23, HIGH);
 digitalWrite(25, LOW);
 digitalWrite(27, HIGH);
}

void Linkslauf(void)
{
 digitalWrite(23, LOW);
 digitalWrite(25, HIGH);
 digitalWrite(27, HIGH); 
}

void MotorAus(void)
{
 digitalWrite(23, LOW);
 digitalWrite(25, LOW);
 digitalWrite(27, 0); 
}


void Temperaturanzeige(){

  for(u=0; u<3000; u++){
   summe = summe + analogRead(A9);
  }
   schnitt = summe/3000;
  
  temp = schnitt/1024;
  temp = temp * 5;
  temp = temp - 0.5;
  temp = temp*137;
  
  
  lcd.setCursor(3, 0);
  lcd.print("Temperatur");
  lcd.setCursor(3, 1);
  lcd.print(temp);
  lcd.setCursor(10, 1);
  lcd.print("Grad");
  summe=0;

}


void Starttext(void){
  lcd.setCursor(4, 0);
  lcd.print("CHICKEN");
  lcd.setCursor(1, 1);
  lcd.print("SMARTHOME V1.0");
  
}

void DimmenPruefen(){
  
    if(verz>1500 && Abend_Licht == 1){ // Wenn Abendlicht 25 Minuten an, dimme über 6 Minuten runter
      analogWrite(6, 150);          
    }

    if(verz>1560 && Abend_Licht == 1){ 
      analogWrite(6, 100);
    }
    
    if(verz>1620 && Abend_Licht == 1){ 
      analogWrite(6, 70);
    }
    
    if(verz>1680 && Abend_Licht == 1){ 
      analogWrite(6, 30);
    }
    
    if(verz>1740 && Abend_Licht == 1){ 
      analogWrite(6, 15);
    }
    
    if(verz>1800 && Abend_Licht == 1){ 
      analogWrite(6, 5);
    }
    
    if(verz>1860 && Abend_Licht == 1){ //Wenn die 6te Minute um ist, schalte Licht ganz aus
      analogWrite(6, LOW);
      Abend_Licht = 10;
    }
}
