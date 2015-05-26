#!/usr/bin/python
from time import sleep
import serial
tout=0.8


ser = serial.Serial("/dev/ttyUSB0", 57600, timeout=tout)
waitTime1=0.2
sleep(waitTime1)

ser.write("getWaterTemp1")
resp=ser.readline()

sleep(waitTime1)
resp=resp.strip()

t2=resp.split('>')[1]
t2=t2.split('<')[0]

print(t2)
sleep(waitTime1)

ser.close()
sleep(waitTime1)
