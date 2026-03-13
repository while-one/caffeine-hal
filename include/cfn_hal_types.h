/**
 * Copyright (c) 2026 Hisham Moussa Daou <https://www.whileone.me>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @file cfn_hal_types.h
 * @brief Common type definitions and enums for the HAL.
 */

#ifndef CAFFEINE_HAL_HAL_TYPES_H
#define CAFFEINE_HAL_HAL_TYPES_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
/* Defines ----------------------------------------------------------*/
#define DRIVER_CONFIG_DEINIT false
#define DRIVER_CONFIG_INIT   true

/* Macro ------------------------------------------------------------*/
/* Types Enums ------------------------------------------------------*/
typedef enum error_codes
{
    CFN_HAL_ERROR_OK = 0x00,

    CFN_HAL_ERROR_FAIL = 0x01,
    CFN_HAL_ERROR_BAD_PARAM,
    CFN_HAL_ERROR_NOT_READY,
    CFN_HAL_ERROR_NOT_FOUND,
    CFN_HAL_ERROR_OUT_OF_BOUND,
    CFN_HAL_ERROR_NOT_SUPPORTED,
    CFN_HAL_ERROR_EXTERNAL,
    CFN_HAL_ERROR_BUSY,

    CFN_HAL_ERROR_PERIPHERAL_GPIO_FAIL = 0x100,
    CFN_HAL_ERROR_PERIPHERAL_UART_FAIL,
    CFN_HAL_ERROR_PERIPHERAL_I2C_FAIL,
    CFN_HAL_ERROR_PERIPHERAL_SPI_FAIL,
    CFN_HAL_ERROR_PERIPHERAL_ADC_FAIL,
    CFN_HAL_ERROR_PERIPHERAL_RTC_FAIL,
    CFN_HAL_ERROR_PERIPHERAL_TIMER_FAIL,

    CFN_HAL_ERROR_TIMING_TIMEOUT = 0x200,

    CFN_HAL_ERROR_DRIVER_GENERAL = 0x300,
    CFN_HAL_ERROR_DRIVER_NOT_INIT,
    CFN_HAL_ERROR_DRIVER_ALREADY_INIT,

    CFN_HAL_ERROR_DATA_CRC = 0x400,
    CFN_HAL_ERROR_DATA_BAD_FORMAT,
    CFN_HAL_ERROR_DATA_MISMATCH,

    CFN_HAL_ERROR_MEMORY_NULL = 0x500,
    CFN_HAL_ERROR_MEMORY_FULL,
    CFN_HAL_ERROR_MEMORY_ALLOC,
    CFN_HAL_ERROR_MEMORY_EMPTY,

    CFN_HAL_ERROR_UNKNOWN
} cfn_hal_error_code_t;

typedef uint32_t cfn_hal_peripheral_type_t;

// Macro to pack 4 characters into a 32-bit integer (FourCC)
#define CFN_HAL_MAKE_TYPE(a, b, c, d)                                                                                  \
    (((uint32_t) (a)) | ((uint32_t) (b) << 8) | ((uint32_t) (c) << 16) | ((uint32_t) (d) << 24))

#define CFN_HAL_PERIPHERAL_TYPE_UNKNOWN 0
#define CFN_HAL_PERIPHERAL_TYPE_ADC     CFN_HAL_MAKE_TYPE('A', 'D', 'C', ' ')
#define CFN_HAL_PERIPHERAL_TYPE_CAN     CFN_HAL_MAKE_TYPE('C', 'A', 'N', ' ')
#define CFN_HAL_PERIPHERAL_TYPE_CLOCK   CFN_HAL_MAKE_TYPE('C', 'L', 'K', ' ')
#define CFN_HAL_PERIPHERAL_TYPE_DAC     CFN_HAL_MAKE_TYPE('D', 'A', 'C', ' ')
#define CFN_HAL_PERIPHERAL_TYPE_DMA     CFN_HAL_MAKE_TYPE('D', 'M', 'A', ' ')
#define CFN_HAL_PERIPHERAL_TYPE_GPIO    CFN_HAL_MAKE_TYPE('G', 'P', 'I', 'O')
#define CFN_HAL_PERIPHERAL_TYPE_I2C     CFN_HAL_MAKE_TYPE('I', '2', 'C', ' ')
#define CFN_HAL_PERIPHERAL_TYPE_PWM     CFN_HAL_MAKE_TYPE('P', 'W', 'M', ' ')
#define CFN_HAL_PERIPHERAL_TYPE_RTC     CFN_HAL_MAKE_TYPE('R', 'T', 'C', ' ')
#define CFN_HAL_PERIPHERAL_TYPE_SPI     CFN_HAL_MAKE_TYPE('S', 'P', 'I', ' ')
#define CFN_HAL_PERIPHERAL_TYPE_TIMER   CFN_HAL_MAKE_TYPE('T', 'I', 'M', 'R')
#define CFN_HAL_PERIPHERAL_TYPE_UART    CFN_HAL_MAKE_TYPE('U', 'A', 'R', 'T')
#define CFN_HAL_PERIPHERAL_TYPE_WDT     CFN_HAL_MAKE_TYPE('W', 'D', 'T', ' ')
#define CFN_HAL_PERIPHERAL_TYPE_IRQ     CFN_HAL_MAKE_TYPE('I', 'R', 'Q', ' ')
#define CFN_HAL_PERIPHERAL_TYPE_NVM     CFN_HAL_MAKE_TYPE('N', 'V', 'M', ' ')
#define CFN_HAL_PERIPHERAL_TYPE_I2S     CFN_HAL_MAKE_TYPE('I', '2', 'S', ' ')
#define CFN_HAL_PERIPHERAL_TYPE_QSPI    CFN_HAL_MAKE_TYPE('Q', 'S', 'P', 'I')
#define CFN_HAL_PERIPHERAL_TYPE_CRYPTO  CFN_HAL_MAKE_TYPE('C', 'R', 'Y', 'P')
#define CFN_HAL_PERIPHERAL_TYPE_SDIO    CFN_HAL_MAKE_TYPE('S', 'D', 'I', 'O')
#define CFN_HAL_PERIPHERAL_TYPE_USB     CFN_HAL_MAKE_TYPE('U', 'S', 'B', ' ')
#define CFN_HAL_PERIPHERAL_TYPE_ETH     CFN_HAL_MAKE_TYPE('E', 'T', 'H', ' ')
#define CFN_HAL_PERIPHERAL_TYPE_COMP    CFN_HAL_MAKE_TYPE('C', 'O', 'M', 'P')

typedef enum
{
    CFN_HAL_DRIVER_STATUS_UNKNOWN = 0, /*!< Memory state is unknown or explicitly unconstructed. */
    CFN_HAL_DRIVER_STATUS_CONSTRUCTED, /*!< Driver is constructed (VMT linked) but hardware is NOT initialized. */
    CFN_HAL_DRIVER_STATUS_INITIALIZED, /*!< Hardware is initialized and ready for use. */
    CFN_HAL_DRIVER_STATUS_ERROR /*!< A fatal hardware error occurred; requires a _deinit/_init cycle to recover. */
} cfn_hal_driver_status_t;

/* Types Structs ----------------------------------------------------*/
typedef enum
{
    CFN_HAL_POWER_STATE_UNKNOWN = 0,
    CFN_HAL_POWER_STATE_ON,        /*!< Fully powered and clocked */
    CFN_HAL_POWER_STATE_LOW_POWER, /*!< Clock gated, retention mode, or
                                  suspended */
    CFN_HAL_POWER_STATE_OFF        /*!< Physically unpowered or fully disabled */
} cfn_hal_power_state_t;

typedef struct cfn_hal_driver_s
{
    cfn_hal_peripheral_type_t type;
    cfn_hal_driver_status_t   status;
    cfn_hal_power_state_t     power_state;

#if (CFN_HAL_USE_LOCK == 1)
    void *lock_obj;
    cfn_hal_error_code_t (*lock)(struct cfn_hal_driver_s *base, uint32_t timeout);
    cfn_hal_error_code_t (*unlock)(struct cfn_hal_driver_s *base);
#endif
    cfn_hal_error_code_t (*on_config)(struct cfn_hal_driver_s *base, void *user_arg, bool is_init);
    void       *on_config_arg;
    void       *dep;
    void       *ext;
    const void *vmt;
} cfn_hal_driver_t;
/* Functions prototypes ---------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif // CAFFEINE_HAL_HAL_TYPES_H
