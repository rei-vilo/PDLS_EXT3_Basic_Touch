//
// hV_Haptics_GUI.cpp
// Library C++ code
// ----------------------------------
//
// Project Pervasive Displays Library Suite
// Based on highView technology
//
// Created by Rei Vilo, 14 Jun 2013
// Last updated by Rei Vilo, 25 Jan 2022
//
// Copyright (c) Rei Vilo, 2010-2023
// Licence All rights reserved
//
// See hV_GUI.h and ReadMe.txt for references
//
//

// Library header
#include "hV_GUI.h"

#if (HAPTICS_MODE != USE_HAPTICS_NONE)
DRV2605L myDRV2605L(HAPTICS_MODE);
#endif // HAPTICS_MODE

// ---- Shared settings
Screen_EPD_EXT3_Fast * _pScreenGUI;
uint16_t _colourFrontGUI, _colourBackGUI, _colourMiddleGUI;
bool _delegateGUI = true;
bool _enableGUI = true;
uint8_t _styleGUI = 0;

GUI::GUI(Screen_EPD_EXT3_Fast * screen)
{
    _pScreenGUI = screen;
}

void GUI::begin()
{
    _colourFrontGUI = myColours.black;
    _colourBackGUI = myColours.white;
    _colourMiddleGUI = myColours.grey;
    _delegateGUI = true;
    _enableGUI = true;
    _styleGUI = 0;

#if (HAPTICS_MODE != USE_HAPTICS_NONE)
    myDRV2605L.begin();
#endif // HAPTICS_MODE
}

void GUI::setColours(uint16_t frontColour, uint16_t backColour)
{
    _colourFrontGUI = frontColour;
    _colourBackGUI = backColour;
}

void GUI::delegate(bool delegate)
{
    _delegateGUI = delegate;
}

// --- text
text::text()
{
    ;
}

void text::dDefine(uint16_t x0, uint16_t y0, uint16_t dx, uint16_t dy,
                   uint8_t size)
{
    _x0 = x0;
    _y0 = y0;
    _dx = dx;
    _dy = dy;
    _fontSize = size;
}

void text::draw(String text)
{
    _pScreenGUI->selectFont(_fontSize);
    uint8_t k = _pScreenGUI->stringLengthToFitX(text, _dx - 8);

    String _text = text.substring(0, k);

    uint16_t _xt = _x0 + (_dx - _pScreenGUI->stringSizeX(_text)) / 2;
    uint16_t _yt = _y0 + (_dy - _pScreenGUI->characterSizeY()) / 2;

    _pScreenGUI->setPenSolid(true);
    _pScreenGUI->dRectangle(_x0, _y0, _dx, _dy, _colourBackGUI);
    _pScreenGUI->gText(_xt, _yt, _text, _colourFrontGUI);

    if (_delegateGUI)
    {
        _pScreenGUI->flush();
    }
}

// ---- button
button::button()
{
    ;
}

void button::dStringDefine(uint16_t x0, uint16_t y0,
                           uint16_t dx, uint16_t dy,
                           String text0,
                           uint8_t size0)
{
    text::dDefine(x0, y0, dx, dy, size0);
    text::draw(text0);
    draw(fsmReleased);
}

void button::draw(fsmGUI_e fsm)
{
    // All cases
    _pScreenGUI->setPenSolid(false);
    _pScreenGUI->dRectangle(_x0 + 1, _y0 + 1, _dx - 2, _dy - 2, _colourFrontGUI);

    switch (fsm)
    {
        case fsmTouched:

            _pScreenGUI->dRectangle(_x0, _y0, _dx, _dy, _colourFrontGUI);
            _pScreenGUI->dRectangle(_x0 + 2, _y0 + 2, _dx - 4, _dy - 4, _colourFrontGUI);
            break;

        default: // fsmReleased

            _pScreenGUI->dRectangle(_x0, _y0, _dx, _dy, _colourBackGUI);
            _pScreenGUI->dRectangle(_x0 + 2, _y0 + 2, _dx - 4, _dy - 4, _colourBackGUI);
            break;
    }

    if (_delegateGUI)
    {
        _pScreenGUI->flush();
    }
}

bool button::check(bool mode)
{
    uint16_t x, y, z, t;
    bool flag = false;

    // down
    if (_pScreenGUI->getTouch(x, y, z, t))
    {
        // pressed
        if ((x >= _x0) and (x < _x0 + _dx) and (y >= _y0) and (y < _y0 + _dy))
        {
            if (mode == checkInstant)
            {
                return true;
            }

            draw(fsmTouched);

            do
            {
                delay(100);
                _pScreenGUI->getTouch(x, y, z, t);
            }
            while (t != TOUCH_EVENT_RELEASE);

            // Released
            if ((x >= _x0) and (x < _x0 + _dx) and (y >= _y0) and (y < _y0 + _dy))
            {
                flag = true;
            }

            draw(fsmReleased);
        } // if pressed
    } // if down
    // Serial.println();
    return flag;
}

