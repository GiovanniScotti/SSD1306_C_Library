/**
 * @file   ssd1306_config.c
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


///////////////////////////////////////////////////////////
// SSD1306 OLED display C library CONFIGURATION FILE!
//
// This file is supposed to be modified by the user to port
// the code on (possibly) any microcontroller.
// The following is an example for Arduino platform.


#include <arduino.h>
#include <Wire.h>

void i2c_write_arduino(uint8_t addr, const uint8_t *data_ptr, size_t data_size) {

    int32_t proc_bytes = 1;

    while(proc_bytes < data_size){
        uint8_t chunk_size =
            ((data_size - proc_bytes) > 31) ? 31 : data_size - proc_bytes;

        // WARNING: the Wire library has a maximum buffer size of 32 bytes.
        // It means that the user cannot write more than 32 bytes in a row.
        Wire.begin();
        Wire.beginTransmission(addr);
        // We need to transmit the command/data specifier before
        // sending a sequence of bytes.
        Wire.write(data_ptr[0]);
        Wire.write(data_ptr + proc_bytes, chunk_size);
        Wire.endTransmission();
        Wire.end();

        proc_bytes += chunk_size;
    }
}


extern "C" {

///////////////////////////////////////////////////////////
// The following function must be implemented as C code
// otherwise it cannot be called by the library code.
// i2c_write_arduino is a C++ function that is able to
// use the Wire library from Arduino to manage the i2c
// peripheral.

#include "ssd1306_driver.h"

ssd1306_status_t
ssd1306_i2c_write(uint8_t channel, uint8_t addr,
        const uint8_t *data_ptr, size_t data_size) {

    // Platform-dependent i2c write function implementation.
    // The channel allows to route the write request to the appropriate
    // i2c peripheral. The address allows to reach the desired slave.
    switch (channel) {
        case 0:
            i2c_write_arduino(addr, data_ptr, data_size);
            break;
        //case 1:
            // Some other i2c peripheral hooked to another display.
            //break;
        // ...
        default:
            return SSD1306_WRONG_PARAMS;
    }

    return SSD1306_OK;
}

} // End of extern "C".