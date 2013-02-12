import binascii
from xbee import ZigBee
from node import XbeeNode

class handler(object):
    port = None
    nodes_by_identifier = {}
    nodes_by_shortaddr = {}
    xb = None

    def xbee_callback(self, x):
        print "DEBUG: x=%s" % repr(x)
        
        node_discovery_info = None
        
        if (    x['id'] == 'at_response'
            and x['command'] == 'ND'):
            # Node discovery response
            node_discovery_info = x['parameter']

        if (x['id'] == 'node_id_indicator'):
            node_discovery_info = x
            node_discovery_info['node_identifier'] = x['node_id']

        if (    node_discovery_info
            and node_discovery_info.has_key('node_identifier')
            and node_discovery_info.has_key('source_addr')
            and node_discovery_info.has_key('source_addr_long')):
            # Node discovery packet
            node = XbeeNode(self.xb, short_addr=node_discovery_info['source_addr'], long_addr=node_discovery_info['source_addr_long'], node_identifier=node_discovery_info['node_identifier'])
            self.nodes_by_identifier[node.node_identifier] = node
            sa_hex = binascii.hexlify(node.short_addr)
            self.nodes_by_shortaddr[sa_hex] = node
            print "New node %s at 0x%s" % (node.node_identifier, sa_hex)

    def start_callback(self, x):
        print "DEBUG: start_callback, x=%s" % repr(x)
        pass

    def __init__(self, port):
        self.xb = ZigBee(port,callback=self.xbee_callback,escaped=True,start_callback=self.start_callback)
        self.xb.start()
        self.discover_nodes()

    def discover_nodes(self):
        self.xb.at(command='ND')

    def ping_nodes(self):
        """Ping each known node to make sure it's still alive, if not remove from node list (or maybe just mark dead somehow)"""
        pass        
