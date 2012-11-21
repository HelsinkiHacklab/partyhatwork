partyhatwork
============

Mesh network of party-hats, see http://kirjoitusalusta.fi/hacklab-partyhatwork

The current board is designed to be a "shield" to the Xmega32A4U 
breakout board from https://github.com/HelsinkiHacklab/XMega-boards 

Essential pins are pre-connected to modules and drivers, rest of the spare pins (both XMega
and XBee) will be brought out to IDC connectors for further use.

## BOM

  * 1 https://raw.github.com/HelsinkiHacklab/XMega-boards/master/Xmega32A4U.jpg
  * 3 http://fi.rsdelivers.com/product/vishay/si1912edh-t1-e3/mosfet-n-channel-20v-113a-sc-70-6/7103235.aspx
  * 1 XBee (Series 2)
  * 2 1x10 2.0mm female header (for XBee)
  * 2 1x16 2.54mm female header (for XMega)
  * 1 2x3 2.54mm female header (ISP stacking)
  * 1 1x3 2.54mm female header (XMega power connector stacking)

We probably need relatively high-powered 3.3V regulator on board since the one
on the xmega board might not have a lot of spare juice for the XBee 
(the leds I guess will have to live with varying battery voltage [note to self,
put in a resistor divider and bring the voltage to ADC port on the xmega])