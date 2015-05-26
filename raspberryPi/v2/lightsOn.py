#!/usr/bin/python
from time import sleep
import serial
tout=0.5
print("opening serial")

ser = serial.Serial("/dev/ttyUSB0", 57600, timeout=tout)
waitTime1=0.2
sleep(waitTime1)


ser.write("R1On")
resp=ser.readlines()
sleep(waitTime1)
print(resp)
sleep(waitTime1)



ser.write("R2On")
resp=ser.readlines()
sleep(waitTime1)
print(resp)

#ser.write("R2On")
ser.close()
sleep(waitTime1)
