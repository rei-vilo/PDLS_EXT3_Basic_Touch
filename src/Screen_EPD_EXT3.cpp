//
// Screen_EPD_EXT3.cpp
// Library C++ code
// ----------------------------------
//
// Project Pervasive Displays Library Suite
// Based on highView technology
//
// Created by Rei Vilo, 28 Jun 2016
//
// Copyright (c) Rei Vilo, 2010-2023
// Licence Creative Commons Attribution-ShareAlike 4.0 International (CC BY-SA 4.0)
//
// Release 509: Added eScreen_EPD_EXT3_271_Fast
// Release 527: Added support for ESP32 PSRAM
// Release 530: Added support for new 3.70"-Touch
// Release 531: Ready for hV_GUI_Basic
// Release 533: Improved touch release
// Release 541: Improved support for ESP32
// Release 550: Tested Xiao ESP32-C3 with SPI exception
// Release 601: Added support for screens with embedded fast update
// Release 602: Improved functions structure
// Release 604: Improved stability
// Release 607: Improved screens names consistency
// Release 608: Added screen report
// Release 609: Added temperature management
// Release 610: Removed partial update
// Release 700: Refactored screen and board functions
// Release 701: Improved functions names consistency
//

// Library header
#include "Screen_EPD_EXT3.h"

#if defined(ENERGIA)
///
/// @brief Proxy for SPISettings
/// @details Not implemented in Energia
/// @see https://www.arduino.cc/en/Reference/SPISettings
///
struct _SPISettings_s
{
    uint32_t clock; ///< in Hz, checked against SPI_CLOCK_MAX = 16000000
    uint8_t bitOrder; ///< LSBFIRST, MSBFIRST
    uint8_t dataMode; ///< SPI_MODE0, SPI_MODE1, SPI_MODE2, SPI_MODE3
};
///
/// @brief SPI settings for screen
///
_SPISettings_s _settingScreen;
#else
///
/// @brief SPI settings for screen
///
SPISettings _settingScreen;
#endif // ENERGIA

#ifndef SPI_CLOCK_MAX
#define SPI_CLOCK_MAX 16000000
#endif

//
// === COG section
//
/// @cond

//
// === List of touch ICs
//
#define TOUCH_271_ADDRESS 0x41
#define TOUCH_370_ADDRESS 0x38

// Common settings
// 0x00, soft-reset, temperature, active temperature, PSR0, PSR1
uint8_t indexE5_data[] = {0x19}; // Temperature 0x19 = 25 °C
uint8_t indexE0_data[] = {0x02}; // Activate temperature
uint8_t index00_data[] = {0xff, 0x8f}; // PSR, constant
uint8_t index50a_data[] = {0x27}; // Only 154 213 266 and 370 screens, constant
uint8_t index50b_data[] = {0x07}; // Only 154 213 266 and 370 screens, constant
uint8_t index50c_data[] = {0x07}; // All screens, constant

void Screen_EPD_EXT3_Fast::COG_initial(uint8_t updateMode)
{
    // Work settings
    uint8_t indexE0_work[1]; // Activate temperature
    uint8_t indexE5_work[1]; // Temperature
    uint8_t index00_work[2]; // PSR

    indexE0_work[0] = indexE0_data[0];
    indexE5_data[0] = u_temperature;
    if ((u_codeExtra & FEATURE_FAST) and (updateMode != UPDATE_GLOBAL)) // Specific settings for fast update
    {
        indexE5_work[0] = indexE5_data[0] | 0x40; // temperature | 0x40
        index00_work[0] = index00_data[0] | 0x10; // PSR0 | 0x10
        index00_work[1] = index00_data[1] | 0x02; // PSR1 | 0x02
    }
    else // Common settings
    {
        indexE5_work[0] = indexE5_data[0]; // Temperature
        index00_work[0] = index00_data[0]; // PSR0
        index00_work[1] = index00_data[1]; // PSR1
    } // u_codeExtra updateMode

    // New algorithm
    uint8_t index00_reset[] = {0x0e};
    b_sendIndexData(0x00, index00_reset, 1); // Soft-reset
    b_waitBusy();

    b_sendIndexData(0xe5, indexE5_work, 1); // Input Temperature
    b_sendIndexData(0xe0, indexE0_work, 1); // Activate Temperature
    b_sendIndexData(0x00, index00_work, 2); // PSR

    // Specific settings for fast update, all screens
    if ((u_codeExtra & FEATURE_FAST) and (updateMode != UPDATE_GLOBAL))
    {
        uint8_t index50c_work[1]; // Vcom
        index50c_work[0] = index50c_data[0]; // 0x07
        b_sendIndexData(0x50, index50c_work, 1); // Vcom and data interval setting
    }

    // Additional settings for fast update, 154 213 266 and 370 screens (_flag50)
    if ((u_codeExtra & FEATURE_FAST) and (updateMode != UPDATE_GLOBAL) and _flag50)
    {
        uint8_t index50a_work[1]; // Vcom
        index50a_work[0] = index50a_data[0]; // 0x27
        b_sendIndexData(0x50, index50a_work, 1); // Vcom and data interval setting
    }
}

void Screen_EPD_EXT3_Fast::COG_getUserData()
{
    uint16_t u_codeSizeType = u_eScreen_EPD_EXT3 & 0xffff;

    // Size cSize cType Driver
    switch (u_codeSizeType)
    {
        case 0x2709: // 2.71” = 0xcf, 0x8d

            index00_data[0] = 0xcf;
            index00_data[1] = 0x8d;
            _flag50 = false;
            break;

        case 0x370C: // 3.70” = 0xcf, 0x0f

            index00_data[0] = 0xcf;
            index00_data[1] = 0x8f;
            _flag50 = true;
            break;

        default:

            break;
    }
}

void Screen_EPD_EXT3_Fast::COG_sendImageDataFast()
{
    uint8_t * nextBuffer = u_newImage;
    uint8_t * previousBuffer = u_newImage + u_pageColourSize;

    b_sendIndexData(0x10, previousBuffer, u_frameSize); // Previous frame
    b_sendIndexData(0x13, nextBuffer, u_frameSize); // Next frame
    memcpy(previousBuffer, nextBuffer, u_frameSize); // Copy displayed next to previous
}

void Screen_EPD_EXT3_Fast::COG_update(uint8_t updateMode)
{
    // Specific settings for fast update, 154 213 266 and 370 screens (_flag50)
    if ((u_codeExtra & FEATURE_FAST) and (updateMode != UPDATE_GLOBAL) and _flag50)
    {
        uint8_t index50b_work[1]; // Vcom
        index50b_work[0] = index50b_data[0]; // 0x07
        b_sendIndexData(0x50, index50b_work, 1); // Vcom and data interval setting
    }

    b_sendCommand8(0x04); // Power on
    digitalWrite(b_pin.panelCS, HIGH); // CS# = 1
    b_waitBusy();

    b_sendCommand8(0x12); // Display Refresh
    digitalWrite(b_pin.panelCS, HIGH); // CS# = 1
    b_waitBusy();
}

void Screen_EPD_EXT3_Fast::COG_powerOff()
{
    b_sendCommand8(0x02); // Turn off DC/DC
    digitalWrite(b_pin.panelCS, HIGH); // CS# = 1
    b_waitBusy();
}
/// @endcond
//
// === End of COG section
//

//
// === Class section
//
Screen_EPD_EXT3_Fast::Screen_EPD_EXT3_Fast(eScreen_EPD_EXT3_t eScreen_EPD_EXT3, pins_t board)
{
    u_eScreen_EPD_EXT3 = eScreen_EPD_EXT3;
    b_pin = board;
    u_newImage = 0; // nullptr
}

void Screen_EPD_EXT3_Fast::begin()
{
    u_codeExtra = (u_eScreen_EPD_EXT3 >> 16) & 0xff;
    u_codeSize = (u_eScreen_EPD_EXT3 >> 8) & 0xff;
    u_codeType = u_eScreen_EPD_EXT3 & 0xff;
    _screenColourBits = 2; // BWR and BWRY

    // Configure board
    u_begin(b_pin, FAMILY_SMALL, 0);

    //
    // === Touch section
    //
    if (!(u_codeExtra & FEATURE_TOUCH))
    {
        Serial.println("");
        Serial.println(formatString("! Error - Screen %06x non touch", u_eScreen_EPD_EXT3));
    }
    //
    // === End of touch section
    //

    switch (u_codeSize)
    {
        case 0x27: // 2.71" and 2.71"-Touch

            _screenSizeV = 264; // vertical = wide size
            _screenSizeH = 176; // horizontal = small size
            _screenDiagonal = 271;
            break;

        case 0x37: // 3.70" and 3.70"-Touch

            _screenSizeV = 416; // vertical = wide size
            _screenSizeH = 240; // horizontal = small size
            _screenDiagonal = 370;
            break;

        default:

            break;
    } // u_codeSize

    u_bufferDepth = _screenColourBits; // 2 colours
    u_bufferSizeV = _screenSizeV; // vertical = wide size
    u_bufferSizeH = _screenSizeH / 8; // horizontal = small size 112 / 8, 1 bit per pixel

    // Force conversion for two unit16_t multiplication into uint32_t.
    // Actually for 1 colour; BWR requires 2 pages.
    u_pageColourSize = (uint32_t)u_bufferSizeV * (uint32_t)u_bufferSizeH;

    // u_frameSize = u_pageColourSize, except for 9.69 and 11.98
    u_frameSize = u_pageColourSize;
    // 9.69 and 11.98 combine two half-screens, hence two frames with adjusted size

#if defined(BOARD_HAS_PSRAM) // ESP32 PSRAM specific case

    if (u_newImage == 0)
    {
        static uint8_t * _newFrameBuffer;
        _newFrameBuffer = (uint8_t *) ps_malloc(u_pageColourSize * u_bufferDepth);
        u_newImage = (uint8_t *) _newFrameBuffer;
    }

#else // default case

    if (u_newImage == 0)
    {
        static uint8_t * _newFrameBuffer;
        _newFrameBuffer = new uint8_t[u_pageColourSize * u_bufferDepth];
        u_newImage = (uint8_t *) _newFrameBuffer;
    }

#endif // ESP32 BOARD_HAS_PSRAM

    memset(u_newImage, 0x00, u_pageColourSize * u_bufferDepth);

    // Initialise the /CS pins
    pinMode(b_pin.panelCS, OUTPUT);
    digitalWrite(b_pin.panelCS, HIGH); // CS# = 1

    // New generic solution
    pinMode(b_pin.panelDC, OUTPUT);
    pinMode(b_pin.panelReset, OUTPUT);
    pinMode(b_pin.panelBusy, INPUT); // All Pins 0

    // Initialise Flash /CS as HIGH
    if (b_pin.flashCS != NOT_CONNECTED)
    {
        pinMode(b_pin.flashCS, OUTPUT);
        digitalWrite(b_pin.flashCS, HIGH);
    }

    // Initialise slave panel /CS as HIGH
    if (b_pin.panelCSS != NOT_CONNECTED)
    {
        pinMode(b_pin.panelCSS, OUTPUT);
        digitalWrite(b_pin.panelCSS, HIGH);
    }

    // Initialise slave Flash /CS as HIGH
    if (b_pin.flashCSS != NOT_CONNECTED)
    {
        pinMode(b_pin.flashCSS, OUTPUT);
        digitalWrite(b_pin.flashCSS, HIGH);
    }

    // Initialise SD-card /CS as HIGH
    if (b_pin.cardCS != NOT_CONNECTED)
    {
        pinMode(b_pin.cardCS, OUTPUT);
        digitalWrite(b_pin.cardCS, HIGH);
    }

    // Initialise SPI
    _settingScreen = {4000000, MSBFIRST, SPI_MODE0};

#if defined(ENERGIA)

    SPI.begin();
    SPI.setBitOrder(_settingScreen.bitOrder);
    SPI.setDataMode(_settingScreen.dataMode);
    SPI.setClockDivider(SPI_CLOCK_MAX / min(SPI_CLOCK_MAX, _settingScreen.clock));

#else

#if defined(ARDUINO_XIAO_ESP32C3)

    // Board Xiao ESP32-C3 crashes if pins are specified.
    SPI.begin(8, 9, 10); // SCK MISO MOSI

#elif defined(ARDUINO_NANO_ESP32)

    // Board Arduino Nano ESP32 arduino_nano_nora v2.0.11
    SPI.begin();

#elif defined(ARDUINO_ARCH_ESP32)

    // Board ESP32-Pico-DevKitM-2 crashes if pins are not specified.
    SPI.begin(14, 12, 13); // SCK MISO MOSI

#else

    SPI.begin();

#endif // ARDUINO_ARCH_ESP32

    SPI.beginTransaction(_settingScreen);

#endif // ENERGIA

    // Reset
    b_reset(5, 5, 10, 5, 5);

    // Check type and get tables
    COG_getUserData(); // nothing sent to panel

    // Standard
    hV_Screen_Buffer::begin();

    setOrientation(0);
    if (f_fontMax() > 0)
    {
        f_selectFont(0);
    }
    f_fontSolid = false;

    _penSolid = false;
    u_invert = false;

    // Report
    Serial.println(formatString("= Screen %s %ix%i", WhoAmI().c_str(), screenSizeX(), screenSizeY()));
    Serial.println(formatString("= PDLS %s v%i.%i.%i", SCREEN_EPD_EXT3_VARIANT, SCREEN_EPD_EXT3_RELEASE / 100, (SCREEN_EPD_EXT3_RELEASE / 10) % 10, SCREEN_EPD_EXT3_RELEASE % 10));

    clear();

    //
    // === Touch section
    //
    _touchTrim = 0x00; // no touch
    _touchEvent = false;

#if (TOUCH_MODE != USE_TOUCH_NONE)

    _beginTouch();

#endif // TOUCH_MODE
    //
    // === End of Touch section
    //

    setTemperatureC(25); // 25 Celsius = 77 Fahrenheit
}

String Screen_EPD_EXT3_Fast::WhoAmI()
{
    char work[64] = {0};
    u_WhoAmI(work);

    return formatString("iTC %i.%02i\"%s", _screenDiagonal / 100, _screenDiagonal % 100, work);
}

uint8_t Screen_EPD_EXT3_Fast::flushMode(uint8_t updateMode)
{
    updateMode = checkTemperatureMode(updateMode);

    switch (updateMode)
    {
        case UPDATE_FAST:
        case UPDATE_GLOBAL:

            _flushFast();
            break;

        default:

            Serial.println("* PDLS - UPDATE_NONE invoked");
            break;
    }

    return updateMode;
}

void Screen_EPD_EXT3_Fast::flush()
{
    flushMode(UPDATE_FAST);
}

void Screen_EPD_EXT3_Fast::_flushFast()
{
    // Configure
    COG_initial(UPDATE_FAST);

    // Send image data
    COG_sendImageDataFast();

    // Update
    COG_update(UPDATE_FAST);

    COG_powerOff();
}

void Screen_EPD_EXT3_Fast::clear(uint16_t colour)
{
    if (colour == myColours.grey)
    {
        for (uint16_t i = 0; i < u_bufferSizeV; i++)
        {
            uint16_t pattern = (i % 2) ? 0b10101010 : 0b01010101;
            for (uint16_t j = 0; j < u_bufferSizeH; j++)
            {
                u_newImage[i * u_bufferSizeH + j] = pattern;
            }
        }
    }
    else if ((colour == myColours.white) xor u_invert)
    {
        // physical black 00
        memset(u_newImage, 0x00, u_pageColourSize);
    }
    else
    {
        // physical white 10
        memset(u_newImage, 0xff, u_pageColourSize);
    }
}

void Screen_EPD_EXT3_Fast::regenerate()
{
    clear(myColours.black);
    flush();
    delay(100);

    clear(myColours.white);
    flush();
    delay(100);
}

void Screen_EPD_EXT3_Fast::_setPoint(uint16_t x1, uint16_t y1, uint16_t colour)
{
    // Orient and check coordinates are within screen
    // _orientCoordinates() returns false = success, true = error
    if (_orientCoordinates(x1, y1))
    {
        return;
    }

    // Convert combined colours into basic colours
    bool flagOdd = ((x1 + y1) % 2 == 0);

    if (colour == myColours.grey)
    {
        if (flagOdd)
        {
            colour = myColours.black; // black
        }
        else
        {
            colour = myColours.white; // white
        }
    }

    // Coordinates
    uint32_t z1 = _getZ(x1, y1);
    uint16_t b1 = _getB(x1, y1);

    // Basic colours
    if ((colour == myColours.white) xor u_invert)
    {
        // physical black 00
        bitClear(u_newImage[z1], b1);
    }
    else if ((colour == myColours.black) xor u_invert)
    {
        // physical white 10
        bitSet(u_newImage[z1], b1);
    }
}

void Screen_EPD_EXT3_Fast::_setOrientation(uint8_t orientation)
{
    _orientation = orientation % 4;
}

bool Screen_EPD_EXT3_Fast::_orientCoordinates(uint16_t & x, uint16_t & y)
{
    bool _flagError = true; // false = success, true = error
    switch (_orientation)
    {
        case 3: // checked, previously 1

            if ((x < _screenSizeV) and (y < _screenSizeH))
            {
                x = _screenSizeV - 1 - x;
                _flagError = false;
            }
            break;

        case 2: // checked

            if ((x < _screenSizeH) and (y < _screenSizeV))
            {
                x = _screenSizeH - 1 - x;
                y = _screenSizeV - 1 - y;
                swap(x, y);
                _flagError = false;
            }
            break;

        case 1: // checked, previously 3

            if ((x < _screenSizeV) and (y < _screenSizeH))
            {
                y = _screenSizeH - 1 - y;
                _flagError = false;
            }
            break;

        default: // checked

            if ((x < _screenSizeH) and (y < _screenSizeV))
            {
                swap(x, y);
                _flagError = false;
            }
            break;
    }

    return _flagError;
}

uint32_t Screen_EPD_EXT3_Fast::_getZ(uint16_t x1, uint16_t y1)
{
    uint32_t z1 = 0;
    // According to 11.98 inch Spectra Application Note
    // at http:// www.pervasivedisplays.com/LiteratureRetrieve.aspx?ID=245146

    z1 = (uint32_t)x1 * u_bufferSizeH + (y1 >> 3);

    return z1;
}

uint16_t Screen_EPD_EXT3_Fast::_getB(uint16_t x1, uint16_t y1)
{
    uint16_t b1 = 0;

    b1 = 7 - (y1 % 8);

    return b1;
}

uint16_t Screen_EPD_EXT3_Fast::_getPoint(uint16_t x1, uint16_t y1)
{
    // Orient and check coordinates are within screen
    // _orientCoordinates() returns false = success, true = error
    if (_orientCoordinates(x1, y1))
    {
        return 0;
    }

    uint16_t _result = 0;
    uint8_t _value = 0;

    // Coordinates
    uint32_t z1 = _getZ(x1, y1);
    uint16_t b1 = _getB(x1, y1);

    _value = bitRead(u_newImage[z1], b1);
    _value <<= 4;
    _value &= 0b11110000;

    // red = 0-1, black = 1-0, white 0-0
    switch (_value)
    {
        case 0x10:

            _result = myColours.black;
            break;

        default:

            _result = myColours.white;
            break;
    }

    return _result;
}
//
// === End of Class section
//

//
// === Touch section
//
#if (TOUCH_MODE != USE_TOUCH_NONE)

void Screen_EPD_EXT3_Fast::_wireWriteRead(uint8_t address, uint8_t * dataWrite, size_t sizeWrite, uint8_t * dataRead, size_t sizeRead)
{
    Wire.beginTransmission(address);
    if (sizeWrite > 0)
    {
        for (uint8_t index = 0; index < sizeWrite; index++)
        {
            Wire.write(dataWrite[index]);
        }
    }
    Wire.endTransmission();

    if (sizeRead > 0)
    {
        memset(dataRead, 0x00, sizeRead);
        Wire.requestFrom(address, sizeRead);
        while (Wire.available() < sizeRead)
        {
            delay(10);
        }

        for (uint8_t index = 0; index < sizeRead; index++)
        {
            dataRead[index] = Wire.read();
        }
    }
}

void Screen_EPD_EXT3_Fast::_beginTouch()
{
    _touchTrim = 0x10; // standard threshold
    _touchEvent = true;

    Wire.begin();

    if (b_pin.touchInt != NOT_CONNECTED)
    {
        pinMode(b_pin.touchInt, INPUT_PULLUP);
    }

    if (b_pin.touchReset != NOT_CONNECTED)
    {
        pinMode(b_pin.touchReset, OUTPUT);
    }

    if (u_codeSize == 0x27)
    {
        digitalWrite(b_pin.touchReset, HIGH);
        delay(100);
        digitalWrite(b_pin.touchReset, LOW);
        delay(100);
        digitalWrite(b_pin.touchReset, HIGH);
        delay(100);

        _touchAddress = TOUCH_271_ADDRESS; // 0x41

        uint8_t bufferWrite[1] = {0};
        uint8_t bufferRead[10] = {0};

        bufferWrite[0] = 0x20;
        _wireWriteRead(_touchAddress, bufferWrite, 1, bufferRead, 10);
        _touchXmin = 0;
        _touchXmax = bufferRead[0] + (bufferRead[1] << 8); // Xmax
        _touchYmin = 0;
        _touchYmax = bufferRead[2] + (bufferRead[3] << 8); // Ymax
    }
    else if (u_codeSize == 0x37)
    {
        digitalWrite(b_pin.touchReset, HIGH);
        delay(10);
        digitalWrite(b_pin.touchReset, LOW);
        delay(10);
        digitalWrite(b_pin.touchReset, HIGH);
        delay(1000);

        _touchAddress = TOUCH_370_ADDRESS; // 0x38

        _touchXmin = 0;
        _touchXmax = 239; // Xmax, hardware hard-coded
        _touchYmin = 0;
        _touchYmax = 415; // Ymax, hardware hard-coded
    } // u_codeSize

    _touchPrevious = TOUCH_EVENT_NONE;
}

void Screen_EPD_EXT3_Fast::_getRawTouch(uint16_t & x0, uint16_t & y0, uint16_t & z0, uint16_t & t0)
{
    delay(10);

    if (u_codeSize == 0x27)
    {
        uint8_t bufferWrite[1] = {0};
        uint8_t bufferRead[5] = {0};

        bufferWrite[0] = 0x10; // check
        _wireWriteRead(_touchAddress, bufferWrite, 1, bufferRead, 1);

        uint8_t number = bufferRead[0];
        z0 = 0;
        t0 = TOUCH_EVENT_NONE;

        // Only one finger read, interrupt not used
        // uint8_t flagInterrupt = 1 - digitalRead(b_pin.touchInt);
        // if ((number > 0) and (number < 3) and (flagInterrupt > 0))
        if ((number > 0) and (number < 3))
        {
            bufferWrite[0] = 0x11; // report
            _wireWriteRead(_touchAddress, bufferWrite, 1, bufferRead, 5);

            uint8_t status = bufferRead[0];
            x0 = (bufferRead[1] << 8) + bufferRead[2];
            y0 = (bufferRead[3] << 8) + bufferRead[4];

            if (status & 0x80) // touch
            {
                t0 = (_touchPrevious != TOUCH_EVENT_NONE) ? TOUCH_EVENT_MOVE : TOUCH_EVENT_PRESS;

                // Keep position for next release
                _touchPrevious = TOUCH_EVENT_PRESS;
                _touchX = x0;
                _touchY = y0;
            }
            else
            {
                t0 = TOUCH_EVENT_RELEASE;
            }
            z0 = 0x16;
        }
        else // no touch
        {
            if (_touchPrevious == TOUCH_EVENT_NONE)
            {
                t0 = TOUCH_EVENT_NONE;
                z0 = 0;
            }
            else
            {
                // Take previous position for release
                _touchPrevious = TOUCH_EVENT_NONE;
                t0 = TOUCH_EVENT_RELEASE;
                x0 = _touchX;
                y0 = _touchY;
                z0 = 0x16;
            }
        }
    }
    else if (u_codeSize == 0x37)
    {
        uint8_t flagInterrupt = 1 - digitalRead(b_pin.touchInt);

        bool flagValid;
        z0 = 0;
        t0 = TOUCH_EVENT_NONE;

        // Only one finger read
        if (flagInterrupt) // touch
        {
            uint8_t bufferWrite[1];
            uint8_t bufferRead[3 + 6];

            bufferWrite[0] = 0x00;
            delay(10);
            _wireWriteRead(_touchAddress, bufferWrite, 1, bufferRead, 3 + 6); // report

            // char * stringEvent[] = {"Down", "Up", "Contact", "Reserved"};
            // uint8_t event = bufferRead[3 + 6 * 0 + 0] >> 6;
            uint8_t id = bufferRead[3 + 6 * 0 + 2] >> 4; // 0= Down, 1= Up, 2= Contact, 3= Reserved
            bool flagValid = (id < 0x0f);

            if (flagValid)
            {
                x0 = ((bufferRead[3 + 6 * 0 + 0] & 0x0f) << 8) + bufferRead[3 + 6 * 0 + 1];
                y0 = ((bufferRead[3 + 6 * 0 + 2] & 0x0f) << 8) + bufferRead[3 + 6 * 0 + 3];

                t0 = (_touchPrevious != TOUCH_EVENT_NONE) ? TOUCH_EVENT_MOVE : TOUCH_EVENT_PRESS;

                // Keep position for next release
                _touchPrevious = TOUCH_EVENT_PRESS;
                _touchX = x0;
                _touchY = y0;
                z0 = 0x16;
            }
            else
            {
                t0 = TOUCH_EVENT_RELEASE;
            }
        }
        else // no touch
        {
            if (_touchPrevious == TOUCH_EVENT_NONE)
            {
                t0 = TOUCH_EVENT_NONE;
                z0 = 0;
            }
            else // Take previous position for release
            {
                _touchPrevious = TOUCH_EVENT_NONE;
                t0 = TOUCH_EVENT_RELEASE;
                x0 = _touchX;
                y0 = _touchY;
                z0 = 0x16;
            }
        }
    } // u_codeSize
}

bool Screen_EPD_EXT3_Fast::_getInterruptTouch()
{
    if (b_pin.touchInt != NOT_CONNECTED)
    {
        // Translate for true = interrupt
        // 271 and 370: LOW = false for interrupt
        return (digitalRead(b_pin.touchInt) == LOW);
    }
    else
    {
        return false;
    }
}
#endif // TOUCH_MODE
//
// === End of Touch section
//

