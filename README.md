# SSD1306 OLED display C library
## Introduction
The SSD1306 OLED display C library is a flexible and portable driver
library designed to be deployed on possibly any microcontroller.
The library can drive 128x64 OLED displays based on the SSD1306 controller
through i2c communication and it is written in pure C.

## Library Features
- Portable on any microcontroller architecture.

- Can manage both multiple displays on the same i2c bus (if they have
different addresses) and on different i2c peripheral.

- The software architecture decouples the library code from the HAL
code (e.g. ST HAL) to manage i2c communication on basically any platform.
The user is required to write a small piece of code as an adaptation layer.

- The library has been written to provide a robust api. Easy debugging
is guaranteed by function status codes that are percolated up to the caller.

- C++ readyness is guaranteed by name mangling avoidance.

## Library API
Each function returns a status code as described by the `ssd1306_status_t`
enumeration reported down below. This allows the user to detect issues
without going crazy.

```C
typedef enum {
    SSD1306_OK           = 0, /*!< Display command success. */
    SSD1306_NOINIT       = 1, /*!< Display has not been initialized before use. */
    SSD1306_WRONG_PARAMS = 2, /*!< Invalid arguments. */
    SSD1306_COMM_ERROR   = 3, /*!< Error while communicating with the display. */
    SSD1306_STATUS_COUNT = 4  /*!< Total number of possible states. */
} ssd1306_status_t;
```

Please, refer to `lib/src/ssd1306_driver.h` embedded documentation for more
information and details about arguments and returned values of each
implemented function. The following is a summarized list of the provided
functionalities.

### Basic functionalities
* display on/off
* set contrast/inversion/scrolling
* cursor positioning
* draw single pixel
* clear display

### Advanced functionalities
* draw character/string
* draw line
* draw (filled) rectangle
* draw (filled) triangle
* draw (filled) circle
* draw bitmap

## Library usage
You must copy into your project all the files found in the `lib` folder.
Then, you can pick one of the `ssd1306_config.c` files found in the `config`
folder or provide your own version depending on the hardware platform.

You are required to adapt the `ssd1306_config.c` file as needed,
include the `lib/inc/ssd1306_driver.h` file in your code,
declare a variable of type `ssd1306_t` and call the `ssd1306_init`
function to initialize the target display. That's it!

Each drawing function (identified by the keyword \*_draw_\*) must be
followed by an `ssd1306_update` call if you want your changes to appear
on the display. This allows to draw in the internal software buffer (cache)
many things and have them flushed to the hardware at the same time.

The library is shipped with just one font enabled (7x10) in order to
reduce the memory footprint. There are two other ready-to-use fonts:
11x18 and 16x26. Uncomment the respective macros found in `ssd1306_font.c`
in order to enable them.

```C
// Macros to shrink the library.
#define ENABLE_FONT_11X18 /// Uncomment to enable 11x18 font.
#define ENABLE_FONT_16X26 /// Uncomment to enable 16x26 font.
```

## Compatibility
| Microcontroller | Works              | Partially works        | Notes                     |
| --------------- | :----------------: | :--------------------: | ------------------------- |
| STM32           | :white_check_mark: |                        |                           |
| ATMega328       |                    | :white_check_mark:     | All fonts disabled due to high memory footprint.
| ATMega2560      | :white_check_mark: |                        |                           |
| ESP8266         | :white_check_mark: |                        |                           |
