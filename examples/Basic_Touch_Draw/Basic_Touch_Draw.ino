///
/// @file Basic_Touch_Draw.ino
/// @brief Example of features for basic edition
///
/// @details Library for Pervasive Displays EXT3 - Basic level
///
/// @author Rei Vilo
/// @date 12 Nov 2022
/// @version 601
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
#define DISPLAY_TOUCH 1

// Include application, user and local libraries
#include "Wire.h"
#include "SPI.h"

// Define structures and classes

// Define variables and constants

// === Pervasive Displays iTC
// --- Touch
#include "PDLS_EXT3_Basic_Touch.h"

// Screen_EPD_EXT3_Fast myScreen(eScreen_EPD_EXT3_270_Touch, boardRaspberryPiPico_RP2040);
Screen_EPD_EXT3_Fast myScreen(eScreen_EPD_EXT3_370_Touch, boardRaspberryPiPico_RP2040);

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

#if (DISPLAY_TOUCH == 1)
///
/// @brief Minimal test screen
/// @param flag true=default=perform flush, otherwise no
///
void displayTouch(bool flag = true)
{
    myScreen.selectFont(Font_Terminal12x16);

    myScreen.clear();
    myScreen.setOrientation(3);
    myScreen.gText(10, 10, "Draw!");

    myScreen.flush();

    uint16_t x, y, z, t;
    uint16_t k = 32; // 64; // 256;

    bool flagLine = false;
    uint16_t x0, y0;

    myScreen.setPenSolid(true);
    while (k > 0)
    {
        if (myScreen.getTouch(x, y, z, t))
        {
            // hV_HAL_Debug_println(formatString("%3i: xyzt= %4i %4i %4i %4i %8i", k, x, y, z, t, hV_HAL_getMilliseconds()));
            if ((t == TOUCH_EVENT_RELEASE) or (t == TOUCH_EVENT_MOVE))
            {
                myScreen.circle(x, y, 4, myColours.black);
                myScreen.flush();

                Serial.print(-k);
                k--;
            }

            /*
                        if (t == TOUCH_EVENT_PRESS)
                        {
                            flagLine = false;
                        }

                        if (flagLine)
                        {
                            myScreen.line(x0, y0, x, y, myColours.black);
                            myScreen.flush();
                        }

                        x0 = x;
                        y0 = y;
                        flagLine = true;
                        k--;
            */
        }

        delay(10);
    }
}
#endif

// Add setup code
///
/// @brief Setup
///
void setup()
{
    // Start
    Serial.begin(115200);
    delay(500);
    Serial.println("");
    Serial.println("");
    Serial.println("");
    Serial.println("=== " __FILE__);
    Serial.println("=== " __DATE__ " " __TIME__);
    Serial.println("");

    Serial.print("begin... ");
    myScreen.begin();
    Serial.println(" done");

    myScreen.regenerate();

    Serial.println(myScreen.WhoAmI());

#if (DISPLAY_TOUCH == 1)

    Serial.print("DISPLAY_TOUCH... ");
    myScreen.clear();
    displayTouch();
    Serial.println("done");

    wait(4);

#endif // DISPLAY_TOUCH

    myScreen.regenerate();

    Serial.println("=== ");
    Serial.println();

#if defined(ARDUINO_ARCH_PIDUINO)
    exit(0);
#endif
}

// Add loop code
///
/// @brief Loop, empty
///
void loop()
{
    delay(1000);
}
