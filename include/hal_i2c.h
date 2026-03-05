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
 * @file hal_i2c.h
 * @brief Inter-Integrated Circuit (I2C) HAL API.
 */

#ifndef CAFFEINE_HAL_HAL_I2C_H
#define CAFFEINE_HAL_HAL_I2C_H

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
    HAL_I2C_INTERRUPT_TX_COMPLETE,
    HAL_I2C_INTERRUPT_RX_READY,
    HAL_I2C_INTERRUPT_START_BIT_SENT,
    HAL_I2C_INTERRUPT_ADDRESS_SENT,
    HAL_I2C_INTERRUPT_10B_HEADER_SENT,
    HAL_I2C_INTERRUPT_DATA_BYTE_SENT,
    HAL_I2C_INTERRUPT_STOP_RX,

    HAL_I2C_INTERRUPT_ERROR,
    HAL_I2C_INTERRUPT_ERROR_ARBITRATION,
    HAL_I2C_INTERRUPT_ERROR_ACK,
    HAL_I2C_INTERRUPT_ERROR_OVERRUN,
    HAL_I2C_INTERRUPT_ERROR_PEC,
    HAL_I2C_INTERRUPT_ERROR_TIMEOUT,
    HAL_I2C_INTERRUPT_ERROR_SMB
} hal_i2c_interrupts_t;
/* Types Structs ----------------------------------------------------*/
typedef enum
{
    HAL_I2C_CFG_SPEED_100KHZ,
    HAL_I2C_CFG_SPEED_400KHZ,
    HAL_I2C_CFG_SPEED_1000KHZ,
    HAL_I2C_CFG_SPEED_3330KHZ,
    HAL_I2C_CFG_SPEED_3400KHZ,
    HAL_I2C_CFG_SPEED_5000KHZ,
    HAL_I2C_CFG_SPEED_CUSTOM
} hal_i2c_config_speed_t;

typedef struct
{
    void       *port;
    hal_gpio_t *sda;
    hal_gpio_t *scl;
    hal_gpio_t *irq;
    void       *user_arg;
} hal_i2c_phy_t;

typedef struct i2c_config
{
    hal_i2c_config_speed_t speed;
    void                  *custom;
} hal_i2c_config_t;

typedef struct
{
    uint8_t        slave_address;
    const uint8_t *tx_payload;
    size_t         nbr_of_tx_bytes;
    uint8_t       *rx_payload;
    size_t         nbr_of_rx_bytes;
} hal_i2c_transaction_t;

typedef struct hal_i2c_s hal_i2c_t;

typedef struct hal_i2c_api_s hal_i2c_api_t;

typedef void (*hal_i2c_callback_t) (hal_i2c_t *driver, hal_i2c_interrupts_t interrupt, uint8_t *payload,
                                    size_t nbr_of_bytes, void *user_arg);

/**
 * @brief Virtual Method Table (VMT) for the peripheral.
 * Hardware-specific implementations must populate these function pointers.
 * Functions can be set to NULL if not implemented.
 */
struct hal_i2c_api_s
{
    hal_error_code_t (*hal_i2c_register_cb) (hal_i2c_t *driver, hal_i2c_callback_t cb, void *user_arg);
    hal_error_code_t (*hal_i2c_set_cb_arg) (hal_i2c_t *driver, void *user_arg);
    hal_error_code_t (*hal_i2c_xfr_irq) (hal_i2c_t *driver, hal_i2c_transaction_t *xfr);
    hal_error_code_t (*hal_i2c_xfr_irq_abort) (hal_i2c_t *driver);
    hal_error_code_t (*hal_i2c_xfr_polling) (hal_i2c_t *driver, hal_i2c_transaction_t *xfr, uint32_t timeout);
    hal_error_code_t (*hal_i2c_mem_read) (hal_i2c_t *driver, uint16_t dev_addr, uint16_t mem_addr,
                                          uint16_t mem_addr_size, uint8_t *data, size_t size, uint32_t timeout);
    hal_error_code_t (*hal_i2c_mem_write) (hal_i2c_t *driver, uint16_t dev_addr, uint16_t mem_addr,
                                           uint16_t mem_addr_size, const uint8_t *data, size_t size, uint32_t timeout);
    hal_error_code_t (*hal_i2c_check_error) (hal_i2c_t *driver);
    hal_error_code_t (*hal_i2c_init) (hal_i2c_t *driver);
    hal_error_code_t (*hal_i2c_deinit) (hal_i2c_t *driver);
    hal_error_code_t (*hal_i2c_cfg_get) (hal_i2c_t *driver, hal_i2c_config_t *config);
    hal_error_code_t (*hal_i2c_cfg_set) (hal_i2c_t *driver, const hal_i2c_config_t *config);
    hal_error_code_t (*hal_i2c_cfg_tx_enable) (hal_i2c_t *driver);
    hal_error_code_t (*hal_i2c_cfg_rx_enable) (hal_i2c_t *driver);
    hal_error_code_t (*hal_i2c_cfg_irq_enable) (hal_i2c_t *driver, hal_i2c_interrupts_t irq);
    hal_error_code_t (*hal_i2c_cfg_tx_disable) (hal_i2c_t *driver);
    hal_error_code_t (*hal_i2c_cfg_rx_disable) (hal_i2c_t *driver);
    hal_error_code_t (*hal_i2c_cfg_irq_disable) (hal_i2c_t *driver, hal_i2c_interrupts_t irq);
    hal_error_code_t (*hal_i2c_xfr_dma) (hal_i2c_t *driver, hal_i2c_transaction_t *xfr);
};

HAL_CREATE_DRIVER_TYPE (i2c, hal_i2c_config_t, hal_i2c_api_t, hal_i2c_phy_t, hal_i2c_callback_t);

/* Functions prototypes ---------------------------------------------*/
/**
 * @brief hal_i2c_register_cb implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param cb Callback function to register.
 * @param user_arg User-defined argument passed to the callback.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_i2c_register_cb (hal_i2c_t *driver, hal_i2c_callback_t cb, void *user_arg)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_I2C, hal_i2c_register_cb, driver, error, cb, user_arg);
    return error;
}

/**
 * @brief hal_i2c_set_cb_arg implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param user_arg User-defined argument passed to the callback.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_i2c_set_cb_arg (hal_i2c_t *driver, void *user_arg)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_I2C, hal_i2c_set_cb_arg, driver, error, user_arg);
    return error;
}

/**
 * @brief hal_i2c_xfr_irq implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param xfr Pointer to the transaction structure defining the transfer.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_i2c_xfr_irq (hal_i2c_t *driver, hal_i2c_transaction_t *xfr)
{
    hal_error_code_t error = HAL_LOCK (driver, HAL_MAX_DELAY);
    if (error != HAL_ERROR_OK)
    {
        return error;
    }
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_I2C, hal_i2c_xfr_irq, driver, error, xfr);
    HAL_UNLOCK (driver);
    return error;
}

/**
 * @brief hal_i2c_xfr_irq_abort implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param address The address parameter.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_i2c_xfr_irq_abort (hal_i2c_t *driver)
{
    hal_error_code_t error = HAL_LOCK (driver, HAL_MAX_DELAY);
    if (error != HAL_ERROR_OK)
    {
        return error;
    }
    HAL_CHECK_AND_CALL_FUNC (HAL_PERIPHERAL_TYPE_I2C, hal_i2c_xfr_irq_abort, driver, error);
    HAL_UNLOCK (driver);
    return error;
}

/**
 * @brief hal_i2c_xfr_polling implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param xfr Pointer to the transaction structure defining the transfer.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_i2c_xfr_polling (hal_i2c_t *driver, hal_i2c_transaction_t *xfr, uint32_t timeout)
{
    hal_error_code_t error = HAL_LOCK (driver, HAL_MAX_DELAY);
    if (error != HAL_ERROR_OK)
    {
        return error;
    }
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_I2C, hal_i2c_xfr_polling, driver, error, xfr, timeout);
    HAL_UNLOCK (driver);
    return error;
}

/**
 * @brief hal_i2c_mem_read implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param dev_addr Target device address.
 * @param mem_addr Internal memory address.
 * @param mem_addr_size Size of internal memory address in bytes (1 or 2).
 * @param data Pointer to the data buffer.
 * @param size Number of bytes to transfer.
 * @param timeout Timeout duration in milliseconds.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_i2c_mem_read (hal_i2c_t *driver, uint16_t dev_addr, uint16_t mem_addr,
                                                 uint16_t mem_addr_size, uint8_t *data, size_t size, uint32_t timeout)
{
    hal_error_code_t error = HAL_LOCK (driver, HAL_MAX_DELAY);
    if (error != HAL_ERROR_OK)
    {
        return error;
    }
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_I2C, hal_i2c_mem_read, driver, error, dev_addr, mem_addr,
                                  mem_addr_size, data, size, timeout);
    HAL_UNLOCK (driver);
    return error;
}

/**
 * @brief hal_i2c_mem_write implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param dev_addr Target device address.
 * @param mem_addr Internal memory address.
 * @param mem_addr_size Size of internal memory address in bytes (1 or 2).
 * @param data Pointer to the data buffer.
 * @param size Number of bytes to transfer.
 * @param timeout Timeout duration in milliseconds.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_i2c_mem_write (hal_i2c_t *driver, uint16_t dev_addr, uint16_t mem_addr,
                                                  uint16_t mem_addr_size, const uint8_t *data, size_t size,
                                                  uint32_t timeout)
{
    hal_error_code_t error = HAL_LOCK (driver, HAL_MAX_DELAY);
    if (error != HAL_ERROR_OK)
    {
        return error;
    }
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_I2C, hal_i2c_mem_write, driver, error, dev_addr, mem_addr,
                                  mem_addr_size, data, size, timeout);
    HAL_UNLOCK (driver);
    return error;
}

/**
 * @brief hal_i2c_check_error implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_i2c_check_error (hal_i2c_t *driver)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC (HAL_PERIPHERAL_TYPE_I2C, hal_i2c_check_error, driver, error);
    return error;
}

/**
 * @brief hal_i2c_init implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_i2c_init (hal_i2c_t *driver)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    if (driver)
    {
        driver->base.type = HAL_PERIPHERAL_TYPE_I2C;
        if (driver->base.on_config)
        {
            error = driver->base.on_config (&driver->base, DRIVER_CONFIG_INIT);
            if (error != HAL_ERROR_OK)
                return error;
        }
    }
    HAL_CHECK_AND_CALL_FUNC (HAL_PERIPHERAL_TYPE_I2C, hal_i2c_init, driver, error);
    return error;
}

/**
 * @brief hal_i2c_deinit implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_i2c_deinit (hal_i2c_t *driver)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC (HAL_PERIPHERAL_TYPE_I2C, hal_i2c_deinit, driver, error);
    if (error == HAL_ERROR_OK && driver && driver->base.on_config)
    {
        error = driver->base.on_config (&driver->base, DRIVER_CONFIG_DEINIT);
    }
    return error;
}

/**
 * @brief hal_i2c_cfg_get implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param config Pointer to the peripheral configuration structure.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_i2c_cfg_get (hal_i2c_t *driver, hal_i2c_config_t *config)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_I2C, hal_i2c_cfg_get, driver, error, config);
    return error;
}

/**
 * @brief hal_i2c_cfg_set implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param config Pointer to the peripheral configuration structure.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_i2c_cfg_set (hal_i2c_t *driver, const hal_i2c_config_t *config)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_I2C, hal_i2c_cfg_set, driver, error, config);
    return error;
}

/**
 * @brief hal_i2c_cfg_tx_enable implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_i2c_cfg_tx_enable (hal_i2c_t *driver)
{
    hal_error_code_t error = HAL_LOCK (driver, HAL_MAX_DELAY);
    if (error != HAL_ERROR_OK)
    {
        return error;
    }
    HAL_CHECK_AND_CALL_FUNC (HAL_PERIPHERAL_TYPE_I2C, hal_i2c_cfg_tx_enable, driver, error);
    HAL_UNLOCK (driver);
    return error;
}

/**
 * @brief hal_i2c_cfg_rx_enable implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_i2c_cfg_rx_enable (hal_i2c_t *driver)
{
    hal_error_code_t error = HAL_LOCK (driver, HAL_MAX_DELAY);
    if (error != HAL_ERROR_OK)
    {
        return error;
    }
    HAL_CHECK_AND_CALL_FUNC (HAL_PERIPHERAL_TYPE_I2C, hal_i2c_cfg_rx_enable, driver, error);
    HAL_UNLOCK (driver);
    return error;
}

/**
 * @brief hal_i2c_cfg_irq_enable implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param irq Interrupt type or event identifier.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_i2c_cfg_irq_enable (hal_i2c_t *driver, hal_i2c_interrupts_t irq)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_I2C, hal_i2c_cfg_irq_enable, driver, error, irq);
    return error;
}

/**
 * @brief hal_i2c_cfg_tx_disable implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_i2c_cfg_tx_disable (hal_i2c_t *driver)
{
    hal_error_code_t error = HAL_LOCK (driver, HAL_MAX_DELAY);
    if (error != HAL_ERROR_OK)
    {
        return error;
    }
    HAL_CHECK_AND_CALL_FUNC (HAL_PERIPHERAL_TYPE_I2C, hal_i2c_cfg_tx_disable, driver, error);
    HAL_UNLOCK (driver);
    return error;
}

/**
 * @brief hal_i2c_cfg_rx_disable implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_i2c_cfg_rx_disable (hal_i2c_t *driver)
{
    hal_error_code_t error = HAL_LOCK (driver, HAL_MAX_DELAY);
    if (error != HAL_ERROR_OK)
    {
        return error;
    }
    HAL_CHECK_AND_CALL_FUNC (HAL_PERIPHERAL_TYPE_I2C, hal_i2c_cfg_rx_disable, driver, error);
    HAL_UNLOCK (driver);
    return error;
}

/**
 * @brief hal_i2c_cfg_irq_disable implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param irq Interrupt type or event identifier.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_i2c_cfg_irq_disable (hal_i2c_t *driver, hal_i2c_interrupts_t irq)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_I2C, hal_i2c_cfg_irq_disable, driver, error, irq);
    return error;
}

/**
 * @brief hal_i2c_xfr_dma implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param xfr Pointer to the transaction structure defining the transfer.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_i2c_xfr_dma (hal_i2c_t *driver, hal_i2c_transaction_t *xfr)
{
    hal_error_code_t error = HAL_LOCK (driver, HAL_MAX_DELAY);
    if (error != HAL_ERROR_OK)
    {
        return error;
    }
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_I2C, hal_i2c_xfr_dma, driver, error, xfr);
    HAL_UNLOCK (driver);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif // CAFFEINE_HAL_HAL_I2C_H
