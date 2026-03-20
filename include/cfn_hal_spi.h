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
#include "cfn_hal_base.h"
#include "cfn_hal_gpio.h"

/* Defines ----------------------------------------------------------*/

/* Types Enums ------------------------------------------------------*/

/**
 * @brief SPI nominal event flags.
 */
typedef enum
{
    CFN_HAL_SPI_EVENT_NONE        = 0,
    CFN_HAL_SPI_EVENT_TX_COMPLETE = CFN_HAL_BIT(0), /*!< Data transmission finished */
    CFN_HAL_SPI_EVENT_RX_READY    = CFN_HAL_BIT(1), /*!< Data reception finished */
} cfn_hal_spi_event_t;

/**
 * @brief SPI exception error flags.
 */
typedef enum
{
    CFN_HAL_SPI_ERROR_NONE    = 0,
    CFN_HAL_SPI_ERROR_FRAMING = CFN_HAL_BIT(0), /*!< Frame synchronization error */
    CFN_HAL_SPI_ERROR_OVERRUN = CFN_HAL_BIT(1), /*!< Rx overrun or Tx underrun */
    CFN_HAL_SPI_ERROR_CRC     = CFN_HAL_BIT(2), /*!< CRC checksum mismatch */
    CFN_HAL_SPI_ERROR_GENERAL = CFN_HAL_BIT(3), /*!< General hardware error */
} cfn_hal_spi_error_t;

/**
 * @brief SPI clock phase and polarity formats.
 */
typedef enum
{
    CFN_HAL_SPI_CONFIG_FMT_POL0_PHA0, /*!< CPOL=0, CPHA=0 */
    CFN_HAL_SPI_CONFIG_FMT_POL0_PHA1, /*!< CPOL=0, CPHA=1 */
    CFN_HAL_SPI_CONFIG_FMT_POL1_PHA0, /*!< CPOL=1, CPHA=0 */
    CFN_HAL_SPI_CONFIG_FMT_POL1_PHA1, /*!< CPOL=1, CPHA=1 */

    CFN_HAL_SPI_CONFIG_FMT_MAX
} cfn_hal_spi_config_format_t;

/**
 * @brief SPI Chip Select (CS) management modes.
 */
typedef enum
{
    CFN_HAL_SPI_CONFIG_CS_USER_CONTROLLED, /*!< CS toggled manually by application */
    CFN_HAL_SPI_CONFIG_CS_HAL_CONTROLLED,  /*!< CS toggled by HAL during xfr */
    CFN_HAL_SPI_CONFIG_CS_HW_CONTROLLED,   /*!< CS toggled by hardware peripheral */

    CFN_HAL_SPI_CONFIG_CS_MAX
} cfn_hal_spi_config_cs_mode_t;

/* Types Structs ----------------------------------------------------*/

/**
 * @brief SPI hardware physical mapping.
 */
typedef struct
{
    void                      *instance;  /*!< Peripheral base instance */
    cfn_hal_gpio_pin_handle_t *mosi;      /*!< MOSI pin driver mapping */
    cfn_hal_gpio_pin_handle_t *miso;      /*!< MISO pin driver mapping */
    cfn_hal_gpio_pin_handle_t *sck;       /*!< SCK pin driver mapping */
    void                      *user_data; /*!< Peripheral instance user argument */
} cfn_hal_spi_phy_t;

/**
 * @brief SPI configuration structure.
 */
typedef struct
{
    uint32_t                     bitrate;   /*!< Bus frequency in bps */
    uint32_t                     data_size; /*!< Bits per frame (e.g. 8, 16) */
    cfn_hal_spi_config_format_t  fmt;       /*!< Clock phase/polarity */
    cfn_hal_spi_config_cs_mode_t cs_mode;   /*!< Chip Select behavior */
    void                        *custom;    /*!< Vendor-specific custom configuration */
} cfn_hal_spi_config_t;

/**
 * @brief SPI transaction configuration.
 */
typedef struct
{
    const uint8_t             *tx_payload;   /*!< Outgoing data buffer */
    uint8_t                   *rx_payload;   /*!< Incoming data buffer */
    size_t                     nbr_of_bytes; /*!< Bytes to exchange */
    cfn_hal_gpio_pin_handle_t *cs;           /*!< Target CS driver mapping if HAL controlled */
} cfn_hal_spi_transaction_t;

typedef struct cfn_hal_spi_s     cfn_hal_spi_t;
typedef struct cfn_hal_spi_api_s cfn_hal_spi_api_t;

/**
 * @brief SPI callback signature.
 * @param driver Pointer to the SPI driver instance.
 * @param event_mask Mask of triggered nominal events.
 * @param error_mask Mask of triggered exception errors.
 * @param data Pointer to the transaction buffer (if applicable).
 * @param nbr_of_bytes Number of bytes transferred.
 * @param user_arg User-defined argument passed during registration.
 */
typedef void (*cfn_hal_spi_callback_t)(cfn_hal_spi_t *driver,
                                       uint32_t       event_mask,
                                       uint32_t       error_mask,
                                       uint8_t       *data,
                                       size_t         nbr_of_bytes,
                                       void          *user_arg);

/**
 * @brief SPI Virtual Method Table (VMT).
 */
struct cfn_hal_spi_api_s
{
    cfn_hal_api_base_t base;

    /* SPI Specific Extensions */
    cfn_hal_error_code_t (*xfr_irq)(cfn_hal_spi_t *driver, const cfn_hal_spi_transaction_t *xfr);
    cfn_hal_error_code_t (*xfr_irq_abort)(cfn_hal_spi_t *driver);
    cfn_hal_error_code_t (*xfr_polling)(cfn_hal_spi_t *driver, const cfn_hal_spi_transaction_t *xfr, uint32_t timeout);
    cfn_hal_error_code_t (*xfr_dma)(cfn_hal_spi_t *driver, const cfn_hal_spi_transaction_t *xfr);
};

CFN_HAL_VMT_CHECK(struct cfn_hal_spi_api_s);

CFN_HAL_CREATE_DRIVER_TYPE(spi, cfn_hal_spi_config_t, cfn_hal_spi_api_t, cfn_hal_spi_phy_t, cfn_hal_spi_callback_t);

#define CFN_HAL_SPI_INITIALIZER(api_ptr, phy_ptr, config_ptr)                                                          \
    CFN_HAL_DRIVER_INITIALIZER(CFN_HAL_PERIPHERAL_TYPE_SPI, api_ptr, phy_ptr, config_ptr)

/* Functions inline ------------------------------------------------- */

/**
 * @brief Validates the SPI configuration.
 * @param config Pointer to the configuration structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_spi_config_validate(const cfn_hal_spi_t        *driver,
                                                                const cfn_hal_spi_config_t *config)
{
    if (driver == NULL || config == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    if (config->fmt >= CFN_HAL_SPI_CONFIG_FMT_MAX || config->cs_mode >= CFN_HAL_SPI_CONFIG_CS_MAX)
    {
        return CFN_HAL_ERROR_BAD_CONFIG;
    }

    return cfn_hal_base_config_validate(&driver->base, CFN_HAL_PERIPHERAL_TYPE_SPI, config);
}

/**
 * @brief Initializes the SPI driver.
 * @param driver Pointer to the SPI driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_spi_init(cfn_hal_spi_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->base.vmt = (const struct cfn_hal_api_base_s *) driver->api;
    return cfn_hal_base_init(&driver->base, CFN_HAL_PERIPHERAL_TYPE_SPI);
}

/**
 * @brief Deinitializes the SPI driver.
 * @param driver Pointer to the SPI driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_spi_deinit(cfn_hal_spi_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_deinit(&driver->base, CFN_HAL_PERIPHERAL_TYPE_SPI);
}

/**
 * @brief Sets the SPI configuration.
 * @param driver Pointer to the SPI driver instance.
 * @param config Pointer to the configuration structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_spi_config_set(cfn_hal_spi_t *driver, const cfn_hal_spi_config_t *config)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    {
        driver->config = config;
    }
    return cfn_hal_base_config_set(&driver->base, CFN_HAL_PERIPHERAL_TYPE_SPI, (const void *) config);
}

/**
 * @brief Gets the current SPI configuration.
 * @param driver Pointer to the SPI driver instance.
 * @param config [out] Pointer to store the configuration.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_spi_config_get(cfn_hal_spi_t *driver, cfn_hal_spi_config_t *config)
{
    if (!driver || !config || !driver->config)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    *config = *(driver->config);
    return CFN_HAL_ERROR_OK;
}

/**
 * @brief Registers a callback for SPI events and errors.
 * @param driver Pointer to the SPI driver instance.
 * @param callback The callback function to register.
 * @param user_arg User-defined argument passed to the callback.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_spi_callback_register(cfn_hal_spi_t               *driver,
                                                                  const cfn_hal_spi_callback_t callback,
                                                                  void                        *user_arg)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    {
        driver->cb          = callback;
        driver->cb_user_arg = user_arg;
    }
    return cfn_hal_base_callback_register(
        &driver->base, CFN_HAL_PERIPHERAL_TYPE_SPI, (cfn_hal_callback_t) callback, user_arg);
}

/**
 * @brief Sets the SPI power state.
 * @param driver Pointer to the SPI driver instance.
 * @param state Target power state.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_spi_power_state_set(cfn_hal_spi_t *driver, cfn_hal_power_state_t state)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_power_state_set(&driver->base, CFN_HAL_PERIPHERAL_TYPE_SPI, state);
}

/**
 * @brief Enables one or more SPI nominal events.
 * @param driver Pointer to the SPI driver instance.
 * @param event_mask Mask of events to enable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_spi_event_enable(cfn_hal_spi_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_enable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_SPI, event_mask);
}

/**
 * @brief Disables one or more SPI nominal events.
 * @param driver Pointer to the SPI driver instance.
 * @param event_mask Mask of events to disable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_spi_event_disable(cfn_hal_spi_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_disable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_SPI, event_mask);
}

/**
 * @brief Retrieves the current SPI nominal event status.
 * @param driver Pointer to the SPI driver instance.
 * @param event_mask [out] Pointer to store the event mask.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_spi_event_get(cfn_hal_spi_t *driver, uint32_t *event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_get(&driver->base, CFN_HAL_PERIPHERAL_TYPE_SPI, event_mask);
}

/**
 * @brief Enables one or more SPI exception errors.
 * @param driver Pointer to the SPI driver instance.
 * @param error_mask Mask of errors to enable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_spi_error_enable(cfn_hal_spi_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_enable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_SPI, error_mask);
}

/**
 * @brief Disables one or more SPI exception errors.
 * @param driver Pointer to the SPI driver instance.
 * @param error_mask Mask of errors to disable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_spi_error_disable(cfn_hal_spi_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_disable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_SPI, error_mask);
}

/**
 * @brief Retrieves the current SPI exception error status.
 * @param driver Pointer to the SPI driver instance.
 * @param error_mask [out] Pointer to store the error mask.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_spi_error_get(cfn_hal_spi_t *driver, uint32_t *error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_get(&driver->base, CFN_HAL_PERIPHERAL_TYPE_SPI, error_mask);
}

/* SPI Specific Functions ------------------------------------------- */

/**
 * @brief Starts an SPI transaction using interrupts (non-blocking).
 * @param driver Pointer to the SPI driver instance.
 * @param xfr Pointer to the transaction configuration.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_spi_xfr_irq(cfn_hal_spi_t *driver, const cfn_hal_spi_transaction_t *xfr)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_SPI, xfr_irq, driver, error, xfr);
    return error;
}

/**
 * @brief Aborts an ongoing non-blocking SPI transaction.
 * @param driver Pointer to the SPI driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_spi_xfr_irq_abort(cfn_hal_spi_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_SPI, xfr_irq_abort, driver, error);
    return error;
}

/**
 * @brief Executes an SPI transaction using polling (blocking).
 * @param driver Pointer to the SPI driver instance.
 * @param xfr Pointer to the transaction configuration.
 * @param timeout Timeout duration in milliseconds.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_spi_xfr_polling(cfn_hal_spi_t                   *driver,
                                                            const cfn_hal_spi_transaction_t *xfr,
                                                            uint32_t                         timeout)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_SPI, xfr_polling, driver, error, xfr, timeout);
    return error;
}

/**
 * @brief Starts an SPI transaction using DMA (non-blocking).
 * @param driver Pointer to the SPI driver instance.
 * @param xfr Pointer to the transaction configuration.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_spi_xfr_dma(cfn_hal_spi_t *driver, const cfn_hal_spi_transaction_t *xfr)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_SPI, xfr_dma, driver, error, xfr);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif // CAFFEINE_HAL_HAL_SPI_H
