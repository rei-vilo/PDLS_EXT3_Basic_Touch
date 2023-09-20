///
/// @file Screen_EPD_EXT3.h
/// @brief Driver for Pervasive Displays iTC monochrome touch screens with EXT3-1 and EXT3-Touch boards
///
/// @details Project Pervasive Displays Library Suite
/// @n Based on highView technology
///
/// @n @b B-T-F
/// * Edition: Basic
/// * Family: Touch 2.71 3.70
/// * Update: Fast
/// * Feature: none
/// * Temperature: 15 to 30 °C
///
/// @n Supported screens with touch
/// * 2.71"-Touch reference xTP271PGH0x
/// * 3.70"-Touch reference xTP370PGH0x
///
/// @author Rei Vilo
/// @date 21 Sep 2023
/// @version 700
///
/// @copyright (c) Rei Vilo, 2010-2023
/// @copyright Creative Commons Attribution-ShareAlike 4.0 International (CC BY-SA 4.0)
///
/// The highView Library Suite is shared under the Creative Commons licence Attribution-ShareAlike 4.0 International (CC BY-SA 4.0).
///
/// * Basic edition: for hobbyists and for basic usage
/// @n Creative Commons Attribution-ShareAlike 4.0 International (CC BY-SA 4.0)
///
/// @see https://creativecommons.org/licenses/by-sa/4.0/
///
/// @n Consider the Evaluation or Commercial editions for professionals or organisations and for commercial usage
///

// SDK
#include "hV_HAL_Peripherals.h"

// Configuration
#include "hV_Configuration.h"

// Other libraries
#include "hV_Screen_Buffer.h"

// Board
#include "hV_Board.h"

// EPD utilities
#include "hV_Utilities_EPD.h"

#if (hV_CONFIGURATION_RELEASE < 700)
#error Required hV_CONFIGURATION_RELEASE 700
#endif // hV_CONFIGURATION_RELEASE

#if (hV_SCREEN_BUFFER_RELEASE < 700)
#error Required hV_SCREEN_BUFFER_RELEASE 700
#endif // hV_SCREEN_BUFFER_RELEASE

#if (hV_BOARD_RELEASE < 700)
#error Required hV_BOARD_RELEASE 700
#endif // hV_BOARD_RELEASE

#ifndef SCREEN_EPD_EXT3_RELEASE
///
/// @brief Library release number
///
#define SCREEN_EPD_EXT3_RELEASE 700

///
/// @brief Library variant
///
#define SCREEN_EPD_EXT3_VARIANT "Basic-Touch"

// Other libraries
#include "hV_Screen_Buffer.h"

#if (hV_SCREEN_BUFFER_RELEASE < 700)
#error Required hV_SCREEN_BUFFER_RELEASE 700
#endif // hV_SCREEN_BUFFER_RELEASE

#if (TOUCH_MODE != USE_TOUCH_YES)
#error TOUCH_MODE should be USE_TOUCH_YES
#endif // TOUCH_MODE

// Objects
//
///
/// @brief Class for Pervasive Displays iTC monochome touch screens with embedded fast update
/// @details Screen controllers
/// * LCD: proprietary, SPI
/// * touch: proprietary, I2C
/// * fonts: no external Flash
///
/// @note All commands work on the frame-buffer,
/// to be displayed on screen with flush()
///
class Screen_EPD_EXT3_Fast final : public hV_Screen_Buffer, public hV_Utilities_EPD
{
  public:
    ///
    /// @brief Constructor with default pins
    /// @param eScreen_EPD_EXT3 size and model of the e-screen
    /// @param board board configuration
    /// @note To be used with begin() with no parameter
    ///
    Screen_EPD_EXT3_Fast(eScreen_EPD_EXT3_t eScreen_EPD_EXT3, pins_t board);

    ///
    /// @brief Initialisation
    /// @note Frame-buffer generated internally, not suitable for FRAM
    /// @warning begin() initialises SPI and I2C
    ///
    void begin();

    ///
    /// @brief Who Am I
    /// @return Who Am I string
    ///
    String WhoAmI();

    ///
    /// @brief Clear the screen
    /// @param colour default = white
    /// @note Clear next frame-buffer
    ///
    void clear(uint16_t colour = myColours.white);

    ///
    /// @brief Update the display, fast update
    /// @note Display next frame-buffer on screen and copy next frame-buffer into old frame-buffer
    ///
    void flush();

    ///
    /// @brief Regenerate the panel
    /// @details White-to-black-to-white cycle to reduce ghosting
    ///
    void regenerate();

    ///
    /// @brief Update the display
    /// @details Display next frame-buffer on screen and copy next frame-buffer into old frame-buffer
    /// @param updateMode expected update mode
    /// @return uint8_t recommended mode
    /// @note Mode checked with checkTemperatureMode()
    ///
    uint8_t flushMode(uint8_t updateMode = UPDATE_FAST);

  protected:
    /// @cond

    // Orientation
    ///
    /// @brief Set orientation
    /// @param orientation 1..3, 6, 7
    ///
    void _setOrientation(uint8_t orientation); // compulsory

    ///
    /// @brief Check and orient coordinates, logical coordinates
    /// @param x x-axis coordinate, modified
    /// @param y y-axis coordinate, modified
    /// @return false = success, true = error
    ///
    bool _orientCoordinates(uint16_t & x, uint16_t & y); // compulsory

    // Write and Read
    /// @brief Set point
    /// @param x1 x coordinate
    /// @param y1 y coordinate
    /// @param colour 16-bit colour
    /// @n @b More: @ref Colour, @ref Coordinate
    ///
    void _setPoint(uint16_t x1, uint16_t y1, uint16_t colour);

    /// @brief Get point
    /// @param x1 x coordinate
    /// @param y1 y coordinate
    /// @return colour 16-bit colour
    /// @n @b More: @ref Colour, @ref Coordinate
    ///
    uint16_t _getPoint(uint16_t x1, uint16_t y1);

    // Position
    ///
    /// @brief Convert
    /// @param x1 x-axis coordinate
    /// @param y1 y-axis coordinate
    /// @return index for _newImage[]
    ///
    uint32_t _getZ(uint16_t x1, uint16_t y1);

    ///
    /// @brief Convert
    /// @param x1 x-axis coordinate
    /// @param y1 y-axis coordinate
    /// @return bit for _newImage[]
    ///
    uint16_t _getB(uint16_t x1, uint16_t y1);

    //
    // === Energy section
    //

    //
    // === End of Energy section
    //

    // * Other functions specific to the screen
    void COG_initial(uint8_t updateMode);
    void COG_getUserData();
    void COG_sendImageDataFast();
    void COG_update(uint8_t updateMode);
    void COG_powerOff();

    // * Flush
    void _flushFast();

    bool _flag50;

    //
    // === Touch section
    //
#if (TOUCH_MODE != USE_TOUCH_NONE)

    uint8_t _touchAddress;
    uint8_t _touchPrevious;
    uint16_t _touchX, _touchY;

    void _wireWriteRead(uint8_t address, uint8_t * dataWrite, size_t sizeWrite, uint8_t * dataRead = 0, size_t sizeRead = 0);

    void _beginTouch();
    void _getRawTouch(uint16_t & x0, uint16_t & y0, uint16_t & z0, uint16_t & t0); // compulsory
    bool _getInterruptTouch(); // compulsory

#endif // TOUCH_MODE
    //
    // === End of Touch section
    //

    /// @endcond
};

#endif // SCREEN_EPD_EXT3_RELEASE

