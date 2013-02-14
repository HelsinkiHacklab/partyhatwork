# Revision1 board

The current board is designed to be a "shield" to the Xmega32A4U 
breakout board from https://github.com/HelsinkiHacklab/XMega-boards 

Essential pins are pre-connected to modules and drivers, rest of the spare pins (both XMega
and XBee) will be brought out to IDC connectors for further use.

## BOM

  * 1 https://raw.github.com/HelsinkiHacklab/XMega-boards/master/Xmega32A4U_v1.jpg (v2 is perfectly fine as well)
    * Flash with [atxmega32a4u_104_PD5.hex](xmegabootloaders/atxmega32a4u_104_PD5.hex) to use PD5 to activate DFU.
      * Convert hex to bin for avrdude: `avr-objcopy -I ihex -O binary atxmega32a4u_104_PD5.hex atxmega32a4u_104_PD5.bin`
      * program with AVRISPmkII: `avrdude -e -c avrisp2 -P usb -p x32a4 -U boot:w:atxmega32a4u_104_PD5.bin`
  * 3 Dual- N-Channel MOSFETs http://fi.rsdelivers.com/product/vishay/si1912edh-t1-e3/mosfet-n-channel-20v-113a-sc-70-6/7103235.aspx
  * 1 XBee (Series 2)
  * 2 1x10 2.0mm female header (for XBee) http://fi.rsdelivers.com/product/harwin/m22-7131042/10w-sil-vertical-pcb-skt/6058796.aspx | http://www.digikey.fi/product-detail/en/M22-7131042/952-1354-5-ND/2264335?cur=USD
  * 2 1x16 2.54mm female header (for XMega) http://fi.rsdelivers.com/product/te-connectivity/1-215297-6/16-way-single-row-vertical-gold-skt/5092880.aspx
  * 1 2x3 2.54mm female header (ISP stacking) http://fi.rsdelivers.com/product/rs/w3482106trc/3-way-2-row-top-entry-socket-01in-pitch/2518216.aspx
  * 1 1x3 2.54mm female header (XMega power connector stacking) http://fi.rsdelivers.com/product/rs/w3481103trc/3way-1-row-top-entry-socket-254mm-pitch/2518187.aspx
  * 1 SOT-223 packaged LDO 3.3V regulator (optional but high-powered one is recommended). **Doublecheck the pinout**: this one is GND-Vin-Vout (LM1117 pinout).
  * 1 MCP73831T LiPo charge controller http://fi.rsdelivers.com/product/microchip/mcp73831t-2dci-ot/charge-controller-li-ion-li-pol/7240599.aspx
  * 1 JST battery connector http://fi.rsdelivers.com/product/jst/s2b-ph-sm4-tblfsn/ph-smt-side-entry-shroud-header-2-way/6881353.aspx | http://www.digikey.fi/product-detail/en/S2B-PH-SM4-TB%28LF%29%28SN%29/455-1749-1-ND/926846?cur=USD
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

## Bugs

  * No on/off control for battery power (apart from disconnecting alltotger)
  * without the "optional" regulator battery cannot power the XMega board
    (you could also jumper VBAT to VUSB but that will cause trouble when you connect the USB cable)
  * The 3.3V regulator has weird pinout (whyever eagle has more parts with that one than with the logical and actually more easily available one beats me)
