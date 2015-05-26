#!/usr/bin/python
from time import sleep
import serial
tout=0.5


ser = serial.Serial("/dev/ttyUSB0", 57600, timeout=tout)
waitTime1=0.2
sleep(waitTime1)


ser.write("getAirTemp1")
resp=ser.readline()
sleep(waitTime1)
print(resp)
sleep(waitTime1)

ser.close()
sleep(waitTime1)
