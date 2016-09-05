/**
 * This program manages Rlieh (Remote Location Intelligent Ecosystem Handler)
 * It is meant to automatically control an aquarium, terrarium, or another closed ecosystem.
 * It manages lights, measures temperatures, and monitors parameters. It can work on it's own
 * on an Arduino compatible chip, or can work with any serial capable device, using the uart 
 * port to communicate, send and recieve data.
 * Made by Audrey Robinel
 * GNU GPL V3
 * 
 * program version 1.0
 */

#include <Wire.h> 
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal_I2C.h>
#include "RTClib.h"

#define AUTO 0            //fully auto mode
#define MANAGED 1         //managed by the pi
#define TEMPO_MANAGED 2   //temporarily managed by the pi


//variables for automatic light management
int lightsMode = AUTO;
int lightOnHour=10;
int lightOffHour=22;



int lightsTemporaryMangedTimeOut=3600;//time for temporary management, in seconds
unsigned long lightsTemporaryManagedStartTime=0;//temporary management start time, used to revert to auto mode after some time

int lcdMode = AUTO;
int lcdTemporaryManagedTimeOut=60;//time for temporary management, in seconds
unsigned long lcdTemporaryManagedStartTime=0;//temporary management start time, used to revert to auto mode after some time


#define LCD_LINES 4
#define LCD_COLS 20


//constants for the verbosity level
#define SILENT 0 //no outputs at all
#define NORMAL 1 //only outputs planed outputs (answers to questions asked on uart)
#define ERRORS 2 //same as the previous, and also outputs error messages.
#define DEBUG 3  //All the previous outputs are kept, but also prints messages for most actions, even successful ones.

//defines the verbosity of the device over the serial port. The higher the number, the more it chats.
int verbosity = NORMAL;

LiquidCrystal_I2C lcd(0x27,LCD_COLS,LCD_LINES);// set the LCD address to 0x27 for a 20 chars and 4 line display
RTC_DS1307 rtc;
int DS18S20_Pin = 2;            //The GPIO where are connected the DS18B20 temperature sensors
OneWire ds(DS18S20_Pin);        

DallasTemperature tSensors(&ds);//temp sensors instantiation

#define NB_TRANSISTORS 4        //number of PWM capable output transistors
#define NB_TEMP_SENSORS 4       //Maximum number of connected temperature sensors


int lightsSlowFadeTime=7000;
int lightsQuickFadeTime=100;

int lightsFadeDefaultTime=lightsSlowFadeTime;    //how long to wait before the next pwm rate modification, in ms
const int lightsPins[4]={3,5,6,9};               //GPIO where are connected the transistors that controls the lights, must be PWM pins.
int lightsStatus[4]={0,0,0,0};                   //Light status. 0=off, 1= on.
int lightsFade[4]={0,0,0,0};                     //The fading status. 0 = no fade, -1 = fade out, 1 = fade in.
int lightsFadeAmount[4]={1,1,1,1};               //By how much do we change the PWM rate each time.
int lightsFadeTime[4]={lightsFadeDefaultTime,lightsFadeDefaultTime,lightsFadeDefaultTime,lightsFadeDefaultTime};//fade time for each light, in ms
int lightsLevel[4]={0,0,0,0};                    // current light level for each light.
int lightsMaxLevel[4]={255,255,255,255};         // the maximum light level (255= fully on, 0= fully off)
unsigned long lightsLastChange[4]={0,0,0,0};     // when the correponding light's level was last updated.




unsigned long currTime=0;

//variables used when pushing the button 1 to bypass certain automated actions.
long bypassTimerDuration=300; //the duration of the bypass due to the button
int bypassTimer=0;            //what is this? should be checked, seems redundant with the next variable
long bypassStartTime=0;       //when we did start to bypass

long debounceDelay = 50;    // the debounce time for the buttons. 

//variables for the first button
const int button1Pin=4;
long lastDebounceTime1 = 0; // the last time the output pin was toggled
int button1State; 
int lastButton1State = HIGH;

//variables for the second, yet unused button.
const int button2Pin=13;
long lastDebounceTime2 = 0; // the last time the output pin was toggled
int button2State; 
int lastButton2State = HIGH;

int backLightStatus=LOW;

int printedLightStatus=0;   //tells if the light status has been printed or not.

int lastTimeReadTemp=0;
float airTemp=0.0;
float waterTemp1=0.0;
float waterTemp2=0.0;

float TC[4]={-127.0,-127.0,-127.0,-127.0};


int  DS18B20resolution = 12;
DeviceAddress tempDeviceAddress;

const char separator=':';//separator for multiple args , and to separate the command from args.
const int cmdLength=5;   //the size, in chars, of the command.

//lcd commands constants
const int colLength=2;
const int rowLength=2;

//led commands constants
const int ledIdLength=2;
const int colorLength=3;

//temp  commands constants
const int tempProbeIdLength=2;
const int lightIdLength=2;

//serial port constants
const long serialSpeed=115200;  //serial speed. 1
const int serialTimeout=10;     //the timeout for the serial terminal. If too high, it will slow down the PWM changes, since the loop function will wait that much time on each pass. 10 works at 250000 speed.


//-------------------------------------------------------------//
//----------------------------SETUP----------------------------//
//-------------------------------------------------------------//
/**
 * This function initializes the LCD, sets the terminal speed and timeout,a few prints on the LCD,
 * pinmodes for a few pins of the Arduino, initializes the sensors, and a few other variables.
 */
void setup()
{
  lcd.init();// initialize the lcd
  //Serial.begin(500000);
  Serial.begin(serialSpeed);
  Serial.setTimeout(serialTimeout);
  if(verbosity>ERRORS)
        {Serial.println("Rlieh V1.0");}
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Rlieh V1.0");

  rtc.begin();
  
  pinMode(button1Pin, INPUT_PULLUP);
  pinMode(button2Pin, INPUT_PULLUP);

  int i=0;
  for(i=0;i<NB_TRANSISTORS;i++)
  {
    pinMode(lightsPins[i],OUTPUT);
    analogWrite(lightsPins[i],lightsLevel[i]);
    lightsLastChange[i]=millis();
  }

  tSensors.begin();
  for(i=0;i<NB_TRANSISTORS;i++)
  {
    tSensors.getAddress(tempDeviceAddress, i);
    tSensors.setResolution(tempDeviceAddress, DS18B20resolution);
  }

  tSensors.setWaitForConversion(false); //set non blocking mode
  tSensors.requestTemperatures();       //asks for temperature readings
  lastTimeReadTemp=millis();            //initializes the timestamp of the last read temperature

}

//-------------------------------------------------------------//
//----------------------------LOOP-----------------------------//
//-------------------------------------------------------------//
/**
 * The main loop of the arduino reads a string from the serial terminal, and passes the string
 * to the parseCMD function (that will mostly return things through the serial port, update flags
 * or variables).
 * 
 * The loop function then lanches the fadeLights function for each transistor, and fetches the values
 * returned by the sensors.
 * 
 * it also monitors a button, and if pressed, it starts the lights if off, or turn it off if it was up.
 * If the button is not pressed, it checks if some timed commands are finished and changes automated modes
 * accordingly. Note that in managed mode, this part is ignored.
 * 
 * Depending on the mode, it also manages automated printing on the screen (if in automated screen mode).
 */
void loop()
{
   String s=Serial.readString();      //reads a string from serial (UART)
  
   if(verbosity>ERRORS)
        {Serial.println(s);}
        
   parseCMD(s);                       //parses the command and acts accordingly
   
   int i=0;
   for(i=0;i<NB_TRANSISTORS;i++)
   {
    fadeLights(i);//this updates the lights PWM level if they are fading 
   }
  
   if(millis()-lastTimeReadTemp>800)          //reading sensors takes 800ms, so if more time has elapsed since last read, we get the values
   {

    for(i=0;i<NB_TEMP_SENSORS;i++)
      {TC[i]=tSensors.getTempCByIndex(i);}  //we store the temperature of each sensors

    tSensors.requestTemperatures();         //request for another read
    lastTimeReadTemp=millis();              //and reset the timer to current time
   }

   //this part is legacy, and handles button 1, but it's likely to change
   int readingB1 = digitalRead(button1Pin); //read the button
   if (readingB1 != lastButton1State) 
   {
    lastDebounceTime1 = millis(); //debouncint stuff
   }
   
   if ((millis() - lastDebounceTime1) > debounceDelay) //more debouncint stuff
   {
    if (readingB1 != button1State) 
    {
      button1State = readingB1;
      
      if (button1State == LOW) //if the button was pressed (it's a pull up, so pressed is LOW)
      {
        bypassTimer=1;        //we bypass the automated timed stuff
        
        for(i=0;i<NB_TRANSISTORS;i++)
          {lightsFadeTime[i]=lightsQuickFadeTime;} //we define the fade time as the fast value

          
        //backLightStatus = !backLightStatus;         //we also invert the backlight
        bypassStartTime=millis();                   //and record when we started bypassing the automated timers


        for(i=0;i<NB_TRANSISTORS;i++)               //this applies the following changes to all lights
         {
            if(lightsFade[i]==1)//if lights fading in
            {
              setLightsFadeOut(i);  //set to fade out
              backLightStatus=LOW;
            }
            else if(lightsFade[i]==-1)//else if lights fading out
            {
              setLightsFadeIn(i);     //set to fade in
              backLightStatus=HIGH;
            }
            else if(lightsLevel[i]<lightsMaxLevel[i])//else if lights not fading, but not fully on
            {
              setLightsFadeIn(i); //finish fading in
              backLightStatus=HIGH;
            }
            else//else : lights not fading and fully on
            {
              setLightsFadeOut(i); // fade out
              backLightStatus=LOW;
            }  
         }
         
      }
      
    }
    
  }//end debounced push button management
  
  lastButton1State = readingB1;
  DateTime now = rtc.now();
  if(lcdMode==AUTO)//if in auto mode, we can print the temperature
  {
  //this manages the backight, according to the backLightStatus variable
    if(backLightStatus==LOW)
    {
      lcd.noBacklight();
    }
    else
    {
      lcd.backlight();
    }
    currTime=millis();
    
    
    if(LCD_LINES>=4)
    {
    printTimeOnLCD(lcd,3,0);  // prints the time in a consistant format
    lcd.print("  ");
    printDateOnLCD(lcd,3,10); //prints the date on an also consistant format
    lcd.setCursor(0, 1);
    }

  
    //this section prints the temperatures if a probe is connected.
    for(i=0;i<NB_TEMP_SENSORS;i++)
    {
      TC[i]=tSensors.getTempCByIndex(i);
      if(i==2)
      {
        lcd.setCursor(0, 2);
      }
      if(TC[i]>-126)
      {
        lcd.print("T");
        lcd.print(i+1);
        lcd.print(":");
        lcd.print(TC[i]);
        lcd.print(" ");
      }
    }
  }

  
  if(bypassTimer==0)//if we are not bypassing the automated timers
  {
    
    if(lightsMode==AUTO) //if in automated mode
    {
      if(now.hour()>=lightOnHour && now.hour()<lightOffHour)
      {
        for(i=0;i<NB_TRANSISTORS;i++)
          {setLightsFadeIn(i);}
        backLightStatus = HIGH;
      }
      else
      {
        backLightStatus = LOW;
        for(i=0;i<NB_TRANSISTORS;i++)
          {setLightsFadeOut(i);}
      }
    }
  }
  else
  {
    if(millis()>bypassStartTime+bypassTimerDuration*1000)
    {
      bypassTimer=0;
      for(i=0;i<NB_TRANSISTORS;i++)
      {
        lightsFadeTime[i]=lightsSlowFadeTime;
        if(verbosity>ERRORS)
          {Serial.println("revert to slow fade time");}
      }
    }
  }

  if(lightsMode==TEMPO_MANAGED) //if in temporary managed mode
  {
    if(millis()>lightsTemporaryManagedStartTime+lightsTemporaryMangedTimeOut*1000)//if we have exceeded the temporary managed mode time
    {
      lightsMode=AUTO;  //revert to auto mode
    }
  }

  if(lcdMode==TEMPO_MANAGED) //if in temporary managed mode
  {
    if(millis()>lcdTemporaryManagedStartTime+lcdTemporaryManagedTimeOut*1000)//if we have exceeded the temporary managed mode time
    {
      lcdMode=AUTO;  //revert to auto mode
    }
  }
}

/**
 * This function parses a string to extract a command. It uses multiple global variables, 
 * mainly for the length of the commands, arguments, etc.
 * @param s the string to parse, formated as commd:args, where commd is the command, and args is a list of arguments, 
 * separated by ':' if multiple arguments are present. There is no terminal character for now.
 */
void parseCMD(String s)
{
  int i=0;
  if(s!=NULL)
  {
    //print message : lcd01:rw:cl:message
    if(s.substring(0,cmdLength).compareTo("lcd01")==0)
    {
      if(verbosity>ERRORS)
        {Serial.println("match lcd01");}
      int row=s.substring(cmdLength+1,cmdLength+1+rowLength).toInt();
      int col=s.substring(cmdLength+1+rowLength+1,cmdLength+1+rowLength+1+colLength).toInt();
      lcd.setCursor(col,row);
      if(lcdMode==AUTO)
      {
        lcdMode=TEMPO_MANAGED;
        lcdTemporaryManagedStartTime=millis();
      }
      lcd.print(s.substring(cmdLength+1+rowLength+1+colLength+1));
    }
    //delete line : lcd02:ln (ln= line number)
    else if(s.substring(0,cmdLength).compareTo("lcd02")==0)
    {
      if(verbosity>ERRORS)
        {Serial.println("match lcd02");}
      int ln=s.substring(cmdLength+1,cmdLength+1+rowLength).toInt();
      lcd.setCursor(ln,0);
      clearLine(lcd,ln);
      if(lcdMode==AUTO)
      {
        lcdMode=TEMPO_MANAGED;
        lcdTemporaryManagedStartTime=millis();
      }
    }
    //clear all : lcd03
    else if(s.substring(0,cmdLength).compareTo("lcd03")==0)
    {
      if(verbosity>ERRORS)
        {Serial.println("match lcd03");}
      lcd.clear();
      if(lcdMode==AUTO)
      {
        lcdMode=TEMPO_MANAGED;
        lcdTemporaryManagedStartTime=millis();
      }
    }
    //turn backlight off : lcd04
    else if(s.substring(0,cmdLength).compareTo("lcd04")==0)
    {
      if(verbosity>ERRORS)
        {Serial.println("match lcd04");}
      lcd.noBacklight();
      backLightStatus=LOW;
      if(lcdMode==AUTO)
      {
        lcdMode=TEMPO_MANAGED;
        lcdTemporaryManagedStartTime=millis();
      }
    }
    //turn backlight on : lcd05
    else if(s.substring(0,cmdLength).compareTo("lcd05")==0)
    {
      if(verbosity>ERRORS)
        {Serial.println("match lcd05");}
      lcd.backlight();
      backLightStatus=HIGH;
      if(lcdMode==AUTO)
      {
        lcdMode=TEMPO_MANAGED;
        lcdTemporaryManagedStartTime=millis();
      }
    }
    //sets or gets the verbosity level
    else if(s.substring(0,cmdLength).compareTo("verbo")==0)
    {
      if(s.length()==cmdLength)
      {
        Serial.println(verbosity);
      }
      else
      {
        verbosity=s.substring(cmdLength+1,cmdLength+1+1).toInt();
      }
      if(verbosity>ERRORS)
        {Serial.println("match verbo");}
    }
    else if(s.substring(0,cmdLength).compareTo("lcdto")==0)//sets or gets LCD temporary managed timeout
    {
      if(s.length()==cmdLength)
      {
        Serial.println(lcdTemporaryManagedTimeOut);
      }
      else
      {
        lcdTemporaryManagedTimeOut=s.substring(cmdLength+1).toInt();
      }
      if(verbosity>ERRORS)
        {Serial.println("match lcdto");}
    }
     else if(s.substring(0,cmdLength).compareTo("lcdmd")==0)//sets LCD mode
     {
      if(s.length()==cmdLength)
      {
        Serial.println(lcdMode);
      }
      else
      {
        lcdMode=s.substring(cmdLength+1).toInt();
        if(lcdMode==TEMPO_MANAGED)
        {
          lcdTemporaryManagedStartTime=TEMPO_MANAGED ;
          lcdTemporaryManagedStartTime=millis();
        }
      }
      if(verbosity>ERRORS)
        {Serial.println("match lcdmo");}
    }
    else if(s.substring(0,cmdLength).compareTo("ligto")==0)//sets lights temporary managed timeout
    {
      if(s.length()==cmdLength)
      {
        Serial.println(lightsTemporaryMangedTimeOut);
      }
      else
      {
        lightsTemporaryMangedTimeOut=s.substring(cmdLength+1).toInt();
      }
      if(verbosity>ERRORS)
        {Serial.println("match ligto");}
    }
    else if(s.substring(0,cmdLength).compareTo("ligmd")==0)//sets lights temporary managed timeout
    {
      if(s.length()==cmdLength)
      {
        Serial.println(lightsMode);
      }
      else
      {
        lightsMode=s.substring(cmdLength+1).toInt();
      }
      if(verbosity>ERRORS)
        {Serial.println("match ligmd");}
    }
    else if(s.substring(0,cmdLength).compareTo("liqft")==0)//sets or gets lights quick fade time (in ms)
    {
      if(s.length()==cmdLength)
      {
        Serial.println(lightsQuickFadeTime);
      }
      else
      {
        lightsQuickFadeTime=s.substring(cmdLength+1).toInt();
      }
      if(verbosity>ERRORS)
        {Serial.println("match liqft");}
    }
    else if(s.substring(0,cmdLength).compareTo("lisft")==0)//sets or gets lights slow fade time (in ms)
    {
      if(s.length()==cmdLength)
      {
        Serial.println(lightsSlowFadeTime);
      }
      else
      {
        lightsSlowFadeTime=s.substring(cmdLength+1).toInt();
      }
      if(verbosity>ERRORS)
        {Serial.println("match lisft");}
    }
    else if(s.substring(0,cmdLength).compareTo("ligh1")==0)//sets or gets the lights on hour in auto mode
    {
      if(s.length()==cmdLength)
      {
        Serial.println(lightOnHour);
      }
      else
      {
        lightOnHour=s.substring(cmdLength+1).toInt();
      }
      if(verbosity>ERRORS)
        {Serial.println("match ligh1");}
    }
    else if(s.substring(0,cmdLength).compareTo("ligh2")==0)//sets or gets the lights on hour in auto mode
    {
      if(s.length()==cmdLength)
      {
        Serial.println(lightOffHour);
      }
      else
      {
        lightOffHour=s.substring(cmdLength+1).toInt();
      }
      if(verbosity>ERRORS)
        {Serial.println("match ligh2");}
    }
    else if(s.substring(0,cmdLength).compareTo("lisqf")==0)//set lights fade speed to quick setting
    {
      int lightIdVar=s.substring(cmdLength+1,cmdLength+1+lightIdLength).toInt();
      if(verbosity>ERRORS)
        {Serial.println("match lisqf");}
      if(lightIdVar==0)
      {
        for(i=1;i<NB_TRANSISTORS+1;i++)
          {lightsFadeTime[i-1]=lightsQuickFadeTime;}
      }
      else
      {
        lightsFadeTime[lightIdVar-1]=lightsQuickFadeTime;;
      }
    }
    else if(s.substring(0,cmdLength).compareTo("lissf")==0)//set lights fade speed to quick setting
    {
      int lightIdVar=s.substring(cmdLength+1,cmdLength+1+lightIdLength).toInt();
      if(verbosity>ERRORS)
        {Serial.println("match lissf");}
      if(lightIdVar==0)
      {
        for(i=1;i<NB_TRANSISTORS+1;i++)
        {lightsFadeTime[i-1]=lightsSlowFadeTime;}
      }
      else
      {
        lightsFadeTime[lightIdVar-1]=lightsSlowFadeTime;;
      }
    }
    else if(s.substring(0,cmdLength).compareTo("lisfs")==0)//set lights fade speed to parametred value
    {
      int lightIdVar=s.substring(cmdLength+1,cmdLength+1+lightIdLength).toInt();
      int lightFadeTimeVar=s.substring(cmdLength+1+lightIdLength+1).toInt();
      if(verbosity>ERRORS)
        {Serial.println("match lisfs");}
      if(lightIdVar==0)
      {
        for(i=1;i<NB_TRANSISTORS+1;i++)
        {lightsFadeTime[i-1]=lightFadeTimeVar;}
      }
      else
      {
        lightsFadeTime[lightIdVar-1]=lightFadeTimeVar;
      }
    }
    else if(s.substring(0,cmdLength).compareTo("li_fi")==0)//set lights to fade in. if in auto mode, switches to temporary managed mode.
    {
      int lightIdVar=s.substring(cmdLength+1,cmdLength+1+lightIdLength).toInt();
      if(verbosity>ERRORS)
        {Serial.println("match li_fi");}
      if(lightIdVar==0)
      {
        for(i=1;i<NB_TRANSISTORS+1;i++)
        {setLightsFadeIn(i-1);}
      }
      else
      {
        setLightsFadeIn(lightIdVar-1);
      }
      if(lightsMode==AUTO)
      {
        lightsMode=TEMPO_MANAGED;
        lightsTemporaryManagedStartTime=millis();
      }
    }
    else if(s.substring(0,cmdLength).compareTo("li_fo")==0)//set lights to fade in. if in auto mode, switches to temporary managed mode.
    {
      int lightIdVar=s.substring(cmdLength+1,cmdLength+1+lightIdLength).toInt();
      if(verbosity>ERRORS)
        {Serial.println("match li_fo");}
      if(lightIdVar==0)
      {
        for(i=1;i<NB_TRANSISTORS+1;i++)
        {setLightsFadeOut(i-1);}
      }
      else
      {
        setLightsFadeOut(lightIdVar-1);
      }
      if(lightsMode==AUTO)
      {
        lightsMode=TEMPO_MANAGED;
        lightsTemporaryManagedStartTime=millis();
      }
    }
    else if(s.substring(0,cmdLength).compareTo("liset")==0)//set lights without fading. if in auto mode, switches to temporary managed mode.
    {
      if(verbosity>ERRORS)
        {Serial.println("match liset");}
      int lightIdVar=s.substring(cmdLength+1,cmdLength+1+lightIdLength).toInt();
      if(verbosity>ERRORS)
        {
          Serial.print("lightIdVar=");
          Serial.println(lightIdVar);
        }
      int lightSettingVar=s.substring(cmdLength+1+lightIdLength+1,cmdLength+1+lightIdLength+1+1).toInt();
      if(verbosity>ERRORS)
        {
          Serial.print("lightSettingVar=");
          Serial.println(lightSettingVar);
        }
      if(lightIdVar==0)
      {
        for(i=1;i<NB_TRANSISTORS+1;i++)
        {
          if(lightSettingVar==0)
            {disableLights(i-1);}
          else
            {enableLights(i-1);}  
        }
      }
      else
      {
          if(lightSettingVar==0)
            {disableLights(lightIdVar-1);}
          else
            {enableLights(lightIdVar-1);}
      }
      if(lightsMode==AUTO)
      {
        lightsMode=TEMPO_MANAGED;
        lightsTemporaryManagedStartTime=millis();
      }
    }
    else if(s.substring(0,cmdLength).compareTo("getTC")==0)
    {
      if(verbosity>SILENT)
      {
      int sensorId=s.substring(cmdLength+1,cmdLength+1+tempProbeIdLength).toInt();

      if(sensorId==0)
      {
        for(i=1;i<NB_TEMP_SENSORS+1;i++)
        {
          if(TC[i-1]>-126)
          {
          if(i>1)
          {Serial.print(";");}
          Serial.print(TC[i-1]);
          }
        }
        Serial.println("");
      }
      else
      {
          Serial.println(TC[sensorId-1]);
      }
      }
      if(verbosity>ERRORS)
        {Serial.println("match getTC");}
    }
    else
    {
      if(verbosity>NORMAL)
        {Serial.println("error:unknown command");}
    }
  }
}

/**
 * Clears the specified line by printing blank characters
 * @param line the line to be cleared
 * @param lcd the lcd on which the line has to be cleared
 */
void clearLine(LiquidCrystal_I2C lcd,int line)
{
  lcd.setCursor(0, line);
  int i=0;
  for(i=0;i<LCD_COLS;i++)
  {
    lcd.print(" ");
  }
}

/**
 * Clears the specified line by printing blank characters from a starting position
 * @param line the line to be cleared
 * @param lcd the lcd on which the line has to be cleared
 * @param from the index from which the line will be cleared
 */
void clearLineFrom(LiquidCrystal_I2C lcd,int line, int from)
{
  lcd.setCursor(from, line);
  int i=0;
  for(i=from;i<LCD_COLS;i++)
  {
    lcd.print(" ");
  }
}

/**
 * Clears the specified line by printing blank characters from a starting position
 * @param line the line to be cleared
 * @param lcd the lcd on which the line has to be cleared
 * @param until the index until which the line will be cleared
 */
void clearLineUntil(LiquidCrystal_I2C lcd,int line, int until)
{
  lcd.setCursor(0, line);
  int i=0;
  for(i=0;i<until;i++)
  {
    lcd.print(" ");
  }
}

/**
 * Clears the specified line by printing blank characters from a starting position until an ending position
 * @param line the line to be cleared
 * @param lcd the lcd on which the line has to be cleared
 * @param from the index from which the line will be cleared
 * @param until the index until which the line will be cleared
 */
void clearLineFromUntil(LiquidCrystal_I2C lcd,int line,int from, int until)
{
  lcd.setCursor(0, line);
  int i=0;
  for(i=from;i<until;i++)
  {
    lcd.print(" ");
  }
}

/**
 * This function prints the time on the screen, with a consistant format.
 * The format is HH:MM:SS.
 * @param lcd the lcd to print on
 * @param line the line where we'll print
 * @param col the column where we'll print
 */
void printTimeOnLCD(LiquidCrystal_I2C lcd,int line,int col)
{
  lcd.setCursor(col, line);
  DateTime now = rtc.now();
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
}

/**
 * This function prints the date on the screen, with a consistant format.
 * The format is JJ/MM/AAAA.
 * @param lcd the lcd to print on
 * @param line the line where we'll print
 * @param col the column where we'll print
 */
void printDateOnLCD(LiquidCrystal_I2C lcd,int line,int col)
{
  lcd.setCursor(col, line);
  DateTime now = rtc.now();
  if(now.day()<10)
    {lcd.print(0);}
  lcd.print(now.day(), DEC);
  lcd.print('/');
  if(now.month()<10)
    {lcd.print(0);}
  lcd.print(now.month(), DEC);
  lcd.print('/');
  lcd.print(now.year(), DEC);
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

/**
 * This function manages the fading of the light passed as an argument.
 * It changes the PWM rate at the according time, depending on multiple variables.
 * this function does the actual variations, and is not used to set fading in or out.
 * Depending on the fading direction, saved in lightsFade[i], it fades in (value=1)
 * or out (value=-1).
 * @param lightId the id of the light to fade, int starting from 0 (icnluded)
 */
void fadeLights(int lightId)
{
  if(lightsFade[lightId]!=0)
  {
    if( millis()>lightsLastChange[lightId]+lightsFadeTime[lightId] )
    {
      //Serial.print(lightsFadeTime[lightId]);
      lightsLastChange[lightId]=millis();
      lightsLevel[lightId]=lightsLevel[lightId]+lightsFade[lightId]*lightsFadeAmount[lightId];
      //setLights1FadeIn or setLights1FadeOut
      if(lightsLevel[lightId]>=lightsMaxLevel[lightId])
      {
        lightsLevel[lightId]=lightsMaxLevel[lightId];
        lightsFade[lightId]=0;
        lightsStatus[lightId]=1;
      }
      else if(lightsLevel[lightId]<=0)
      {
        lightsLevel[lightId]=0;
        lightsFade[lightId]=0;
        lightsStatus[lightId]=0;
      }
      if(lightsStatus[lightId]==1 || lightsStatus[lightId]==0)
      {
        lightsFade[lightId]=0;
      }
      
      analogWrite(lightsPins[lightId],lightsLevel[lightId]);
      //Serial.println(lights1Level);
    }
  }
}

/**
 * This function is used to calculate the necessary fade delay to have a total fade time of
 * totalFadeTimeSec, and reach a PWM rate of lightMaxLevel (max value: 255).
 * This function assumes a PWM change rate of 1, meaning that it will increase or decrease of 1 
 * each time.
 * This function is currently not used anymore, and should be used with an extra argument,
 * for the pwm change rate, if it is kept in subsequent versions.
 * @param totalFadeTimeSec the total fade time wanted, in seconds
 * @param the maximum light level
 */
int calcFadeDelay(int totalFadeTimeSec, int lightMaxLevel)
{
  return ceil(totalFadeTimeSec*1000.0/(float)lightMaxLevel);
}


/**
 * Enables the defined set of lights if it is disabled.
 * @lightsId the id of the lights to enable
 * @returns 1 if the lights 'lightsId'  have been enabled, 0 if it was already enabled.
 */
int enableLights(int lightsId)
{
  if(lightsStatus[lightsId]!=1)
  {
    lightsStatus[lightsId]=1;
    lightsFade[lightsId]=0;
    lightsLevel[lightsId]=lightsMaxLevel[lightsId];
    digitalWrite(lightsPins[lightsId],lightsStatus[lightsId]);
    return 1;
  }
  else
  {
    return 0;
  }
}

/**
 * Disables the defined set of lights if it is enabled.
 * @lightsId the id of the lights to disable
 * @returns 1 if the lights 'lightsId' have been disabled, 0 if it was already disabled.
 */
int disableLights(int lightsId)
{
  if(lightsStatus[lightsId]!=LOW)
  {
    lightsStatus[lightsId]=0;
    lightsFade[lightsId]=0;
    lightsLevel[lightsId]=0;
    digitalWrite(lightsPins[lightsId],lightsStatus[lightsId]);
    return 1;
  }
  else
    {return 0;}
}

/**
 * Sets the defined set of light to fade in.
 * @returns 1 if the lights have been set to fade in, 0 if they were already fully on.
 */
int setLightsFadeIn(int lightsId)
{
  if(lightsStatus[lightsId]!=1)
  {
    lightsStatus[lightsId]=2;
    lightsFade[lightsId]=1;
    return 1;
  }
  else
  {
    lightsFade[lightsId]=0;
    return 0;
  }
}

/**
 * Sets the defined set of light to fade out.
 * @returns 1 if the lights have been set to fade out, 0 if they were already fully off.
 */
int setLightsFadeOut(int lightsId)
{
  if(lightsStatus[lightsId]!=0)
  {
    lightsStatus[lightsId]=2;
    lightsFade[lightsId]=-1;
    return 1;
  }
  else
  {
    lightsFade[lightsId]=0;
    return 0;
  }
}




