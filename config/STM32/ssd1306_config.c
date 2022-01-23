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

/* C++ detection */
#ifdef __cplusplus
    extern "C" {
#endif

///////////////////////////////////////////////////////////
// SSD1306 OLED display C library CONFIGURATION FILE!
//
// This file is supposed to be modified by the user to port
// the code on (possibly) any microcontroller.
// The following is an example for STM32F103 platform.


#include "ssd1306_driver.h"


// This import depends on the selected microcontroller.
// In this example we use ST hardware abstraction layer.
#include "stm32f1xx_hal.h"


// i2c peripherals instantiated in the main.c file.
// Needed to use the i2c API provided by ST HAL.
extern I2C_HandleTypeDef hi2c1;


/**
 * This is the core function of this configuration file.
 * It allows to decouple the whole driver from the underlying
 * hardware platform. Its implementation is strongly hardware-
 * dependent.
 *
 * @param  channel
 * @param  addr
 * @param  data_ptr
 * @param  data_size
 * @return the outcome of the function call.
 */
ssd1306_status_t
ssd1306_i2c_write(uint8_t channel, uint8_t addr,
        const uint8_t *data_ptr, size_t data_size) {

    HAL_StatusTypeDef status;

    // Platform-dependent i2c write function implementation.
    // The channel allows to route the write request to the appropriate
    // i2c peripheral. The address allows to reach the desired slave.
    switch (channel) {
        case 0:
            status = HAL_I2C_Master_Transmit(&hi2c1, addr, data_ptr, data_size, 1000);
            if (status != HAL_OK) return SSD1306_COMM_ERROR;
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

/* C++ detection */
#ifdef __cplusplus
    }
#endif
