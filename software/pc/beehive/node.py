from struct import pack, unpack


class XbeeNode(object):
    """Simple encapsulation for an XBee node, makes it less painfull to handle all the bookkeeping"""
    node_identifier = None
    short_addr = None
    long_addr = None
    xb = None #xbee instance

    def __init__(self, xbee, **kwargs):
        self.xb = xbee
        self.short_addr = kwargs['short_addr']
        # TODO: autodiscover this if not given
        self.long_addr = kwargs['long_addr']
        self.node_identifier = kwargs['node_identifier']

    def tx(self, *args):
        """Send data to target node, each argument is single byte to send (if you have a tuple/list mydata you can pass it as arguments with *mydata"""
        data_packed = pack("%dB" % len(args), *args)
        self.xb.tx( dest_addr = self.short_addr, dest_addr_long = self.long_addr, data = data_packed )

    def tx_string(self, send_string):
        """Send a string (ASCII) to node, this will handle unpacking of the string to list of bytes and passing it correctly"""
        send_args = [ ord(x) for x in send_string ]
        self.tx(*send_args)
