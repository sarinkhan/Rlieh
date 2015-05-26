#!/usr/bin/python
from time import sleep
import serial
from lxml import etree
tout=0.5


ser = serial.Serial("/dev/ttyUSB0", 57600, timeout=tout)
waitTime1=0.2
sleep(waitTime1)


ser.write("getWaterTemp1")
resp=ser.readlines()
sleep(waitTime1)
#tree = etree.parse(resp)
#wt=user.get("/waterTemp1")
#waterTemp=wt.text
#for user in tree.xpath("/waterTemp1"):
#    print(user.text)
#print(wt)
sleep(waitTime1)

ser.close()
sleep(waitTime1)
