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
 * @file hal_types.h
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
#define HAL_MAX_DELAY UINT32_MAX

typedef enum error_codes
{
    HAL_ERROR_OK = 0x00,

    HAL_ERROR_FAIL = 0x01,
    HAL_ERROR_BAD_PARAM,
    HAL_ERROR_NOT_READY,
    HAL_ERROR_NOT_FOUND,
    HAL_ERROR_OUT_OF_BOUND,
    HAL_ERROR_NOT_SUPPORTED,
    HAL_ERROR_EXTERNAL,

    HAL_ERROR_PERIPHERAL_GPIO_FAIL = 0x100,
    HAL_ERROR_PERIPHERAL_UART_FAIL,
    HAL_ERROR_PERIPHERAL_I2C_FAIL,
    HAL_ERROR_PERIPHERAL_SPI_FAIL,
    HAL_ERROR_PERIPHERAL_ADC_FAIL,
    HAL_ERROR_PERIPHERAL_RTC_FAIL,
    HAL_ERROR_PERIPHERAL_TIMER_FAIL,

    HAL_ERROR_TIMING_TIMEOUT = 0x200,

    HAL_ERROR_DRIVER_GENERAL = 0x300,
    HAL_ERROR_DRIVER_BUSY,
    HAL_ERROR_DRIVER_NOT_INIT,
    HAL_ERROR_DRIVER_ALREADY_INIT,

    HAL_ERROR_DATA_CRC = 0x400,
    HAL_ERROR_DATA_BAD_FORMAT,
    HAL_ERROR_DATA_MISMATCH,

    HAL_ERROR_MEMORY_NULL = 0x500,
    HAL_ERROR_MEMORY_FULL,
    HAL_ERROR_MEMORY_ALLOC,
    HAL_ERROR_MEMORY_EMPTY,

    HAL_ERROR_UNKNOWN
} hal_error_code_t;

typedef uint32_t hal_peripheral_type_t;

// Macro to pack 4 characters into a 32-bit integer (FourCC)
#define HAL_MAKE_TYPE(a, b, c, d)                                                                                      \
    (((uint32_t) (a)) | ((uint32_t) (b) << 8) | ((uint32_t) (c) << 16) | ((uint32_t) (d) << 24))

#define HAL_PERIPHERAL_TYPE_UNKNOWN 0
#define HAL_PERIPHERAL_TYPE_ADC     HAL_MAKE_TYPE ('A', 'D', 'C', ' ')
#define HAL_PERIPHERAL_TYPE_CAN     HAL_MAKE_TYPE ('C', 'A', 'N', ' ')
#define HAL_PERIPHERAL_TYPE_CLOCK   HAL_MAKE_TYPE ('C', 'L', 'K', ' ')
#define HAL_PERIPHERAL_TYPE_DAC     HAL_MAKE_TYPE ('D', 'A', 'C', ' ')
#define HAL_PERIPHERAL_TYPE_DMA     HAL_MAKE_TYPE ('D', 'M', 'A', ' ')
#define HAL_PERIPHERAL_TYPE_GPIO    HAL_MAKE_TYPE ('G', 'P', 'I', 'O')
#define HAL_PERIPHERAL_TYPE_I2C     HAL_MAKE_TYPE ('I', '2', 'C', ' ')
#define HAL_PERIPHERAL_TYPE_PWM     HAL_MAKE_TYPE ('P', 'W', 'M', ' ')
#define HAL_PERIPHERAL_TYPE_RTC     HAL_MAKE_TYPE ('R', 'T', 'C', ' ')
#define HAL_PERIPHERAL_TYPE_SPI     HAL_MAKE_TYPE ('S', 'P', 'I', ' ')
#define HAL_PERIPHERAL_TYPE_TIMER   HAL_MAKE_TYPE ('T', 'I', 'M', 'R')
#define HAL_PERIPHERAL_TYPE_UART    HAL_MAKE_TYPE ('U', 'A', 'R', 'T')
#define HAL_PERIPHERAL_TYPE_WDT     HAL_MAKE_TYPE ('W', 'D', 'T', ' ')
#define HAL_PERIPHERAL_TYPE_IRQ     HAL_MAKE_TYPE ('I', 'R', 'Q', ' ')
#define HAL_PERIPHERAL_TYPE_NVM     HAL_MAKE_TYPE ('N', 'V', 'M', ' ')
#define HAL_PERIPHERAL_TYPE_I2S     HAL_MAKE_TYPE ('I', '2', 'S', ' ')
#define HAL_PERIPHERAL_TYPE_QSPI    HAL_MAKE_TYPE ('Q', 'S', 'P', 'I')
#define HAL_PERIPHERAL_TYPE_CRYPTO  HAL_MAKE_TYPE ('C', 'R', 'Y', 'P')
#define HAL_PERIPHERAL_TYPE_SDIO    HAL_MAKE_TYPE ('S', 'D', 'I', 'O')
#define HAL_PERIPHERAL_TYPE_USB     HAL_MAKE_TYPE ('U', 'S', 'B', ' ')
#define HAL_PERIPHERAL_TYPE_ETH     HAL_MAKE_TYPE ('E', 'T', 'H', ' ')
#define HAL_PERIPHERAL_TYPE_COMP    HAL_MAKE_TYPE ('C', 'O', 'M', 'P')

typedef enum
{
    HAL_PERIPHERAL_STATUS_UNKNOWN = 0,
    HAL_PERIPHERAL_STATUS_READY = 11,
    HAL_PERIPHERAL_STATUS_BUSY = 22,
    HAL_PERIPHERAL_STATUS_BUSY_TX = 33,
    HAL_PERIPHERAL_STATUS_BUSY_RX = 44,
    HAL_PERIPHERAL_STATUS_ERROR = 55,
    HAL_PERIPHERAL_STATUS_NOT_INIT = 66
} hal_driver_status_t;

/* Types Structs ----------------------------------------------------*/
typedef enum
{
    HAL_POWER_STATE_UNKNOWN = 0,
    HAL_POWER_STATE_ON,        /*!< Fully powered and clocked */
    HAL_POWER_STATE_LOW_POWER, /*!< Clock gated, retention mode, or
                                  suspended */
    HAL_POWER_STATE_OFF        /*!< Physically unpowered or fully disabled */
} hal_power_state_t;

/* Functions prototypes ---------------------------------------------*/
typedef struct hal_driver_s
{
    hal_peripheral_type_t type;
    hal_driver_status_t   status;

    hal_power_state_t power_state;
    hal_error_code_t (*set_power_state) (struct hal_driver_s *base, hal_power_state_t state);

    /* Concurrency / RTOS Management */
    void *lock_obj;
    hal_error_code_t (*lock) (struct hal_driver_s *base, uint32_t timeout);
    hal_error_code_t (*unlock) (struct hal_driver_s *base);

    hal_error_code_t (*on_config) (struct hal_driver_s *base, bool is_init);
    void *dep;
    void *_ext;
} hal_driver_t;

#ifdef __cplusplus
}
#endif

#endif // CAFFEINE_HAL_HAL_TYPES_H
