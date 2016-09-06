// Basé sur le code de LadyAda traduit et modifie par moi, domaine public
#include "DHT.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal.h>


#include <Wire.h>
#include "RTClib.h"

#define ONE_WIRE_BUS 3
OneWire oneWire(ONE_WIRE_BUS);
 
static int tempProbe1=3;
OneWire ds(tempProbe1);

DallasTemperature T1(&ds);


RTC_DS1307 rtc;
int printCycle=0;

int lcdBackLightPin=5;
int lcdPWMLight=32;
int lcdLevel=3;

 LiquidCrystal lcd(2, 4, 7, 8, 12, 13);
void setup()
{

  /*pinMode(2,OUTPUT);
  pinMode(4,OUTPUT);
  pinMode(7,OUTPUT);
  pinMode(8,OUTPUT);
  pinMode(12,OUTPUT);
  pinMode(13,OUTPUT);*/


  pinMode(lcdBackLightPin,OUTPUT);
  analogWrite(lcdBackLightPin,lcdPWMLight);
 Serial.begin(9600);
 lcd.begin(16, 2);
 lcd.print("Rlieh");
 T1.begin();

 
    
 
 Wire.begin();
 rtc.begin();

 DateTime now = rtc.now();
 lcd.setCursor(0, 1);
 lcd.print("date:");
 lcd.print(now.day(), DEC);
 lcd.print('/');
 lcd.print(now.month(), DEC);
 lcd.print('/');
 lcd.print(now.year(), DEC);
 delay(2000);
 lcd.clear();
 
}
void loop()
{

  lcd.setCursor(0, 0);
  T1.requestTemperatures();
  Serial.print("T1 : ");
  Serial.println(T1.getTempCByIndex(0));
  Serial.print("T2 : ");
  Serial.println(T1.getTempCByIndex(1));

  lcd.print("A:");
  lcd.print(T1.getTempCByIndex(0));
  //lcd.setCursor(3, 0);
  lcd.print(" E:");
  lcd.print(T1.getTempCByIndex(1));
  
  
  /*lcd.setCursor(0, 1);
  lcd.print("T1:");
  lcd.println(T1.getTempCByIndex(0));

  lcd.setCursor(8, 1);
  lcd.print("  ");

  lcd.setCursor(10, 1);
  // print the number of seconds since reset:
  //lcd.print(millis() / 1000);*/



DateTime now = rtc.now();
/*lcd.setCursor(0,0);
 lcd.print("date:");
 lcd.print(now.day(), DEC);
 lcd.print('/');
 lcd.print(now.month(), DEC);
 lcd.print('/');
 lcd.print(now.year(), DEC);*/

lcd.setCursor(0, 1);

    lcd.print("    ");
    if(now.hour()<10)
      {lcd.print(0);}
    lcd.print(now.hour(), DEC);
    lcd.print(':');
    if(now.minute()<10)
      {lcd.print(0);}
    lcd.print(now.minute(), DEC);
    lcd.print(':');
    if(now.second()<10)
      {lcd.print(0);}
    lcd.print(now.second(), DEC);
    
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(' ');
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(),DEC);
    Serial.println();

    delay(500);
    
}
