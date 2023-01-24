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
// Release 531: Improved performance
// Release 605: Improved elements link to GUI
//

// Library header
#include "hV_GUI.h"

#if (HAPTICS_MODE != USE_HAPTICS_NONE)
DRV2605L myDRV2605L(HAPTICS_MODE);
#endif // HAPTICS_MODE

GUI::GUI(Screen_EPD_EXT3_Fast * screen)
{
    _pScreen = screen;
}

void GUI::begin()
{
    _colourFront = myColours.black;
    _colourBack = myColours.white;
    _delegate = true;

#if (HAPTICS_MODE != USE_HAPTICS_NONE)
    myDRV2605L.begin();
#endif // HAPTICS_MODE
}

void GUI::setColours(uint16_t frontColour, uint16_t backColour)
{
    _colourFront = frontColour;
    _colourBack = backColour;
}

void GUI::delegate(bool delegate)
{
    _delegate = delegate;
}

// --- Text
Text::Text(GUI * gui)
{
    _pGUI = gui;
}

void Text::dDefine(uint16_t x0, uint16_t y0, uint16_t dx, uint16_t dy,
                   uint8_t size)
{
    _x0 = x0;
    _y0 = y0;
    _dx = dx;
    _dy = dy;
    _fontSize = size;
}

void Text::draw(String text)
{
    _pGUI->_pScreen->selectFont(_fontSize);
    uint8_t k = _pGUI->_pScreen->stringLengthToFitX(text, _dx - 8);

    String _text = text.substring(0, k);

    uint16_t _xt = _x0 + (_dx - _pGUI->_pScreen->stringSizeX(_text)) / 2;
    uint16_t _yt = _y0 + (_dy - _pGUI->_pScreen->characterSizeY()) / 2;

    _pGUI->_pScreen->setPenSolid(true);
    _pGUI->_pScreen->dRectangle(_x0, _y0, _dx, _dy, _pGUI->_colourBack);
    _pGUI->_pScreen->gText(_xt, _yt, _text, _pGUI->_colourFront);

    if (_pGUI->_delegate)
    {
        _pGUI->_pScreen->flush();
    }
}

// ---- Button
Button::Button(GUI * gui)
{
    _pGUI = gui;
}

void Button::dStringDefine(uint16_t x0, uint16_t y0,
                           uint16_t dx, uint16_t dy,
                           String text0,
                           uint8_t size0)
{
    Text::dDefine(x0, y0, dx, dy, size0);
    Text::draw(text0);
    draw(fsmReleased);
}

void Button::draw(fsmGUI_e fsm)
{
    // All cases
    _pGUI->_pScreen->setPenSolid(false);
    _pGUI->_pScreen->dRectangle(_x0 + 1, _y0 + 1, _dx - 2, _dy - 2, _pGUI->_colourFront);

    switch (fsm)
    {
        case fsmTouched:

            _pGUI->_pScreen->dRectangle(_x0, _y0, _dx, _dy, _pGUI->_colourFront);
            _pGUI->_pScreen->dRectangle(_x0 + 2, _y0 + 2, _dx - 4, _dy - 4, _pGUI->_colourFront);
            break;

        default: // fsmReleased

            _pGUI->_pScreen->dRectangle(_x0, _y0, _dx, _dy, _pGUI->_colourBack);
            _pGUI->_pScreen->dRectangle(_x0 + 2, _y0 + 2, _dx - 4, _dy - 4, _pGUI->_colourBack);
            break;
    }

    if (_pGUI->_delegate)
    {
        _pGUI->_pScreen->flush();
    }
}

bool Button::check(bool mode)
{
    uint16_t x, y, z, t;
    bool flag = false;

    // down
    if (_pGUI->_pScreen->getTouch(x, y, z, t))
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
                _pGUI->_pScreen->getTouch(x, y, z, t);
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

