#!/usr/bin/python
from time import sleep
import serial

tout=0.2


ser = serial.Serial("/dev/ttyUSB0", 57600, timeout=tout)
waitTime1=0.2

sleep(waitTime1)
ser.write("T2On")
sleep(waitTime1)
ser.close()
sleep(waitTime1)