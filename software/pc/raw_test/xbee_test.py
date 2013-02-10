#!/usr/bin/python
import ConfigParser, os

import ConfigParser
config = ConfigParser.SafeConfigParser()
if not os.path.isfile('xbee.ini'):
    config.add_section('modem')
    config.set('modem', 'port', '/dev/whatever')
    with open('xbee.ini', 'wb') as configfile:
        config.write(configfile)
config.read('xbee.ini')



from xbee import ZigBee
import serial
from struct import pack
import time

def cb(x):
	print x

ser = serial.Serial(config.get('modem', 'port'), 57600)
xb = ZigBee(ser,callback=cb,escaped=True)

daddr = pack('>Q',0x13A20040300000) # set 64-bit address here

print xb.at(command='ND')

if 0:
	while True:
		print "one"
		for x in range(0xFF):
			data = pack('BBBB', 0, x,x,x)
			xb.tx( dest_addr = '\xff\xfe', dest_addr_long = daddr, data = data )
			time.sleep(0.1)

if 0:
	data = pack('BBBB', 0, 0xff,0xff,0xff)
	xb.tx( dest_addr = '\xff\xfe', dest_addr_long = daddr, data = data )
