#!/usr/bin/python -i
import ConfigParser, os, sys, binascii
config = ConfigParser.SafeConfigParser()
if not os.path.isfile('xbee.ini'):
    config.add_section('modem')
    config.set('modem', 'port', '/dev/whatever')
    config.add_section('node')
    config.set('node', 'short_addr', '0x4916')
    config.set('node', 'long_addr', '0x0013A200403AA07A')
    with open('xbee.ini', 'wb') as configfile:
        config.write(configfile)
    print "Edit xbee.ini for your modem port"
    sys.exit(1)
config.read('xbee.ini')

import logging
logger = logging.getLogger('ZigBee')
logger.setLevel(logging.DEBUG)

def inify_address(addr)
    return "0x%s" % binascii.hexlify(addrd).upper()


class Unbuffered:
   def __init__(self, stream):
       self.stream = stream
   def write(self, data):
       self.stream.write(data)
       self.stream.flush()
   def __getattr__(self, attr):
       return getattr(self.stream, attr)

import sys
sys.stdout=Unbuffered(sys.stdout)



from xbee import ZigBee
import serial
from struct import pack
import time

def cb(x):
    print x

def scb(x):
    print "Start, got x %s" % repr(x)


short_int = eval(config.get('node','short_addr'))
short_daddr = pack(">H", short_int)
long_daddr = pack('>Q',eval(config.get('node','long_addr'))) # set 64-bit address here

print "parsed short_daddr=%s\n" % repr(short_daddr)
print "parsed long_daddr=%s\n" % repr(long_daddr)


ser = serial.Serial(config.get('modem', 'port'), 57600)
xb = ZigBee(ser,callback=cb,escaped=True,start_callback=scb)
xb.start()



print xb.at(command='ND')

if 0:
    while True:
        print "one"
        for x in range(0xFF):
            data = pack('BBBB', 0, x,x,x)
            xb.tx( dest_addr = short_daddr , dest_addr_long = long_daddr, data = data )
            time.sleep(0.1)

if 0:
    print "foo"
    data = pack('BBBB', 0, 0xff,0xff,0xff)
    xb.tx( dest_addr = short_daddr, dest_addr_long = long_daddr, data = data )


print "use inify('adress') to dump address in format suitable for the .ini file\n"

