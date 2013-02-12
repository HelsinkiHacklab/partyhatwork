#!/usr/bin/python -i
if __name__ == '__main__':
    from handler import handler
    import ConfigParser, os, sys, serial
    from struct import pack, unpack
    config = ConfigParser.SafeConfigParser()
    if not os.path.isfile('xbee.ini'):
        config.add_section('modem')
        config.set('modem', 'port', '/dev/whatever')
        with open('xbee.ini', 'wb') as configfile:
            config.write(configfile)
        print "Edit xbee.ini for your modem port"
        sys.exit(1)
    config.read('xbee.ini')
    ser = serial.Serial(config.get('modem', 'port'), 57600)
    hd = handler(ser)
    nodes = hd.nodes_by_identifier
    
