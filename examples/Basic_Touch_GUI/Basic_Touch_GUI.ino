///
/// @file Basic_Touch_GUI.ino
/// @brief Example of features for Evaluation edition
///
/// @details Library for Pervasive Displays EXT3 - Basic edition
///
/// @author Rei Vilo
/// @date 25 Jan 2023
/// @version 605
///
/// @copyright (c) Rei Vilo, 2010-2023
/// @copyright CC = BY SA NC
///
/// @see ReadMe.txt for references
/// @n
///

// Screen 
#include "PDLS_EXT3_Basic_Touch.h"

// SDK
// #include <Arduino.h>
#include "hV_HAL_Peripherals.h"

// Configuration
#include "hV_Configuration.h"

// Set parameters
#define DISPLAY_GUI 1

// Include application, user and local libraries
// #include <Wire.h>
// #include <SPI.h>

#if (SCREEN_EPD_EXT3_RELEASE < 531)
#error Required SCREEN_EPD_EXT3_RELEASE 531
#endif SCREEN_EPD_EXT3_RELEASE

#include "hV_GUI.h"

#if (hV_GUI_BASIC_RELEASE < 605)
#error Required hV_GUI_BASIC_RELEASE 605
#endif hV_GUI_BASIC_RELEASE

// Define structures and classes

// Define variables and constants
// Screen_EPD_EXT3_Fast myScreen(eScreen_EPD_EXT3_271_09_Touch, boardRaspberryPiPico_RP2040);
Screen_EPD_EXT3_Fast myScreen(eScreen_EPD_EXT3_370_0C_Touch, boardRaspberryPiPico_RP2040);

uint8_t fontText;

// Prototypes

// Utilities
///
/// @brief Wait with countdown
/// @param second duration, s
///
void wait(uint8_t second)
{
    for (uint8_t i = second; i > 0; i--)
    {
        Serial.print(formatString(" > %i  \r", i));
        delay(1000);
    }
    Serial.print("         \r");
}

#if (DISPLAY_GUI == 1)

GUI myGUI(&myScreen);

void displayGUI()
{
    myScreen.selectFont(fontText);

    myScreen.clear();
    myScreen.setOrientation(3);

    myGUI.begin();

    Button myButtonNormal(&myGUI);
    Button myButtonInstant(&myGUI);
    Text myText(&myGUI);

    uint16_t x = myScreen.screenSizeX();
    uint16_t y = myScreen.screenSizeY();

    uint16_t dx = x / 7;
    uint16_t dy = y / 5;

    myGUI.delegate(false);

    myButtonNormal.dStringDefine(dx * 1, dy * 3, dx * 2, dy, "Normal", fontText);
    myButtonInstant.dStringDefine(dx * 4, dy * 3, dx * 2, dy, "Instant", fontText);
    myText.dDefine(0, dy, x, dy, fontText);

    myButtonNormal.draw();
    myButtonInstant.draw();
    myText.draw("Empty");

    myScreen.flush();

    myGUI.delegate(true);

    uint8_t k = 8;
    uint32_t chrono32;
    while (k > 0)
    {
        if (myScreen.getTouchInterrupt())
        {
            chrono32 = millis();
            if (myButtonNormal.check(checkNormal))
            {
                k -= 1;
                chrono32 = millis() - chrono32;
                myText.draw(formatString("%s in %i ms (%i left)", "Normal", chrono32, k));
                Serial.println(formatString("%3i: %s in %i ms", k, "Normal", chrono32));
            }

            chrono32 = millis();
            if (myButtonInstant.check(checkInstant))
            {
                k -= 1;
                chrono32 = millis() - chrono32;
                myText.draw(formatString("%s in %i ms (%i left)", "Instant", chrono32, k));
                Serial.println(formatString("%3i: %s in %i ms", k, "Instant", chrono32));
            }
        } // getTouchInterrupt

        delay(100);
    }

    myScreen.clear();
}
#endif // DISPLAY_GUI

// Add setup code
void setup()
{
    Serial.begin(115200);
    delay(500);
    Serial.println();
    Serial.println("=== " __FILE__);
    Serial.println("=== " __DATE__ " " __TIME__);
    Serial.println();

    Serial.print("begin... ");
    myScreen.begin();
    Serial.println(formatString("%s %ix%i", myScreen.WhoAmI().c_str(), myScreen.screenSizeX(), myScreen.screenSizeY()));

    myScreen.regenerate();

    fontText = Font_Terminal12x16;

#if (DISPLAY_GUI == 1)

    Serial.println("DISPLAY_GUI... ");
    myScreen.clear();
    displayGUI();
    Serial.println(". done");

    // wait(4);

#endif // DISPLAY_GUI

    myScreen.regenerate();

    Serial.println("=== ");
    Serial.println();
}

// Add loop code
void loop()
{
    delay(1000);
}
