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

    def tx(self, data_packed):
        self.xb.tx( dest_addr = self.short_addr, dest_addr_long = self.long_addr, data = data_packed )

