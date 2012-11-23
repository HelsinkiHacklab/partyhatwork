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
  * 2 1x10 2.0mm female header (for XBee) http://www.digikey.fi/product-detail/en/M22-7131042/952-1354-5-ND/2264335?cur=USD
  * 2 1x16 2.54mm female header (for XMega)
  * 1 2x3 2.54mm female header (ISP stacking)
  * 1 1x3 2.54mm female header (XMega power connector stacking)
  * 1 SOT-223 packaged 3.3V regulator (optional but high-powered one is recommended)
  * 1 MCP73831T LiPo charge controller http://fi.rsdelivers.com/product/microchip/mcp73831t-2dci-ot/charge-controller-li-ion-li-pol/7240599.aspx
  * 1 JST battery connector http://www.digikey.fi/product-detail/en/S2B-PH-SM4-TB%28LF%29%28SN%29/455-1749-1-ND/926846?cur=USD
  * 1 LiPo battery with JST-connector https://www.sparkfun.com/products/8483 or https://www.sparkfun.com/products/8484
  * 1 2x6 2.54mm pin header -right angle
  * 2 2x8 2.54mm pin header -right angle (the other one can alternatively be straight)
  * 2 0805 4.7uF SMD caps
  * 1 0603 SMD led (charge indicator)
  * 1 0603 300r SMD resistor (for charge indicator, adjust value for your LED)
  * 1 0603 2.8k SMD resistor
  * 1 0603 10k SMD resistor
  * 1 0603 50k SMD resistor
  * 1 0603 100k SMD resistor
  * 6 0805 SMD resistors, values based on your desired LED currents
  * 1 0603 0.1uF SMD cap
  * 1 1210 SMD cap (size for your LEDs), 0805 cap should still be solderable too
