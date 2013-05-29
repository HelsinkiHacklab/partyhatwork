# Rev2 board

## TODO

  * Design it, remembering to fix bugs of rev1
  * Decide on on/off solution, heavily leaning towards: [LTC2954](http://www.digikey.fi/product-detail/en/LTC2954CTS8-2%23TRMPBF/LTC2954CTS8-2%23TRMPBFCT-ND/1232682) with [SI4465ADY](http://www.digikey.fi/product-detail/en/SI4465ADY-T1-E3/SI4465ADY-T1-E3CT-ND/1656942)
  * Doublecheck that the 3.3V reg on the XMega board will not throw a hissy fit if  we provide 3.3V from the partyboard (with and without 5V to xmega board provided)
    * In any case [MCP1826T](http://www.digikey.fi/product-detail/en/MCP1826T-3302E%2FDC/MCP1826T-3302E%2FDCCT-ND/1776943) looks like a good choice for regulator (very low drop even at high current) and decent price.
  * Consider MCP73837 as alternative LiPo charger
    * See http://ww1.microchip.com/downloads/en/AppNotes/01149c.pdf
  * Consider adding a [microphone](../micamp/) (for sound-sensitive stuff)
    * or at least a standard header for connecting that board...
