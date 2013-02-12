

class XbeeNode(object):
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

    def tx(self, data):
        """Send ready packed data string"""
        self.xb.tx( dest_addr = self.short_addr, dest_addr_long = self.long_addr, data = data )

