/**
 * @file   ssd1306_driver.h
 * @brief  SSD1306 OLED display C library.
 *
 * @copyright
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  any later version.
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * @author Giovanni Scotti
 */

#ifndef __SSD1306_H__
#define __SSD1306_H__

/* C++ detection */
#ifdef __cplusplus
    extern "C" {
#endif


#include <stdint.h>  // for uint_t.
#include <stdlib.h>  // for size_t.
#include <stdbool.h> // for bool.
#include "ssd1306_fonts.h"


#define SSD1306_PXL_WIDTH   128
#define SSD1306_PXL_HEIGHT  64
#define SSD1306_BUFFER_SIZE (SSD1306_PXL_WIDTH * SSD1306_PXL_HEIGHT / 8)


/**
 * Display status enumeration.
 */
typedef enum {
    SSD1306_OK           = 0, /*!< Display command success. */
    SSD1306_NOINIT       = 1, /*!< Display has not been initialized before use. */
    SSD1306_WRONG_PARAMS = 2, /*!< Invalid arguments. */
    SSD1306_COMM_ERROR   = 3, /*!< Error while communicating with the display. */
    SSD1306_STATUS_COUNT = 4  /*!< Total number of possible status. */
} ssd1306_status_t;


/**
 * Display color enumeration.
 */
typedef enum {
    SSD1306_COLOR_BLACK = 0, /*!< Black color, no pixel. */
    SSD1306_COLOR_WHITE = 1  /*!< Pixel is set and lit. */
} ssd1306_color_t;


/**
 * Types of scrolling animations.
 */
typedef enum {
    RIGHT_HORIZONTAL_SCROLL,
    LEFT_HORIZONTAL_SCROLL,
    VERTICAL_RIGHT_HORIZONTAL_SCROLL,
    VERTICAL_LEFT_HORIZONTAL_SCROLL
} ssd1306_scroll_t;


/**
 * Display pages enumeration. Mainly used for scroll command.
 */
typedef enum {
    SSD1306_PAGE_0 = 0, /*!< COM0  - COM7  */
    SSD1306_PAGE_1 = 1, /*!< COM8  - COM15 */
    SSD1306_PAGE_2 = 2, /*!< COM16 - COM23 */
    SSD1306_PAGE_3 = 3, /*!< COM24 - COM31 */
    SSD1306_PAGE_4 = 4, /*!< COM32 - COM39 */
    SSD1306_PAGE_5 = 5, /*!< COM40 - COM47 */
    SSD1306_PAGE_6 = 6, /*!< COM48 - COM55 */
    SSD1306_PAGE_7 = 7  /*!< COM56 - COM63 */
} ssd1306_page_t;


/**
 * Time interval between each scroll step in terms of frame frequency.
 * The higher the number of frames, the lower the scrolling speed.
 */
typedef enum {
    SSD1306_TIME_INT_5_FRAMES   = 0,
    SSD1306_TIME_INT_64_FRAMES  = 1,
    SSD1306_TIME_INT_128_FRAMES = 2,
    SSD1306_TIME_INT_256_FRAMES = 3,
    SSD1306_TIME_INT_3_FRAMES   = 4,
    SSD1306_TIME_INT_4_FRAMES   = 5,
    SSD1306_TIME_INT_25_FRAMES  = 6,
    SSD1306_TIME_INT_2_FRAMES   = 7
} ssd1306_time_int_t;


/**
 * Structure to store information about the ssd1306 display status.
 * The internal software buffer is configured as follows:
 * Each vertical segment represents a buffer byte. The LSB is row 0
 * and the MSB is row 7. Each byte belongs to the same column (SEG x).
 *
 * | | | | ... | | page 0 (byte 0 to byte 127)
 * | | | | ... | | page 1 (byte 128 to byte 255)
 *   ...            ...
 *
 */
typedef struct {
    uint8_t x_pos;       /*!< Current position of the cursor on x axis. */
    uint8_t y_pos;       /*!< Current position of the cursor on y axis. */
    bool    inverted;    /*!< Display color is inverted. */
    bool    initialized; /*!< Display initialization flag. */
    bool    scrolling;   /*!< Display is performing scrolling activities. */
    uint8_t i2c_channel; /*!< Defines the i2c peripheral connected to the display. */
    uint8_t i2c_addr;    /*!< Address of the display for i2c communication. */
    uint8_t buffer[SSD1306_BUFFER_SIZE]; /*!< Holds display content. */
} ssd1306_t;


/**
 * Turns the display on. This function has a direct effect on the
 * display hardware.
 *
 * @param  ssd1306_ptr a pointer to a ssd1306_t structure.
 * @return the outcome of the function call.
 */
ssd1306_status_t
ssd1306_display_on(ssd1306_t *ssd1306_ptr);


/**
 * Turns the display off. This function has a direct effect on the
 * display hardware.
 *
 * @param  ssd1306_ptr a pointer to a ssd1306_t structure.
 * @return the outcome of the function call.
 */
ssd1306_status_t
ssd1306_display_off(ssd1306_t *ssd1306_ptr);


/**
 * Sets display contrast. The output current increases as the contrast
 * step value gets higher. This function has a direct effect on the display
 * hardware.
 *
 * @param  ssd1306_ptr    a pointer to a ssd1306_t structure.
 * @param  contrast_value a value in the range of 0x0 - 0xFF.
 * @return the outcome of the function call.
 */
ssd1306_status_t
ssd1306_set_contrast(ssd1306_t *ssd1306_ptr, uint8_t contrast_value);


/**
 * Inverts the display color. This function has a direct effect on the
 * display hardware.
 *
 * @param  ssd1306_ptr a pointer to a ssd1306_t structure.
 * @param  is_inverted if true, inverts the display color. If false,
 *                     sets the display color as normal.
 * @return the outcome of the function call.
 */
ssd1306_status_t
ssd1306_set_inversion(ssd1306_t *ssd1306_ptr, bool is_inverted);


/**
 * Allows to turn on and off the scrolling functionality.
 * Must be called after setting scrolling parameters via the
 * ssd1306_scroll function. This function has a direct effect
 * on the display hardware.
 *
 * @param  ssd1306_ptr  a pointer to a ssd1306_t structure.
 * @param  is_scrolling if true, enables scrolling. If false, disables it.
 * @return the outcome of the function call.
 */
ssd1306_status_t
ssd1306_set_scroll_status(ssd1306_t *ssd1306_ptr, bool is_scrolling);


/*
 * Sets up scrolling behaviour and starts scrolling. After calling this
 * function, the user can enable and disable scrolling by means of
 * ssd1306_set_scroll_status function. This function has a direct effect
 * on the display hardware.
 *
 * @param  ssd1306_ptr a pointer to a ssd1306_t structure.
 * @param  type        type of scrolling. Valid values are offered
 *                     by the ssd1306_scroll_t enumeration.
 * @param  start_page  start page address. Valid values are offered
 *                     by the ssd1306_page_t enumeration.
 * @param  end_page    end page address. Valid values are offered
 *                     by the ssd1306_page_t enumeration.
 * @param  interval    the time interval between each scroll step expressed
 *                     in terms of frame frequency. Valid values are
 *                     offered by the ssd1306_time_int_t enumeration.
 * @return the outcome of the function call.
 */
ssd1306_status_t
ssd1306_scroll(ssd1306_t *ssd1306_ptr, ssd1306_scroll_t type, ssd1306_page_t start_page,
    ssd1306_page_t end_page, ssd1306_time_int_t interval);


/**
 * Sets the cursor to desired location expressed in pixels.
 * If the new cursor coordinates are outside the display area,
 * SSD1306_WRONG_PARAMS status code is returned.
 *
 * @param  ssd1306_ptr a pointer to a ssd1306_t structure.
 * @param  x           new x coordinates for the cursor. Allowed values
 *                     are between 0 and SSD1306_PXL_WIDTH-1.
 * @param  y           new y coordinates for the cursor. Allowed values
 *                     are between 0 and SSD1306_PXL_HEIGHT-1.
 * @return the outcome of the function call.
 */
ssd1306_status_t
ssd1306_goto_xy(ssd1306_t *ssd1306_ptr, uint8_t x, uint8_t y);


/**
 * Fills the entire display with desired color. This function modifies only
 * the software buffer of the given ssd1306_ptr structure. It needs to be
 * followed by an ssd1306_update function call to take effect on the display.
 *
 * @param  ssd1306_ptr a pointer to a ssd1306_t structure.
 * @param  color       color to be used to fill the screen. Valid colors
 *                     are offered by the ssd1306_color_t enumeration.
 * @return the outcome of the function call.
 */
ssd1306_status_t
ssd1306_draw_fill(ssd1306_t *ssd1306_ptr, ssd1306_color_t color);


/**
 * Draws one pixel at the desired location. No error is returned if the
 * user provides invalid coordinates: no action is taken. This function
 * modifies only the software buffer of the given ssd1306_ptr structure.
 * It needs to be followed by an ssd1306_update function call to take
 * effect on the display.
 *
 * @param  ssd1306_ptr a pointer to a ssd1306_t structure.
 * @param  x           the x coordinate of the pixel to draw.
 * @param  y           the y coordinate of the pixel to draw.
 * @param  color       color of the drawn pixel. Valid colors
 *                     are offered by the ssd1306_color_t enumeration.
 * @return the outcome of the function call.
 */
ssd1306_status_t
ssd1306_draw_pixel(ssd1306_t *ssd1306_ptr, uint8_t x, uint8_t y,
        ssd1306_color_t color);


/**
 * Draws the given character to the current cursor position. This function
 * modifies only the software buffer of the given ssd1306_ptr structure.
 * It needs to be followed by an ssd1306_update function call to take
 * effect on the display.
 *
 * @param  ssd1306_ptr a pointer to a ssd1306_t structure.
 * @param  ch          the character to draw.
 * @param  font_name   the font to use. Valid fonts are specified by the
 *                     ssd1306_font_name_t enumeration.
 * @param  color       color of the drawn character. Valid colors
 *                     are offered by the ssd1306_color_t enumeration.
 * @return the outcome of the function call.
 */
ssd1306_status_t
ssd1306_draw_char(ssd1306_t *ssd1306_ptr, const char ch,
        ssd1306_font_name_t font_name, ssd1306_color_t color);


/**
 * Draws the given string from the current cursor position. This function
 * modifies only the software buffer of the given ssd1306_ptr structure.
 * It needs to be followed by an ssd1306_update function call to take
 * effect on the display.
 *
 * @param  ssd1306_ptr a pointer to a ssd1306_t structure.
 * @param  str         a pointer to a char array (string).
 * @param  str_len     the length of the string to draw.
 * @param  font_name   the font to use. Valid fonts are specified by the
 *                     ssd1306_font_name_t enumeration.
 * @param  color       color of the drawn string. Valid colors
 *                     are offered by the ssd1306_color_t enumeration.
 * @return the outcome of the function call.
 */
ssd1306_status_t
ssd1306_draw_str(ssd1306_t *ssd1306_ptr, const char *str, size_t str_len,
        ssd1306_font_name_t font_name, ssd1306_color_t color);


/**
 * This function performs an integer (base 10) to string conversion. The string
 * represents the number itself converted to the given base. This function
 * modifies only the software buffer of the given ssd1306_ptr structure.
 * It needs to be followed by an ssd1306_update function call to take
 * effect on the display.
 *
 * @param  ssd1306_ptr a pointer to a ssd1306_t structure.
 * @param  num         the integer (base 10) to draw on the screen.
 * @param  base        the new base of the given number (2 to 32 supported).
 * @param  font_name   the font to use. Valid fonts are specified by the
 *                     ssd1306_font_name_t enumeration.
 * @param  color       color of the drawn pixels. Valid colors
 *                     are offered by the ssd1306_color_t enumeration.
 * @return the outcome of the function call.
 */
ssd1306_status_t
ssd1306_draw_int(ssd1306_t *ssd1306_ptr, int32_t num, uint8_t base,
        ssd1306_font_name_t font_name, ssd1306_color_t color);


/**
 * Draws a segment from (x0,y0) to (x1,y1). this function takes advantage
 * of the Bresenham's algorithm. No error is returned if the segment is
 * printed outside the display area. This function only modifies the
 * software buffer of the given ssd1306_ptr structure. It needs to be
 * followed by an ssd1306_update function call to take effect on the display.
 *
 * @param  ssd1306_ptr a pointer to a ssd1306_t structure.
 * @param  x0          the x coordinate of the starting segment point.
 * @param  y0          the y coordinate of the starting segment point.
 * @param  x1          the x coordinate of the ending segment point.
 * @param  y1          the y coordinate of the ending segment point.
 * @param  color       color of the drawn pixels. Valid colors
 *                     are offered by the ssd1306_color_t enumeration.
 * @return the outcome of the function call.
 */
ssd1306_status_t
ssd1306_draw_line(ssd1306_t *ssd1306_ptr, uint8_t x0, uint8_t y0,
        uint8_t x1, uint8_t y1, ssd1306_color_t color);


/**
 * Draws a rectangle whose top-left vertex is at (x,y), with width w and
 * height h. This function only modifies the software buffer of the given
 * ssd1306_ptr structure. It needs to be followed by an ssd1306_update
 * function call to take effect on the display.
 *
 * @param  ssd1306_ptr a pointer to a ssd1306_t structure.
 * @param  x           the x coordinate of the top-left vertex.
 * @param  y           the y coordinate of the top-left vertex.
 * @param  w           the width of the rectangle.
 * @param  h           the height og the rectangle.
 * @param  color       color of the drawn pixels. Valid colors
 *                     are offered by the ssd1306_color_t enumeration.
 * @return the outcome of the function call.
 */
ssd1306_status_t
ssd1306_draw_rect(ssd1306_t *ssd1306_ptr, uint8_t x, uint8_t y,
        uint8_t w, uint8_t h, ssd1306_color_t color);


/**
 * Draws a filled rectangle whose top-left vertex is at (x,y), with width w
 * and height h. This function only modifies the software buffer of the given
 * ssd1306_ptr structure. It needs to be followed by an ssd1306_update
 * function call to take effect on the display.
 *
 * @param  ssd1306_ptr a pointer to a ssd1306_t structure.
 * @param  x           the x coordinate of the top-left vertex.
 * @param  y           the y coordinate of the top-left vertex.
 * @param  w           the width of the rectangle.
 * @param  h           the height og the rectangle.
 * @param  color       color of the drawn pixels. Valid colors
 *                     are offered by the ssd1306_color_t enumeration.
 * @return the outcome of the function call.
 */
ssd1306_status_t
ssd1306_draw_filled_rect(ssd1306_t *ssd1306_ptr, uint8_t x, uint8_t y,
        uint8_t w, uint8_t h, ssd1306_color_t color) ;


/**
 * Draws a circle whose center is at (x0,y0), with radius r. This function
 * takes advantage of the Bresenham's (midpoint circle) algorithm.
 * This function only modifies the software buffer of the given
 * ssd1306_ptr structure. It needs to be followed by an ssd1306_update
 * function call to take effect on the display.
 *
 * @param  ssd1306_ptr a pointer to a ssd1306_t structure.
 * @param  x0          the x coordinate of the center.
 * @param  y0          the y coordinate of the center.
 * @param  r           the radius of the circle.
 * @param  color       color of the drawn pixels. Valid colors
 *                     are offered by the ssd1306_color_t enumeration.
 * @return the outcome of the function call.
 */
ssd1306_status_t
ssd1306_draw_circle(ssd1306_t *ssd1306_ptr, uint8_t x0, uint8_t y0, uint16_t r,
        ssd1306_color_t color);


/**
 * Draws a filled circle whose center is at (x0,y0), with radius r.
 * This function takes advantage of the Bresenham's (midpoint circle) algorithm.
 * This function only modifies the software buffer of the given
 * ssd1306_ptr structure. It needs to be followed by an ssd1306_update
 * function call to take effect on the display.
 *
 * @param  ssd1306_ptr a pointer to a ssd1306_t structure.
 * @param  x0          the x coordinate of the center.
 * @param  y0          the y coordinate of the center.
 * @param  r           the radius of the circle.
 * @param  color       color of the drawn pixels. Valid colors
 *                     are offered by the ssd1306_color_t enumeration.
 * @return the outcome of the function call.
 */
ssd1306_status_t
ssd1306_draw_filled_circle(ssd1306_t *ssd1306_ptr, uint8_t x0, uint8_t y0,
        uint16_t r, ssd1306_color_t color);


/**
 * Draws a triangle given its three vertices.
 * This function only modifies the software buffer of the given
 * ssd1306_ptr structure. It needs to be followed by an ssd1306_update
 * function call to take effect on the display.
 *
 * @param  ssd1306_ptr a pointer to a ssd1306_t structure.
 * @param  x1          the x coordinate of the first vertex.
 * @param  y1          the y coordinate of the first vertex.
 * @param  x2          the x coordinate of the second vertex.
 * @param  y2          the y coordinate of the second vertex.
 * @param  x3          the x coordinate of the third vertex.
 * @param  y3          the y coordinate of the third vertex.
 * @param  color       color of the drawn pixels. Valid colors
 *                     are offered by the ssd1306_color_t enumeration.
 * @return the outcome of the function call.
 */
ssd1306_status_t
ssd1306_draw_triangle(ssd1306_t *ssd1306_ptr, uint8_t x1, uint8_t y1,
        uint8_t x2, uint8_t y2, uint8_t x3, uint8_t y3, ssd1306_color_t color);


/**
 * Draws a filled triangle given its three vertices.
 * This function only modifies the software buffer of the given
 * ssd1306_ptr structure. It needs to be followed by an ssd1306_update
 * function call to take effect on the display.
 *
 * @param  ssd1306_ptr a pointer to a ssd1306_t structure.
 * @param  x1          the x coordinate of the first vertex.
 * @param  y1          the y coordinate of the first vertex.
 * @param  x2          the x coordinate of the second vertex.
 * @param  y2          the y coordinate of the second vertex.
 * @param  x3          the x coordinate of the third vertex.
 * @param  y3          the y coordinate of the third vertex.
 * @param  color       color of the drawn pixels. Valid colors
 *                     are offered by the ssd1306_color_t enumeration.
 * @return the outcome of the function call.
 */
ssd1306_status_t
ssd1306_draw_filled_triangle(ssd1306_t *ssd1306_ptr, uint8_t x1, uint8_t y1,
        uint8_t x2, uint8_t y2, uint8_t x3, uint8_t y3, ssd1306_color_t color);


/**
 * Draws the given bitmap encoded as an array of little-endian bytes,
 * 8 pixels per byte and horizontal orientation.
 * This function only modifies the software buffer of the given
 * ssd1306_ptr structure. It needs to be followed by an ssd1306_update
 * function call to take effect on the display.
 *
 * @param  ssd1306_ptr a pointer to a ssd1306_t structure.
 * @param  bitmap      a pointer to the array encoding the bitmap.
 * @param  x           the x coordinate where to start to draw.
 * @param  y           the y coordinate where to start to draw.
 * @param  w           the width of the bitmap in pixels.
 * @param  h           the height of the bitmap in pixels.
 * @param  color       color of the drawn pixels. Valid colors
 *                     are offered by the ssd1306_color_t enumeration.
 * @return the outcome of the function call.
 */
ssd1306_status_t
ssd1306_draw_bitmap(ssd1306_t *ssd1306_ptr, uint8_t x, uint8_t y,
        const unsigned char* bitmap, uint8_t w, uint8_t h, ssd1306_color_t color);


/**
 * The update function allows to flush the internal software buffer
 * of the given ssd1306_ptr object to the display ram. The whole GDDRAM
 * is written when this function is called. It is useful to let *_draw_*
 * functions take effect on the screen.
 *
 * @param  ssd1306_ptr a pointer to a ssd1306_t structure.
 * @return the outcome of the function call.
 */
ssd1306_status_t
ssd1306_update(ssd1306_t *ssd1306_ptr);


/**
 * Clears the display by resetting color inversion and filling the screen
 * with black pixels.
 *
 * @param  ssd1306_ptr a pointer to a ssd1306_t structure.
 * @return the outcome of the function call.
 */
ssd1306_status_t
ssd1306_clear_display(ssd1306_t *ssd1306_ptr);


/**
 * Clears the internal software buffer of the given ssd1306_ptr object.
 * No action is taken on the display hardware.
 *
 * @param  ssd1306_ptr a pointer to a ssd1306_t structure.
 * @return the outcome of the function call.
 */
ssd1306_status_t
ssd1306_clear_buffer(ssd1306_t *ssd1306_ptr);


/**
 * Initializes the ssd1306 i2c display.
 *
 * @param  ssd1306_ptr a pointer to a ssd1306_t structure.
 * @param  i2c_channel the i2c peripheral identifier to route requests to
 *                     the correct i2c master (see ssd1306_config.c for more
 *                     information).
 * @param  i2c_addr    the i2c address of the target ssd1306 display.
 * @return the outcome of the function call.
 */
ssd1306_status_t
ssd1306_init(ssd1306_t *ssd1306_ptr, uint8_t i2c_channel, uint8_t i2c_addr);


/* C++ detection */
#ifdef __cplusplus
    }
#endif

#endif // __SSD1306_H
