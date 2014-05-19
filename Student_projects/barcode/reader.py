import serial
import time

onReader = True

if onReader:
	ser = serial.Serial(
		port='/dev/ttyUSB0',
		baudrate=9600,
		bytesize=8,
		parity='N',
		stopbits=1,
		timeout=None
		)
	
	ser.open()
	print ser.isOpen()

transitter = 0;
line =[]
while (1):
	if onReader:
		data = ser.read()           # Wait forever for anything
	else:
		data = raw_input()
	
	time.sleep(1)              # Sleep (or inWaiting() doesn't give the correct value)
	
	data_left = ser.inWaiting()  # Get the number of characters ready to be read
	
	data += ser.read(data_left) # Do the read and combine it with the first character

	print data 
	linestr = data.rstrip() # removes extra newline
	data = linestr

	if data =="37C200MSRZ":
		print "Its a Box"
		ser.write("Box\r\n")
	elif data == "F8904084707560":
		print "Its a book"
		ser.write("Book RS 100\r\n")
	else:
		print "Not in database"
