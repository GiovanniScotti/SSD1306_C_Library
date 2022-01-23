/**
 * @file   ssd1306_fonts.h
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

#ifndef __SSD1306_FONTS_H__
#define __SSD1306_FONTS_H__

/* C++ detection */
#ifdef __cplusplus
    extern "C" {
#endif


#include <stdint.h> // for uint_t.


/**
 * Defines names for the implemented fonts.
 * They can be used to select the desired font when needed.
 */
typedef enum {
    FONT_7X10  = 0,
    FONT_11X18 = 1,
    FONT_16X26 = 2,
    FONT_COUNT = 3
} ssd1306_font_name_t;


/**
 * Structure for storing font data.
 */
typedef struct {
    const uint8_t font_width;  /*!< Font width in pixels. */
    const uint8_t font_height; /*!< Font height in pixels. */
    const void    *font_map;   /*!< Pointer to font data array. */
} ssd1306_font_t;


/**
 * Returns a pointer to the selected font. If the given font name
 * is invalid, returns NULL.
 *
 * @param  font_name font name as described by its enumeration type.
 * @return a pointer to the selected font or NULL if font is invalid.
 */
const ssd1306_font_t * get_font_ptr(ssd1306_font_name_t font_name);


/* C++ detection */
#ifdef __cplusplus
    }
#endif

#endif // __SSD1306_FONTS_H__
