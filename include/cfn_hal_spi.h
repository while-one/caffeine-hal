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
 * @file cfn_hal_spi.h
 * @brief Serial Peripheral Interface (SPI) HAL API.
 */

#ifndef CAFFEINE_HAL_HAL_SPI_H
#define CAFFEINE_HAL_HAL_SPI_H

#ifdef __cplusplus
extern "C"
{
#endif
/* Includes ---------------------------------------------------------*/
#include "cfn_hal_types.h"
#include "cfn_hal.h"
/* Defines ----------------------------------------------------------*/
/* Macro ------------------------------------------------------------*/
/* Types Enums ------------------------------------------------------*/
typedef enum
{
    CFN_HAL_SPI_INTERRUPT_TX_COMPLETE,
    CFN_HAL_SPI_INTERRUPT_RX_READY,

    CFN_HAL_SPI_INTERRUPT_ERROR_FRAMING,
    CFN_HAL_SPI_INTERRUPT_ERROR_OVERRUN,
    CFN_HAL_SPI_INTERRUPT_ERROR_CRC,
} cfn_hal_spi_interrupts_t;

typedef enum
{
    CFN_HAL_SPI_CFG_FMT_POL0_PHA0,
    CFN_HAL_SPI_CFG_FMT_POL0_PHA1,
    CFN_HAL_SPI_CFG_FMT_POL1_PHA0,
    CFN_HAL_SPI_CFG_FMT_POL1_PHA1,
} cfn_hal_spi_cfg_format_t;

typedef enum
{
    CFN_HAL_SPI_CFG_CS_USER_CONTROLLED,
    CFN_HAL_SPI_CFG_CS_HAL_CONTROLLED,
    CFN_HAL_SPI_CFG_CS_HW_CONTROLLED
} cfn_hal_spi_cfg_cs_t;

/* Types Structs ----------------------------------------------------*/
typedef struct
{
    void             *port;
    cfn_hal_driver_t *mosi;
    cfn_hal_driver_t *miso;
    cfn_hal_driver_t *sck;
    void             *user_data;
} cfn_hal_spi_phy_t;

typedef struct
{
    uint32_t                 bitrate;
    uint32_t                 data_size;
    cfn_hal_spi_cfg_format_t fmt;
    cfn_hal_spi_cfg_cs_t     chip_select;
    void                    *custom;
} cfn_hal_spi_config_t;

typedef struct
{
    const uint8_t    *tx_payload;
    uint8_t          *rx_payload;
    size_t            nbr_of_bytes;
    cfn_hal_driver_t *cs;
} cfn_hal_spi_transaction_t;

typedef struct cfn_hal_spi_s cfn_hal_spi_t;

typedef struct cfn_hal_spi_api_s cfn_hal_spi_api_t;

typedef void (*cfn_hal_spi_callback_t)(
    cfn_hal_spi_t *driver, cfn_hal_spi_interrupts_t interrupt, uint8_t *data, size_t nbr_of_bytes, void *user_arg);

/**
 * @brief Virtual Method Table (VMT) for the peripheral.
 * Hardware-specific implementations must populate these function pointers.
 * Functions can be set to NULL if not implemented.
 */
struct cfn_hal_spi_api_s
{
    cfn_hal_error_code_t (*cfn_hal_spi_register_cb)(cfn_hal_spi_t *driver, cfn_hal_spi_callback_t cb, void *user_arg);
    cfn_hal_error_code_t (*cfn_hal_spi_set_cb_arg)(cfn_hal_spi_t *driver, void *user_arg);
    cfn_hal_error_code_t (*cfn_hal_spi_xfr_irq)(cfn_hal_spi_t *driver, cfn_hal_spi_transaction_t *xfr);
    cfn_hal_error_code_t (*cfn_hal_spi_xfr_irq_abort)(cfn_hal_spi_t *driver);
    cfn_hal_error_code_t (*cfn_hal_spi_xfr_polling)(
        cfn_hal_spi_t *driver, cfn_hal_spi_transaction_t *xfr, uint32_t timeout);
    cfn_hal_error_code_t (*cfn_hal_spi_check_error)(cfn_hal_spi_t *driver);
    cfn_hal_error_code_t (*cfn_hal_spi_init)(cfn_hal_spi_t *driver);
    cfn_hal_error_code_t (*cfn_hal_spi_deinit)(cfn_hal_spi_t *driver);
    cfn_hal_error_code_t (*cfn_hal_spi_cfg_get)(cfn_hal_spi_t *driver, cfn_hal_spi_config_t *config);
    cfn_hal_error_code_t (*cfn_hal_spi_cfg_set)(cfn_hal_spi_t *driver, const cfn_hal_spi_config_t *config);
    cfn_hal_error_code_t (*cfn_hal_spi_cfg_tx_enable)(cfn_hal_spi_t *driver);
    cfn_hal_error_code_t (*cfn_hal_spi_cfg_rx_enable)(cfn_hal_spi_t *driver);
    cfn_hal_error_code_t (*cfn_hal_spi_cfg_irq_enable)(cfn_hal_spi_t *driver, cfn_hal_spi_interrupts_t irq);
    cfn_hal_error_code_t (*cfn_hal_spi_cfg_tx_disable)(cfn_hal_spi_t *driver);
    cfn_hal_error_code_t (*cfn_hal_spi_cfg_rx_disable)(cfn_hal_spi_t *driver);
    cfn_hal_error_code_t (*cfn_hal_spi_cfg_irq_disable)(cfn_hal_spi_t *driver, cfn_hal_spi_interrupts_t irq);
    cfn_hal_error_code_t (*cfn_hal_spi_xfr_dma)(cfn_hal_spi_t *driver, cfn_hal_spi_transaction_t *xfr);
};

/**
 * @brief Generated driver structure for spi.
 *
 * This macro expands to the following structure:
 * \code{c}
 * struct cfn_hal_spi_s {
 *     cfn_hal_driver_t       base;
 *     const cfn_hal_spi_config_t *config;
 *     const cfn_hal_spi_api_t    *api;
 *     const cfn_hal_spi_phy_t    *phy;
 *     cfn_hal_spi_callback_t            cb;
 *     void              *cb_user_arg;
 * };
 * typedef struct cfn_hal_spi_s cfn_hal_spi_t;
 * \endcode
 */
CFN_HAL_CREATE_DRIVER_TYPE(spi, cfn_hal_spi_config_t, cfn_hal_spi_api_t, cfn_hal_spi_phy_t, cfn_hal_spi_callback_t);

/* Functions inline  ---------------------------------------------*/
/**
 * @brief cfn_hal_spi_register_cb implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param cb Callback function to register.
 * @param user_arg User-defined argument passed to the callback.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t
cfn_hal_spi_register_cb(cfn_hal_spi_t *driver, cfn_hal_spi_callback_t cb, void *user_arg)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_SPI, cfn_hal_spi_register_cb, driver, error, cb, user_arg);
    return error;
}

/**
 * @brief cfn_hal_spi_set_cb_arg implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param user_arg User-defined argument passed to the callback.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_spi_set_cb_arg(cfn_hal_spi_t *driver, void *user_arg)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_SPI, cfn_hal_spi_set_cb_arg, driver, error, user_arg);
    return error;
}

/**
 * @brief cfn_hal_spi_xfr_irq implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param xfr Pointer to the transaction structure defining the transfer.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_spi_xfr_irq(cfn_hal_spi_t *driver, cfn_hal_spi_transaction_t *xfr)
{
    cfn_hal_error_code_t error = CFN_HAL_LOCK(driver, CFN_HAL_MAX_DELAY);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_SPI, cfn_hal_spi_xfr_irq, driver, error, xfr);
    CFN_HAL_UNLOCK(driver);
    return error;
}

/**
 * @brief cfn_hal_spi_xfr_irq_abort implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_spi_xfr_irq_abort(cfn_hal_spi_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_LOCK(driver, CFN_HAL_MAX_DELAY);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_SPI, cfn_hal_spi_xfr_irq_abort, driver, error);
    CFN_HAL_UNLOCK(driver);
    return error;
}

/**
 * @brief cfn_hal_spi_xfr_polling implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param xfr Pointer to the transaction structure defining the transfer.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t
cfn_hal_spi_xfr_polling(cfn_hal_spi_t *driver, cfn_hal_spi_transaction_t *xfr, uint32_t timeout)
{
    cfn_hal_error_code_t error = CFN_HAL_LOCK(driver, CFN_HAL_MAX_DELAY);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_SPI, cfn_hal_spi_xfr_polling, driver, error, xfr, timeout);
    CFN_HAL_UNLOCK(driver);
    return error;
}

/**
 * @brief cfn_hal_spi_check_error implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_spi_check_error(cfn_hal_spi_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_SPI, cfn_hal_spi_check_error, driver, error);
    return error;
}

/**
 * @brief cfn_hal_spi_init implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_spi_init(cfn_hal_spi_t *driver)
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
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_SPI, cfn_hal_spi_init, driver, error);
    if (error == CFN_HAL_ERROR_OK && driver)
    {
        driver->base.status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    }
    return error;
}

/**
 * @brief cfn_hal_spi_cfg_get implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param config Pointer to the peripheral configuration structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_spi_cfg_get(cfn_hal_spi_t *driver, cfn_hal_spi_config_t *config)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_SPI, cfn_hal_spi_cfg_get, driver, error, config);
    return error;
}

/**
 * @brief cfn_hal_spi_cfg_set implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param config Pointer to the peripheral configuration structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_spi_cfg_set(cfn_hal_spi_t *driver, const cfn_hal_spi_config_t *config)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_SPI, cfn_hal_spi_cfg_set, driver, error, config);
    return error;
}

/**
 * @brief cfn_hal_spi_cfg_tx_enable implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_spi_cfg_tx_enable(cfn_hal_spi_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_LOCK(driver, CFN_HAL_MAX_DELAY);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_SPI, cfn_hal_spi_cfg_tx_enable, driver, error);
    CFN_HAL_UNLOCK(driver);
    return error;
}

/**
 * @brief cfn_hal_spi_cfg_rx_enable implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_spi_cfg_rx_enable(cfn_hal_spi_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_LOCK(driver, CFN_HAL_MAX_DELAY);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_SPI, cfn_hal_spi_cfg_rx_enable, driver, error);
    CFN_HAL_UNLOCK(driver);
    return error;
}

/**
 * @brief cfn_hal_spi_cfg_irq_enable implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param irq Interrupt type or event identifier.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_spi_cfg_irq_enable(cfn_hal_spi_t *driver, cfn_hal_spi_interrupts_t irq)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_SPI, cfn_hal_spi_cfg_irq_enable, driver, error, irq);
    return error;
}

/**
 * @brief cfn_hal_spi_cfg_tx_disable implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_spi_cfg_tx_disable(cfn_hal_spi_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_LOCK(driver, CFN_HAL_MAX_DELAY);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_SPI, cfn_hal_spi_cfg_tx_disable, driver, error);
    CFN_HAL_UNLOCK(driver);
    return error;
}

/**
 * @brief cfn_hal_spi_cfg_rx_disable implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_spi_cfg_rx_disable(cfn_hal_spi_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_LOCK(driver, CFN_HAL_MAX_DELAY);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_SPI, cfn_hal_spi_cfg_rx_disable, driver, error);
    CFN_HAL_UNLOCK(driver);
    return error;
}

/**
 * @brief cfn_hal_spi_cfg_irq_disable implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param irq Interrupt type or event identifier.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_spi_cfg_irq_disable(cfn_hal_spi_t *driver, cfn_hal_spi_interrupts_t irq)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_FAIL;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_SPI, cfn_hal_spi_cfg_irq_disable, driver, error, irq);
    return error;
}

/**
 * @brief cfn_hal_spi_xfr_dma implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param xfr Pointer to the transaction structure defining the transfer.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_spi_xfr_dma(cfn_hal_spi_t *driver, cfn_hal_spi_transaction_t *xfr)
{
    cfn_hal_error_code_t error = CFN_HAL_LOCK(driver, CFN_HAL_MAX_DELAY);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_SPI, cfn_hal_spi_xfr_dma, driver, error, xfr);
    CFN_HAL_UNLOCK(driver);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif // CAFFEINE_HAL_HAL_SPI_H
