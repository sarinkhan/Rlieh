import serial, time
#ser = serial.Serial('/dev/ttyAMA0', 9600, timeout=2)
ser = serial.Serial()

#ser.port = "/dev/ttyUSB0"
ser.port = "/dev/ttyAMA0"

#ser.port = "/dev/ttyS2"
ser.baudrate = 57600
ser.bytesize = serial.EIGHTBITS #number of bits per bytes
ser.parity = serial.PARITY_NONE #set parity check: no parity
ser.stopbits = serial.STOPBITS_ONE #number of stop bits

#ser.timeout = None          #block read
ser.timeout = 1            #non-block read

#ser.timeout = 2              #timeout block read
ser.xonxoff = False     #disable software flow control

ser.rtscts = False     #disable hardware (RTS/CTS) flow control

ser.dsrdtr = False       #disable hardware (DSR/DTR) flow control
ser.writeTimeout = 2     #timeout for write


try: 
    ser.open()
except Exception, e:
    print "error open serial port: " + str(e)
    exit()
	
time.sleep(0.3)
	
if ser.isOpen():
    try:
		ser.flushInput() #flush input buffer, discarding all its contents
		ser.flushOutput()#flush output buffer, aborting current output 
		ser.write("lightOn")
		time.sleep(0.3)  #give the serial port sometime to receive the data
		response = ser.readline()
		print( response)
		ser.close()
    except Exception, e1:

		print "error communicating...: " + str(e1)

else:

    print "cannot open serial port "
