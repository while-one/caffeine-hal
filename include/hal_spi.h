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
 * @file hal_spi.h
 * @brief Serial Peripheral Interface (SPI) HAL API.
 */

#ifndef CAFFEINE_HAL_HAL_SPI_H
#define CAFFEINE_HAL_HAL_SPI_H

#ifdef __cplusplus
extern "C"
{
#endif
/* Includes ---------------------------------------------------------*/
#include "hal_types.h"
#include "hal.h"
/* Defines ----------------------------------------------------------*/
/* Macro ------------------------------------------------------------*/
/* Types Enums ------------------------------------------------------*/
typedef enum
{
    HAL_SPI_INTERRUPT_TX_COMPLETE,
    HAL_SPI_INTERRUPT_RX_READY,

    HAL_SPI_INTERRUPT_ERROR_FRAMING,
    HAL_SPI_INTERRUPT_ERROR_OVERRUN,
    HAL_SPI_INTERRUPT_ERROR_CRC,
} hal_spi_interrupts_t;

typedef enum
{
    HAL_SPI_CFG_FMT_POL0_PHA0,
    HAL_SPI_CFG_FMT_POL0_PHA1,
    HAL_SPI_CFG_FMT_POL1_PHA0,
    HAL_SPI_CFG_FMT_POL1_PHA1,
} hal_spi_cfg_format_t;

typedef enum
{
    HAL_SPI_CFG_CS_USER_CONTROLLED,
    HAL_SPI_CFG_CS_HAL_CONTROLLED,
    HAL_SPI_CFG_CS_HW_CONTROLLED
} hal_spi_cfg_cs_t;

/* Types Structs ----------------------------------------------------*/
typedef struct
{
    void         *port;
    hal_driver_t *mosi;
    hal_driver_t *miso;
    hal_driver_t *sck;
    void         *user_data;
} hal_spi_phy_t;

typedef struct
{
    uint32_t             bitrate;
    uint32_t             data_size;
    hal_spi_cfg_format_t fmt;
    hal_spi_cfg_cs_t     chip_select;
    void                *custom;
} hal_spi_config_t;

typedef struct
{
    const uint8_t *tx_payload;
    uint8_t       *rx_payload;
    size_t         nbr_of_bytes;
    hal_driver_t  *cs;
} hal_spi_transaction_t;

typedef struct hal_spi_s hal_spi_t;

typedef struct hal_spi_api_s hal_spi_api_t;

typedef void (*hal_spi_callback_t) (hal_spi_t *driver, hal_spi_interrupts_t interrupt, uint8_t *data,
                                    size_t nbr_of_bytes, void *user_arg);

/**
 * @brief Virtual Method Table (VMT) for the peripheral.
 * Hardware-specific implementations must populate these function pointers.
 * Functions can be set to NULL if not implemented.
 */
struct hal_spi_api_s
{
    hal_error_code_t (*hal_spi_register_cb) (hal_spi_t *driver, hal_spi_callback_t cb, void *user_arg);
    hal_error_code_t (*hal_spi_set_cb_arg) (hal_spi_t *driver, void *user_arg);
    hal_error_code_t (*hal_spi_xfr_irq) (hal_spi_t *driver, hal_spi_transaction_t *xfr);
    hal_error_code_t (*hal_spi_xfr_irq_abort) (hal_spi_t *driver);
    hal_error_code_t (*hal_spi_xfr_polling) (hal_spi_t *driver, hal_spi_transaction_t *xfr, uint32_t timeout);
    hal_error_code_t (*hal_spi_check_error) (hal_spi_t *driver);
    hal_error_code_t (*hal_spi_init) (hal_spi_t *driver);
    hal_error_code_t (*hal_spi_deinit) (hal_spi_t *driver);
    hal_error_code_t (*hal_spi_cfg_get) (hal_spi_t *driver, hal_spi_config_t *config);
    hal_error_code_t (*hal_spi_cfg_set) (hal_spi_t *driver, const hal_spi_config_t *config);
    hal_error_code_t (*hal_spi_cfg_tx_enable) (hal_spi_t *driver);
    hal_error_code_t (*hal_spi_cfg_rx_enable) (hal_spi_t *driver);
    hal_error_code_t (*hal_spi_cfg_irq_enable) (hal_spi_t *driver, hal_spi_interrupts_t irq);
    hal_error_code_t (*hal_spi_cfg_tx_disable) (hal_spi_t *driver);
    hal_error_code_t (*hal_spi_cfg_rx_disable) (hal_spi_t *driver);
    hal_error_code_t (*hal_spi_cfg_irq_disable) (hal_spi_t *driver, hal_spi_interrupts_t irq);
    hal_error_code_t (*hal_spi_xfr_dma) (hal_spi_t *driver, hal_spi_transaction_t *xfr);
};

HAL_CREATE_DRIVER_TYPE (spi, hal_spi_config_t, hal_spi_api_t, hal_spi_phy_t, hal_spi_callback_t);

/* Functions inline  ---------------------------------------------*/
/**
 * @brief hal_spi_register_cb implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param cb Callback function to register.
 * @param user_arg User-defined argument passed to the callback.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_spi_register_cb (hal_spi_t *driver, hal_spi_callback_t cb, void *user_arg)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_SPI, hal_spi_register_cb, driver, error, cb, user_arg);
    return error;
}

/**
 * @brief hal_spi_set_cb_arg implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param user_arg User-defined argument passed to the callback.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_spi_set_cb_arg (hal_spi_t *driver, void *user_arg)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_SPI, hal_spi_set_cb_arg, driver, error, user_arg);
    return error;
}

/**
 * @brief hal_spi_xfr_irq implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param xfr Pointer to the transaction structure defining the transfer.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_spi_xfr_irq (hal_spi_t *driver, hal_spi_transaction_t *xfr)
{
    hal_error_code_t error = HAL_LOCK (driver, HAL_MAX_DELAY);
    if (error != HAL_ERROR_OK)
    {
        return error;
    }
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_SPI, hal_spi_xfr_irq, driver, error, xfr);
    HAL_UNLOCK (driver);
    return error;
}

/**
 * @brief hal_spi_xfr_irq_abort implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_spi_xfr_irq_abort (hal_spi_t *driver)
{
    hal_error_code_t error = HAL_LOCK (driver, HAL_MAX_DELAY);
    if (error != HAL_ERROR_OK)
    {
        return error;
    }
    HAL_CHECK_AND_CALL_FUNC (HAL_PERIPHERAL_TYPE_SPI, hal_spi_xfr_irq_abort, driver, error);
    HAL_UNLOCK (driver);
    return error;
}

/**
 * @brief hal_spi_xfr_polling implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param xfr Pointer to the transaction structure defining the transfer.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_spi_xfr_polling (hal_spi_t *driver, hal_spi_transaction_t *xfr, uint32_t timeout)
{
    hal_error_code_t error = HAL_LOCK (driver, HAL_MAX_DELAY);
    if (error != HAL_ERROR_OK)
    {
        return error;
    }
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_SPI, hal_spi_xfr_polling, driver, error, xfr, timeout);
    HAL_UNLOCK (driver);
    return error;
}

/**
 * @brief hal_spi_check_error implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_spi_check_error (hal_spi_t *driver)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC (HAL_PERIPHERAL_TYPE_SPI, hal_spi_check_error, driver, error);
    return error;
}

/**
 * @brief hal_spi_init implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_spi_init (hal_spi_t *driver)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    if (driver)
    {
        driver->base.type = HAL_PERIPHERAL_TYPE_SPI;
        if (driver->base.on_config)
        {
            error = driver->base.on_config (&driver->base, DRIVER_CONFIG_INIT);
            if (error != HAL_ERROR_OK)
                return error;
        }
    }
    HAL_CHECK_AND_CALL_FUNC (HAL_PERIPHERAL_TYPE_SPI, hal_spi_init, driver, error);
    return error;
}

/**
 * @brief hal_spi_deinit implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_spi_deinit (hal_spi_t *driver)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC (HAL_PERIPHERAL_TYPE_SPI, hal_spi_deinit, driver, error);
    if (error == HAL_ERROR_OK && driver && driver->base.on_config)
    {
        error = driver->base.on_config (&driver->base, DRIVER_CONFIG_DEINIT);
    }
    return error;
}

/**
 * @brief hal_spi_cfg_get implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param config Pointer to the peripheral configuration structure.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_spi_cfg_get (hal_spi_t *driver, hal_spi_config_t *config)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_SPI, hal_spi_cfg_get, driver, error, config);
    return error;
}

/**
 * @brief hal_spi_cfg_set implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param config Pointer to the peripheral configuration structure.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_spi_cfg_set (hal_spi_t *driver, const hal_spi_config_t *config)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_SPI, hal_spi_cfg_set, driver, error, config);
    return error;
}

/**
 * @brief hal_spi_cfg_tx_enable implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_spi_cfg_tx_enable (hal_spi_t *driver)
{
    hal_error_code_t error = HAL_LOCK (driver, HAL_MAX_DELAY);
    if (error != HAL_ERROR_OK)
    {
        return error;
    }
    HAL_CHECK_AND_CALL_FUNC (HAL_PERIPHERAL_TYPE_SPI, hal_spi_cfg_tx_enable, driver, error);
    HAL_UNLOCK (driver);
    return error;
}

/**
 * @brief hal_spi_cfg_rx_enable implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_spi_cfg_rx_enable (hal_spi_t *driver)
{
    hal_error_code_t error = HAL_LOCK (driver, HAL_MAX_DELAY);
    if (error != HAL_ERROR_OK)
    {
        return error;
    }
    HAL_CHECK_AND_CALL_FUNC (HAL_PERIPHERAL_TYPE_SPI, hal_spi_cfg_rx_enable, driver, error);
    HAL_UNLOCK (driver);
    return error;
}

/**
 * @brief hal_spi_cfg_irq_enable implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param irq Interrupt type or event identifier.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_spi_cfg_irq_enable (hal_spi_t *driver, hal_spi_interrupts_t irq)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_SPI, hal_spi_cfg_irq_enable, driver, error, irq);
    return error;
}

/**
 * @brief hal_spi_cfg_tx_disable implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_spi_cfg_tx_disable (hal_spi_t *driver)
{
    hal_error_code_t error = HAL_LOCK (driver, HAL_MAX_DELAY);
    if (error != HAL_ERROR_OK)
    {
        return error;
    }
    HAL_CHECK_AND_CALL_FUNC (HAL_PERIPHERAL_TYPE_SPI, hal_spi_cfg_tx_disable, driver, error);
    HAL_UNLOCK (driver);
    return error;
}

/**
 * @brief hal_spi_cfg_rx_disable implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_spi_cfg_rx_disable (hal_spi_t *driver)
{
    hal_error_code_t error = HAL_LOCK (driver, HAL_MAX_DELAY);
    if (error != HAL_ERROR_OK)
    {
        return error;
    }
    HAL_CHECK_AND_CALL_FUNC (HAL_PERIPHERAL_TYPE_SPI, hal_spi_cfg_rx_disable, driver, error);
    HAL_UNLOCK (driver);
    return error;
}

/**
 * @brief hal_spi_cfg_irq_disable implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param irq Interrupt type or event identifier.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_spi_cfg_irq_disable (hal_spi_t *driver, hal_spi_interrupts_t irq)
{
    hal_error_code_t error = HAL_ERROR_FAIL;
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_SPI, hal_spi_cfg_irq_disable, driver, error, irq);
    return error;
}

/**
 * @brief hal_spi_xfr_dma implementation.
 * @param driver Pointer to the peripheral driver instance.
 * @param xfr Pointer to the transaction structure defining the transfer.
 * @return HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline hal_error_code_t hal_spi_xfr_dma (hal_spi_t *driver, hal_spi_transaction_t *xfr)
{
    hal_error_code_t error = HAL_LOCK (driver, HAL_MAX_DELAY);
    if (error != HAL_ERROR_OK)
    {
        return error;
    }
    HAL_CHECK_AND_CALL_FUNC_VARG (HAL_PERIPHERAL_TYPE_SPI, hal_spi_xfr_dma, driver, error, xfr);
    HAL_UNLOCK (driver);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif // CAFFEINE_HAL_HAL_SPI_H
