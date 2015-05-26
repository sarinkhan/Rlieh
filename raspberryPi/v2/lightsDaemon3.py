#!/usr/bin/python
from time import sleep
import smbus
import datetime
import subprocess as sub  
import urllib2, urllib
import time
import os
import RPi.GPIO as GPIO
import subprocess
import serial



while True:
    now = datetime.datetime.now()
    hour=int(now.strftime("%H"))
    hour=int(now.strftime("%H"))
    ts = time.time()
    lightStartHour=11		#heure d'allumage
    lightDuration=10 		#duree d'eclairement
	
    bypassDelayOn=600		#duree d'allumage force, secondes (appui sur le bouton hors horaires d'eclairement)
	
    fh = open("/home/pi/code/lastAction.txt", "r")
    lastActionTS=float(fh.read().replace('\n', ''))
    print lastActionTS
    print ts
    elapsed = ts-lastActionTS
    print elapsed
    fh.close()
	
	
    if elapsed>=bypassDelayOn : 
        if hour>=lightStartHour and hour <24 and hour<lightStartHour+lightDuration : 
            print("lights on")
            #subprocess.Popen("sudo python /home/pi/code/lightsOn.py")
            os.system('sudo python /home/pi/code/lightsOn.py')
        else :
            if lightStartHour+lightDuration >=24 and hour>=(lightStartHour+lightDuration)%24 :
                print("lights off");
                #subprocess.call('sudo python /home/pi/code/lightsOff.py')
                os.system('sudo python /home/pi/code/lightsOff.py')
                #print((lightStartHour+lightDuration)%24)
            elif hour>=lightStartHour+lightDuration or hour < lightStartHour :
                print("lights off");
                #subprocess.call('sudo python /home/pi/code/lightsOff.py')
                os.system('sudo python /home/pi/code/lightsOff.py')
                #print((lightStartHour+lightDuration)%24)
            else :
                print("lights on")
                #subprocess.Popen("sudo python /home/pi/code/lightsOn.py")
                os.system('sudo python /home/pi/code/lightsOn.py')
    else :
        print "allumage manuel pendant %s secondes" % str(bypassDelayOn)

    sleep(6)