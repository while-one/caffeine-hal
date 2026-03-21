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
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
/* Defines ----------------------------------------------------------*/

/* Macro ------------------------------------------------------------*/
/* Types Enums ------------------------------------------------------*/
typedef enum
{
    CFN_HAL_CONFIG_PHASE_DEINIT = 0,
    CFN_HAL_CONFIG_PHASE_INIT
} cfn_hal_config_phase_t;

typedef enum cfn_hal_error_codes
{
    CFN_HAL_ERROR_OK   = 0x00,

    CFN_HAL_ERROR_FAIL = 0x01,
    CFN_HAL_ERROR_BAD_PARAM,
    CFN_HAL_ERROR_NOT_READY,
    CFN_HAL_ERROR_NOT_FOUND,
    CFN_HAL_ERROR_OUT_OF_BOUND,
    CFN_HAL_ERROR_NOT_SUPPORTED,
    CFN_HAL_ERROR_EXTERNAL,
    CFN_HAL_ERROR_BUSY,
    CFN_HAL_ERROR_BAD_CONFIG,

    CFN_HAL_ERROR_PERIPHERAL_FAIL = 0x100, /*!< Generic peripheral hardware failure. Use peripheral-specific
                                              error masks for details. */

    CFN_HAL_ERROR_TIMING_TIMEOUT  = 0x200,

    CFN_HAL_ERROR_DRIVER_GENERAL  = 0x300,
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

struct cfn_hal_api_base_s;

/**
 * @brief Generic function pointer for HAL callbacks.
 * Used as a standard-compliant carrier in the base layer.
 *
 * @warning Calling this pointer directly as a void(void) function is UNDEFINED
 * BEHAVIOR if the original callback has a different signature. Hardware port
 * implementers MUST cast this pointer back to the peripheral-specific callback
 * type (e.g., cfn_hal_uart_callback_t) before execution.
 */
typedef void (*cfn_hal_callback_t)(void);

// Macro to pack 4 characters into a 32-bit integer (FourCC)
// Note: This packs in Little-Endian order (a is LSB), ensuring
// debug readability in memory dumps on Little-Endian systems (e.g., ARM).
#define CFN_HAL_MAKE_TYPE(a, b, c, d)                                                                                  \
    (((uint32_t) (a)) | ((uint32_t) (b) << 8) | ((uint32_t) (c) << 16) | ((uint32_t) (d) << 24))

/**
 * @brief Prefix for all HAL-level peripheral types.
 * Reserving 'A' for the base HAL layer to avoid collisions with other layers.
 */
#define CFN_HAL_PERIPHERAL_PREFIX 'A'

/**
 * @brief Helper macro to create a HAL-level peripheral FourCC.
 * Packs the HAL prefix and 3 characters into a 32-bit integer.
 */
#define CFN_HAL_PERIPHERAL_TYPE(a, b, c) CFN_HAL_MAKE_TYPE(CFN_HAL_PERIPHERAL_PREFIX, a, b, c)

#define CFN_HAL_PERIPHERAL_TYPE_UNKNOWN 0
#define CFN_HAL_PERIPHERAL_TYPE_ADC     CFN_HAL_PERIPHERAL_TYPE('A', 'D', 'C')
#define CFN_HAL_PERIPHERAL_TYPE_CAN     CFN_HAL_PERIPHERAL_TYPE('C', 'A', 'N')
#define CFN_HAL_PERIPHERAL_TYPE_CLOCK   CFN_HAL_PERIPHERAL_TYPE('C', 'L', 'K')
#define CFN_HAL_PERIPHERAL_TYPE_DAC     CFN_HAL_PERIPHERAL_TYPE('D', 'A', 'C')
#define CFN_HAL_PERIPHERAL_TYPE_DMA     CFN_HAL_PERIPHERAL_TYPE('D', 'M', 'A')
#define CFN_HAL_PERIPHERAL_TYPE_GPIO    CFN_HAL_PERIPHERAL_TYPE('G', 'P', 'O')
#define CFN_HAL_PERIPHERAL_TYPE_I2C     CFN_HAL_PERIPHERAL_TYPE('I', '2', 'C')
#define CFN_HAL_PERIPHERAL_TYPE_PWM     CFN_HAL_PERIPHERAL_TYPE('P', 'W', 'M')
#define CFN_HAL_PERIPHERAL_TYPE_RTC     CFN_HAL_PERIPHERAL_TYPE('R', 'T', 'C')
#define CFN_HAL_PERIPHERAL_TYPE_SPI     CFN_HAL_PERIPHERAL_TYPE('S', 'P', 'I')
#define CFN_HAL_PERIPHERAL_TYPE_TIMER   CFN_HAL_PERIPHERAL_TYPE('T', 'I', 'M')
#define CFN_HAL_PERIPHERAL_TYPE_UART    CFN_HAL_PERIPHERAL_TYPE('U', 'R', 'T')
#define CFN_HAL_PERIPHERAL_TYPE_WDT     CFN_HAL_PERIPHERAL_TYPE('W', 'D', 'T')
#define CFN_HAL_PERIPHERAL_TYPE_IRQ     CFN_HAL_PERIPHERAL_TYPE('I', 'R', 'Q')
#define CFN_HAL_PERIPHERAL_TYPE_NVM     CFN_HAL_PERIPHERAL_TYPE('N', 'V', 'M')
#define CFN_HAL_PERIPHERAL_TYPE_I2S     CFN_HAL_PERIPHERAL_TYPE('I', '2', 'S')
#define CFN_HAL_PERIPHERAL_TYPE_QSPI    CFN_HAL_PERIPHERAL_TYPE('Q', 'S', 'P')
#define CFN_HAL_PERIPHERAL_TYPE_CRYPTO  CFN_HAL_PERIPHERAL_TYPE('C', 'R', 'Y')
#define CFN_HAL_PERIPHERAL_TYPE_SDIO    CFN_HAL_PERIPHERAL_TYPE('S', 'D', 'I')
#define CFN_HAL_PERIPHERAL_TYPE_USB     CFN_HAL_PERIPHERAL_TYPE('U', 'S', 'B')
#define CFN_HAL_PERIPHERAL_TYPE_ETH     CFN_HAL_PERIPHERAL_TYPE('E', 'T', 'H')
#define CFN_HAL_PERIPHERAL_TYPE_COMP    CFN_HAL_PERIPHERAL_TYPE('C', 'M', 'P')

typedef enum
{
    CFN_HAL_DRIVER_STATUS_UNKNOWN = 0, /*!< Memory state is unknown or explicitly unconstructed. */
    CFN_HAL_DRIVER_STATUS_CONSTRUCTED, /*!< Driver is constructed (VMT linked) but
                                          hardware is NOT initialized. */
    CFN_HAL_DRIVER_STATUS_INITIALIZED, /*!< Hardware is initialized and ready for
                                          use. */
    CFN_HAL_DRIVER_STATUS_ERROR        /*!< A fatal hardware error occurred; requires a
                                          _deinit/_init cycle to recover. */
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

struct cfn_hal_clock_s;

/**
 * @brief Base structure for all peripheral drivers.
 * Contains common state and polymorphic interface linkage.
 */
typedef struct cfn_hal_driver_s
{
    cfn_hal_peripheral_type_t type;        /*!< FourCC peripheral type code (e.g. 'UART') */
    cfn_hal_driver_status_t   status;      /*!< Current software state of the driver */
    cfn_hal_power_state_t     power_state; /*!< Current power/clocking state of the hardware */

    struct cfn_hal_clock_s *clock_driver; /*!< Pointer to the clock driver instance */

#if (CFN_HAL_USE_LOCK == 1)
    void *lock_obj; /*!< Opaque pointer to an RTOS mutex or critical section
                       object */
#endif

    /**
     * @brief Board-level configuration hook (BSP).
     * Called by the generic HAL to handle hardware-specific setup like pin
     * muxing.
     */
    cfn_hal_error_code_t (*on_config)(struct cfn_hal_driver_s *base, void *user_arg, cfn_hal_config_phase_t phase);

    void *on_config_arg; /*!< User argument passed to the on_config hook */

    void *dependency; /*!< Pointer to an optional peripheral dependency (e.g., DMA
                         handle or parent bus) */
    void *extension;  /*!< Pointer to an optional driver extension or private
                         implementation state */

    uint32_t                         flags; /*!< Generic generic state tracking flags for the peripheral */
    uint32_t                         peripheral_id;
    const struct cfn_hal_api_base_s *vmt; /*!< Pointer to the peripheral-specific Virtual Method Table (API) */
} cfn_hal_driver_t;
/* Functions prototypes ---------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif // CAFFEINE_HAL_HAL_TYPES_H
