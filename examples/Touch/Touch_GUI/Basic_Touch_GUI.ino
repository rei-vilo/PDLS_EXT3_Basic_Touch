///
/// @file Basic_Touch_GUI.ino
/// @brief Example of features for Evaluation edition
///
/// @details Library for Pervasive Displays EXT3 - Basic edition
///
/// @author Rei Vilo
/// @date 21 Jan 2025
/// @version 812
///
/// @copyright (c) Rei Vilo, 2010-2025
/// @copyright Creative Commons Attribution-ShareAlike 4.0 International (CC BY-SA 4.0)
/// @copyright For exclusive use with Pervasive Displays screens
///
/// @see ReadMe.txt for references
/// @n
///

// Screen
#include "PDLS_EXT3_Basic_Touch.h"

// SDK
// #include <Arduino.h>
#include "hV_HAL_Peripherals.h"

// Include application, user and local libraries
// #include <Wire.h>
// #include <SPI.h>

// Configuration
#include "hV_Configuration.h"

// Set parameters
#define DISPLAY_GUI 1

#if (SCREEN_EPD_EXT3_RELEASE < 812)
#error Required SCREEN_EPD_EXT3_RELEASE 812
#endif SCREEN_EPD_EXT3_RELEASE

#include "hV_GUI.h"

#if (hV_GUI_BASIC_RELEASE < 812)
#error Required hV_GUI_BASIC_RELEASE 812
#endif hV_GUI_BASIC_RELEASE

// Define structures and classes

// Define variables and constants
// Screen_EPD_EXT3_Fast myScreen(eScreen_EPD_271_KS_09_Touch, boardRaspberryPiPico_RP2040);
Screen_EPD_EXT3_Fast myScreen(eScreen_EPD_370_PS_0C_Touch, boardRaspberryPiPico_RP2040);

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
        mySerial.print(formatString(" > %i  \r", i));
        delay(1000);
    }
    mySerial.print("         \r");
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
                mySerial.println(formatString("%3i: %s in %i ms", k, "Normal", chrono32));
            }

            chrono32 = millis();
            if (myButtonInstant.check(checkInstant))
            {
                k -= 1;
                chrono32 = millis() - chrono32;
                myText.draw(formatString("%s in %i ms (%i left)", "Instant", chrono32, k));
                mySerial.println(formatString("%3i: %s in %i ms", k, "Instant", chrono32));
            }
        } // getTouchInterrupt

        delay(100);
    }

    myScreen.clear();
}
#endif // DISPLAY_GUI

// Add setup code
///
/// @brief Setup
///
void setup()
{
    // Start
    // mySerial = Serial by default, otherwise edit hV_HAL_Peripherals.h
    mySerial.begin(115200);
    delay(500);
    mySerial.println();
    mySerial.println("=== " __FILE__);
    mySerial.println("=== " __DATE__ " " __TIME__);
    mySerial.println();

    mySerial.print("begin... ");
    myScreen.begin();
    mySerial.println(myScreen.WhoAmI());

    myScreen.regenerate();

    fontText = Font_Terminal12x16;

#if (DISPLAY_GUI == 1)

    mySerial.println("DISPLAY_GUI... ");
    myScreen.clear();
    displayGUI();
    wait(4);

#endif // DISPLAY_GUI

    mySerial.println("White... ");
    myScreen.regenerate();

    mySerial.println("=== ");
    mySerial.println();
}

// Add loop code
///
/// @brief Loop, empty
///
void loop()
{
    delay(1000);
}
