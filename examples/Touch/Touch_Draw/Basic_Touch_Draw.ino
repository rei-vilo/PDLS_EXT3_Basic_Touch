///
/// @file Basic_Touch_Draw.ino
/// @brief Example of features for basic edition
///
/// @details Library for Pervasive Displays EXT3 - Basic level
///
/// @author Rei Vilo
/// @date 21 Nov 2024
/// @version 810
///
/// @copyright (c) Rei Vilo, 2010-2024
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
#define DISPLAY_TOUCH 1

// Define structures and classes

// Define variables and constants
// Screen_EPD_EXT3_Fast myScreen(eScreen_EPD_271_KS_09_Touch, boardRaspberryPiPico_RP2040);
Screen_EPD_EXT3_Fast myScreen(eScreen_EPD_370_PS_0C_Touch, boardRaspberryPiPico_RP2040);

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

                mySerial.print(-k);
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

#if (DISPLAY_TOUCH == 1)

    mySerial.println("DISPLAY_TOUCH... ");
    myScreen.clear();
    displayTouch();
    wait(4);

#endif // DISPLAY_TOUCH

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
