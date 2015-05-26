#!/usr/bin/python
from time import sleep
from Adafruit_I2C import Adafruit_I2C
from Adafruit_MCP230xx import Adafruit_MCP230XX
from CharLCD import CharLCD
import smbus
import datetime
import subprocess as sub  
import urllib2, urllib
import time
import os
import RPi.GPIO as GPIO

GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)


#fonction lisant les donnees SPI de la puce MCP3008, parmi 8 entrees, de 0 a 7
def readadc(adcnum, clockpin, mosipin, misopin, cspin):
        if ((adcnum > 7) or (adcnum < 0)):
                return -1
        GPIO.output(cspin, True)
        GPIO.output(clockpin, False)  # start clock low
        GPIO.output(cspin, False)     # bring CS low
 
        commandout = adcnum
        commandout |= 0x18  # start bit + single-ended bit
        commandout <<= 3    # we only need to send 5 bits here
        for i in range(5):
                if (commandout & 0x80):
                        GPIO.output(mosipin, True)
                else:   
                        GPIO.output(mosipin, False)
                commandout <<= 1
                GPIO.output(clockpin, True)
                GPIO.output(clockpin, False)
 
        adcout = 0
        # read in one empty bit, one null bit and 10 ADC bits
        for i in range(12):
                GPIO.output(clockpin, True)
                GPIO.output(clockpin, False)
                adcout <<= 1
                if (GPIO.input(misopin)):
                        adcout |= 0x1
 
        GPIO.output(cspin, True)
		
        adcout /= 2       # first bit is 'null' so drop it
        return adcout
 
def Run(cmd):  
    try:  
        p = sub.Popen(cmd, stdout=sub.PIPE, stderr=sub.PIPE)  
        p.wait()   
        return p.communicate()  
    except OSError:  
        print "OSError"  

if __name__ == '__main__':
    # ces numeros de pins GPIO doivent etre modifies pour correspondre aux broches utilisees.
    SPICLK = 18
    SPIMISO = 23
    SPIMOSI = 24
    SPICS = 25
	
    adcnumTmp36 = 0
    adcnumLight = 1
 
    # definition de l'interface SPI
    GPIO.setup(SPIMOSI, GPIO.OUT)
    GPIO.setup(SPIMISO, GPIO.IN)
    GPIO.setup(SPICLK, GPIO.OUT)
    GPIO.setup(SPICS, GPIO.OUT)

    lcd = CharLCD(busnum = 1)
    lcd.begin(20,4)
    lcd.clear()
    lcd.writeTextAutoWrap("Welcome to Rlieh.")
    sleep(0.5)
    lcd.clear()
	
    counter1=0
    counter2=0
    counter3=0
    counter4=0

    ipcmd = "ip addr show eth0 | grep inet | awk '{print $2}' | cut -d/ -f1"

    while True:
	
	sleep(0.2)
	
	#network - line 0
        if counter1<8 :	
			ip01 = sub.Popen(["/home/pi/code/./ip_wlan0.sh"],stdout = sub.PIPE,stderr = sub.PIPE)
			eth0, error = ip01.communicate()
			if(counter1==0):
				lcd.clearLine(0)
			if eth0!=" ":
				lcd.writeLine("wlan0 : %s" % eth0.rstrip() ,0)
			else :
				lcd.writeLine("wlan0 : no connection"  ,0)
			counter1=counter1+1
        else :
			ping01 = sub.Popen(["/home/pi/code/./ping.sh"],stdout = sub.PIPE,stderr = sub.PIPE)
			ping, error = ping01.communicate()
			if(counter1==8):
				lcd.clearLine(0)
			lcd.writeLine("Ping : %s ms" % ping.rstrip() ,0)
			counter1=counter1+1
			
        if(counter1>10):
			counter1=0
			lcd.clearLine(0)
			
        if(counter2>10):
			counter2=0
			lcd.clearLine(1)
			lcd.clearLine(2)  	
			
		#tank1 - line 1
        wt01 = sub.Popen(["/home/pi/code/./getWT1.py"],stdout = sub.PIPE,stderr = sub.PIPE)
        wt1, error = wt01.communicate()
        lcd.writeLine("Tank1 = %s*C" % wt1.rstrip() ,1)
        #print(wt1.rstrip())

		#tank2 - line 2	
        wt02 = sub.Popen(["/home/pi/code/./getWT2.py"],stdout = sub.PIPE,stderr = sub.PIPE)
        wt2, error = wt02.communicate()
        lcd.writeLine("Tank2 = %s*C" % wt2.rstrip() ,2)
        counter2=counter2+1
       
		
		
        #now = datetime.datetime.now()
        #lcd.writeLine(now.strftime("%d/%m/%Y %H:%M"),2)
		
		#system - line 3
        if counter4<5 :
			if(counter4==0):
				lcd.clearLine(3)	
			at01 = sub.Popen(["/home/pi/code/./getAT1.py"],stdout = sub.PIPE,stderr = sub.PIPE)
			at1, error = at01.communicate()
			lcd.writeLine("T air = %s*C" % at1.rstrip() ,3)
			counter4=counter4+1			
        else :	
			if(counter4==5):
				lcd.clearLine(3)
			up01 = sub.Popen(["/home/pi/code/./getUptime2.sh"],stdout = sub.PIPE,stderr = sub.PIPE)
			up02, error = up01.communicate()
			lcd.writeLine("                    " ,3)
			lcd.writeLine("uptime : %s" % up02.rstrip() ,3)
			counter4=counter4+1
        if(counter4>10):
			counter4=0	
		
