# Pervasive Displays Library Suite - Basic edition - Touch

[![Release](https://img.shields.io/github/v/release/rei-vilo/PDLS_EXT3_Basic_Touch)](https://github.com/rei-vilo/PDLS_EXT3_Basic_Touch/releases) [![Codacy Badge](https://app.codacy.com/project/badge/Grade/79b420dc349245f0b9548255884de41b)](https://app.codacy.com/gh/rei-vilo/PDLS_EXT3_Basic_Touch/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=rei-vilo/PDLS_EXT3_Basic_Touch&amp;utm_campaign=Badge_Grade) ![Arduino lint](https://github.com/rei-vilo/PDLS_EXT3_Basic_Touch/actions/workflows/main.yml/badge.svg) [![GitHub issues](https://img.shields.io/github/issues/rei-vilo/PDLS_EXT3_Basic_Global)](https://github.com/rei-vilo/PDLS_EXT3_Basic_Global/issues)

[![](https://img.shields.io/badge/-Technical-Wiki-blue)](https://docs.pervasivedisplays.com/) [![](https://img.shields.io/badge/-User_Guide-blue)](https://pdls.pervasivedisplays.com/userguide/index.html) [![](https://img.shields.io/badge/-Reference_Manual-blue)](https://pdls.pervasivedisplays.com/reference/html/index.html)

The Pervasive Displays Library Suite is specifically designed for the [Pervasive Displays](https://www.pervasivedisplays.com) e-paper screens, extension boards and evaluation kits.

![](https://pdls.pervasivedisplays.com/userguide/img/Logo_PDI_text_320.png)

> [!CAUTION]
> ### The library **PDLS_EXT3_Basic_Touch** is deprecated. 
> 
> Use the library [PDLS_Basic](https://github.com/rei-vilo/PDLS_Basic) with the [Pervasive_Touch_Small](https://github.com/PervasiveDisplays/Pervasive_Touch_Small) instead.
>
> For more information, please refer to the technical note [Migrate from release 8 to release 9](https://pdls.pervasivedisplays.com/userguide/Volume5/Chapter1/Part6/Section22/).

The **PDLS_EXT3_Basic_Touch** supports the screens

* iTC [monochrome screens with touch](https://www.pervasivedisplays.com/products/?_sft_etc_itc=tp);

the boards

* [EPD Extension Kit Gen 3 (EXT3 or EXT3.1)](https://www.pervasivedisplays.com/product/epd-extension-kit-gen-3-EXT3/); 
* [E-paper touch display development kit (EXT3-Touch)](https://www.pervasivedisplays.com/product/touch-expansion-board-ext3-touch/);

the kits

* [EPD Pico Kit (EPDK)](https://www.pervasivedisplays.com/product/epd-pico-kit-epdk/), which includes a Raspberry Pi Pico RP2040, an EXT3.1 extension board and a 2.66" monochrome panel,
* [EXT3-Touch with 2.71" Touch EPD (EPDK-271-Touch)](https://www.pervasivedisplays.com/product/touch-expansion-board-ext3-touch/), which includes the EXT3.1 and EXT3-Touch extension boards and a 2.71"-Touch monochrome panel with embedded fast update and capacitive touch.

## Features

The Pervasive Displays Library Suite provides a high-level interface to drive the e-paper screens.

* Fast update
* Graphics routines
* Text routines
* Four extended fonts with double-sized variants
* Touch management
* GUI with button and text

## Documentation

* [Wiki](https://docs.pervasivedisplays.com/) 
* [User guide](https://pdls.pervasivedisplays.com/userguide/index.html) with commented examples
* [Reference manual](https://rei-vilo.github.io/PDLS_EXT3_Basic_Documentation/index.html) 

## Support

* Please refer to [PDLS_EXT3_Basic_Global](https://github.com/rei-vilo/PDLS_EXT3_Basic_Global/issues) for reporting issues and contributing pull requests.

## Installation

Software

* [Arduino IDE](https://www.arduino.cc/en/software) or [Arduino CLI](https://arduino.github.io/arduino-cli/)
* [Installing Additional Arduino Libraries](https://www.arduino.cc/en/guide/libraries)

    + Using the Library Manager
    + Importing a `.zip` Library

Hardware

* [Assembing the Pervasive Displays EXT3-Touch](https://embeddedcomputing.weebly.com/assembing-the-pervasive-displays-ext3-touch.html)
* [Connecting the Pervasive Displays e-Paper EPD Extension Kit Gen 3](https://embeddedcomputing.weebly.com/connecting-the-e-paper-epd-extension-kit-gen-3.html)

## Configuration

Software

* Arduino SDK
* SPI library
* I²C / Wire library

Hardware

* Pervasive Displays EPD Extension Kit Gen 3 (EXT3 or EXT3.1)
* Pervasive Displays E-paper touch display development kit (EXT3-Touch)
* Pervasive Displays iTC monochrome screens with touch

## Licence

**Copyright** &copy; Rei Vilo, 2010-2025

*For exclusive use with Pervasive Displays screens*

**Licence** [Attribution-ShareAlike 4.0 International (CC BY-SA 4.0)](./LICENCE.md)

![](img/by-sa.svg)

**Portions** &copy; Pervasive Displays, 2010-2025

