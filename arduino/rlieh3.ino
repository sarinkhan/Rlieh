#include <OneWire.h>
#include <Wire.h>
#include <EEPROMex.h>
const int airTempSensorPin=6;    //air temperature DS18B20 signal pin
const int waterTemp1SensorPin=7;  //water temperature DS18B20 signal pin
const int waterTemp2SensorPin=8;  //water temperature DS18B20 signal pin
const int fanPin=10;             //The GPIO controlling the TIP120 for the fans

const int relay1Pin=11;          //The pin controlling the first relay  (and the first AC socket)
const int relay2Pin=12;          //The pin controlling the second relay (and the second AC socket)
int relay1Status=LOW;            //The status of the first relay. LOW= off, HIHGH= on.
int relay2Status=LOW;            //The status of the second relay. LOW= off, HIHGH= on.


const int transistor1Pin=3;      //First transistor pin
int transistor1PWM=0;            //Default PWM level for transistor 1

const int transistor2Pin=5;      //First transistor pin
int transistor2PWM=0;            //Default PWM level for transistor 1

int lights1Pin=transistor1Pin;   //defines the output pin for the first set of lights
int lights1Status=0;             //current status of the first set of lights



int lights1Fade=0;              //defines if the lights1 are fading.
                                //if 0, they do not, if 1, they fade in, if -1 they fade out
int lights1FadeAmount=1;        //by how much the pwm rate is increased each time
int lights1FadeTime=50;         //how long to wait before the next pwm rate modification
int lights1Level=0;             //current level of lights1
int lights1Max=255;             //max value of lights1


unsigned long lights1LastChange=0;

const int lowTemp1Address=0;                                // EPROM Adress for the low temp
const int highTemp1Address=lowTemp1Address+sizeof(float);     // EPROM Address for the high temp

float lowTemp1=24.0;            //The temperature at which the cooling systems stops
float highTemp1=24.5;           //The temperature at which the cooling systems starts

OneWire waterTempSensor(waterTemp1SensorPin);
OneWire waterTempSensor2(waterTemp2SensorPin);
OneWire airTempSensor(airTempSensorPin);


float waterTemp=0;
float waterTemp2=0;
float airTemp=0;


//variables used for structured messages
String openTag1="<";
String openTag2="</";
String closingTag=">";
String tagPwm="pwm";
String tagFanStatus="fanStatus";
String tagStatus="fanStatus";
String tagR1Status="R1Status";
String tagR2Status="R2Status";
String offString ="OFF";
String onString = "ON";
String waterTemp1Tag="waterTemp1";
String waterTemp2Tag="waterTemp2";
String airTemp1Tag="airTemp1";
String lowTemp1Tag="lowTemp1";
String highTemp1Tag="highTemp1";
String cmdResultTag="cmdRes";
String resultTag="result";
String textTag="text";
String errorCodeTag="errorCode";
String success="success";
String failure="failure";
String durationSetString="Duration set.";
String missingArgumentString="Missing argument";
String invalidArgumentString="Invalid value for argument";
String argumentRangeString="must be in";
String unchangedString="no change";

char cmdSeparator=':';

void setup()
{
  pinMode(relay1Pin,OUTPUT);
  pinMode(relay2Pin,OUTPUT);
  
  
  pinMode(transistor1Pin,OUTPUT);
  pinMode(transistor2Pin,OUTPUT);
  analogWrite(transistor1Pin,transistor1PWM);
  analogWrite(transistor2Pin,transistor2PWM);
  lights1LastChange=millis();
  
  
  Serial.begin(57600);            //9600 bits/s seems enough, this value may be increased later on
  Serial.setTimeout(100);        //lowering the serial timout for faster response
  
  digitalWrite(relay1Pin,relay1Status);
  digitalWrite(relay2Pin,relay2Status);
  
  lowTemp1=EEPROM.readFloat(lowTemp1Address);   //eprom lib only support int writes, so we divide by 100 on read to obtain a float, and we save 100*temp
  highTemp1=EEPROM.readFloat(highTemp1Address); //eprom lib only support int writes, so we divide by 100 on read to obtain a float, and we save 100*temp
}


  byte number = 0;
  char inData[64]; // Allocate some space for the string
  char inParam[64]; // Allocate some space for the string
  char inChar=-1; // Where to store the character read
  byte index = 0; // Index into array; where to store the character



void loop()
{
  unsigned long currTime=millis();
  int param0=-1;
  float param0f=-1.0;
  
  waterTemp = getWaterTemp();
  waterTemp2 = getWaterTemp2();
  airTemp = getAirTemp();
  
  int paramIndex=-1;
  int i=0, j=0;

  int recievedCmd=0;
  
  fadeLights1(lights1FadeAmount, lights1FadeTime);
  
  
  
  String cmdline="";
  String cmd="";
  String param0Str="";
  if((Serial.available() > 0))
  {
    cmdline=String(Serial.readString());
    if (cmdline.indexOf(':')==-1)
    {
      cmd=String(cmdline);
    }
    else
    {
      cmd=cmdline.substring(0,cmdline.indexOf(cmdSeparator));
      param0Str=cmdline.substring(cmdline.indexOf(cmdSeparator)+1);
      param0=param0Str.toInt();
      param0f=param0Str.toFloat();
    }
  }
  delay(50);
  
    if(cmd  == "getWaterTemp1")
    {
      Serial.print(openTag1+waterTemp1Tag+closingTag);
      Serial.print(waterTemp);
      Serial.println(openTag2+waterTemp1Tag+closingTag);
    }
    if(cmd  == "getWaterTemp2")
    {
      Serial.print(openTag1+waterTemp2Tag+closingTag);
      Serial.print(waterTemp2);
      Serial.println(openTag2+waterTemp2Tag+closingTag);
    }
    else if(cmd  =="getAirTemp1")
    {
      Serial.print(openTag1+airTemp1Tag+closingTag);
      //Serial.print("<airTemp>");
      Serial.print(airTemp);
      Serial.println(openTag2+airTemp1Tag+closingTag);
      //Serial.println("</airTemp>");
    }
    else if(cmd  =="L1On")
    {
      enableLights1();
      printCmdResultXML(0, true, "L1 "+onString);
    }
    else if(cmd  =="L1Off")
    {
      disableLights1();
      printCmdResultXML(0, true, "L1 "+offString);
    }
    else if(cmd  =="L1FadeIn")
    {
      if(param0!=-1)
      {
        lights1FadeTime=calcFadeDelay(param0);
        printCmdResultXML(0, true, "L1 - fade in."+durationSetString);
      }
      else
      {
        printCmdResultXML(0, true, "L1 - fade in.");
      }
      setLights1FadeIn();
    }
    else if(cmd  =="L1FadeOut")
    {
      if(param0!=-1)
      {
        lights1FadeTime=calcFadeDelay(param0);
        printCmdResultXML(0, true, "L1 - fade out."+durationSetString);
      }
      else
      {
        printCmdResultXML(0, true, "L1 - fade out.");
      }
      setLights1FadeOut();
    }
    else if(cmd  =="T1On")
    {
       transistor1PWM=255;
       digitalWrite(transistor1Pin,HIGH);
       printCmdResultXML(0, true, "T1 "+onString);
    }
    else if(cmd  =="T1Off")
    {
       transistor1PWM=0;
       digitalWrite(transistor1Pin,LOW);
       printCmdResultXML(0, true, "T1 "+offString);
    }
    else if(cmd  == "setT1Lvl")
    {
      if(param0!=-1)
      {
        transistor1PWM=param0;
        if(setPWMOutput(transistor1Pin,transistor1PWM)==true)
        {
          printCmdResultXML(0, true, "T1 PWM set.");
        }
        else
        {
          printCmdResultXML(2, false, invalidArgumentString+" PWM,"+argumentRangeString+"[0,255].");
        }
      }
      else
      {
        printCmdResultXML(1, false, missingArgumentString+":PWM Level.");
      }
    }
    else if(cmd  =="setT1Pct")
    {
      if(param0!=-1)
      {
        if(setPMWMoutputPct(transistor1Pin,param0)==true)
        {
          printCmdResultXML(0, true, "T1 duty cycle set (%).");
        }
        else
        {
          printCmdResultXML(2, false, invalidArgumentString+" %, "+argumentRangeString+"[0,100].");
        }
      }
      else
      {
        printCmdResultXML(1, false, missingArgumentString+":%.");
      }
    }
    
    
    else if(cmd  =="T2On")
    {
       transistor2PWM=255;
       digitalWrite(transistor2Pin,HIGH);
       printCmdResultXML(0, true, "T2 ON");
    }
    else if(cmd  =="T2Off")
    {
       transistor1PWM=0;
       digitalWrite(transistor2Pin,LOW);
       printCmdResultXML(0, true, "T2 OFF.");
    }
    else if(cmd  =="setT2Lvl")
    {
      if(param0!=-1)
      {
        transistor2PWM=param0;
        if(setPWMOutput(transistor2Pin,transistor2PWM)==true)
        {
          printCmdResultXML(0, true, "T2 PWM set.");
        }
        else
        {
          printCmdResultXML(2, false, invalidArgumentString+" PWM, "+argumentRangeString+"[0,255].");
        }
      }
      else
      {
        printCmdResultXML(1, false, missingArgumentString+":PWM.");
      }
    }
    else if(cmd  =="setT2Pct")
    {
      if(param0!=-1)
      {
        if(setPMWMoutputPct(transistor2Pin,param0)==true)
        {
          printCmdResultXML(0, true, "T2 duty cycle set (%).");
        }
        else
        {
          printCmdResultXML(2, false, invalidArgumentString+" %, "+argumentRangeString+"[0,100].");
        }
      }
      else
      {
        printCmdResultXML(1, false, missingArgumentString+"%.");
      }
    }
    else if(cmd  =="R1On")
    {
      if(enableRelay1()==true)
      {
        printCmdResultXML(0, true, "R1 -"+onString+".");
      }
      else
      {
        printCmdResultXML(-1, true, "R1 "+offString+","+unchangedString);
      }
      
    }
    else if(cmd  =="R1Off")
    {
      if(disableRelay1()==true)
      {
        printCmdResultXML(0, true, "R1 - "+offString+".");
      }
      else
      {
        printCmdResultXML(-1, true, "R1 "+offString+", "+unchangedString);
      }
      
    }
    else if(cmd  =="R1Status")
    {
      Serial.println(getRelay1Status());
    }
    else if(cmd  =="R2On")
    {
      if(enableRelay2()==true)
      {
        printCmdResultXML(0, true, "R2 - "+onString+".");
      }
      else
      {
        printCmdResultXML(-1, true, "R2 "+onString+", "+unchangedString);
      }
      
    }
    else if(cmd  =="R2Off")
    {
      if(disableRelay2()==true)
      {
        printCmdResultXML(0, true, "R2 - "+offString);
      }
      else
      {
        printCmdResultXML(-1, true, "R2 "+offString+", "+unchangedString);
      }
    }
    else if(cmd  =="R2Status")
    {
      Serial.println(getRelay2Status());
    }
    else if(cmd  =="setLowTemp1")
    {
      if(param0f!=-1.0)
      {
        Serial.println(param0f);
        lowTemp1=param0f;
        EEPROM.updateFloat(lowTemp1Address,lowTemp1);//eprom support int writes, so we multiply by 100 on save to obtain an int
        printCmdResultXML(0, true, lowTemp1Tag+" set");
      }
      else
      {
        printCmdResultXML(1, true, missingArgumentString+":"+lowTemp1Tag);
      }
    }
    else if(cmd  =="getLowTemp1")
    {
      
      Serial.print(openTag1+lowTemp1Tag+closingTag);
      Serial.print(lowTemp1);
      Serial.println(openTag2+lowTemp1Tag+closingTag);
    }
    else if(cmd  =="setHighTemp1")
    {
      if(param0f!=-1.0)
      {
        highTemp1=param0f;
        EEPROM.updateFloat(highTemp1Address,highTemp1);//eprom support int writes, so we multiply by 100 on save to obtain an int
        printCmdResultXML(0, true, highTemp1Tag+" set");
      }
      else
      {
        printCmdResultXML(1, true, missingArgumentString+":"+highTemp1Tag);
      }
    }
    else if(cmd  =="getHighTemp1")
    {
      Serial.print(openTag1+highTemp1Tag+closingTag);
      Serial.print(lowTemp1);
      Serial.println(openTag2+highTemp1Tag+closingTag);
      //Serial.print("<highTemp1>");
      //Serial.print(highTemp1);
      //Serial.println("</highTemp1>");
    }
}

float getWaterTemp()
{
  return getTemp(waterTempSensor);
}

float getWaterTemp2()
{
  return getTemp(waterTempSensor2);
}

float getAirTemp()
{
  getTemp(airTempSensor);
}

int fansAutoManagement()
{
  if(waterTemp<lowTemp1)
  {
    digitalWrite(transistor2Pin,LOW);
    transistor2PWM=0;
  }
  else if(waterTemp>highTemp1 && waterTemp<highTemp1+0.2)
  {
    transistor2PWM=128;
    setPWMOutput(transistor2Pin,transistor2PWM);
  }
  else
  {
    digitalWrite(transistor2Pin,HIGH);
    transistor2PWM=255;
  }
}


void getFansStatus()
{
  Serial.println(openTag1+tagFanStatus+closingTag);
  Serial.print(openTag1+tagStatus+closingTag);
  if(transistor2PWM==0)
  {
    Serial.print(offString);
  }
  else 
  {
    Serial.print(onString);
  }
  Serial.println(openTag2+tagStatus+closingTag);
  Serial.print(openTag1+tagPwm+closingTag);
  Serial.print(transistor2PWM);
  Serial.println(openTag2+tagPwm+closingTag);
  Serial.println(openTag2+tagFanStatus+closingTag);
}


/**
 * Sets a PWM output level. Used mainly for output transistors.
 * it does so if the asked level is between 0 and 255,
 * and returns true. Otherwise it returns false.
 * @param output the output pin to be set.
 * @param pwmLevel the wanted pwm level for the pin.
 */
int setPWMOutput(int output,int pwmLevel)
{
  if(pwmLevel>=0 && pwmLevel<=255)
  {
    analogWrite(output,pwmLevel);
    return true;
  }
  else
  {
    return false;
  }
}

/**
 * Sets a PWM output level. Used mainly for output transistors.
 * The user passes a percentage for the output duty cycle. It 
 * thus converts the percentage to a PWM level then calls setPWMOutput.
 * it returns setPWMOutput return value.
 * This function is less precise than setPWMOutput, due to having only
 * 100 possible values, compared to 255 for setPWMOutput.
 * @param output the output pin to be set.
 * @param percent the wanted duty cycle (percentage).
 */
int setPMWMoutputPct(int output,int percent)
{
  return setPWMOutput(output,map(percent, 0, 100, 0, 255));
}

void fadeLights1(int lightsFadeAmount, int lightsFadeDelay)
{
  if(lights1Fade!=0)
  {
    if( millis()>lights1LastChange+lightsFadeDelay )
    {
      lights1LastChange=millis();
      lights1Level=lights1Level+lights1Fade*lightsFadeAmount;
      //setLights1FadeIn or setLights1FadeOut
      if(lights1Level>=lights1Max)
      {
        lights1Level=lights1Max;
        lights1Fade=0;
        lights1Status=1;
      }
      else if(lights1Level<=0)
      {
        lights1Level=0;
        lights1Fade=0;
        lights1Status=0;
      }
      if(lights1Status==1 || lights1Status==0)
      {
        lights1Fade=0;
      }
      
      analogWrite(lights1Pin,lights1Level);
      //Serial.println(lights1Level);
    }
  }
}

int calcFadeDelay(int totalFadeTimeSec)
{
  return ceil(totalFadeTimeSec*1000.0/(float)lights1Max);
}

/**
 * Enables the first set of lights if it is disabled.
 * @returns 1 if the lights1 have been enabled, 0 if it was already enabled.
 */
int enableLights1()
{
  if(lights1Status!=1)
  {
    lights1Status=1;
    lights1Fade=0;
    lights1Level=lights1Max;
    digitalWrite(lights1Pin,lights1Status);
    return 1;
  }
  else
  {
    return 0;
  }
}

/**
 * Disables the first set of lights if it is enabled.
 * @returns 1 if the lights1 have been disabled, 0 if it was already disabled.
 */
int disableLights1()
{
  if(lights1Status!=LOW)
  {
    lights1Status=0;
    lights1Fade=0;
    lights1Level=0;
    digitalWrite(lights1Pin,lights1Status);
    return 1;
  }
  else
    {return 0;}
}

/**
 * Sets the first set of light to fade in.
 * @returns 1 if the lights have been set to fade in, 0 if they were already on.
 */
int setLights1FadeIn()
{
  if(lights1Status!=1)
  {
    lights1Status=2;
    lights1Fade=1;
    return 1;
  }
  else
  {
    lights1Fade=0;
    return 0;
  }
}

/**
 * Disables the first relay if it is enabled.
 * @returns 1 if the lights1 have been set to fade out, 0 if they were already off.
 */
int setLights1FadeOut()
{
  if(lights1Status!=0)
  {
    lights1Status=2;
    lights1Fade=-1;
    return 1;
  }
  else
  {
    lights1Fade=0;
    return 0;
  }
}



/**
 * Enables the first relay if it is disabled.
 * @returns 1 if the relay have been enabled, 0 if it was already enabled.
 */
int enableRelay1()
{
  if(relay1Status!=HIGH)
  {
    relay1Status=HIGH;
    digitalWrite(relay1Pin,relay1Status);
    return true;
  }
  else
  {
    return false;
  }
}

/**
 * Disables the first relay if it is enabled.
 * @returns 1 if the relay have been disabled, 0 if it was already disabled.
 */
int disableRelay1()
{
  if(relay1Status!=LOW)
  {
    relay1Status=LOW;
    digitalWrite(relay1Pin,relay1Status);
    return true;
  }
  else
  {
    return false;
  }
}

/**
 * Generate a XML node indicating if the relay 1 is active or not.
 */
String getRelay1Status()
{
   if(relay1Status==LOW)
   {
     return String(openTag1+tagR1Status+closingTag+offString+openTag2+tagR1Status+closingTag);
   }
   else if(relay1Status==HIGH)
   {
     return String(openTag1+tagR1Status+closingTag+onString+openTag2+tagR1Status+closingTag);
   }
}

/**
 * Enables the second relay if it is disabled.
 * @returns 1 if the relay have been enabled, 0 if it was already enabled.
 */
int enableRelay2()
{
  if(relay2Status!=HIGH)
  {
    relay2Status=HIGH;
    digitalWrite(relay2Pin,relay2Status);
    return true;
  }
  else
  {
    return false;
  }
}

/**
 * Disables the second relay if it is enabled.
 * @returns 1 if the relay have been disabled, 0 if it was already disabled.
 */
int disableRelay2()
{
  if(relay2Status!=LOW)
  {
    relay2Status=LOW;
    digitalWrite(relay2Pin,relay2Status);
    return true;
  }
    else
  {
    return false;
  }
}

/**
 * Generate a XML node indicating if the relay 2 is active or not.
 */
String getRelay2Status()
{
  
   if(relay2Status==LOW)
   {
     return String(openTag1+tagR2Status+closingTag+offString+openTag2+tagR2Status+closingTag);
   }
   else if(relay2Status==HIGH)
   {
     return String(openTag1+tagR2Status+closingTag+onString+openTag2+tagR2Status+closingTag);
   }
}

/**
 * Generate a XML node indicating if the relays are active or not.
 */
/*
String getRelaysStatus()
{
  String msg=new String("<relaysStatus>\n");
  msg=new String(msg+getRelay1Status()+"\n");
  msg=new String(msg+getRelay2Status()+"\n");
  msg=new String(msg+"</relaysStatus>");
  return msg;
}*/


void printCmdResultXML(int errorCode, boolean success, String text)
{
  Serial.println(openTag1+cmdResultTag+closingTag);
    Serial.print(openTag1+resultTag+closingTag);
    if(success=true)
    {
      Serial.print(success);
    }
    else
    {
      Serial.print(failure);
    }
    Serial.println(openTag2+resultTag+closingTag);
    
    Serial.print(openTag1+errorCodeTag+closingTag);
    Serial.print(errorCode);
    Serial.println(openTag2+errorCodeTag+closingTag);
    
    Serial.print(openTag1+textTag+closingTag);
    Serial.print(text);
    Serial.println(openTag2+textTag+closingTag);
  Serial.println(openTag2+cmdResultTag+closingTag);
}


/**
 * Reads the temperature from a DS18S20 probe
 */
float getTemp(OneWire ds){
  //Serial.println("reading");
  //returns the temperature from one DS18S20 in DEG Celsius
  byte data[12];
  byte addr[8];
  if ( !ds.search(addr)) {
      //no more sensors on chain, reset search
      ds.reset_search();
      return -1000;
  }
  if ( OneWire::crc8( addr, 7) != addr[7]) {
      //Serial.println("CRC!");
      return -1000;
  }
  if ( addr[0] != 0x10 && addr[0] != 0x28) {
      //Serial.print("Device");
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
