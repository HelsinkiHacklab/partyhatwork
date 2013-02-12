#!/usr/bin/python
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

from struct import pack,unpack

print repr(config.get('node','short_addr'))
print repr(config.get('node','long_addr'))

short_int = eval(config.get('node','short_addr'))
short_daddr = pack(">H", short_int)
long_daddr = pack('>Q',eval(config.get('node','long_addr'))) # set 64-bit address here

print "short_daddr=%s" % repr(short_daddr)
print "long_daddr=%s" % repr(long_daddr)
print "the addresses above should look same as the ones you get in the ND responses"
