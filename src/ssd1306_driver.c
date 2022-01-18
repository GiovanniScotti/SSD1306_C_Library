/**
 * @file   ssd1306_driver.c
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


#include <string.h> // for memcpy, memset.
#include <assert.h> // for assert function.
#include "ssd1306_driver.h"


///////////////////////////////////////////////////////////
// COMMAND DEFINITIONS
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
// Addressing setting commands.
///////////////////////////////////////////////////////////

#define SSD1306_CMD_SET_MEMORY_ADDRESSING_MODE       0x20
#define SSD1306_CMD_SET_COLUMN_ADDRESS               0x21 /// Only for horizontal and vertical addressing modes.
#define SSD1306_CMD_SET_PAGE_ADDRESS                 0x22 /// Only for horizontal and vertical addressing modes.

// Addressing modes.
#define SSD1306_SUBCMD_MEM_ADDR_MODE_HAM             0x00 /// Horizontal addressing mode.
#define SSD1306_SUBCMD_MEM_ADDR_MODE_VAM             0x01 /// Vertical addressing mode.
#define SSD1306_SUBCMD_MEM_ADDR_MODE_PAM             0x02 /// Page addressing mode.
#define SSD1306_SUBCMD_MEM_ADDR_MODE_INV             0x03 /// Invalid addressing mode.

// Page start address for page addressing mode.
#define SSD1306_CMD_PAGE_START_ADDRESS_PAGE_0        0xB0
#define SSD1306_CMD_PAGE_START_ADDRESS_PAGE_1        0xB1
#define SSD1306_CMD_PAGE_START_ADDRESS_PAGE_2        0xB2
#define SSD1306_CMD_PAGE_START_ADDRESS_PAGE_3        0xB3
#define SSD1306_CMD_PAGE_START_ADDRESS_PAGE_4        0xB4
#define SSD1306_CMD_PAGE_START_ADDRESS_PAGE_5        0xB5
#define SSD1306_CMD_PAGE_START_ADDRESS_PAGE_6        0xB6
#define SSD1306_CMD_PAGE_START_ADDRESS_PAGE_7        0xB7

///////////////////////////////////////////////////////////
// Hardware configuration commands.
///////////////////////////////////////////////////////////

#define SSD1306_CMD_SEGMENT_REMAP_COL0_SEG0          0xA0
#define SSD1306_CMD_SEGMENT_REMAP_COL127_SEG0        0xA1
#define SSD1306_CMD_SET_MULTIPLEX_RATIO              0xA8
#define SSD1306_CMD_COM_SCAN_DIRECTION_NORMAL        0xC0
#define SSD1306_CMD_COM_SCAN_DIRECTION_REMAPPED      0xC8
#define SSD1306_CMD_SET_DISPLAY_OFFSET               0xD3
#define SSD1306_CMD_SET_COM_PINS_HW_CONFIG           0xDA

///////////////////////////////////////////////////////////
// Timing and driving scheme setting commands.
///////////////////////////////////////////////////////////

#define SSD1306_CMD_SET_DIS_CLK_OSC_FREQ             0xD5
#define SSD1306_CMD_SET_PRECHARGE_PERIOD             0xD9
#define SSD1306_CMD_SET_VCOMH_DESELECT_LEVEL         0xDB

///////////////////////////////////////////////////////////
// Scrolling commands.
///////////////////////////////////////////////////////////

#define SSD1306_CMD_RIGHT_HORIZONTAL_SCROLL          0x26
#define SSD1306_CMD_LEFT_HORIZONTAL_SCROLL           0x27
#define SSD1306_CMD_VERTICAL_RIGHT_HORIZONTAL_SCROLL 0x29
#define SSD1306_CMD_VERTICAL_LEFT_HORIZONTAL_SCROLL  0x2A
#define SSD1306_CMD_DEACTIVATE_SCROLL                0x2E
#define SSD1306_CMD_ACTIVATE_SCROLL                  0x2F
#define SSD1306_CMD_SET_VERTICAL_SCROLL_AREA         0xA3

///////////////////////////////////////////////////////////
// Fundamental commands.
///////////////////////////////////////////////////////////

#define SSD1306_CMD_SETCONTRAST                      0x81
#define SSD1306_CMD_CHARGE_PUMP_SETTING              0x8D
#define SSD1306_CMD_RESUMETORAM                      0xA4
#define SSD1306_CMD_IGNORERAM                        0xA5
#define SSD1306_CMD_NORMALDISPLAY                    0xA6
#define SSD1306_CMD_INVERTDISPLAY                    0xA7
#define SSD1306_CMD_DISPLAYOFF                       0xAE
#define SSD1306_CMD_DISPLAYON                        0xAF

#define SSD1306_SUBCMD_DISABLE_CHARGE_PUMP           0x10
#define SSD1306_SUBCMD_ENABLE_CHARGE_PUMP            0x14


///////////////////////////////////////////////////////////
// MISC DEFINITIONS
///////////////////////////////////////////////////////////

/// A command list must declare this byte as first element
/// in order to inform the hardware that the following bytes
/// are commands.
#define SSD1306_CMD_CONTROL_BYTE                     0x00

/// A data list must declare this byte as first element
/// in order to inform the hardware that the following bytes
/// are data.
#define SSD1306_DATA_CONTROL_BYTE                    0x40

/// Buffer size in bytes used by the ssd1306_data_write.
#define SSD1306_DATA_WRITE_BUFFER_SIZE               129

#define SSD1306_DECLARE_STATUS_VARIABLE() \
    ssd1306_status_t status;

#define SSD1306_DECLARE_COMMAND_WRITE(cmd)        \
    status = ssd1306_cmd_write(ssd1306_ptr, cmd); \
    if (status != SSD1306_OK) return status;

#define SSD1306_DECLARE_COMMAND_WRITE_MULTI(cmd_list)                          \
    status = ssd1306_cmd_write_multi(ssd1306_ptr, cmd_list, sizeof(cmd_list)); \
    if (status != SSD1306_OK) return status;

#define SSD1306_NUM_PAGES 8           /// Number of display pages.
#define ABS(x) ((x) > 0 ? (x) : -(x)) /// Computes the absolute value of x.


///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

// Defined in ssd1306_config.h. Allows to communicate to the
// display over i2c bus.
extern ssd1306_status_t
ssd1306_i2c_write(uint8_t channel, uint8_t addr,
        const uint8_t *data_ptr, size_t data_size);


/**
 * Issues the specified command to the given display.
 *
 * @param  ssd1306_ptr a pointer to a ssd1306_t structure.
 * @param  cmd         the command to execute.
 * @return the outcome of the function call.
 */
static ssd1306_status_t
ssd1306_cmd_write(ssd1306_t *ssd1306_ptr, uint8_t cmd) {

    if (ssd1306_ptr->initialized == false)
        return SSD1306_NOINIT;

    uint8_t cmd_list[2] = {SSD1306_CMD_CONTROL_BYTE, cmd};

    return ssd1306_i2c_write(ssd1306_ptr->i2c_channel, ssd1306_ptr->i2c_addr,
            cmd_list, sizeof(cmd_list));
}


/**
 * Issues the specified list of commands to the given display.
 *
 * @param  ssd1306_ptr a pointer to a ssd1306_t structure.
 * @param  cmd_list    a pointer to an array of command bytes. This array
 *                     must present SSD1306_CMD_CONTROL_BYTE as first element.
 * @return the outcome of the function call.
 */
static ssd1306_status_t
ssd1306_cmd_write_multi(ssd1306_t *ssd1306_ptr, const uint8_t *cmd_list,
        uint8_t cmd_list_size) {

    if (ssd1306_ptr->initialized == false)
        return SSD1306_NOINIT;

    return ssd1306_i2c_write(ssd1306_ptr->i2c_channel, ssd1306_ptr->i2c_addr,
            cmd_list, cmd_list_size);
}


/**
 * Writes the given data buffer to the specified display.
 *
 * @param  ssd1306_ptr a pointer to a ssd1306_t structure.
 * @param  data_ptr    the data buffer to be written.
 * @param  data_size   the size of the data buffer in bytes.
 * @return the outcome of the function call.
 */
static ssd1306_status_t
ssd1306_data_write(ssd1306_t *ssd1306_ptr, const uint8_t *data_ptr,
        size_t data_size) {

    if (ssd1306_ptr->initialized == false)
        return SSD1306_NOINIT;
    if (data_size >= SSD1306_DATA_WRITE_BUFFER_SIZE)
        return SSD1306_WRONG_PARAMS;

    // The given data buffer must be prepended by 0x40 in order to signal
    // data bytes to the display hardware. Since we do not have memory
    // allocation here, a data buffer with 128+1 bytes is preallocated
    // on the stack. Its size has been chosen not to saturate the stack.
    uint8_t data_array[SSD1306_DATA_WRITE_BUFFER_SIZE] =
            {SSD1306_DATA_CONTROL_BYTE};

    memcpy(data_array + 1, data_ptr, data_size);

    return ssd1306_i2c_write(ssd1306_ptr->i2c_channel, ssd1306_ptr->i2c_addr,
            data_array, data_size + 1);
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
// PUBLIC API
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
// FUNDAMENTAL COMMANDS
///////////////////////////////////////////////////////////

ssd1306_status_t
ssd1306_display_on(ssd1306_t *ssd1306_ptr) {

    SSD1306_DECLARE_STATUS_VARIABLE()

    const uint8_t cmd_list[] = {
            SSD1306_CMD_CONTROL_BYTE,
            SSD1306_CMD_CHARGE_PUMP_SETTING,
            SSD1306_SUBCMD_ENABLE_CHARGE_PUMP,
            SSD1306_CMD_DISPLAYON
    };

    SSD1306_DECLARE_COMMAND_WRITE_MULTI(cmd_list)
    return SSD1306_OK;
}


ssd1306_status_t
ssd1306_display_off(ssd1306_t *ssd1306_ptr) {

    SSD1306_DECLARE_STATUS_VARIABLE()

    const uint8_t cmd_list[] = {
            SSD1306_CMD_CONTROL_BYTE,
            SSD1306_CMD_CHARGE_PUMP_SETTING,
            SSD1306_SUBCMD_DISABLE_CHARGE_PUMP,
            SSD1306_CMD_DISPLAYOFF
    };

    SSD1306_DECLARE_COMMAND_WRITE_MULTI(cmd_list)
    return SSD1306_OK;
}


ssd1306_status_t
ssd1306_set_contrast(ssd1306_t *ssd1306_ptr, uint8_t contrast_value) {

    SSD1306_DECLARE_STATUS_VARIABLE()

    const uint8_t cmd_list[] = {
            SSD1306_CMD_CONTROL_BYTE,
            SSD1306_CMD_SETCONTRAST,
            contrast_value
    };

    SSD1306_DECLARE_COMMAND_WRITE_MULTI(cmd_list)
    return SSD1306_OK;
}


ssd1306_status_t
ssd1306_set_inversion(ssd1306_t *ssd1306_ptr, bool is_inverted) {

    SSD1306_DECLARE_STATUS_VARIABLE()

    const uint8_t cmd_list[] = {
            SSD1306_CMD_CONTROL_BYTE,
            (is_inverted) ?
                    SSD1306_CMD_INVERTDISPLAY : SSD1306_CMD_NORMALDISPLAY
    };

    SSD1306_DECLARE_COMMAND_WRITE_MULTI(cmd_list)

    ssd1306_ptr->inverted = is_inverted;
    return SSD1306_OK;
}


///////////////////////////////////////////////////////////
// SCROLLING
///////////////////////////////////////////////////////////

ssd1306_status_t
ssd1306_set_scroll_status(ssd1306_t *ssd1306_ptr, bool is_scrolling) {

    SSD1306_DECLARE_STATUS_VARIABLE()

    const uint8_t cmd_list[] = {
            SSD1306_CMD_CONTROL_BYTE,
            (is_scrolling) ?
                    SSD1306_CMD_ACTIVATE_SCROLL : SSD1306_CMD_DEACTIVATE_SCROLL
    };

    SSD1306_DECLARE_COMMAND_WRITE_MULTI(cmd_list)

    ssd1306_ptr->scrolling = is_scrolling;
    return SSD1306_OK;
}


ssd1306_status_t
ssd1306_scroll(ssd1306_t *ssd1306_ptr, ssd1306_scroll_t type,
        ssd1306_page_t start_page, ssd1306_page_t end_page,
        ssd1306_time_int_t interval) {

    SSD1306_DECLARE_STATUS_VARIABLE()
    uint8_t scroll_cmd;

    switch (type) {
        case RIGHT_HORIZONTAL_SCROLL:
            scroll_cmd = SSD1306_CMD_RIGHT_HORIZONTAL_SCROLL;          break;
        case LEFT_HORIZONTAL_SCROLL:
            scroll_cmd = SSD1306_CMD_LEFT_HORIZONTAL_SCROLL;           break;
        case VERTICAL_RIGHT_HORIZONTAL_SCROLL:
            scroll_cmd = SSD1306_CMD_VERTICAL_RIGHT_HORIZONTAL_SCROLL; break;
        case VERTICAL_LEFT_HORIZONTAL_SCROLL:
            scroll_cmd = SSD1306_CMD_VERTICAL_LEFT_HORIZONTAL_SCROLL;  break;
        default:
            return SSD1306_WRONG_PARAMS;
    }

    // Common commands.
    const uint8_t cmd_list[] = {
            SSD1306_CMD_CONTROL_BYTE,
            scroll_cmd, // Type of scrolling.
            0x00,       // Dummy byte.
            start_page,
            interval,
            end_page
    };

    SSD1306_DECLARE_COMMAND_WRITE_MULTI(cmd_list)

    switch (type) {
        case RIGHT_HORIZONTAL_SCROLL:
        case LEFT_HORIZONTAL_SCROLL:
            SSD1306_DECLARE_COMMAND_WRITE(0x00); // dummy byte.
            SSD1306_DECLARE_COMMAND_WRITE(0xFF); // dummy byte.
            break;
        case VERTICAL_RIGHT_HORIZONTAL_SCROLL:
        case VERTICAL_LEFT_HORIZONTAL_SCROLL:
            // Fixed vertical scrolling offset (1 row).
            SSD1306_DECLARE_COMMAND_WRITE(0x01);
            break;
    }

    // Starts scrolling.
    return ssd1306_set_scroll_status(ssd1306_ptr, true);
}


///////////////////////////////////////////////////////////
// DRAWING
///////////////////////////////////////////////////////////

ssd1306_status_t
ssd1306_goto_xy(ssd1306_t *ssd1306_ptr, uint8_t x, uint8_t y) {

    if (x >= SSD1306_PXL_WIDTH || y >= SSD1306_PXL_HEIGHT)
        return SSD1306_WRONG_PARAMS;

    ssd1306_ptr->x_pos = x;
    ssd1306_ptr->y_pos = y;

    return SSD1306_OK;
}


ssd1306_status_t
ssd1306_draw_fill(ssd1306_t *ssd1306_ptr, ssd1306_color_t color) {
    uint8_t pxl_color;

    switch (color) {
        case SSD1306_COLOR_BLACK:
            pxl_color = (ssd1306_ptr->inverted) ? 0xFF : 0x00; break;
        case SSD1306_COLOR_WHITE:
            pxl_color = (ssd1306_ptr->inverted) ? 0x00 : 0xFF; break;
        default:
            return SSD1306_WRONG_PARAMS;
    }

    memset(ssd1306_ptr->buffer, pxl_color, sizeof(ssd1306_ptr->buffer));
    return SSD1306_OK;
}


ssd1306_status_t
ssd1306_draw_pixel(ssd1306_t *ssd1306_ptr, uint8_t x, uint8_t y,
        ssd1306_color_t color) {

    if (x >= SSD1306_PXL_WIDTH || y >= SSD1306_PXL_HEIGHT)
        return SSD1306_OK;

    ssd1306_color_t pxl_color = (ssd1306_ptr->inverted) ? !color : color;

    switch (pxl_color) {
        case SSD1306_COLOR_WHITE:
            ssd1306_ptr->buffer[x + (y >> 3) * SSD1306_PXL_WIDTH] |= 1 << (y & 0x7);
            break;
        case SSD1306_COLOR_BLACK:
            ssd1306_ptr->buffer[x + (y >> 3) * SSD1306_PXL_WIDTH] &= ~(1 << (y & 0x7));
            break;
        default:
            return SSD1306_WRONG_PARAMS;
    }

    return SSD1306_OK;
}


ssd1306_status_t
ssd1306_draw_char(ssd1306_t *ssd1306_ptr, const char ch,
        ssd1306_font_name_t font_name, ssd1306_color_t color) {

    SSD1306_DECLARE_STATUS_VARIABLE()
    const ssd1306_font_t *font = get_font_ptr(font_name);

    for (uint8_t i = 0; i < font->font_height; i++) {
        // There are fonts encoded in array of bytes and fonts encoded
        // in array of half-words (16-bits). The sdd1306_font_t carries
        // a void pointer in order to be used with both kinds of fonts.
        // This is why a little manipulation is required here.
        //
        // Since the first available character of the ASCII table is 'space'
        // (32d), subtracts it from the given char to compute the font array
        // index to extract char pixels encoding.
        uint16_t char_line;
        if (font->font_width <= 8) {
            // Font is encoded as uint8_t array.
            uint8_t *font_map_ptr = (uint8_t *)font->font_map;
            char_line = font_map_ptr[(ch - 32) * font->font_height + i] << 8;
        } else {
            // Font is encoded as uint16_t array.
            uint16_t *font_map_ptr = (uint16_t *)font->font_map;
            char_line = font_map_ptr[(ch - 32) * font->font_height + i];
        }

        for (uint8_t j = 0; j < font->font_width; j++) {
            ssd1306_color_t pixel_color = ((char_line << j) & 0x8000) ?
                    color : !color;

            status = ssd1306_draw_pixel(
                    ssd1306_ptr,
                    ssd1306_ptr->x_pos + j,
                    ssd1306_ptr->y_pos + i,
                    pixel_color);
            if (status != SSD1306_OK) return status;
        }
    }

    ssd1306_ptr->x_pos += font->font_width;
    return SSD1306_OK;
}


ssd1306_status_t
ssd1306_draw_str(ssd1306_t *ssd1306_ptr, const char *str, size_t str_len,
        ssd1306_font_name_t font_name, ssd1306_color_t color) {

    SSD1306_DECLARE_STATUS_VARIABLE()

    for (size_t i = 0; i < str_len; i++) {
        // Writes string character by character for the given length.
        status = ssd1306_draw_char(ssd1306_ptr, str[i], font_name, color);
        if (status != SSD1306_OK) return status;
    }

    return SSD1306_OK;
}


ssd1306_status_t
ssd1306_draw_int(ssd1306_t *ssd1306_ptr, int32_t num, uint8_t base,
        ssd1306_font_name_t font_name, ssd1306_color_t color) {

    char str_buf[32] = "0";

    // Unsupported base.
    if (base < 2 || base > 32) return SSD1306_WRONG_PARAMS;

    if (num != 0) {
        int32_t i = 0;
        int32_t num_abs = ABS(num);
        bool is_negative = false;

        if (num < 0 && base == 10) is_negative = true;

        // Processes individual digits.
        while (num_abs) {
            int32_t rem = num_abs % base;
            str_buf[i++] = (rem > 9) ? (rem - 10) + 'A' : rem + '0';
            num_abs /= base;
        }

        if (is_negative) str_buf[i++] = '-';
        str_buf[i] = '\0';

        // Inverts the buffer by swapping characters.
        uint8_t start_idx = 0;
        uint8_t end_idx = i - 1;
        while (start_idx < end_idx) {
            char c_tmp = str_buf[start_idx];
            str_buf[start_idx] = str_buf[end_idx];
            str_buf[end_idx] = c_tmp;

            start_idx++;
            end_idx--;
        }
    }

    // Prints the string carrying the given number converted to the given base.
    return ssd1306_draw_str(
            ssd1306_ptr,
            str_buf,
            strlen(str_buf),
            font_name,
            color);
}


ssd1306_status_t
ssd1306_draw_line(ssd1306_t *ssd1306_ptr, uint8_t x0, uint8_t y0,
        uint8_t x1, uint8_t y1, ssd1306_color_t color) {

    SSD1306_DECLARE_STATUS_VARIABLE()

    // Bresenham's algorithm.
    int32_t dx  = ABS(x1 - x0);
    int32_t sx  = (x0 < x1) ? 1 : -1;
    int32_t dy  = -ABS(y1 - y0);
    int32_t sy  = (y0 < y1) ? 1 : -1;
    int32_t err = dx + dy;

    while (true) {
        status = ssd1306_draw_pixel(ssd1306_ptr, x0, y0, color);
        if (status != SSD1306_OK) return status;

        if (x0 == x1 && y0 == y1) break;

        int32_t e2 = err << 1; // err * 2.
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }

    return SSD1306_OK;
}


ssd1306_status_t
ssd1306_draw_rect(ssd1306_t *ssd1306_ptr, uint8_t x, uint8_t y,
        uint8_t w, uint8_t h, ssd1306_color_t color) {

    SSD1306_DECLARE_STATUS_VARIABLE()

    // Top line.
    status = ssd1306_draw_line(ssd1306_ptr, x, y, x + w, y, color);
    if (status != SSD1306_OK) return status;

    // Bottom line.
    status = ssd1306_draw_line(ssd1306_ptr, x, y + h, x + w, y + h, color);
    if (status != SSD1306_OK) return status;

    // Left line.
    status = ssd1306_draw_line(ssd1306_ptr, x, y, x, y + h, color);
    if (status != SSD1306_OK) return status;

    // Right line.
    status = ssd1306_draw_line(ssd1306_ptr, x + w, y, x + w, y + h, color);
    if (status != SSD1306_OK) return status;

    return SSD1306_OK;
}


ssd1306_status_t
ssd1306_draw_filled_rect(ssd1306_t *ssd1306_ptr, uint8_t x, uint8_t y,
        uint8_t w, uint8_t h, ssd1306_color_t color) {

    SSD1306_DECLARE_STATUS_VARIABLE()

    for (uint8_t i = 0; i <= h; i++) {
        status = ssd1306_draw_line(ssd1306_ptr, x, y + i, x + w, y + i, color);
        if (status != SSD1306_OK) return status;
    }

    return SSD1306_OK;
}


ssd1306_status_t
ssd1306_draw_circle(ssd1306_t *ssd1306_ptr, uint8_t x0, uint8_t y0, uint16_t r,
        ssd1306_color_t color) {

    SSD1306_DECLARE_STATUS_VARIABLE()
    int32_t xc = x0, yc = y0, rc = r;
    int32_t x = -rc, y = 0, err = 2-2*rc;

    // Bresenham's algorithm.
    do {
        status = ssd1306_draw_pixel(ssd1306_ptr, xc-x, yc+y, color);
        if (status != SSD1306_OK) return status;
        status = ssd1306_draw_pixel(ssd1306_ptr, xc-y, yc-x, color);
        if (status != SSD1306_OK) return status;
        status = ssd1306_draw_pixel(ssd1306_ptr, xc+x, yc-y, color);
        if (status != SSD1306_OK) return status;
        status = ssd1306_draw_pixel(ssd1306_ptr, xc+y, yc+x, color);
        if (status != SSD1306_OK) return status;

        rc = err;
        if (rc >  x) err += ++x*2+1;
        if (rc <= y) err += ++y*2+1;
    } while (x < 0);

    return SSD1306_OK;
}


ssd1306_status_t
ssd1306_draw_filled_circle(ssd1306_t *ssd1306_ptr, uint8_t x0, uint8_t y0,
        uint16_t r, ssd1306_color_t color) {

    SSD1306_DECLARE_STATUS_VARIABLE()
    int32_t xc = x0, yc = y0, rc = r;
    int32_t x = -rc, y = 0, err = 2-2*rc;

    // Bresenham's algorithm.
    // FIXME: does not generate a perfectly filled circle.
    do {
        status = ssd1306_draw_line(ssd1306_ptr, xc-x, yc+y, xc+x, yc-y, color);
        if (status != SSD1306_OK) return status;
        status = ssd1306_draw_line(ssd1306_ptr, xc-y, yc-x, xc+y, yc+x, color);
        if (status != SSD1306_OK) return status;
        status = ssd1306_draw_line(ssd1306_ptr, xc+x, yc-y, xc-x, yc+y, color);
        if (status != SSD1306_OK) return status;
        status = ssd1306_draw_line(ssd1306_ptr, xc+y, yc+x, xc-y, yc-x, color);
        if (status != SSD1306_OK) return status;

        rc = err;
        if (rc >  x) err += ++x*2+1;
        if (rc <= y) err += ++y*2+1;
    } while (x < 0);

    return SSD1306_OK;
}


ssd1306_status_t
ssd1306_draw_triangle(ssd1306_t *ssd1306_ptr, uint8_t x1, uint8_t y1,
        uint8_t x2, uint8_t y2, uint8_t x3, uint8_t y3, ssd1306_color_t color) {

    SSD1306_DECLARE_STATUS_VARIABLE()

    status = ssd1306_draw_line(ssd1306_ptr, x1, y1, x2, y2, color);
    if (status != SSD1306_OK) return status;
    status = ssd1306_draw_line(ssd1306_ptr, x2, y2, x3, y3, color);
    if (status != SSD1306_OK) return status;
    status = ssd1306_draw_line(ssd1306_ptr, x3, y3, x1, y1, color);
    if (status != SSD1306_OK) return status;

    return SSD1306_OK;
}


ssd1306_status_t
ssd1306_draw_filled_triangle(ssd1306_t *ssd1306_ptr, uint8_t x1, uint8_t y1,
        uint8_t x2, uint8_t y2, uint8_t x3, uint8_t y3, ssd1306_color_t color) {

    int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0;
    int16_t yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0;
    int16_t numpixels = 0, curpixel = 0;

    deltax = ABS(x2 - x1);
    deltay = ABS(y2 - y1);
    x = x1;
    y = y1;

    if (x2 >= x1) { xinc1 = 1;  xinc2 = 1;  }
    else          { xinc1 = -1; xinc2 = -1; }

    if (y2 >= y1) { yinc1 = 1;  yinc2 = 1;  }
    else          { yinc1 = -1; yinc2 = -1; }

    if (deltax >= deltay) {
        xinc1     = 0;
        yinc2     = 0;
        den       = deltax;
        num       = deltax / 2;
        numadd    = deltay;
        numpixels = deltax;
    } else {
        xinc2     = 0;
        yinc1     = 0;
        den       = deltay;
        num       = deltay / 2;
        numadd    = deltax;
        numpixels = deltay;
    }

    for (curpixel = 0; curpixel <= numpixels; curpixel++) {
        SSD1306_DECLARE_STATUS_VARIABLE()

        status = ssd1306_draw_line(ssd1306_ptr, x, y, x3, y3, color);
        if (status != SSD1306_OK) return status;

        num += numadd;
        if (num >= den) {
            num -= den;
            x += xinc1;
            y += yinc1;
        }
        x += xinc2;
        y += yinc2;
    }

    return SSD1306_OK;
}


ssd1306_status_t
ssd1306_draw_bitmap(ssd1306_t *ssd1306_ptr, uint8_t x, uint8_t y,
        const unsigned char* bitmap, uint8_t w, uint8_t h,
        ssd1306_color_t color) {

    uint16_t byte_width = (w + 7) >> 3; // Bitmap scanline pad = whole byte.
    uint8_t  byte = 0;

    for (uint8_t j = 0; j < h; j++, y++) {
        for (uint8_t i = 0; i < w; i++) {
            if (i & 7)
                byte <<= 1;
            else
                byte = bitmap[j * byte_width + (i >> 3)];

            if (byte & 0x80) {
                SSD1306_DECLARE_STATUS_VARIABLE()
                status = ssd1306_draw_pixel(ssd1306_ptr, x + i, y, color);
                if (status != SSD1306_OK) return status;
            }
        }
    }

    return SSD1306_OK;
}


ssd1306_status_t
ssd1306_update(ssd1306_t *ssd1306_ptr) {
    SSD1306_DECLARE_STATUS_VARIABLE()

    assert(SSD1306_PXL_WIDTH < SSD1306_DATA_WRITE_BUFFER_SIZE);

    // The software buffer is flushed to the display ram.
    // The GDDRAM is written entirely when this function is called.
    // Exploiting horizontal addressing mode allows to get rid of setting
    // the right page and column address because it is automatic.
    for (uint8_t p = 0; p < SSD1306_NUM_PAGES; p++) {
        status = ssd1306_data_write(ssd1306_ptr,
            &ssd1306_ptr->buffer[SSD1306_PXL_WIDTH * p], SSD1306_PXL_WIDTH);
        if (status != SSD1306_OK) return status;
    }

    return SSD1306_OK;
}


///////////////////////////////////////////////////////////
// INITIALIZATION FUNCTIONS
///////////////////////////////////////////////////////////

ssd1306_status_t
ssd1306_clear_display(ssd1306_t *ssd1306_ptr) {
    SSD1306_DECLARE_STATUS_VARIABLE()

    status = ssd1306_set_inversion(ssd1306_ptr, false);
    if (status != SSD1306_OK) return status;

    status = ssd1306_draw_fill(ssd1306_ptr, SSD1306_COLOR_BLACK);
    if (status != SSD1306_OK) return status;

    status = ssd1306_update(ssd1306_ptr);
    if (status != SSD1306_OK) return status;

    return SSD1306_OK;
}


ssd1306_status_t
ssd1306_clear_buffer(ssd1306_t *ssd1306_ptr) {

    memset(ssd1306_ptr->buffer, 0, sizeof(ssd1306_ptr->buffer));
    return SSD1306_OK;
}


ssd1306_status_t
ssd1306_init(ssd1306_t *ssd1306_ptr, uint8_t i2c_channel, uint8_t i2c_addr) {
    // Resets the display structure.
    memset(ssd1306_ptr, 0, sizeof(ssd1306_t));

    ssd1306_ptr->i2c_channel = i2c_channel;
    ssd1306_ptr->i2c_addr    = i2c_addr;
    ssd1306_ptr->initialized = 1;

    // Soft-resets the display hardware.
    SSD1306_DECLARE_STATUS_VARIABLE()

    // Fundamental commands.
    status = ssd1306_display_off(ssd1306_ptr);
    if (status != SSD1306_OK) return status;

    status = ssd1306_set_inversion(ssd1306_ptr, false);
    if (status != SSD1306_OK) return status;

    status = ssd1306_set_contrast(ssd1306_ptr, 0xFF); // Max contrast.
    if (status != SSD1306_OK) return status;

    // Disables scrolling.
    status = ssd1306_set_scroll_status(ssd1306_ptr, false);
    if (status != SSD1306_OK) return status;

    const uint8_t cmd_list[] = {
            SSD1306_CMD_CONTROL_BYTE,
            SSD1306_CMD_RESUMETORAM,
            // Addressing settings (default is HAM).
            SSD1306_CMD_SET_MEMORY_ADDRESSING_MODE,
            SSD1306_SUBCMD_MEM_ADDR_MODE_HAM,
            SSD1306_CMD_SET_COLUMN_ADDRESS,
            0x00,                // Column start address is 0.
            0x7F,                // Column end address is 127.
            SSD1306_CMD_SET_PAGE_ADDRESS,
            0x00,                // Page start address is 0.
            0x07,                // Page end address is 7.
            // Hardware configuration.
            0x40,                // Display start line.
            SSD1306_CMD_SEGMENT_REMAP_COL127_SEG0,
            SSD1306_CMD_SET_MULTIPLEX_RATIO,
            0x3F,                // Value of multiplex ratio.
            SSD1306_CMD_COM_SCAN_DIRECTION_REMAPPED,
            SSD1306_CMD_SET_DISPLAY_OFFSET,
            0x00,                // No display offset.
            SSD1306_CMD_SET_COM_PINS_HW_CONFIG,
            0x12,                // Alt. config., disables COM left/right remap.
            // Timing and driving scheme.
            SSD1306_CMD_SET_DIS_CLK_OSC_FREQ,
            0x80,
            SSD1306_CMD_SET_PRECHARGE_PERIOD,
            0x22,                // Pre-charge period.
            SSD1306_CMD_SET_VCOMH_DESELECT_LEVEL,
            0x20                 // 0.77 x Vcc.
    };

    SSD1306_DECLARE_COMMAND_WRITE_MULTI(cmd_list)

    status = ssd1306_display_on(ssd1306_ptr);
    if (status != SSD1306_OK) return status;

    status = ssd1306_clear_display(ssd1306_ptr);
    if (status != SSD1306_OK) return status;

    return SSD1306_OK;
}
