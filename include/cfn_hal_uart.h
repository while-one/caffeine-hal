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
#include "cfn_hal_base.h"
#include "cfn_hal_gpio.h"

/* Defines ----------------------------------------------------------*/

/* Types Enums ------------------------------------------------------*/

/**
 * @brief UART nominal event flags.
 */
typedef enum
{
    CFN_HAL_UART_EVENT_NONE = 0,
    CFN_HAL_UART_EVENT_TX_COMPLETE = CFN_HAL_BIT(0), /*!< Data transmission finished */
    CFN_HAL_UART_EVENT_RX_READY = CFN_HAL_BIT(1),    /*!< Data reception finished */
    CFN_HAL_UART_EVENT_BUS_IDLE = CFN_HAL_BIT(2),    /*!< UART bus idle detected */
} cfn_hal_uart_event_t;

/**
 * @brief UART exception error flags.
 */
typedef enum
{
    CFN_HAL_UART_ERROR_NONE = 0,
    CFN_HAL_UART_ERROR_FRAMING = CFN_HAL_BIT(0), /*!< Start/stop bit synchronization error */
    CFN_HAL_UART_ERROR_PARITY = CFN_HAL_BIT(1),  /*!< Parity check failed */
    CFN_HAL_UART_ERROR_OVERRUN = CFN_HAL_BIT(2), /*!< Buffer overflow error */
    CFN_HAL_UART_ERROR_TIMEOUT = CFN_HAL_BIT(3), /*!< Data reception timeout */
    CFN_HAL_UART_ERROR_GENERAL = CFN_HAL_BIT(4), /*!< General hardware error */
} cfn_hal_uart_error_t;

/**
 * @brief UART parity configuration.
 */
typedef enum
{
    CFN_HAL_UART_CONFIG_PARITY_NONE, /*!< No parity */
    CFN_HAL_UART_CONFIG_PARITY_EVEN, /*!< Even parity */
    CFN_HAL_UART_CONFIG_PARITY_ODD,  /*!< Odd parity */
} cfn_hal_uart_config_parity_t;

/**
 * @brief UART stop bits configuration.
 */
typedef enum
{
    CFN_HAL_UART_CONFIG_STOP_ONE_BIT,  /*!< 1 stop bit */
    CFN_HAL_UART_CONFIG_STOP_TWO_BITS, /*!< 2 stop bits */
} cfn_hal_uart_config_stop_bits_t;

/**
 * @brief UART data length configuration.
 */
typedef enum
{
    CFN_HAL_UART_CONFIG_DATA_LEN_5, /*!< 5 bits per word */
    CFN_HAL_UART_CONFIG_DATA_LEN_6, /*!< 6 bits per word */
    CFN_HAL_UART_CONFIG_DATA_LEN_7, /*!< 7 bits per word */
    CFN_HAL_UART_CONFIG_DATA_LEN_8, /*!< 8 bits per word */
    CFN_HAL_UART_CONFIG_DATA_LEN_9, /*!< 9 bits per word */
} cfn_hal_uart_config_data_len_t;

/**
 * @brief UART hardware flow control configuration.
 */
typedef enum
{
    CFN_HAL_UART_CONFIG_FLOW_CTRL_NONE,    /*!< No flow control */
    CFN_HAL_UART_CONFIG_FLOW_CTRL_CTS,     /*!< Hardware CTS only */
    CFN_HAL_UART_CONFIG_FLOW_CTRL_RTS,     /*!< Hardware RTS only */
    CFN_HAL_UART_CONFIG_FLOW_CTRL_RTS_CTS, /*!< Hardware RTS and CTS */
    CFN_HAL_UART_CONFIG_FLOW_CTRL_DTR,     /*!< Hardware DTR only */
    CFN_HAL_UART_CONFIG_FLOW_CTRL_DCD,     /*!< Hardware DCD only */
    CFN_HAL_UART_CONFIG_FLOW_CTRL_DE,      /*!< RS-485 Driver Enable */
} cfn_hal_uart_config_flow_ctrl_t;

/**
 * @brief UART I/O operation mode.
 */
typedef enum
{
    CFN_HAL_UART_CONFIG_MODE_NONE,      /*!< Disabled */
    CFN_HAL_UART_CONFIG_MODE_BLOCKING,  /*!< Polling based */
    CFN_HAL_UART_CONFIG_MODE_INTERRUPT, /*!< Interrupt based */
    CFN_HAL_UART_CONFIG_MODE_DMA,       /*!< DMA based */
} cfn_hal_uart_config_mode_t;

/* Types Structs ----------------------------------------------------*/

/**
 * @brief UART hardware physical mapping.
 */
typedef struct
{
    void                      *port;      /*!< Peripheral base register address */
    cfn_hal_gpio_pin_driver_t *tx;        /*!< TX pin driver mapping */
    cfn_hal_gpio_pin_driver_t *rx;        /*!< RX pin driver mapping */
    cfn_hal_gpio_pin_driver_t *cts;       /*!< CTS pin driver mapping */
    cfn_hal_gpio_pin_driver_t *rts;       /*!< RTS pin driver mapping */
    cfn_hal_gpio_pin_driver_t *dtr;       /*!< DTR pin driver mapping */
    cfn_hal_gpio_pin_driver_t *dcd;       /*!< DCD pin driver mapping */
    cfn_hal_gpio_pin_driver_t *de;        /*!< RS-485 DE pin driver mapping */
    void                      *user_data; /*!< Peripheral instance user argument */
} cfn_hal_uart_phy_t;

/**
 * @brief UART configuration structure.
 */
typedef struct
{
    bool                            echo;          /*!< Local echo enabled */
    uint32_t                        read_timeout;  /*!< Read timeout in ms */
    uint32_t                        write_timeout; /*!< Write timeout in ms */
    uint32_t                        baudrate;      /*!< Bus speed in bps */
    cfn_hal_uart_config_mode_t      read_mode;     /*!< RX operational mode */
    cfn_hal_uart_config_mode_t      write_mode;    /*!< TX operational mode */
    cfn_hal_uart_config_data_len_t  data_len;      /*!< Word length */
    cfn_hal_uart_config_stop_bits_t stop_bits;     /*!< Stop bits */
    cfn_hal_uart_config_parity_t    parity;        /*!< Parity check mode */
    cfn_hal_uart_config_flow_ctrl_t flow_ctrl;     /*!< Hardware flow control */
    void                           *custom;        /*!< Vendor-specific custom configuration */
} cfn_hal_uart_config_t;

typedef struct cfn_hal_uart_s     cfn_hal_uart_t;
typedef struct cfn_hal_uart_api_s cfn_hal_uart_api_t;

/**
 * @brief UART callback signature.
 * @param driver Pointer to the UART driver instance.
 * @param event_mask Mask of triggered nominal events.
 * @param error_mask Mask of triggered exception errors.
 * @param data Pointer to the transaction buffer (if applicable).
 * @param nbr_of_bytes Number of bytes transferred.
 * @param user_arg User-defined argument passed during registration.
 */
typedef void (*cfn_hal_uart_callback_t)(cfn_hal_uart_t *driver,
                                        uint32_t        event_mask,
                                        uint32_t        error_mask,
                                        uint8_t        *data,
                                        size_t          nbr_of_bytes,
                                        void           *user_arg);

/**
 * @brief UART Virtual Method Table (VMT).
 */
struct cfn_hal_uart_api_s
{
    cfn_hal_api_base_t base;

    /* UART Specific Extensions */
    cfn_hal_error_code_t (*tx_irq)(cfn_hal_uart_t *driver, const uint8_t *data, size_t nbr_of_bytes);
    cfn_hal_error_code_t (*tx_irq_abort)(cfn_hal_uart_t *driver);
    cfn_hal_error_code_t (*rx_irq)(cfn_hal_uart_t *driver, uint8_t *data, size_t nbr_of_bytes);
    cfn_hal_error_code_t (*rx_irq_abort)(cfn_hal_uart_t *driver);
    cfn_hal_error_code_t (*tx_polling)(cfn_hal_uart_t *driver,
                                       const uint8_t  *data,
                                       size_t          nbr_of_bytes,
                                       uint32_t        timeout);
    cfn_hal_error_code_t (*rx_polling)(cfn_hal_uart_t *driver, uint8_t *data, size_t nbr_of_bytes, uint32_t timeout);
    cfn_hal_error_code_t (*rx_to_idle)(
        cfn_hal_uart_t *driver, uint8_t *data, size_t max_bytes, size_t *received_bytes, uint32_t timeout);
    cfn_hal_error_code_t (*tx_dma)(cfn_hal_uart_t *driver, const uint8_t *data, size_t nbr_of_bytes);
    cfn_hal_error_code_t (*rx_dma)(cfn_hal_uart_t *driver, uint8_t *data, size_t nbr_of_bytes);
};

CFN_HAL_CREATE_DRIVER_TYPE(
    uart, cfn_hal_uart_config_t, cfn_hal_uart_api_t, cfn_hal_uart_phy_t, cfn_hal_uart_callback_t);

/* Functions inline ------------------------------------------------- */

/**
 * @brief Initializes the UART driver.
 * @param driver Pointer to the UART driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_uart_init(cfn_hal_uart_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->base.vmt = (const void *) driver->api;
    return cfn_hal_base_init(&driver->base, CFN_HAL_PERIPHERAL_TYPE_UART);
}

/**
 * @brief Deinitializes the UART driver.
 * @param driver Pointer to the UART driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_uart_deinit(cfn_hal_uart_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_deinit(&driver->base, CFN_HAL_PERIPHERAL_TYPE_UART);
}

/**
 * @brief Sets the UART configuration.
 * @param driver Pointer to the UART driver instance.
 * @param config Pointer to the configuration structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_uart_config_set(cfn_hal_uart_t *driver, const cfn_hal_uart_config_t *config)
{
    if (driver)
    {
        driver->config = config;
    }
    return cfn_hal_base_config_set(&driver->base, CFN_HAL_PERIPHERAL_TYPE_UART, (const void *) config);
}

/**
 * @brief Gets the current UART configuration.
 * @param driver Pointer to the UART driver instance.
 * @param config [out] Pointer to store the configuration.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_uart_config_get(cfn_hal_uart_t *driver, cfn_hal_uart_config_t *config)
{
    if (!driver || !config || !driver->config)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    *config = *(driver->config);
    return CFN_HAL_ERROR_OK;
}

/**
 * @brief Registers a callback for UART events and errors.
 * @param driver Pointer to the UART driver instance.
 * @param CALLBACK The callback function to register.
 * @param user_arg User-defined argument passed to the callback.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t
cfn_hal_uart_callback_register(cfn_hal_uart_t *driver, const cfn_hal_uart_callback_t CALLBACK, void *user_arg)
{
    if (driver)
    {
        driver->cb = CALLBACK;
        driver->cb_user_arg = user_arg;
    }
    return cfn_hal_base_callback_register(
        &driver->base, CFN_HAL_PERIPHERAL_TYPE_UART, (const void *) CALLBACK, user_arg);
}

/**
 * @brief Sets the UART power state.
 * @param driver Pointer to the UART driver instance.
 * @param state Target power state.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_uart_power_state_set(cfn_hal_uart_t *driver, cfn_hal_power_state_t state)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_power_state_set(&driver->base, CFN_HAL_PERIPHERAL_TYPE_UART, state);
}

/**
 * @brief Enables one or more UART nominal events.
 * @param driver Pointer to the UART driver instance.
 * @param event_mask Mask of events to enable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_uart_event_enable(cfn_hal_uart_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_enable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_UART, event_mask);
}

/**
 * @brief Disables one or more UART nominal events.
 * @param driver Pointer to the UART driver instance.
 * @param event_mask Mask of events to disable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_uart_event_disable(cfn_hal_uart_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_disable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_UART, event_mask);
}

/**
 * @brief Retrieves the current UART nominal event status.
 * @param driver Pointer to the UART driver instance.
 * @param event_mask [out] Pointer to store the event mask.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_uart_event_get(cfn_hal_uart_t *driver, uint32_t *event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_get(&driver->base, CFN_HAL_PERIPHERAL_TYPE_UART, event_mask);
}

/**
 * @brief Enables one or more UART exception errors.
 * @param driver Pointer to the UART driver instance.
 * @param error_mask Mask of errors to enable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_uart_error_enable(cfn_hal_uart_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_enable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_UART, error_mask);
}

/**
 * @brief Disables one or more UART exception errors.
 * @param driver Pointer to the UART driver instance.
 * @param error_mask Mask of errors to disable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_uart_error_disable(cfn_hal_uart_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_disable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_UART, error_mask);
}

/**
 * @brief Retrieves the current UART exception error status.
 * @param driver Pointer to the UART driver instance.
 * @param error_mask [out] Pointer to store the error mask.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_uart_error_get(cfn_hal_uart_t *driver, uint32_t *error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_get(&driver->base, CFN_HAL_PERIPHERAL_TYPE_UART, error_mask);
}

/* UART Specific Functions ------------------------------------------ */

/**
 * @brief Starts UART data transmission using interrupts (non-blocking).
 * @param driver Pointer to the UART driver instance.
 * @param data Pointer to the buffer containing data to transmit.
 * @param nbr_of_bytes Number of bytes to transmit.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_uart_tx_irq(cfn_hal_uart_t *driver, const uint8_t *data, size_t nbr_of_bytes)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_UART, tx_irq, driver, error, data, nbr_of_bytes);
    return error;
}

/**
 * @brief Aborts an ongoing interrupt-based transmission.
 * @param driver Pointer to the UART driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_uart_tx_irq_abort(cfn_hal_uart_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_UART, tx_irq_abort, driver, error);
    return error;
}

/**
 * @brief Starts UART data reception using interrupts (non-blocking).
 * @param driver Pointer to the UART driver instance.
 * @param data Pointer to the buffer to store received data.
 * @param nbr_of_bytes Number of bytes to receive.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_uart_rx_irq(cfn_hal_uart_t *driver, uint8_t *data, size_t nbr_of_bytes)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_UART, rx_irq, driver, error, data, nbr_of_bytes);
    return error;
}

/**
 * @brief Aborts an ongoing interrupt-based reception.
 * @param driver Pointer to the UART driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_uart_rx_irq_abort(cfn_hal_uart_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_UART, rx_irq_abort, driver, error);
    return error;
}

/**
 * @brief Transmits data using polling (blocking).
 * @param driver Pointer to the UART driver instance.
 * @param data Pointer to the buffer containing data to transmit.
 * @param nbr_of_bytes Number of bytes to transmit.
 * @param timeout Timeout duration in milliseconds.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t
cfn_hal_uart_tx_polling(cfn_hal_uart_t *driver, const uint8_t *data, size_t nbr_of_bytes, uint32_t timeout)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(
        CFN_HAL_PERIPHERAL_TYPE_UART, tx_polling, driver, error, data, nbr_of_bytes, timeout);
    return error;
}

/**
 * @brief Receives data using polling (blocking).
 * @param driver Pointer to the UART driver instance.
 * @param data Pointer to the buffer to store received data.
 * @param nbr_of_bytes Number of bytes to receive.
 * @param timeout Timeout duration in milliseconds.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t
cfn_hal_uart_rx_polling(cfn_hal_uart_t *driver, uint8_t *data, size_t nbr_of_bytes, uint32_t timeout)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(
        CFN_HAL_PERIPHERAL_TYPE_UART, rx_polling, driver, error, data, nbr_of_bytes, timeout);
    return error;
}

/**
 * @brief Receives data until an idle condition or max bytes reached.
 * @param driver Pointer to the UART driver instance.
 * @param data Pointer to the buffer to store received data.
 * @param max_bytes Maximum capacity of the data buffer.
 * @param received_bytes [out] Actual number of bytes received.
 * @param timeout Timeout duration in milliseconds.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_uart_rx_to_idle(
    cfn_hal_uart_t *driver, uint8_t *data, size_t max_bytes, size_t *received_bytes, uint32_t timeout)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(
        CFN_HAL_PERIPHERAL_TYPE_UART, rx_to_idle, driver, error, data, max_bytes, received_bytes, timeout);
    return error;
}

/**
 * @brief Transmits data using DMA (non-blocking).
 * @param driver Pointer to the UART driver instance.
 * @param data Pointer to the buffer containing data to transmit.
 * @param nbr_of_bytes Number of bytes to transmit.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_uart_tx_dma(cfn_hal_uart_t *driver, const uint8_t *data, size_t nbr_of_bytes)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_UART, tx_dma, driver, error, data, nbr_of_bytes);
    return error;
}

/**
 * @brief Receives data using DMA (non-blocking).
 * @param driver Pointer to the UART driver instance.
 * @param data Pointer to the buffer to store received data.
 * @param nbr_of_bytes Number of bytes to receive.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_uart_rx_dma(cfn_hal_uart_t *driver, uint8_t *data, size_t nbr_of_bytes)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_UART, rx_dma, driver, error, data, nbr_of_bytes);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif // CAFFEINE_HAL_HAL_UART_H
