#!/usr/bin/python
import time, serial, sys, os
import binascii, string
import ConfigParser
config = ConfigParser.SafeConfigParser()
if not os.path.isfile('xbee.ini'):
    config.add_section('modem')
    config.set('modem', 'port', '/dev/whatever')
    with open('xbee.ini', 'wb') as configfile:
        config.write(configfile)
    print "Edit xbee.ini for your modem port"
    sys.exit(1)
config.read('xbee.ini')
port = serial.Serial(config.get('modem', 'port'), 57600, timeout=0.01)


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


while True:
    if not port.inWaiting():
        # Don't try to read if there is no data, instead sleep (yield) a bit
        time.sleep(0.01)
        continue
    data = port.read(1)
    if len(data) == 0:
        continue
    if data not in "\r\n":
        sys.stdout.write(repr(data))
#        sys.stdout.write(" 0x".join(binascii.hexlify(data)))
# Interestingly enough this comes mixed in with the previous output
#        if data in string.letters.join(string.digits).join(string.punctuation):
#            sys.stdout.write("(%s)" % data)
    else:
        sys.stdout.write(data)

