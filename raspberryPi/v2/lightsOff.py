#!/usr/bin/python
from time import sleep
import serial

tout=0.2

print("opening serial")

ser = serial.Serial("/dev/ttyUSB0", 57600, timeout=tout)
waitTime1=0.2

print ("waiting")
sleep(waitTime1)
print("sending cmd")
ser.write("R1Off")
sleep(waitTime1)
ser.write("R2Off")
sleep(waitTime1)
ser.close()
sleep(waitTime1)