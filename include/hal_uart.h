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
 * @file hal_uart.h
 * @brief Universal Asynchronous Receiver-Transmitter (UART) HAL API.
 */

#ifndef CAFFEINE_HAL_HAL_UART_H
#define CAFFEINE_HAL_HAL_UART_H

#ifdef __cplusplus
extern "C"
{
#endif
/* Includes ---------------------------------------------------------*/
#include "hal_types.h"
#include "hal.h"
#include "hal_gpio.h"
/* Defines ----------------------------------------------------------*/
/* Macro ------------------------------------------------------------*/
/* Types Enums ------------------------------------------------------*/
typedef enum
{
    HAL_UART_INTERRUPT_TX_COMPLETE,
    HAL_UART_INTERRUPT_RX_READY,
    HAL_UART_INTERRUPT_BUS_IDLE,

    HAL_UART_INTERRUPT_ERROR_FRAMING,
    HAL_UART_INTERRUPT_ERROR_PARITY,
    HAL_UART_INTERRUPT_ERROR_OVERRUN,
    HAL_UART_INTERRUPT_ERROR_TIMEOUT,
} hal_uart_interrupts_t;

typedef enum
{
    HAL_UART_CFG_PARITY_NONE, /*!< No parity              */
    HAL_UART_CFG_PARITY_EVEN, /*!< Parity bit is even     */
    HAL_UART_CFG_PARITY_ODD,  /*!< Parity bit is odd      */
} hal_uart_cfg_parity_t;

typedef enum
{
    HAL_UART_CFG_STOP_ONE_BIT, /*!< One stop bit       */
    HAL_UART_CFG_STOP_TWO_BITS /*!< Two stop bits      */
} hal_uart_cfg_stop_bits_t;

typedef enum
{
    HAL_UART_CFG_DATA_LEN_5, /*!< Data length is 5 bits */
    HAL_UART_CFG_DATA_LEN_6, /*!< Data length is 6 bits */
    HAL_UART_CFG_DATA_LEN_7, /*!< Data length is 7 bits */
    HAL_UART_CFG_DATA_LEN_8  /*!< Data length is 8 bits */
} hal_uart_cfg_data_len_t;

typedef enum
{
    HAL_UART_CFG_FLOW_CTRL_NONE,
    HAL_UART_CFG_FLOW_CTRL_CTS,
    HAL_UART_CFG_FLOW_CTRL_RTS,
    HAL_UART_CFG_FLOW_CTRL_DTR,
    HAL_UART_CFG_FLOW_CTRL_DCD,
    HAL_UART_CFG_FLOW_CTRL_DE,
} hal_uart_cfg_flow_ctrl_t;

typedef enum
{
    HAL_UART_CFG_MODE_BLOCKING,
    HAL_UART_CFG_MODE_INTERRUPT,
} hal_uart_cfg_mode_t;

/* Types Structs ----------------------------------------------------*/
typedef struct hal_uart_s     hal_uart_t;
typedef struct hal_uart_api_s hal_uart_api_t;

typedef struct
{
    void       *port;
    hal_gpio_t *tx;
    hal_gpio_t *rx;
    hal_gpio_t *cts;
    hal_gpio_t *rts;
    hal_gpio_t *dtr;
    hal_gpio_t *dcd;
    hal_gpio_t *de;
    void       *user_data;
} hal_uart_phy_t;

typedef struct
{
    bool                     echo;
    uint32_t                 read_timeout;
    uint32_t                 write_timeout;
    uint32_t                 baudrate;
    hal_uart_cfg_mode_t      read_mode;
    hal_uart_cfg_mode_t      write_mode;
    hal_uart_cfg_data_len_t  data_len;
    hal_uart_cfg_stop_bits_t stop_bits;
    hal_uart_cfg_parity_t    parity;
    hal_uart_cfg_flow_ctrl_t flow_ctrl;
    void                    *user_data;
} hal_uart_config_t;

typedef void (*hal_uart_callback_t) (hal_uart_t *driver, hal_uart_interrupts_t interrupt, uint8_t *data,
                                     size_t nbr_of_bytes, void *user_arg);

/**
 * @brief Virtual Method Table (VMT) for the peripheral.
 * Hardware-specific implementations must populate these function pointers.
 * Functions can be set to NULL if not implemented.
 */
struct hal_uart_api_s
{
    hal_error_code_t (*hal_uart_register_cb) (hal_uart_t *driver, hal_uart_callback_t cb, void *user_arg);
    hal_error_code_t (*hal_uart_set_cb_arg) (hal_uart_t *driver, void *user_arg);
    hal_error_code_t (*hal_uart_tx_irq) (hal_uart_t *driver, const uint8_t *data, size_t nbr_of_bytes);
    hal_error_code_t (*hal_uart_tx_irq_abort) (hal_uart_t *driver);
    hal_error_code_t (*hal_uart_rx_irq) (hal_uart_t *driver, uint8_t *data, size_t nbr_of_bytes);
    hal_error_code_t (*hal_uart_rx_irq_abort) (hal_uart_t *driver);
    hal_error_code_t (*hal_uart_tx_polling) (hal_uart_t *driver, const uint8_t *data, size_t nbr_of_bytes,
                                             uint32_t timeout);
    hal_error_code_t (*hal_uart_rx_polling) (hal_uart_t *driver, uint8_t *data, size_t nbr_of_bytes, uint32_t timeout);
    hal_error_code_t (*hal_uart_rx_to_idle) (hal_uart_t *driver, uint8_t *data, size_t max_bytes,
                                             size_t *received_bytes, uint32_t timeout);
    hal_error_code_t (*hal_uart_check_error) (hal_uart_t *driver);
    hal_error_code_t (*hal_uart_init) (hal_uart_t *driver);
    hal_error_code_t (*hal_uart_deinit) (hal_uart_t *driver);
    hal_error_code_t (*hal_uart_cfg_get) (hal_uart_t *driver, hal_uart_config_t *config);
    hal_error_code_t (*hal_uart_cfg_set) (hal_uart_t *driver, const hal_uart_config_t *config);
    hal_error_code_t (*hal_uart_cfg_tx_enable) (hal_uart_t *driver);
    hal_error_code_t (*hal_uart_cfg_rx_enable) (hal_uart_t *driver);
    hal_error_code_t (*hal_uart_cfg_irq_enable) (hal_uart_t *driver, hal_uart_interrupts_t irq);
    hal_error_code_t (*hal_uart_cfg_tx_disable) (hal_uart_t *driver);
    hal_error_code_t (*hal_uart_cfg_rx_disable) (hal_uart_t *driver);
    hal_error_code_t (*hal_uart_cfg_irq_disable) (hal_uart_t *driver, hal_uart_interrupts_t irq);
    hal_error_code_t (*hal_uart_tx_dma) (hal_uart_t *driver, const uint8_t *data, size_t nbr_of_bytes);
    hal_error_code_t (*hal_uart_rx_dma) (hal_uart_t *driver, uint8_t *data, size_t nbr_of_bytes);
};

HAL_CREATE_DRIVER_TYPE (uart, hal_uart_config_t, hal_uart_api_t, hal_uart_phy_t, hal_uart_callback_t);
/* Functions inline  ---------------------------------------------*/

/**
 * @brief hal_uart_register_cb implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param cb Callback function to register.
 * @param user_arg User-defined argument passed to the callback.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_uart_register_cb (hal_uart_t *driver, hal_uart_callback_t cb, void *user_arg)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_UART, hal_uart_register_cb, driver, error, cb, user_arg);
    return error;
}

/**
 * @brief hal_uart_set_cb_arg implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param user_arg User-defined argument passed to the callback.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_uart_set_cb_arg (hal_uart_t *driver, void *user_arg)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_UART, hal_uart_set_cb_arg, driver, error, user_arg);
    return error;
}

/**
 * @brief hal_uart_tx_irq implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param data Pointer to the data buffer.
 * @param nbr_of_bytes Number of bytes to transfer.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_uart_tx_irq (hal_uart_t *driver, const uint8_t *data, size_t nbr_of_bytes)
{
    hal_error_code_t error = HAL_LOCK (driver, HAL_MAX_DELAY);
    if (error != HAL_ERROR_OK)
    {
        return error;
    }
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_UART, hal_uart_tx_irq, driver, error, data, nbr_of_bytes);
    HAL_UNLOCK (driver);
    return error;
}

/**
 * @brief hal_uart_tx_irq_abort implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param data Pointer to the data buffer.
 * @param nbr_of_bytes Number of bytes to transfer.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_uart_tx_irq_abort (hal_uart_t *driver)
{
    hal_error_code_t error = HAL_LOCK (driver, HAL_MAX_DELAY);
    if (error != HAL_ERROR_OK)
    {
        return error;
    }
    HAL_CHECK_AND_CALL_FUNC (HAL_PERIPHERAL_TYPE_UART, hal_uart_tx_irq_abort, driver, error);
    HAL_UNLOCK (driver);
    return error;
}

/**
 * @brief hal_uart_rx_irq implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param data Pointer to the data buffer.
 * @param nbr_of_bytes Number of bytes to transfer.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_uart_rx_irq (hal_uart_t *driver, uint8_t *data, size_t nbr_of_bytes)
{
    hal_error_code_t error = HAL_LOCK (driver, HAL_MAX_DELAY);
    if (error != HAL_ERROR_OK)
    {
        return error;
    }
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_UART, hal_uart_rx_irq, driver, error, data, nbr_of_bytes);
    HAL_UNLOCK (driver);
    return error;
}

/**
 * @brief hal_uart_rx_irq_abort implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param data Pointer to the data buffer.
 * @param nbr_of_bytes Number of bytes to transfer.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_uart_rx_irq_abort (hal_uart_t *driver)
{
    hal_error_code_t error = HAL_LOCK (driver, HAL_MAX_DELAY);
    if (error != HAL_ERROR_OK)
    {
        return error;
    }
    HAL_CHECK_AND_CALL_FUNC (HAL_PERIPHERAL_TYPE_UART, hal_uart_rx_irq_abort, driver, error);
    HAL_UNLOCK (driver);
    return error;
}

/**
 * @brief hal_uart_tx_polling implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param data Pointer to the data buffer.
 * @param nbr_of_bytes Number of bytes to transfer.
 * @param timeout Timeout duration in milliseconds.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_uart_tx_polling (hal_uart_t *driver, const uint8_t *data, size_t nbr_of_bytes,
                                                    uint32_t timeout)
{
    hal_error_code_t error = HAL_LOCK (driver, HAL_MAX_DELAY);
    if (error != HAL_ERROR_OK)
    {
        return error;
    }
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_UART, hal_uart_tx_polling, driver, error, data, nbr_of_bytes,
                                  timeout);
    HAL_UNLOCK (driver);
    return error;
}

/**
 * @brief hal_uart_rx_polling implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param data Pointer to the data buffer.
 * @param nbr_of_bytes Number of bytes to transfer.
 * @param timeout Timeout duration in milliseconds.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_uart_rx_polling (hal_uart_t *driver, uint8_t *data, size_t nbr_of_bytes,
                                                    uint32_t timeout)
{
    hal_error_code_t error = HAL_LOCK (driver, HAL_MAX_DELAY);
    if (error != HAL_ERROR_OK)
    {
        return error;
    }
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_UART, hal_uart_rx_polling, driver, error, data, nbr_of_bytes,
                                  timeout);
    HAL_UNLOCK (driver);
    return error;
}

/**
 * @brief hal_uart_rx_to_idle implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param data Pointer to the data buffer.
 * @param max_bytes Maximum number of bytes to transfer.
 * @param received_bytes Pointer to store the number of bytes actually
 * received.
 * @param timeout Timeout duration in milliseconds.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_uart_rx_to_idle (hal_uart_t *driver, uint8_t *data, size_t max_bytes,
                                                    size_t *received_bytes, uint32_t timeout)
{
    hal_error_code_t error = HAL_LOCK (driver, HAL_MAX_DELAY);
    if (error != HAL_ERROR_OK)
    {
        return error;
    }
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_UART, hal_uart_rx_to_idle, driver, error, data, max_bytes,
                                  received_bytes, timeout);
    HAL_UNLOCK (driver);
    return error;
}

/**
 * @brief hal_uart_check_error implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_uart_check_error (hal_uart_t *driver)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC (HAL_PERIPHERAL_TYPE_UART, hal_uart_check_error, driver, error);
    return error;
}

/**
 * @brief hal_uart_init implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_uart_init (hal_uart_t *driver)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    if (driver)
    {
        driver->base.type = HAL_PERIPHERAL_TYPE_UART;
        if (driver->base.on_config)
        {
            error = driver->base.on_config (&driver->base, DRIVER_CONFIG_INIT);
            if (error != HAL_ERROR_OK)
                return error;
        }
    }
    HAL_CHECK_AND_CALL_FUNC (HAL_PERIPHERAL_TYPE_UART, hal_uart_init, driver, error);
    return error;
}

/**
 * @brief hal_uart_deinit implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_uart_deinit (hal_uart_t *driver)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC (HAL_PERIPHERAL_TYPE_UART, hal_uart_deinit, driver, error);
    if (error == HAL_ERROR_OK && driver && driver->base.on_config)
    {
        error = driver->base.on_config (&driver->base, DRIVER_CONFIG_DEINIT);
    }
    return error;
}

/**
 * @brief hal_uart_cfg_get implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param config Pointer to the peripheral configuration structure.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_uart_cfg_get (hal_uart_t *driver, hal_uart_config_t *config)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_UART, hal_uart_cfg_get, driver, error, config);
    return error;
}

/**
 * @brief hal_uart_cfg_set implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param config Pointer to the peripheral configuration structure.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_uart_cfg_set (hal_uart_t *driver, const hal_uart_config_t *config)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_UART, hal_uart_cfg_set, driver, error, config);
    return error;
}

/**
 * @brief hal_uart_cfg_tx_enable implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_uart_cfg_tx_enable (hal_uart_t *driver)
{
    hal_error_code_t error = HAL_LOCK (driver, HAL_MAX_DELAY);
    if (error != HAL_ERROR_OK)
    {
        return error;
    }
    HAL_CHECK_AND_CALL_FUNC (HAL_PERIPHERAL_TYPE_UART, hal_uart_cfg_tx_enable, driver, error);
    HAL_UNLOCK (driver);
    return error;
}

/**
 * @brief hal_uart_cfg_rx_enable implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_uart_cfg_rx_enable (hal_uart_t *driver)
{
    hal_error_code_t error = HAL_LOCK (driver, HAL_MAX_DELAY);
    if (error != HAL_ERROR_OK)
    {
        return error;
    }
    HAL_CHECK_AND_CALL_FUNC (HAL_PERIPHERAL_TYPE_UART, hal_uart_cfg_rx_enable, driver, error);
    HAL_UNLOCK (driver);
    return error;
}

/**
 * @brief hal_uart_cfg_irq_enable implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param irq Interrupt type or event identifier.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_uart_cfg_irq_enable (hal_uart_t *driver, hal_uart_interrupts_t irq)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_UART, hal_uart_cfg_irq_enable, driver, error, irq);
    return error;
}

/**
 * @brief hal_uart_cfg_tx_disable implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_uart_cfg_tx_disable (hal_uart_t *driver)
{
    hal_error_code_t error = HAL_LOCK (driver, HAL_MAX_DELAY);
    if (error != HAL_ERROR_OK)
    {
        return error;
    }
    HAL_CHECK_AND_CALL_FUNC (HAL_PERIPHERAL_TYPE_UART, hal_uart_cfg_tx_disable, driver, error);
    HAL_UNLOCK (driver);
    return error;
}

/**
 * @brief hal_uart_cfg_rx_disable implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_uart_cfg_rx_disable (hal_uart_t *driver)
{
    hal_error_code_t error = HAL_LOCK (driver, HAL_MAX_DELAY);
    if (error != HAL_ERROR_OK)
    {
        return error;
    }
    HAL_CHECK_AND_CALL_FUNC (HAL_PERIPHERAL_TYPE_UART, hal_uart_cfg_rx_disable, driver, error);
    HAL_UNLOCK (driver);
    return error;
}

/**
 * @brief hal_uart_cfg_irq_disable implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param irq Interrupt type or event identifier.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_uart_cfg_irq_disable (hal_uart_t *driver, hal_uart_interrupts_t irq)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_UART, hal_uart_cfg_irq_disable, driver, error, irq);
    return error;
}

/**
 * @brief hal_uart_tx_dma implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param data Pointer to the data buffer.
 * @param nbr_of_bytes Number of bytes to transfer.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_uart_tx_dma (hal_uart_t *driver, const uint8_t *data, size_t nbr_of_bytes)
{
    hal_error_code_t error = HAL_LOCK (driver, HAL_MAX_DELAY);
    if (error != HAL_ERROR_OK)
    {
        return error;
    }
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_UART, hal_uart_tx_dma, driver, error, data, nbr_of_bytes);
    HAL_UNLOCK (driver);
    return error;
}

/**
 * @brief hal_uart_rx_dma implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param data Pointer to the data buffer.
 * @param nbr_of_bytes Number of bytes to transfer.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_uart_rx_dma (hal_uart_t *driver, uint8_t *data, size_t nbr_of_bytes)
{
    hal_error_code_t error = HAL_LOCK (driver, HAL_MAX_DELAY);
    if (error != HAL_ERROR_OK)
    {
        return error;
    }
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_UART, hal_uart_rx_dma, driver, error, data, nbr_of_bytes);
    HAL_UNLOCK (driver);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif // CAFFEINE_HAL_HAL_UART_H
