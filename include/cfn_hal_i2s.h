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
 * @file cfn_hal_i2s.h
 * @brief I2S (Serial Audio Interface) HAL API.
 */

#ifndef CAFFEINE_HAL_HAL_I2S_H
#define CAFFEINE_HAL_HAL_I2S_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_hal.h"
#include "cfn_hal_base.h"
#include "cfn_hal_gpio.h"
#include "cfn_hal_types.h"

/* Defines ----------------------------------------------------------*/

/* Types Enums ------------------------------------------------------*/

/**
 * @brief I2S nominal event flags.
 */
typedef enum
{
    CFN_HAL_I2S_EVENT_NONE        = 0,
    CFN_HAL_I2S_EVENT_TX_COMPLETE = CFN_HAL_BIT(0), /*!< Audio data transmission complete */
    CFN_HAL_I2S_EVENT_RX_READY    = CFN_HAL_BIT(1), /*!< Audio data reception ready */
} cfn_hal_i2s_event_t;

/**
 * @brief I2S exception error flags.
 */
typedef enum
{
    CFN_HAL_I2S_ERROR_NONE    = 0,
    CFN_HAL_I2S_ERROR_OVERRUN = CFN_HAL_BIT(0), /*!< Rx overrun or Tx underrun */
    CFN_HAL_I2S_ERROR_SYNC    = CFN_HAL_BIT(1), /*!< Frame synchronization error */
    CFN_HAL_I2S_ERROR_GENERAL = CFN_HAL_BIT(2), /*!< General hardware error */
} cfn_hal_i2s_error_t;

/* Types Structs ----------------------------------------------------*/

/**
 * @brief I2S configuration structure.
 */
typedef struct
{
    uint32_t sample_rate; /*!< Audio sampling frequency in Hz */
    uint32_t data_format; /*!< Data bits and standard (e.g. Philips, MSB) */
    void    *custom;      /*!< Vendor-specific custom configuration */
} cfn_hal_i2s_config_t;

/**
 * @brief I2S hardware physical mapping.
 */
typedef struct
{
    void                      *instance; /*!< Peripheral base instance */
    cfn_hal_gpio_pin_handle_t *ck;       /*!< Continuous Serial Clock mapping */
    cfn_hal_gpio_pin_handle_t *ws;       /*!< Word Select mapping */
    cfn_hal_gpio_pin_handle_t *sd;       /*!< Serial Data mapping */
    cfn_hal_gpio_pin_handle_t *ext_sd;   /*!< Full-duplex Serial Data mapping */
    cfn_hal_gpio_pin_handle_t *mck;      /*!< Master Clock mapping */
    void                      *user_arg; /*!< Peripheral instance user argument */
} cfn_hal_i2s_phy_t;

typedef struct cfn_hal_i2s_s     cfn_hal_i2s_t;
typedef struct cfn_hal_i2s_api_s cfn_hal_i2s_api_t;

/**
 * @brief I2S callback signature.
 * @param driver Pointer to the I2S driver instance.
 * @param event_mask Mask of triggered nominal events.
 * @param error_mask Mask of triggered exception errors.
 * @param user_arg User-defined argument passed during registration.
 */
typedef void (*cfn_hal_i2s_callback_t)(cfn_hal_i2s_t *driver, uint32_t event_mask, uint32_t error_mask, void *user_arg);

/**
 * @brief I2S Virtual Method Table (VMT).
 */
struct cfn_hal_i2s_api_s
{
    cfn_hal_api_base_t base;

    /* I2S Specific Extensions */
    cfn_hal_error_code_t (*transmit_dma)(cfn_hal_i2s_t *driver, const uint16_t *data, size_t size);
    cfn_hal_error_code_t (*receive_dma)(cfn_hal_i2s_t *driver, uint16_t *data, size_t size);
    cfn_hal_error_code_t (*pause)(cfn_hal_i2s_t *driver);
    cfn_hal_error_code_t (*resume)(cfn_hal_i2s_t *driver);
    cfn_hal_error_code_t (*stop)(cfn_hal_i2s_t *driver);
};

CFN_HAL_VMT_CHECK(struct cfn_hal_i2s_api_s);

CFN_HAL_CREATE_DRIVER_TYPE(i2s, cfn_hal_i2s_config_t, cfn_hal_i2s_api_t, cfn_hal_i2s_phy_t, cfn_hal_i2s_callback_t);

#define CFN_HAL_I2S_INITIALIZER(api_ptr, phy_ptr, config_ptr)                                                          \
    CFN_HAL_DRIVER_INITIALIZER(CFN_HAL_PERIPHERAL_TYPE_I2S, api_ptr, phy_ptr, config_ptr)

/* Functions inline ------------------------------------------------- */

/**
 * @brief Validates the I2S configuration.
 * @param driver Pointer to the I2S driver instance.
 * @param config Pointer to the configuration structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_i2s_config_validate(const cfn_hal_i2s_t        *driver,
                                                                const cfn_hal_i2s_config_t *config)
{
    if (driver == NULL || config == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    return cfn_hal_base_config_validate(&driver->base, CFN_HAL_PERIPHERAL_TYPE_I2S, config);
}

/**
 * @brief Initializes the I2S driver.
 * @param driver Pointer to the I2S driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_i2s_init(cfn_hal_i2s_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->base.vmt           = (const struct cfn_hal_api_base_s *) driver->api;
    cfn_hal_error_code_t error = cfn_hal_i2s_config_validate(driver, driver->config);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    return cfn_hal_base_init(&driver->base, CFN_HAL_PERIPHERAL_TYPE_I2S);
}

/**
 * @brief Deinitializes the I2S driver.
 * @param driver Pointer to the I2S driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_i2s_deinit(cfn_hal_i2s_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_deinit(&driver->base, CFN_HAL_PERIPHERAL_TYPE_I2S);
}

/**
 * @brief Sets the I2S configuration.
 * @param driver Pointer to the I2S driver instance.
 * @param config Pointer to the configuration structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_i2s_config_set(cfn_hal_i2s_t *driver, const cfn_hal_i2s_config_t *config)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    cfn_hal_error_code_t error = cfn_hal_i2s_config_validate(driver, config);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    {
        driver->config = config;
    }
    return cfn_hal_base_config_set(&driver->base, CFN_HAL_PERIPHERAL_TYPE_I2S, (const void *) config);
}

/**
 * @brief Gets the current I2S configuration.
 * @param driver Pointer to the I2S driver instance.
 * @param config [out] Pointer to store the configuration.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_i2s_config_get(cfn_hal_i2s_t *driver, cfn_hal_i2s_config_t *config)
{
    if (!driver || !config || !driver->config)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    *config = *(driver->config);
    return CFN_HAL_ERROR_OK;
}

/**
 * @brief Registers a callback for I2S events and errors.
 * @param driver Pointer to the I2S driver instance.
 * @param callback The callback function to register.
 * @param user_arg User-defined argument passed to the callback.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_i2s_callback_register(cfn_hal_i2s_t               *driver,
                                                                  const cfn_hal_i2s_callback_t callback,
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
        &driver->base, CFN_HAL_PERIPHERAL_TYPE_I2S, (cfn_hal_callback_t) callback, user_arg);
}

/**
 * @brief Sets the I2S power state.
 * @param driver Pointer to the I2S driver instance.
 * @param state Target power state.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_i2s_power_state_set(cfn_hal_i2s_t *driver, cfn_hal_power_state_t state)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_power_state_set(&driver->base, CFN_HAL_PERIPHERAL_TYPE_I2S, state);
}

/**
 * @brief Enables one or more I2S nominal events.
 * @param driver Pointer to the I2S driver instance.
 * @param event_mask Mask of events to enable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_i2s_event_enable(cfn_hal_i2s_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_enable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_I2S, event_mask);
}

/**
 * @brief Disables one or more I2S nominal events.
 * @param driver Pointer to the I2S driver instance.
 * @param event_mask Mask of events to disable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_i2s_event_disable(cfn_hal_i2s_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_disable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_I2S, event_mask);
}

/**
 * @brief Retrieves the current I2S nominal event status.
 * @param driver Pointer to the I2S driver instance.
 * @param event_mask [out] Pointer to store the event mask.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_i2s_event_get(cfn_hal_i2s_t *driver, uint32_t *event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_get(&driver->base, CFN_HAL_PERIPHERAL_TYPE_I2S, event_mask);
}

/**
 * @brief Enables one or more I2S exception errors.
 * @param driver Pointer to the I2S driver instance.
 * @param error_mask Mask of errors to enable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_i2s_error_enable(cfn_hal_i2s_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_enable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_I2S, error_mask);
}

/**
 * @brief Disables one or more I2S exception errors.
 * @param driver Pointer to the I2S driver instance.
 * @param error_mask Mask of errors to disable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_i2s_error_disable(cfn_hal_i2s_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_disable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_I2S, error_mask);
}

/**
 * @brief Retrieves the current I2S exception error status.
 * @param driver Pointer to the I2S driver instance.
 * @param error_mask [out] Pointer to store the error mask.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_i2s_error_get(cfn_hal_i2s_t *driver, uint32_t *error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_get(&driver->base, CFN_HAL_PERIPHERAL_TYPE_I2S, error_mask);
}

/* I2S Specific Functions ------------------------------------------- */

/**
 * @brief Starts non-blocking audio transmission via DMA.
 * @param driver Pointer to the I2S driver instance.
 * @param data Pointer to the buffer containing audio samples.
 * @param size Number of samples to transmit.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_i2s_transmit_dma(cfn_hal_i2s_t *driver, const uint16_t *data, size_t size)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_I2S, transmit_dma, driver, error, data, size);
    return error;
}

/**
 * @brief Starts non-blocking audio reception via DMA.
 * @param driver Pointer to the I2S driver instance.
 * @param data Pointer to the buffer where received samples will be stored.
 * @param size Number of samples to receive.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_i2s_receive_dma(cfn_hal_i2s_t *driver, uint16_t *data, size_t size)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_I2S, receive_dma, driver, error, data, size);
    return error;
}

/**
 * @brief Pauses the ongoing I2S audio stream.
 * @param driver Pointer to the I2S driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_i2s_pause(cfn_hal_i2s_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_I2S, pause, driver, error);
    return error;
}

/**
 * @brief Resumes a previously paused I2S audio stream.
 * @param driver Pointer to the I2S driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_i2s_resume(cfn_hal_i2s_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_I2S, resume, driver, error);
    return error;
}

/**
 * @brief Stops the I2S audio operation and DMA transfers.
 * @param driver Pointer to the I2S driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_i2s_stop(cfn_hal_i2s_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_I2S, stop, driver, error);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif // CAFFEINE_HAL_HAL_I2S_H
