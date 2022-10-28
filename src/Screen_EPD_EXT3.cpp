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
// Copyright © Rei Vilo, 2010-2022
// Licence Creative Commons Attribution-ShareAlike 4.0 International (CC BY-SA 4.0)
//
// Release 509: Added eScreen_EPD_EXT3_271_Fast
// Release 527: Added support for ESP32 PSRAM
// Release 530: Added support for new 3.70"-Touch
// Release 531: Ready for hV_GUI_Basic
// Release 533: Improved touch release
// Release 541: Improved support for ESP32
// Release 550: Tested Xiao ESP32-C3 with SPI exception
//

// Library header
#include "SPI.h"
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

#if (TOUCH_MODE != USE_TOUCH_NONE)
//
// List of touch ICs
//
#define TOUCH_271_ADDRESS 0x41
#define TOUCH_370_ADDRESS 0x38

#endif // TOUCH_MODE

uint8_t data1[] = {0xff, 0x8f};
uint8_t data4[] = {0x07};

// Class
Screen_EPD_EXT3_Fast::Screen_EPD_EXT3_Fast(eScreen_EPD_EXT3_t eScreen_EPD_EXT3, pins_t board)
{
    _eScreen_EPD_EXT3 = eScreen_EPD_EXT3;
    _pin = board;
    _newImage = 0; // nullptr
}

void Screen_EPD_EXT3_Fast::begin()
{
    _codeExtra = (_eScreen_EPD_EXT3 >> 16) & 0xff;
    _codeSize = (_eScreen_EPD_EXT3 >> 8) & 0xff;
    _codeType = _eScreen_EPD_EXT3 & 0xff;
    _screenColourBits = 2; // BWR

    switch (_codeSize)
    {
        case 0x27: // 2.70"

            _widthScreen = 264; // x = wide size
            _heightScreen = 176; // y = small size
            _screenDiagonal = 270;
            _refreshTime = 19;
            break;

        case 0x37: // 3.70"

            _widthScreen = 416; // vertical = wide size
            _heightScreen = 240; // horizontal = small size
            _screenDiagonal = 370;
            _refreshTime = 15; // ?
            break;

        default:

            break;
    }

    _depthBuffer = _screenColourBits; // 2 colours
    _widthBuffer = _widthScreen; // x = wide size
    _heightBuffer = _heightScreen / 8; // small size 112 / 8;

    // Force conversion for two unit16_t multiplication into uint32_t.
    // Actually for 1 colour; BWR requires 2 pages.
    _sizePageColour = (uint32_t)_widthBuffer * (uint32_t)_heightBuffer;

    // _sizeFrame = _sizePageColour, except for 9.69 and 11.98
    _sizeFrame = _sizePageColour;
    // 9.69 and 11.98 combine two half-screens, hence two frames with adjusted size

#if defined(BOARD_HAS_PSRAM) // ESP32 PSRAM specific case

    if (_newImage == 0)
    {
        static uint8_t * _newFrameBuffer;
        _newFrameBuffer = (uint8_t *) ps_malloc(_sizePageColour * _depthBuffer);
        _newImage = (uint8_t *) _newFrameBuffer;
    }

#else // default case

    if (_newImage == 0)
    {
        static uint8_t * _newFrameBuffer;
        _newFrameBuffer = new uint8_t[_sizePageColour * _depthBuffer];
        _newImage = (uint8_t *) _newFrameBuffer;
    }

#endif // ESP32 BOARD_HAS_PSRAM

    // Check FRAM
    bool flag = true;
    uint8_t count = 8;

    _newImage[1] = 0x00;
    while (flag)
    {
        _newImage[1] = 0xaa;
        delay(100);
        if ((_newImage[1] == 0xaa) or (count == 0))
        {
            flag = false;
        }
        count--;
    }
    memset(_newImage, 0x00, _sizePageColour * _depthBuffer);

    // Initialise the /CS pins
    pinMode(_pin.panelCS, OUTPUT);
    digitalWrite(_pin.panelCS, HIGH); // CS# = 1

    // New generic solution
    pinMode(_pin.panelDC, OUTPUT);
    pinMode(_pin.panelReset, OUTPUT);
    pinMode(_pin.panelBusy, INPUT); // All Pins 0

    // Initialise Flash /CS as HIGH
    if (_pin.flashCS != NOT_CONNECTED)
    {
        pinMode(_pin.flashCS, OUTPUT);
        digitalWrite(_pin.flashCS, HIGH);
    }

    // Initialise slave panel /CS as HIGH
    if (_pin.panelCSS != NOT_CONNECTED)
    {
        pinMode(_pin.panelCSS, OUTPUT);
        digitalWrite(_pin.panelCSS, HIGH);
    }

    // Initialise slave Flash /CS as HIGH
    if (_pin.flashCSS != NOT_CONNECTED)
    {
        pinMode(_pin.flashCSS, OUTPUT);
        digitalWrite(_pin.flashCSS, HIGH);
    }

    // Initialise SD-card /CS as HIGH
    if (_pin.cardCS != NOT_CONNECTED)
    {
        pinMode(_pin.cardCS, OUTPUT);
        digitalWrite(_pin.cardCS, HIGH);
    }

    // Initialise SPI
    _settingScreen = {4000000, MSBFIRST, SPI_MODE0};
    // _settingScreen = {1000000, MSBFIRST, SPI_MODE0 };

#if defined(ENERGIA)

    SPI.begin();
    SPI.setBitOrder(_settingScreen.bitOrder);
    SPI.setDataMode(_settingScreen.dataMode);
    SPI.setClockDivider(SPI_CLOCK_MAX / min(SPI_CLOCK_MAX, _settingScreen.clock));

#else

#if defined(ARDUINO_XIAO_ESP32C3)

    // Board Xiao ESP32-C3 crashes if pins are specified.
    SPI.begin(8, 9, 10); // SCK MISO MOSI

#elif defined(ARDUINO_ARCH_ESP32)

    // Board ESP32-Pico-DevKitM-2 crashes if pins are not specified.
    SPI.begin(14, 12, 13); // SCK MISO MOSI

#else

    SPI.begin();

#endif // ARDUINO_ARCH_ESP32

    SPI.beginTransaction(_settingScreen);

#endif // ENERGIA

    // Reset
    _reset(5, 5, 10, 5, 5);

    _screenWidth = _heightScreen;
    _screenHeigth = _widthScreen;

    // Standard
    hV_Screen_Buffer::begin();

    setOrientation(0);
    if (_f_fontMax() > 0)
    {
        _f_selectFont(0);
    }
    _f_fontSolid = false;

    _penSolid = false;
    _invert = false;

    clear();

    _beginTouch();
}

void Screen_EPD_EXT3_Fast::_reset(uint32_t ms1, uint32_t ms2, uint32_t ms3, uint32_t ms4, uint32_t ms5)
{
    // digitalWrite(PNLON_PIN, HIGH); // PANEL_ON# = 1
    delay_ms(ms1); // delay_ms 5ms
    digitalWrite(_pin.panelReset, HIGH); // RES# = 1
    delay_ms(ms2); // delay_ms 5ms
    digitalWrite(_pin.panelReset, LOW);
    delay_ms(ms3);
    digitalWrite(_pin.panelReset, HIGH);
    delay_ms(ms4);
    digitalWrite(_pin.panelCS, HIGH); // CS# = 1
    delay_ms(ms5);
}

String Screen_EPD_EXT3_Fast::WhoAmI()
{
    String text = "iTC ";
    text += String(_screenDiagonal / 100);
    text += ".";
    text += String(_screenDiagonal % 100);
    text += "\" -";

#if (FONT_MODE == USE_FONT_HEADER)

    text += "H";

#elif (FONT_MODE == USE_FONT_FLASH)

    text += "F";

#elif (FONT_MODE == USE_FONT_TERMINAL)

    text += "T";

#else

    text += "?";

#endif // FONT_MODE

    return text;
}

void Screen_EPD_EXT3_Fast::flush()
{
    uint8_t * nextBuffer = _newImage;
    uint8_t * previousBuffer = _newImage + _sizePageColour;

    _reset(5, 5, 10, 5, 5);

    uint8_t data9[] = {0x0e};
    _sendIndexData(0x00, data9, 1); // Soft-reset
    delay_ms(5);

    uint8_t data7[] = {0x19 | 0x40};
    // uint8_t data7[] = {getTemperature() };
    _sendIndexData(0xe5, data7, 1); // Input Temperature 0°C = 0x00, 22°C = 0x16, 25°C = 0x19
    uint8_t data6[] = {0x02};
    _sendIndexData(0xe0, data6, 1); // Active Temperature

    uint8_t data0[] = {0xcf | 0x10, 0x8d | 0x02};
    _sendIndexData(0x00, data0, 2); // PSR

    _sendIndexData(0x50, data4, 1); // Vcom and data interval setting

    // Send image data
    _sendIndexData(0x10, previousBuffer, _sizeFrame); // Previous frame
    _sendIndexData(0x13, nextBuffer, _sizeFrame); // Next frame
    memcpy(previousBuffer, nextBuffer, _sizeFrame); // Copy displayed next to previous

    delay_ms(50);
    _waitBusy();
    uint8_t data8[] = {0x00};
    _sendIndexData(0x04, data8, 1); // Power on
    delay_ms(5);
    _waitBusy();

    _sendIndexData(0x12, data8, 1); // Display Refresh
    delay_ms(5);
    _waitBusy();

    _sendIndexData(0x02, data8, 1); // Turn off DC/DC
    delay_ms(5);
    _waitBusy();
    digitalWrite(_pin.panelDC, LOW);
    digitalWrite(_pin.panelCS, LOW);

    digitalWrite(_pin.panelReset, LOW);
    // digitalWrite(PNLON_PIN, LOW);

    digitalWrite(_pin.panelCS, HIGH); // CS# = 1
}

void Screen_EPD_EXT3_Fast::clear(uint16_t colour)
{
    if (colour == myColours.grey)
    {
        for (uint16_t i = 0; i < _widthBuffer; i++)
        {
            uint16_t pattern = (i % 2) ? 0b10101010 : 0b01010101;
            for (uint16_t j = 0; j < _heightBuffer; j++)
            {
                _newImage[i * _heightBuffer + j] = pattern;
            }
        }
        // memset(_newImage + _sizePageColour, 0x00, _sizePageColour);
    }
    else if ((colour == myColours.white) xor _invert)
    {
        // physical black 00
        memset(_newImage, 0x00, _sizePageColour);
        // memset(_newImage + _sizePageColour, 0x00, _sizePageColour);
    }
    else
    {
        // physical white 10
        memset(_newImage, 0xff, _sizePageColour);
        // memset(_newImage + _sizePageColour, 0x00, _sizePageColour);
    }
}

void Screen_EPD_EXT3_Fast::invert(bool flag)
{
    _invert = flag;
}

void Screen_EPD_EXT3_Fast::_setPoint(uint16_t x1, uint16_t y1, uint16_t colour)
{
    // Orient and check coordinates are within screen
    // _orientCoordinates() returns false = success, true = error
    if (_orientCoordinates(x1, y1))
    {
        return;
    }

    uint32_t z1 = _getZ(x1, y1);

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

    // Basic colours
    if ((colour == myColours.white) xor _invert)
    {
        // physical black 00
        bitClear(_newImage[z1], 7 - (y1 % 8));
        // bitClear(_newImage[_sizePageColour + z1], 7 - (y1 % 8));
    }
    else if ((colour == myColours.black) xor _invert)
    {
        // physical white 10
        bitSet(_newImage[z1], 7 - (y1 % 8));
        // bitClear(_newImage[_sizePageColour + z1], 7 - (y1 % 8));
    }
}

void Screen_EPD_EXT3_Fast::_setOrientation(uint8_t orientation)
{
    _orientation = orientation % 4;
}

bool Screen_EPD_EXT3_Fast::_orientCoordinates(uint16_t & x, uint16_t & y)
{
    bool flag = true; // false=success, true=error
    switch (_orientation)
    {
        case 3: // checked, previously 1

            if ((x < _widthScreen) and (y < _heightScreen))
            {
                x = _widthScreen - 1 - x;
                flag = false;
            }
            break;

        case 2: // checked

            if ((x < _heightScreen) and (y < _widthScreen))
            {
                x = _heightScreen - 1 - x;
                y = _widthScreen - 1 - y;
                swap(x, y);
                flag = false;
            }
            break;

        case 1: // checked, previously 3

            if ((x < _widthScreen) and (y < _heightScreen))
            {
                y = _heightScreen - 1 - y;
                flag = false;
            }
            break;

        default: // checked

            if ((x < _heightScreen) and (y < _widthScreen))
            {
                swap(x, y);
                flag = false;
            }
            break;
    }

    return flag;
}

uint32_t Screen_EPD_EXT3_Fast::_getZ(uint16_t x1, uint16_t y1)
{
    uint32_t z1 = 0;
    // According to 11,98 inch Spectra Application Note
    // at http:// www.pervasivedisplays.com/LiteratureRetrieve.aspx?ID=245146

    z1 = (uint32_t)x1 * _heightBuffer + (y1 >> 3);

    return z1;
}

uint16_t Screen_EPD_EXT3_Fast::_getPoint(uint16_t x1, uint16_t y1)
{
    // Orient and check coordinates are within screen
    // _orientCoordinates() returns false = success, true = error
    if (_orientCoordinates(x1, y1))
    {
        return 0;
    }

    uint16_t result = 0;
    uint8_t value = 0;

    uint32_t z1 = _getZ(x1, y1);

    value = bitRead(_newImage[z1], 7 - (y1 % 8));
    value <<= 4;
    value &= 0b11110000;

    // red = 0-1, black = 1-0, white 0-0
    switch (value)
    {
        case 0x10:

            result = myColours.black;
            break;

        default:

            result = myColours.white;
            break;
    }

    return result;
}

void Screen_EPD_EXT3_Fast::point(uint16_t x1, uint16_t y1, uint16_t colour)
{
    _setPoint(x1, y1, colour);
}

uint16_t Screen_EPD_EXT3_Fast::readPixel(uint16_t x1, uint16_t y1)
{
    return _getPoint(x1, y1);
}

// Utilities
void Screen_EPD_EXT3_Fast::_sendCommand8(uint8_t command)
{
    digitalWrite(_pin.panelDC, LOW);
    digitalWrite(_pin.panelCS, LOW);

    SPI.transfer(command);

    digitalWrite(_pin.panelCS, HIGH);
}

void Screen_EPD_EXT3_Fast::_waitBusy()
{
    // LOW = busy, HIGH = ready
    while (digitalRead(_pin.panelBusy) != HIGH)
    {
        delay(32); // non-blocking
    }
}

void Screen_EPD_EXT3_Fast::_sendIndexData(uint8_t index, const uint8_t * data, uint32_t size)
{
    digitalWrite(_pin.panelDC, LOW); // DC Low = Command
    digitalWrite(_pin.panelCS, LOW); // CS Low = Select

    delayMicroseconds(50);
    SPI.transfer(index);
    delayMicroseconds(50);

    digitalWrite(_pin.panelCS, HIGH); // CS High = Unselect
    digitalWrite(_pin.panelDC, HIGH); // DC High = Data
    digitalWrite(_pin.panelCS, LOW); // CS Low = Select

    delayMicroseconds(50);
    for (uint32_t i = 0; i < size; i++)
    {
        SPI.transfer(data[i]);
    }
    delayMicroseconds(50);

    digitalWrite(_pin.panelCS, HIGH); // CS High = Unselect
}

uint8_t Screen_EPD_EXT3_Fast::getRefreshTime()
{
    return _refreshTime;
}

void Screen_EPD_EXT3_Fast::regenerate()
{
    clear(myColours.black);
    flush();

    delay(100);

    clear(myColours.white);
    flush();
}

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

    if (_pin.touchInt != NOT_CONNECTED)
    {
        pinMode(_pin.touchInt, INPUT_PULLUP);
    }

    if (_pin.touchReset == NOT_CONNECTED)
    {
        Serial.println("! Error - Pin touchReset not defined");
    }

    // Even if PANEL_RESET_PIN == _pin.touchReset
    pinMode(_pin.touchReset, OUTPUT);
    if (_codeSize == 0x37) // specific
    {
        digitalWrite(_pin.touchReset, HIGH);
        delay(10);
        digitalWrite(_pin.touchReset, LOW);
        delay(10);
        digitalWrite(_pin.touchReset, HIGH);
        delay(1000);
    }
    else
    {
        digitalWrite(_pin.touchReset, HIGH);
        delay(100);
        digitalWrite(_pin.touchReset, LOW);
        delay(100);
        digitalWrite(_pin.touchReset, HIGH);
        delay(100);
    }

    if (_codeSize == 0x27)
    {
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
    else if (_codeSize == 0x37)
    {
        _touchAddress = TOUCH_370_ADDRESS; // 0x38

        uint8_t bufferWrite[1] = {0};
        uint8_t bufferRead[4] = {0};

        bufferWrite[0] = 0xa6;
        _wireWriteRead(_touchAddress, bufferWrite, 1, bufferRead, 4);

        _touchXmin = 0;
        _touchXmax = 239; // Xmax, hardware hard-coded
        _touchYmin = 0;
        _touchYmax = 415; // Ymax, hardware hard-coded
    }
    _touchPrevious = TOUCH_EVENT_NONE;
}

void Screen_EPD_EXT3_Fast::_getRawTouch(uint16_t & x0, uint16_t & y0, uint16_t & z0, uint16_t & t0)
{
    delay(10);

    if (_codeSize == 0x27)
    {
        uint8_t bufferWrite[1] = {0};
        uint8_t bufferRead[5] = {0};

        bufferWrite[0] = 0x10; // check
        _wireWriteRead(_touchAddress, bufferWrite, 1, bufferRead, 1);

        uint8_t number = bufferRead[0];
        z0 = 0;
        t0 = TOUCH_EVENT_NONE;

        // Only one finger read, interrupt not used
        // uint8_t flagInterrupt = 1 - digitalRead(_pin.touchInt);
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
        else if (_touchPrevious != TOUCH_EVENT_NONE)
        {
            // Take previous position for release
            _touchPrevious = TOUCH_EVENT_NONE;
            t0 = TOUCH_EVENT_RELEASE;
            x0 = _touchX;
            y0 = _touchY;
            z0 = 0x16;
        }
        else if (_touchPrevious == TOUCH_EVENT_NONE)
        {
            t0 = TOUCH_EVENT_NONE;
            z0 = 0;
        }
    }
    else if (_codeSize == 0x37)
    {
        uint8_t flagInterrupt = 1 - digitalRead(_pin.touchInt);
        // digitalWrite(BLUE_LED, flagInterrupt);

        bool flagValid;
        uint16_t x, y, z, t;
        z0 = 0;
        t0 = TOUCH_EVENT_NONE;

        // Only one finger read
        if (flagInterrupt)
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
                _touchPrevious = true;
                _touchX = x0;
                _touchY = y0;
                z0 = 0x16;
            }
            else
            {
                t0 = TOUCH_EVENT_RELEASE;
            }
        }
        else if (_touchPrevious != TOUCH_EVENT_NONE)
        {
            // Take previous position for release
            _touchPrevious = TOUCH_EVENT_NONE;
            t0 = TOUCH_EVENT_RELEASE;
            x0 = _touchX;
            y0 = _touchY;
            z0 = 0x16;
        }
        else if (_touchPrevious == TOUCH_EVENT_NONE)
        {
            t0 = TOUCH_EVENT_NONE;
            z0 = 0;
        }
    }
}

bool Screen_EPD_EXT3_Fast::_getInterruptTouch()
{
    if (_pin.touchInt != NOT_CONNECTED)
    {
        // Translate for true = interrupt
        // 271 and 370: LOW = false for interrupt
        return (digitalRead(_pin.touchInt) == LOW);
    }
    else
    {
        return false;
    }
}
#endif // TOUCH_MODE
