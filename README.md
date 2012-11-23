partyhatwork
============

Mesh network of party-hats, see http://kirjoitusalusta.fi/hacklab-partyhatwork

The current board is designed to be a "shield" to the Xmega32A4U 
breakout board from https://github.com/HelsinkiHacklab/XMega-boards 

Essential pins are pre-connected to modules and drivers, rest of the spare pins (both XMega
and XBee) will be brought out to IDC connectors for further use.

## BOM

  * 1 https://raw.github.com/HelsinkiHacklab/XMega-boards/master/Xmega32A4U.jpg
  * 3 Dual- N-Channel MOSFETs http://fi.rsdelivers.com/product/vishay/si1912edh-t1-e3/mosfet-n-channel-20v-113a-sc-70-6/7103235.aspx
  * 1 XBee (Series 2)
  * 2 1x10 2.0mm female header (for XBee)
  * 2 1x16 2.54mm female header (for XMega)
  * 1 2x3 2.54mm female header (ISP stacking)
  * 1 1x3 2.54mm female header (XMega power connector stacking)
  * 1 SOT-223 packaged 3.3V regulator (optional but high-powered one is recommended)
  * 1 MCP73831T LiPo charge controller
  * 1 JST battery connector http://www.digikey.fi/product-detail/en/S2B-PH-SM4-TB%28LF%29%28SN%29/455-1749-1-ND/926846?cur=USD
  * 1 LiPo battery with JST-connector https://www.sparkfun.com/products/8483 or https://www.sparkfun.com/products/8484
  
  * TODO: enumerate the SMD resistors
