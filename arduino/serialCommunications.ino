#include <OneWire.h>
#include <Wire.h>

byte number = 0;

char inData[64]; // Allocate some space for the string
char inChar=-1; // Where to store the character read
byte index = 0; // Index into array; where to store the character

int lightPin=9;

int lights1Status=LOW;

void setup(){
  //Serial.begin(9600);
  Serial.begin(57600);
  pinMode(lightPin,OUTPUT);
  digitalWrite(lightPin,lights1Status);
}
int DS18S20_Pin = 8; //Pin ou on a branché la broche "signal" du DS18S20
OneWire ds(DS18S20_Pin); 
float T_eau;

void loop(){
  T_eau = getWaterTemp();
  
  while(Serial.available() > 0)  
  {
    if(index < 63) // One less than the size of the array
    {
      inChar = Serial.read(); // Read a character
      inData[index] = inChar; // Store it
      index++; // Increment where to write next
      inData[index] = '\0'; // Null terminate the string
    }
  }
    
    //Serial.println(inData);
    
    if(strcmp(inData,"T_eau")  == 0)
    {
      Serial.print("<waterTemp>");
      Serial.print(T_eau);
      Serial.println("</waterTemp>");
    }
    else if(strcmp(inData,"T_air")  == 0)
    {
      Serial.print("<airTemp>");
      Serial.print(T_eau);
      Serial.println("</airTemp>");
    }
    else if(strcmp(inData,"lightOn")  == 0)
    {
      lights1Status=HIGH;
      digitalWrite(lightPin,lights1Status);
      Serial.println(getLights1Status());
    }
    else if(strcmp(inData,"lightOff")  == 0)
    {
      lights1Status=LOW;
      digitalWrite(lightPin,lights1Status);
      Serial.println(getLights1Status());
    }
    else if(strcmp(inData,"getLightStatus")  == 0)
    {
      Serial.println(getLights1Status());
    }
  index=0;
  inData[index] = '\0';
}

String getLights1Status()
{
   if(lights1Status==LOW)
   {
     return "<lightsStatus>off</lightsStatus>";
   }
   else if(lights1Status==HIGH)
   {
     return "<lightsStatus>on</lightsStatus>";
   }
}

/**
 * Lit la température de l'eau via la sonde DS18S20
 */
float getWaterTemp(){
  //returns the temperature from one DS18S20 in DEG Celsius
  byte data[12];
  byte addr[8];
  if ( !ds.search(addr)) {
      //no more sensors on chain, reset search
      ds.reset_search();
      return -1000;
  }
  if ( OneWire::crc8( addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return -1000;
  }
  if ( addr[0] != 0x10 && addr[0] != 0x28) {
      Serial.print("Device is not recognized");
      return -1000;
  }
  ds.reset();
  ds.select(addr);
  ds.write(0x44,1); // start conversion, with parasite power on at the end
  byte present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE); // Read Scratchpad
  for (int i = 0; i < 9; i++) { // we need 9 bytes
    data[i] = ds.read();
  }
  
  ds.reset_search();
  
  byte MSB = data[1];
  byte LSB = data[0];
  float tempRead = ((MSB << 8) | LSB); //using two's compliment
  float TemperatureSum = tempRead / 16;
  return TemperatureSum;
}