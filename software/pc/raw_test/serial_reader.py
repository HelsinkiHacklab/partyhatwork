#!/usr/bin/python
import time, serial, sys
import ConfigParser, os
config = ConfigParser.SafeConfigParser()
if not os.path.isfile('xbee.ini'):
    config.add_section('modem')
    config.set('modem', 'port', '/dev/whatever')
    with open('xbee.ini', 'wb') as configfile:
        config.write(configfile)
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
    else:
        sys.stdout.write(data)

