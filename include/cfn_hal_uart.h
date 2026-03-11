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
 * @file cfn_hal_uart.h
 * @brief Universal Asynchronous Receiver-Transmitter (UART) HAL API.
 */

#ifndef CAFFEINE_HAL_HAL_UART_H
#define CAFFEINE_HAL_HAL_UART_H

#ifdef __cplusplus
extern "C"
{
#endif
/* Includes ---------------------------------------------------------*/
#include "cfn_hal_types.h"
#include "cfn_hal.h"
#include "cfn_hal_gpio.h"
/* Defines ----------------------------------------------------------*/
/* Macro ------------------------------------------------------------*/
/* Types Enums ------------------------------------------------------*/
typedef enum
{
    CFN_HAL_UART_INTERRUPT_TX_COMPLETE,
    CFN_HAL_UART_INTERRUPT_RX_READY,
    CFN_HAL_UART_INTERRUPT_BUS_IDLE,

    CFN_HAL_UART_INTERRUPT_ERROR_FRAMING,
    CFN_HAL_UART_INTERRUPT_ERROR_PARITY,
    CFN_HAL_UART_INTERRUPT_ERROR_OVERRUN,
    CFN_HAL_UART_INTERRUPT_ERROR_TIMEOUT,
} cfn_hal_uart_interrupts_t;

typedef enum
{
    CFN_HAL_UART_CFG_PARITY_NONE, /*!< No parity              */
    CFN_HAL_UART_CFG_PARITY_EVEN, /*!< Parity bit is even     */
    CFN_HAL_UART_CFG_PARITY_ODD,  /*!< Parity bit is odd      */
} cfn_hal_uart_cfg_parity_t;

typedef enum
{
    CFN_HAL_UART_CFG_STOP_ONE_BIT, /*!< One stop bit       */
    CFN_HAL_UART_CFG_STOP_TWO_BITS /*!< Two stop bits      */
} cfn_hal_uart_cfg_stop_bits_t;

typedef enum
{
    CFN_HAL_UART_CFG_DATA_LEN_5, /*!< Data length is 5 bits */
    CFN_HAL_UART_CFG_DATA_LEN_6, /*!< Data length is 6 bits */
    CFN_HAL_UART_CFG_DATA_LEN_7, /*!< Data length is 7 bits */
    CFN_HAL_UART_CFG_DATA_LEN_8, /*!< Data length is 8 bits */
    CFN_HAL_UART_CFG_DATA_LEN_9  /*!< Data length is 9 bits */
} cfn_hal_uart_cfg_data_len_t;

typedef enum
{
    CFN_HAL_UART_CFG_FLOW_CTRL_NONE,
    CFN_HAL_UART_CFG_FLOW_CTRL_CTS,
    CFN_HAL_UART_CFG_FLOW_CTRL_RTS,
    CFN_HAL_UART_CFG_FLOW_CTRL_RTS_CTS,
    CFN_HAL_UART_CFG_FLOW_CTRL_DTR,
    CFN_HAL_UART_CFG_FLOW_CTRL_DCD,
    CFN_HAL_UART_CFG_FLOW_CTRL_DE,
} cfn_hal_uart_cfg_flow_ctrl_t;

typedef enum
{
    CFN_HAL_UART_CFG_MODE_NONE,
    CFN_HAL_UART_CFG_MODE_BLOCKING,
    CFN_HAL_UART_CFG_MODE_INTERRUPT,
} cfn_hal_uart_cfg_mode_t;

/* Types Structs ----------------------------------------------------*/
typedef struct cfn_hal_uart_s     cfn_hal_uart_t;
typedef struct cfn_hal_uart_api_s cfn_hal_uart_api_t;

typedef struct
{
    void           *port;
    cfn_hal_gpio_t *tx;
    cfn_hal_gpio_t *rx;
    cfn_hal_gpio_t *cts;
    cfn_hal_gpio_t *rts;
    cfn_hal_gpio_t *dtr;
    cfn_hal_gpio_t *dcd;
    cfn_hal_gpio_t *de;
    void           *user_data;
} cfn_hal_uart_phy_t;

typedef struct
{
    bool                         echo;
    uint32_t                     read_timeout;
    uint32_t                     write_timeout;
    uint32_t                     baudrate;
    cfn_hal_uart_cfg_mode_t      read_mode;
    cfn_hal_uart_cfg_mode_t      write_mode;
    cfn_hal_uart_cfg_data_len_t  data_len;
    cfn_hal_uart_cfg_stop_bits_t stop_bits;
    cfn_hal_uart_cfg_parity_t    parity;
    cfn_hal_uart_cfg_flow_ctrl_t flow_ctrl;
    void                        *user_data;
} cfn_hal_uart_config_t;

typedef void (*cfn_hal_uart_callback_t)(
    cfn_hal_uart_t *driver, cfn_hal_uart_interrupts_t interrupt, uint8_t *data, size_t nbr_of_bytes, void *user_arg);

/**
 * @brief Virtual Method Table (VMT) for the peripheral.
 * Hardware-specific implementations must populate these function pointers.
 * Functions can be set to NULL if not implemented.
 */
struct cfn_hal_uart_api_s
{
    cfn_hal_error_code_t (*cfn_hal_uart_register_cb)(
        cfn_hal_uart_t *driver, cfn_hal_uart_callback_t cb, void *user_arg);
    cfn_hal_error_code_t (*cfn_hal_uart_set_cb_arg)(cfn_hal_uart_t *driver, void *user_arg);
    cfn_hal_error_code_t (*cfn_hal_uart_tx_irq)(cfn_hal_uart_t *driver, const uint8_t *data, size_t nbr_of_bytes);
    cfn_hal_error_code_t (*cfn_hal_uart_tx_irq_abort)(cfn_hal_uart_t *driver);
    cfn_hal_error_code_t (*cfn_hal_uart_rx_irq)(cfn_hal_uart_t *driver, uint8_t *data, size_t nbr_of_bytes);
    cfn_hal_error_code_t (*cfn_hal_uart_rx_irq_abort)(cfn_hal_uart_t *driver);
    cfn_hal_error_code_t (*cfn_hal_uart_tx_polling)(
        cfn_hal_uart_t *driver, const uint8_t *data, size_t nbr_of_bytes, uint32_t timeout);
    cfn_hal_error_code_t (*cfn_hal_uart_rx_polling)(
        cfn_hal_uart_t *driver, uint8_t *data, size_t nbr_of_bytes, uint32_t timeout);
    cfn_hal_error_code_t (*cfn_hal_uart_rx_to_idle)(
        cfn_hal_uart_t *driver, uint8_t *data, size_t max_bytes, size_t *received_bytes, uint32_t timeout);
    cfn_hal_error_code_t (*cfn_hal_uart_check_error)(cfn_hal_uart_t *driver);
    cfn_hal_error_code_t (*cfn_hal_uart_init)(cfn_hal_uart_t *driver);
    cfn_hal_error_code_t (*cfn_hal_uart_deinit)(cfn_hal_uart_t *driver);
    cfn_hal_error_code_t (*cfn_hal_uart_cfg_get)(cfn_hal_uart_t *driver, cfn_hal_uart_config_t *config);
    cfn_hal_error_code_t (*cfn_hal_uart_cfg_set)(cfn_hal_uart_t *driver, const cfn_hal_uart_config_t *config);
    cfn_hal_error_code_t (*cfn_hal_uart_cfg_tx_enable)(cfn_hal_uart_t *driver);
    cfn_hal_error_code_t (*cfn_hal_uart_cfg_rx_enable)(cfn_hal_uart_t *driver);
    cfn_hal_error_code_t (*cfn_hal_uart_cfg_irq_enable)(cfn_hal_uart_t *driver, cfn_hal_uart_interrupts_t irq);
    cfn_hal_error_code_t (*cfn_hal_uart_cfg_tx_disable)(cfn_hal_uart_t *driver);
    cfn_hal_error_code_t (*cfn_hal_uart_cfg_rx_disable)(cfn_hal_uart_t *driver);
    cfn_hal_error_code_t (*cfn_hal_uart_cfg_irq_disable)(cfn_hal_uart_t *driver, cfn_hal_uart_interrupts_t irq);
    cfn_hal_error_code_t (*cfn_hal_uart_tx_dma)(cfn_hal_uart_t *driver, const uint8_t *data, size_t nbr_of_bytes);
    cfn_hal_error_code_t (*cfn_hal_uart_rx_dma)(cfn_hal_uart_t *driver, uint8_t *data, size_t nbr_of_bytes);
};

/**
 * @brief Generated driver structure for uart.
 *
 * This macro expands to the following structure:
 * \code{c}
 * struct cfn_hal_uart_s {
 *     cfn_hal_driver_t       base;
 *     const cfn_hal_uart_config_t *config;
 *     const cfn_hal_uart_api_t    *api;
 *     const cfn_hal_uart_phy_t    *phy;
 *     cfn_hal_uart_callback_t            cb;
 *     void              *cb_user_arg;
 * };
 * typedef struct cfn_hal_uart_s cfn_hal_uart_t;
 * \endcode
 */
CFN_HAL_CREATE_DRIVER_TYPE(
    uart, cfn_hal_uart_config_t, cfn_hal_uart_api_t, cfn_hal_uart_phy_t, cfn_hal_uart_callback_t);
/* Functions inline  ---------------------------------------------*/

/**
 * @brief cfn_hal_uart_register_cb implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param cb Callback function to register.
 * @param user_arg User-defined argument passed to the callback.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t
cfn_hal_uart_register_cb(cfn_hal_uart_t *driver, cfn_hal_uart_callback_t cb, void *user_arg)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(
        CFN_HAL_PERIPHERAL_TYPE_UART, cfn_hal_uart_register_cb, driver, error, cb, user_arg);
    return error;
}

/**
 * @brief cfn_hal_uart_set_cb_arg implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param user_arg User-defined argument passed to the callback.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_uart_set_cb_arg(cfn_hal_uart_t *driver, void *user_arg)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_UART, cfn_hal_uart_set_cb_arg, driver, error, user_arg);
    return error;
}

/**
 * @brief cfn_hal_uart_tx_irq implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param data Pointer to the data buffer.
 * @param nbr_of_bytes Number of bytes to transfer.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_uart_tx_irq(cfn_hal_uart_t *driver, const uint8_t *data, size_t nbr_of_bytes)
{
    cfn_hal_error_code_t error = CFN_HAL_LOCK(driver, CFN_HAL_MAX_DELAY);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(
        CFN_HAL_PERIPHERAL_TYPE_UART, cfn_hal_uart_tx_irq, driver, error, data, nbr_of_bytes);
    CFN_HAL_UNLOCK(driver);
    return error;
}

/**
 * @brief cfn_hal_uart_tx_irq_abort implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param data Pointer to the data buffer.
 * @param nbr_of_bytes Number of bytes to transfer.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_uart_tx_irq_abort(cfn_hal_uart_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_LOCK(driver, CFN_HAL_MAX_DELAY);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_UART, cfn_hal_uart_tx_irq_abort, driver, error);
    CFN_HAL_UNLOCK(driver);
    return error;
}

/**
 * @brief cfn_hal_uart_rx_irq implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param data Pointer to the data buffer.
 * @param nbr_of_bytes Number of bytes to transfer.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_uart_rx_irq(cfn_hal_uart_t *driver, uint8_t *data, size_t nbr_of_bytes)
{
    cfn_hal_error_code_t error = CFN_HAL_LOCK(driver, CFN_HAL_MAX_DELAY);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(
        CFN_HAL_PERIPHERAL_TYPE_UART, cfn_hal_uart_rx_irq, driver, error, data, nbr_of_bytes);
    CFN_HAL_UNLOCK(driver);
    return error;
}

/**
 * @brief cfn_hal_uart_rx_irq_abort implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param data Pointer to the data buffer.
 * @param nbr_of_bytes Number of bytes to transfer.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_uart_rx_irq_abort(cfn_hal_uart_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_LOCK(driver, CFN_HAL_MAX_DELAY);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_UART, cfn_hal_uart_rx_irq_abort, driver, error);
    CFN_HAL_UNLOCK(driver);
    return error;
}

/**
 * @brief cfn_hal_uart_tx_polling implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param data Pointer to the data buffer.
 * @param nbr_of_bytes Number of bytes to transfer.
 * @param timeout Timeout duration in milliseconds.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t
cfn_hal_uart_tx_polling(cfn_hal_uart_t *driver, const uint8_t *data, size_t nbr_of_bytes, uint32_t timeout)
{
    cfn_hal_error_code_t error = CFN_HAL_LOCK(driver, CFN_HAL_MAX_DELAY);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(
        CFN_HAL_PERIPHERAL_TYPE_UART, cfn_hal_uart_tx_polling, driver, error, data, nbr_of_bytes, timeout);
    CFN_HAL_UNLOCK(driver);
    return error;
}

/**
 * @brief cfn_hal_uart_rx_polling implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param data Pointer to the data buffer.
 * @param nbr_of_bytes Number of bytes to transfer.
 * @param timeout Timeout duration in milliseconds.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t
cfn_hal_uart_rx_polling(cfn_hal_uart_t *driver, uint8_t *data, size_t nbr_of_bytes, uint32_t timeout)
{
    cfn_hal_error_code_t error = CFN_HAL_LOCK(driver, CFN_HAL_MAX_DELAY);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(
        CFN_HAL_PERIPHERAL_TYPE_UART, cfn_hal_uart_rx_polling, driver, error, data, nbr_of_bytes, timeout);
    CFN_HAL_UNLOCK(driver);
    return error;
}

/**
 * @brief cfn_hal_uart_rx_to_idle implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param data Pointer to the data buffer.
 * @param max_bytes Maximum number of bytes to transfer.
 * @param received_bytes Pointer to store the number of bytes actually
 * received.
 * @param timeout Timeout duration in milliseconds.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_uart_rx_to_idle(
    cfn_hal_uart_t *driver, uint8_t *data, size_t max_bytes, size_t *received_bytes, uint32_t timeout)
{
    cfn_hal_error_code_t error = CFN_HAL_LOCK(driver, CFN_HAL_MAX_DELAY);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(
        CFN_HAL_PERIPHERAL_TYPE_UART, cfn_hal_uart_rx_to_idle, driver, error, data, max_bytes, received_bytes, timeout);
    CFN_HAL_UNLOCK(driver);
    return error;
}

/**
 * @brief cfn_hal_uart_check_error implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_uart_check_error(cfn_hal_uart_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_UART, cfn_hal_uart_check_error, driver, error);
    return error;
}

/**
 * @brief cfn_hal_uart_init implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_uart_init(cfn_hal_uart_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    if (driver && driver->base.on_config)
    {
        error = driver->base.on_config(&driver->base, DRIVER_CONFIG_INIT);
        if (error != CFN_HAL_ERROR_OK)
        {
            return error;
        }
    }
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_UART, cfn_hal_uart_init, driver, error);
    if (error == CFN_HAL_ERROR_OK && driver)
    {
        driver->base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    }
    return error;
}

/**
 * @brief cfn_hal_uart_cfg_get implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param config Pointer to the peripheral configuration structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_uart_cfg_get(cfn_hal_uart_t *driver, cfn_hal_uart_config_t *config)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_UART, cfn_hal_uart_cfg_get, driver, error, config);
    return error;
}

/**
 * @brief cfn_hal_uart_cfg_set implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param config Pointer to the peripheral configuration structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_uart_cfg_set(cfn_hal_uart_t *driver, const cfn_hal_uart_config_t *config)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_UART, cfn_hal_uart_cfg_set, driver, error, config);
    return error;
}

/**
 * @brief cfn_hal_uart_cfg_tx_enable implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_uart_cfg_tx_enable(cfn_hal_uart_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_LOCK(driver, CFN_HAL_MAX_DELAY);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_UART, cfn_hal_uart_cfg_tx_enable, driver, error);
    CFN_HAL_UNLOCK(driver);
    return error;
}

/**
 * @brief cfn_hal_uart_cfg_rx_enable implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_uart_cfg_rx_enable(cfn_hal_uart_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_LOCK(driver, CFN_HAL_MAX_DELAY);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_UART, cfn_hal_uart_cfg_rx_enable, driver, error);
    CFN_HAL_UNLOCK(driver);
    return error;
}

/**
 * @brief cfn_hal_uart_cfg_irq_enable implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param irq Interrupt type or event identifier.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_uart_cfg_irq_enable(cfn_hal_uart_t *driver, cfn_hal_uart_interrupts_t irq)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_UART, cfn_hal_uart_cfg_irq_enable, driver, error, irq);
    return error;
}

/**
 * @brief cfn_hal_uart_cfg_tx_disable implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_uart_cfg_tx_disable(cfn_hal_uart_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_LOCK(driver, CFN_HAL_MAX_DELAY);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_UART, cfn_hal_uart_cfg_tx_disable, driver, error);
    CFN_HAL_UNLOCK(driver);
    return error;
}

/**
 * @brief cfn_hal_uart_cfg_rx_disable implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_uart_cfg_rx_disable(cfn_hal_uart_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_LOCK(driver, CFN_HAL_MAX_DELAY);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_UART, cfn_hal_uart_cfg_rx_disable, driver, error);
    CFN_HAL_UNLOCK(driver);
    return error;
}

/**
 * @brief cfn_hal_uart_cfg_irq_disable implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param irq Interrupt type or event identifier.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_uart_cfg_irq_disable(cfn_hal_uart_t *driver, cfn_hal_uart_interrupts_t irq)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_UART, cfn_hal_uart_cfg_irq_disable, driver, error, irq);
    return error;
}

/**
 * @brief cfn_hal_uart_tx_dma implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param data Pointer to the data buffer.
 * @param nbr_of_bytes Number of bytes to transfer.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_uart_tx_dma(cfn_hal_uart_t *driver, const uint8_t *data, size_t nbr_of_bytes)
{
    cfn_hal_error_code_t error = CFN_HAL_LOCK(driver, CFN_HAL_MAX_DELAY);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(
        CFN_HAL_PERIPHERAL_TYPE_UART, cfn_hal_uart_tx_dma, driver, error, data, nbr_of_bytes);
    CFN_HAL_UNLOCK(driver);
    return error;
}

/**
 * @brief cfn_hal_uart_rx_dma implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param data Pointer to the data buffer.
 * @param nbr_of_bytes Number of bytes to transfer.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_uart_rx_dma(cfn_hal_uart_t *driver, uint8_t *data, size_t nbr_of_bytes)
{
    cfn_hal_error_code_t error = CFN_HAL_LOCK(driver, CFN_HAL_MAX_DELAY);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(
        CFN_HAL_PERIPHERAL_TYPE_UART, cfn_hal_uart_rx_dma, driver, error, data, nbr_of_bytes);
    CFN_HAL_UNLOCK(driver);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif // CAFFEINE_HAL_HAL_UART_H
