///
/// @file Basic_Touch_GUI.ino
/// @brief Example of features for Evaluation edition
///
/// @details Library for Pervasive Displays EXT3 - Basic edition
///
/// @author Rei Vilo
/// @date 21 Nov 2022
/// @version 602
///
/// @copyright (c) Rei Vilo, 2010-2022
/// @copyright CC = BY SA NC
///
/// @see ReadMe.txt for references
/// @n
///

// SDK
#if defined(ENERGIA) // LaunchPad specific
#include "Energia.h"
#else // Arduino general
#include "Arduino.h"
#endif // SDK

// Set parameters
#define DISPLAY_GUI 1

// Include application, user and local libraries
#include "SPI.h"
#include "PDLS_EXT3_Basic_Touch.h"
#include "hV_GUI.h"

#if (SCREEN_EPD_EXT3_RELEASE < 531)
#error Required SCREEN_EPD_EXT3_RELEASE 531
#endif SCREEN_EPD_EXT3_RELEASE

// Define structures and classes

// Define variables and constants
// Screen_EPD_EXT3_Fast myScreen(eScreen_EPD_EXT3_270_Touch, boardRaspberryPiPico_RP2040);
Screen_EPD_EXT3_Fast myScreen(eScreen_EPD_EXT3_370_Touch, boardRaspberryPiPico_RP2040);

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

    button myFirst, mySecond;
    text myText;

    uint16_t x = myScreen.screenSizeX();
    uint16_t y = myScreen.screenSizeY();

    uint16_t dx = x / 7;
    uint16_t dy = y / 5;

    myGUI.delegate(false);

    myFirst.dStringDefine(dx * 1, dy * 3, dx * 2, dy, "Normal", fontText);
    mySecond.dStringDefine(dx * 4, dy * 3, dx * 2, dy, "Instant", fontText);
    myText.dDefine(0, dy, x, dy, fontText);

    myFirst.draw();
    mySecond.draw();
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
            if (myFirst.check(checkNormal))
            {
                k -= 1;
                chrono32 = millis() - chrono32;
                myText.draw(formatString("%s in %i ms (%i left)", "Normal", chrono32, k));
                Serial.println(formatString("%3i: %s in %i ms", k, "Normal", chrono32));
            }

            chrono32 = millis();
            if (mySecond.check(checkInstant))
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

#if defined(ARDUINO_ARCH_PIDUINO)
    exit(0);
#endif
}

// Add loop code
void loop()
{
    delay(1000);
}
