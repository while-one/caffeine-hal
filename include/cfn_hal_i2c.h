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
 * @file cfn_hal_i2c.h
 * @brief Inter-Integrated Circuit (I2C) HAL API.
 */

#ifndef CAFFEINE_HAL_HAL_I2C_H
#define CAFFEINE_HAL_HAL_I2C_H

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
    CFN_HAL_I2C_INTERRUPT_TX_COMPLETE,
    CFN_HAL_I2C_INTERRUPT_RX_READY,
    CFN_HAL_I2C_INTERRUPT_START_BIT_SENT,
    CFN_HAL_I2C_INTERRUPT_ADDRESS_SENT,
    CFN_HAL_I2C_INTERRUPT_10B_HEADER_SENT,
    CFN_HAL_I2C_INTERRUPT_DATA_BYTE_SENT,
    CFN_HAL_I2C_INTERRUPT_STOP_RX,

    CFN_HAL_I2C_INTERRUPT_ERROR,
    CFN_HAL_I2C_INTERRUPT_ERROR_ARBITRATION,
    CFN_HAL_I2C_INTERRUPT_ERROR_ACK,
    CFN_HAL_I2C_INTERRUPT_ERROR_OVERRUN,
    CFN_HAL_I2C_INTERRUPT_ERROR_PEC,
    CFN_HAL_I2C_INTERRUPT_ERROR_TIMEOUT,
    CFN_HAL_I2C_INTERRUPT_ERROR_SMB
} cfn_hal_i2c_interrupts_t;
/* Types Structs ----------------------------------------------------*/
typedef enum
{
    CFN_HAL_I2C_CFG_SPEED_100KHZ,
    CFN_HAL_I2C_CFG_SPEED_400KHZ,
    CFN_HAL_I2C_CFG_SPEED_1000KHZ,
    CFN_HAL_I2C_CFG_SPEED_3330KHZ,
    CFN_HAL_I2C_CFG_SPEED_3400KHZ,
    CFN_HAL_I2C_CFG_SPEED_5000KHZ,
    CFN_HAL_I2C_CFG_SPEED_CUSTOM
} cfn_hal_i2c_config_speed_t;

typedef struct
{
    void           *port;
    cfn_hal_gpio_t *sda;
    cfn_hal_gpio_t *scl;
    cfn_hal_gpio_t *irq;
    void           *user_arg;
} cfn_hal_i2c_phy_t;

typedef struct i2c_config
{
    cfn_hal_i2c_config_speed_t speed;
    void                      *custom;
} cfn_hal_i2c_config_t;

typedef struct
{
    uint8_t        slave_address;
    const uint8_t *tx_payload;
    size_t         nbr_of_tx_bytes;
    uint8_t       *rx_payload;
    size_t         nbr_of_rx_bytes;
} cfn_hal_i2c_transaction_t;

typedef struct
{
    uint16_t dev_addr;
    uint16_t mem_addr;
    uint16_t mem_addr_size;
    uint8_t *data;
    size_t   size;
} cfn_hal_i2c_mem_transaction_t;

typedef struct cfn_hal_i2c_s cfn_hal_i2c_t;

typedef struct cfn_hal_i2c_api_s cfn_hal_i2c_api_t;

typedef void (*cfn_hal_i2c_callback_t)(
    cfn_hal_i2c_t *driver, cfn_hal_i2c_interrupts_t interrupt, uint8_t *payload, size_t nbr_of_bytes, void *user_arg);

/**
 * @brief Virtual Method Table (VMT) for the peripheral.
 * Hardware-specific implementations must populate these function pointers.
 * Functions can be set to NULL if not implemented.
 */
struct cfn_hal_i2c_api_s
{
    cfn_hal_error_code_t (*cfn_hal_i2c_register_cb)(cfn_hal_i2c_t *driver, cfn_hal_i2c_callback_t cb, void *user_arg);
    cfn_hal_error_code_t (*cfn_hal_i2c_set_cb_arg)(cfn_hal_i2c_t *driver, void *user_arg);
    cfn_hal_error_code_t (*cfn_hal_i2c_xfr_irq)(cfn_hal_i2c_t *driver, cfn_hal_i2c_transaction_t *xfr);
    cfn_hal_error_code_t (*cfn_hal_i2c_xfr_irq_abort)(cfn_hal_i2c_t *driver);
    cfn_hal_error_code_t (*cfn_hal_i2c_xfr_polling)(
        cfn_hal_i2c_t *driver, cfn_hal_i2c_transaction_t *xfr, uint32_t timeout);
    cfn_hal_error_code_t (*cfn_hal_i2c_mem_read)(
        cfn_hal_i2c_t *driver, cfn_hal_i2c_mem_transaction_t *mem_xfr, uint32_t timeout);
    cfn_hal_error_code_t (*cfn_hal_i2c_mem_write)(
        cfn_hal_i2c_t *driver, const cfn_hal_i2c_mem_transaction_t *mem_xfr, uint32_t timeout);
    cfn_hal_error_code_t (*cfn_hal_i2c_check_error)(cfn_hal_i2c_t *driver);
    cfn_hal_error_code_t (*cfn_hal_i2c_init)(cfn_hal_i2c_t *driver);
    cfn_hal_error_code_t (*cfn_hal_i2c_deinit)(cfn_hal_i2c_t *driver);
    cfn_hal_error_code_t (*cfn_hal_i2c_cfg_get)(cfn_hal_i2c_t *driver, cfn_hal_i2c_config_t *config);
    cfn_hal_error_code_t (*cfn_hal_i2c_cfg_set)(cfn_hal_i2c_t *driver, const cfn_hal_i2c_config_t *config);
    cfn_hal_error_code_t (*cfn_hal_i2c_cfg_tx_enable)(cfn_hal_i2c_t *driver);
    cfn_hal_error_code_t (*cfn_hal_i2c_cfg_rx_enable)(cfn_hal_i2c_t *driver);
    cfn_hal_error_code_t (*cfn_hal_i2c_cfg_irq_enable)(cfn_hal_i2c_t *driver, cfn_hal_i2c_interrupts_t irq);
    cfn_hal_error_code_t (*cfn_hal_i2c_cfg_tx_disable)(cfn_hal_i2c_t *driver);
    cfn_hal_error_code_t (*cfn_hal_i2c_cfg_rx_disable)(cfn_hal_i2c_t *driver);
    cfn_hal_error_code_t (*cfn_hal_i2c_cfg_irq_disable)(cfn_hal_i2c_t *driver, cfn_hal_i2c_interrupts_t irq);
    cfn_hal_error_code_t (*cfn_hal_i2c_xfr_dma)(cfn_hal_i2c_t *driver, cfn_hal_i2c_transaction_t *xfr);
};

/**
 * @brief Generated driver structure for i2c.
 *
 * This macro expands to the following structure:
 * \code{c}
 * struct cfn_hal_i2c_s {
 *     cfn_hal_driver_t       base;
 *     const cfn_hal_i2c_config_t *config;
 *     const cfn_hal_i2c_api_t    *api;
 *     const cfn_hal_i2c_phy_t    *phy;
 *     cfn_hal_i2c_callback_t            cb;
 *     void              *cb_user_arg;
 * };
 * typedef struct cfn_hal_i2c_s cfn_hal_i2c_t;
 * \endcode
 */
CFN_HAL_CREATE_DRIVER_TYPE(i2c, cfn_hal_i2c_config_t, cfn_hal_i2c_api_t, cfn_hal_i2c_phy_t, cfn_hal_i2c_callback_t);

/* Functions prototypes ---------------------------------------------*/
/**
 * @brief cfn_hal_i2c_register_cb implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param cb Callback function to register.
 * @param user_arg User-defined argument passed to the callback.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t
cfn_hal_i2c_register_cb(cfn_hal_i2c_t *driver, cfn_hal_i2c_callback_t cb, void *user_arg)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_I2C, cfn_hal_i2c_register_cb, driver, error, cb, user_arg);
    return error;
}

/**
 * @brief cfn_hal_i2c_set_cb_arg implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param user_arg User-defined argument passed to the callback.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_i2c_set_cb_arg(cfn_hal_i2c_t *driver, void *user_arg)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_I2C, cfn_hal_i2c_set_cb_arg, driver, error, user_arg);
    return error;
}

/**
 * @brief cfn_hal_i2c_xfr_irq implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param xfr Pointer to the transaction structure defining the transfer.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_i2c_xfr_irq(cfn_hal_i2c_t *driver, cfn_hal_i2c_transaction_t *xfr)
{
    cfn_hal_error_code_t error = CFN_HAL_LOCK(driver, CFN_HAL_MAX_DELAY);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_I2C, cfn_hal_i2c_xfr_irq, driver, error, xfr);
    CFN_HAL_UNLOCK(driver);
    return error;
}

/**
 * @brief cfn_hal_i2c_xfr_irq_abort implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param address The address parameter.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_i2c_xfr_irq_abort(cfn_hal_i2c_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_LOCK(driver, CFN_HAL_MAX_DELAY);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_I2C, cfn_hal_i2c_xfr_irq_abort, driver, error);
    CFN_HAL_UNLOCK(driver);
    return error;
}

/**
 * @brief cfn_hal_i2c_xfr_polling implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param xfr Pointer to the transaction structure defining the transfer.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t
cfn_hal_i2c_xfr_polling(cfn_hal_i2c_t *driver, cfn_hal_i2c_transaction_t *xfr, uint32_t timeout)
{
    cfn_hal_error_code_t error = CFN_HAL_LOCK(driver, CFN_HAL_MAX_DELAY);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_I2C, cfn_hal_i2c_xfr_polling, driver, error, xfr, timeout);
    CFN_HAL_UNLOCK(driver);
    return error;
}

/**
 * @brief cfn_hal_i2c_mem_read implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param dev_addr Target device address.
 * @param mem_addr Internal memory address.
 * @param mem_addr_size Size of internal memory address in bytes (1 or 2).
 * @param data Pointer to the data buffer.
 * @param size Number of bytes to transfer.
 * @param timeout Timeout duration in milliseconds.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_i2c_mem_read(

    cfn_hal_i2c_t *driver, cfn_hal_i2c_mem_transaction_t *mem_xfr, uint32_t timeout)

{
    cfn_hal_error_code_t error = CFN_HAL_LOCK(driver, CFN_HAL_MAX_DELAY);

    if (error != CFN_HAL_ERROR_OK)

    {
        return error;
    }

    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(

        CFN_HAL_PERIPHERAL_TYPE_I2C,

        cfn_hal_i2c_mem_read,

        driver,

        error,

        mem_xfr,

        timeout);

    CFN_HAL_UNLOCK(driver);

    return error;
}

/**

 * @brief cfn_hal_i2c_mem_write implementation.

 * @param driver Pointer to the peripheral driver instance.

 * @param mem_xfr Pointer to the memory transaction structure.

 * @param timeout Timeout duration.

 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.

 */

static inline cfn_hal_error_code_t cfn_hal_i2c_mem_write(

    cfn_hal_i2c_t *driver, const cfn_hal_i2c_mem_transaction_t *mem_xfr, uint32_t timeout)

{
    cfn_hal_error_code_t error = CFN_HAL_LOCK(driver, CFN_HAL_MAX_DELAY);

    if (error != CFN_HAL_ERROR_OK)

    {
        return error;
    }

    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(

        CFN_HAL_PERIPHERAL_TYPE_I2C,

        cfn_hal_i2c_mem_write,

        driver,

        error,

        mem_xfr,

        timeout);

    CFN_HAL_UNLOCK(driver);

    return error;
}

/**
 * @brief cfn_hal_i2c_check_error implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_i2c_check_error(cfn_hal_i2c_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_I2C, cfn_hal_i2c_check_error, driver, error);
    return error;
}

/**
 * @brief cfn_hal_i2c_init implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_i2c_init(cfn_hal_i2c_t *driver)
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
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_I2C, cfn_hal_i2c_init, driver, error);
    if (error == CFN_HAL_ERROR_OK && driver)
    {
        driver->base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    }
    return error;
}

/**
 * @brief cfn_hal_i2c_cfg_get implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param config Pointer to the peripheral configuration structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_i2c_cfg_get(cfn_hal_i2c_t *driver, cfn_hal_i2c_config_t *config)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_I2C, cfn_hal_i2c_cfg_get, driver, error, config);
    return error;
}

/**
 * @brief cfn_hal_i2c_cfg_set implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param config Pointer to the peripheral configuration structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_i2c_cfg_set(cfn_hal_i2c_t *driver, const cfn_hal_i2c_config_t *config)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_I2C, cfn_hal_i2c_cfg_set, driver, error, config);
    return error;
}

/**
 * @brief cfn_hal_i2c_cfg_tx_enable implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_i2c_cfg_tx_enable(cfn_hal_i2c_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_LOCK(driver, CFN_HAL_MAX_DELAY);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_I2C, cfn_hal_i2c_cfg_tx_enable, driver, error);
    CFN_HAL_UNLOCK(driver);
    return error;
}

/**
 * @brief cfn_hal_i2c_cfg_rx_enable implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_i2c_cfg_rx_enable(cfn_hal_i2c_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_LOCK(driver, CFN_HAL_MAX_DELAY);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_I2C, cfn_hal_i2c_cfg_rx_enable, driver, error);
    CFN_HAL_UNLOCK(driver);
    return error;
}

/**
 * @brief cfn_hal_i2c_cfg_irq_enable implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param irq Interrupt type or event identifier.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_i2c_cfg_irq_enable(cfn_hal_i2c_t *driver, cfn_hal_i2c_interrupts_t irq)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_I2C, cfn_hal_i2c_cfg_irq_enable, driver, error, irq);
    return error;
}

/**
 * @brief cfn_hal_i2c_cfg_tx_disable implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_i2c_cfg_tx_disable(cfn_hal_i2c_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_LOCK(driver, CFN_HAL_MAX_DELAY);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_I2C, cfn_hal_i2c_cfg_tx_disable, driver, error);
    CFN_HAL_UNLOCK(driver);
    return error;
}

/**
 * @brief cfn_hal_i2c_cfg_rx_disable implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_i2c_cfg_rx_disable(cfn_hal_i2c_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_LOCK(driver, CFN_HAL_MAX_DELAY);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_I2C, cfn_hal_i2c_cfg_rx_disable, driver, error);
    CFN_HAL_UNLOCK(driver);
    return error;
}

/**
 * @brief cfn_hal_i2c_cfg_irq_disable implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param irq Interrupt type or event identifier.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_i2c_cfg_irq_disable(cfn_hal_i2c_t *driver, cfn_hal_i2c_interrupts_t irq)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_I2C, cfn_hal_i2c_cfg_irq_disable, driver, error, irq);
    return error;
}

/**
 * @brief cfn_hal_i2c_xfr_dma implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param xfr Pointer to the transaction structure defining the transfer.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_i2c_xfr_dma(cfn_hal_i2c_t *driver, cfn_hal_i2c_transaction_t *xfr)
{
    cfn_hal_error_code_t error = CFN_HAL_LOCK(driver, CFN_HAL_MAX_DELAY);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_I2C, cfn_hal_i2c_xfr_dma, driver, error, xfr);
    CFN_HAL_UNLOCK(driver);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif // CAFFEINE_HAL_HAL_I2C_H
