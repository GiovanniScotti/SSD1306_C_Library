# SSD1306 OLED display C library
## Introduction
The SSD1306 OLED display C library is a flexible and portable driver
library designed to be deployed on possibly any microcontroller.
The library can drive 128x64 OLED displays based on SSD1306 controller.
The software internal architecture decouples the library code from the HAL
code (e.g. ST HAL) to manage i2c communication on any platform.
The library has been written to provide a robust api. Easy debugging
is guaranteed by function status codes that are percolated up to the caller.

The user can modify the `ssd1306_config.c` file to reshape the way in which
the library deals with the existing code that handles the i2c peripheral
on the selected microcontroller.

The driver can manage both multiple displays on the same i2c bus
(given the fact they have different addresses) and on different i2c
peripheral. See the embedded documentation in `ssd1306_config.c` file.

The library can be easily integrated in C++ code because it is
ready to avoid name mangling.

## API
Each function returns a status code as described by the `ssd1306_status_t`
enumeration reported down below. This allows the user to detect issues
without going crazy.

```C
typedef enum {
    SSD1306_OK           = 0, /*!< Display command success. */
    SSD1306_NOINIT       = 1, /*!< Display has not been initialized before use. */
    SSD1306_WRONG_PARAMS = 2, /*!< Invalid arguments. */
    SSD1306_COMM_ERROR   = 3, /*!< Error while communicating with the display. */
    SSD1306_STATUS_COUNT = 4  /*!< Total number of possible status. */
} ssd1306_status_t;
```

Basic functions are:
* display on/off
* set contrast/inversion/scrolling
* cursor positioning
* draw character/string
* clear display

Advanced graphics functions are:
* draw pixel
* draw line
* draw (filled) rectangle
* draw (filled) triangle
* draw (filled) circle
* draw bitmap

Please, refer to `ssd1306_driver.h`embedded documentation for more
information and details about arguments and returned values of each
implemented function.

## Library usage
The user is required to adapt the `ssd1306_config.c` file as needed,
declare a variable of type `ssd1306_t` and call the `ssd1306_init`
function to initialize the target display. That's it!

Each drawing function (identified by the keyword *_draw_*) must be
followed by an `ssd1306_update` call if you want your changes to appear
on the display. This allows to draw in the internal software buffer (cache)
many things and have them flushed to the hardware at the same time.

The library is shipped with just one font enabled (7x10) in order to
save code space in memory. There are other two ready-to-use fonts:
11x18 and 16x26. Uncomment the respective macros found in `ssd1306_font.c`
in order to enable them.

```C
// Macros to shrink the library.
#define ENABLE_FONT_11X18 /// Uncomment to enable 11x18 font.
#define ENABLE_FONT_16X26 /// Uncomment to enable 16x26 font.
```

## Usage examples
TODO
