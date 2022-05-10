///
/// @file hV_GUI.h
/// @brief Basic GUI with text and button
///
/// @details Project Pervasive Displays Library Suite
/// @n Based on highView technology
///
/// @author Rei Vilo
/// @date 28 Feb 2022
/// @version 531
///
/// @copyright (c) Rei Vilo, 2010-2022
/// @copyright Creative Commons Attribution-ShareAlike 4.0 International (CC BY-SA 4.0)
///
/// The highView Library Suite is shared under the Creative Commons licence Attribution-ShareAlike 4.0 International (CC BY-SA 4.0).
///
/// * Basic edition: for hobbyists and for basic usage
/// @n Creative Commons Attribution-ShareAlike 4.0 International (CC BY-SA 4.0)
///
/// @see https://creativecommons.org/licenses/by-sa/4.0/
///
/// @n Consider the advanced or commercial editions for professionals or organisations and for commercial usage
///

#ifndef hV_GUI_BASIC_RELEASE
///
/// @brief Library release number
///
#define hV_GUI_BASIC_RELEASE 531

// SDK
#if defined(ENERGIA) // LaunchPad specific
#include "Energia.h"
#else // Arduino general
#include "Arduino.h"
#endif // SDK

// Configuration
#include "hV_Configuration.h"

// Other libraries
#include "Screen_EPD_EXT3.h"

#if (SCREEN_EPD_EXT3_RELEASE < 531)
#error Required SCREEN_EPD_EXT3_RELEASE 531
#endif SCREEN_EPD_EXT3_RELEASE

#if (TOUCH_MODE != USE_TOUCH_YES)
#warning TOUCH_MODE should be USE_TOUCH_YES
#endif // TOUCH_MODE

///
/// @name Modes for check()
/// @{
#define checkNormal false ///< normal mode
#define checkInstant true ///< instant mode

///
/// @class GUI
/// @brief GUI
/// @details Shared settings
///
class GUI
{
  public:
    ///
    /// @brief Constructor
    ///
    /// @param screen &screen to which the GUI elements belong
    ///
    GUI(Screen_EPD_EXT3_Fast * screen);

    ///
    /// @brief Initialise the GUI
    ///
    void begin();

    ///
    /// @brief Set the colours palette
    ///
    /// @param frontColour 16-bit colours for text and rectangles
    /// @param backColour 16-bit colours for background
    ///
    void setColours(uint16_t frontColour = myColours.black,
                    uint16_t backColour = myColours.white);

    ///
    /// @brief Delegate the refresh policy
    ///
    /// @param delegate true = default = refresh managed by the GUI element with fast update
    /// @note If false, refresh managed by the caller
    ///
    void delegate(bool delegate = true);
};

///
/// @brief Finite state machine
/// @details Used for Button, CheckBox, Cursor, BarGraph
///
enum fsmGUI_e
{
    fsmReleased, ///< No touch
    fsmTouched ///< First touch
};

///
/// @class text
/// @brief Text
/// @details Display a text with format options
///
class text
{
  public:
    ///
    /// @brief Constructor
    ///
    text();

    ///
    /// @brief Define a text box, vector coordinates
    /// @param x0 point coordinate, x-axis
    /// @param y0 point coordinate, y-axis
    /// @param dx length, x-axis
    /// @param dy height, y-axis
    /// @param size size of the font, default = 0 = small, optional
    ///
    /// @n @b More: @ref Coordinate, @ref Colour
    ///
    void dDefine(uint16_t x0, uint16_t y0,
                 uint16_t dx, uint16_t dy,
                 uint8_t size = 0);

    ///
    /// @brief Display the text
    /// @param text text to be displayed
    ///
    void draw(String text);

  protected:
    /// @cond
    uint16_t _x0, _y0, _dx, _dy;
    uint8_t _fontSize;
    /// @endcond
};

///
/// @class button
/// @brief Button
/// @details Button is an active text
///
class button : public text
{
  public:
    ///
    /// @brief Constructor
    ///
    button();

    ///
    /// @brief Define button, vector coordinates
    /// @param x0 point coordinate, x-axis
    /// @param y0 point coordinate, y-axis
    /// @param dx length, x-axis
    /// @param dy height, y-axis
    /// @param text0 text
    /// @param size0 size for text, default = 0
    ///
    /// @n @b More: @ref Coordinate, @ref Colour
    ///
    void dStringDefine(uint16_t x0, uint16_t y0,
                       uint16_t dx, uint16_t dy,
                       String text0,
                       uint8_t size0 = 0);

    ///
    /// @brief Draw the button
    /// @param fsm state
    /// @n fsm = fsmReleased or fsmTouched
    ///
    void draw(fsmGUI_e fsm = fsmReleased);

    ///
    /// @brief Check button is pressed
    /// @param mode default = checkNormal = false area waits for release, checkInstant = true element is activated by press only
    /// @return true if button pressed
    ///
    bool check(bool mode = checkNormal);
};

#endif // hV_GUI_BASIC_RELEASE
