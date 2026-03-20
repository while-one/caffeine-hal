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
 * @file cfn_hal_dac.h
 * @brief Digital-to-Analog Converter (DAC) HAL API.
 */

#ifndef CAFFEINE_HAL_HAL_DAC_H
#define CAFFEINE_HAL_HAL_DAC_H

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
 * @brief DAC nominal event flags.
 */
typedef enum
{
    CFN_HAL_DAC_EVENT_NONE         = 0,
    CFN_HAL_DAC_EVENT_DMA_HALF     = CFN_HAL_BIT(0), /*!< DMA half transfer complete */
    CFN_HAL_DAC_EVENT_DMA_COMPLETE = CFN_HAL_BIT(1), /*!< DMA transfer complete */
} cfn_hal_dac_event_t;

/**
 * @brief DAC exception error flags.
 */
typedef enum
{
    CFN_HAL_DAC_ERROR_NONE     = 0,
    CFN_HAL_DAC_ERROR_UNDERRUN = CFN_HAL_BIT(0), /*!< Output buffer underrun */
    CFN_HAL_DAC_ERROR_GENERAL  = CFN_HAL_BIT(1), /*!< General hardware error */
} cfn_hal_dac_error_t;

typedef enum
{
    CFN_HAL_DAC_RESOLUTION_BIT_4 = 0,
    CFN_HAL_DAC_RESOLUTION_BIT_6,
    CFN_HAL_DAC_RESOLUTION_BIT_8,
    CFN_HAL_DAC_RESOLUTION_BIT_10,
    CFN_HAL_DAC_RESOLUTION_BIT_12,
    CFN_HAL_DAC_RESOLUTION_BIT_14,
    CFN_HAL_DAC_RESOLUTION_BIT_16,
    CFN_HAL_DAC_RESOLUTION_BIT_24,

    CFN_HAL_DAC_RESOLUTION_BIT_MAX
} cfn_hal_dac_resolution_t;

typedef enum
{
    CFN_HAL_DAC_ALIGN_RIGHT = 0,
    CFN_HAL_DAC_ALIGN_LEFT,

    CFN_HAL_DAC_ALIGN_MAX
} cfn_hal_dac_align_t;
/* Types Structs ----------------------------------------------------*/

/**
 * @brief DAC hardware physical mapping.
 */
typedef struct
{
    void                      *instance; /*!< Peripheral base instance */
    uint32_t                   channel;  /*!< Target DAC channel index */
    cfn_hal_gpio_pin_handle_t *pin;      /*!< Analog output pin mapping */
    void                      *user_arg; /*!< Peripheral instance user argument */
} cfn_hal_dac_phy_t;

/**
 * @brief DAC configuration structure.
 */
typedef struct
{
    cfn_hal_dac_align_t      alignment;  /*!< Data alignment (Left/Right) */
    cfn_hal_dac_resolution_t resolution; /*!< DAC resolution in bits */
    void                    *custom;     /*!< Vendor-specific custom configuration */
} cfn_hal_dac_config_t;

typedef struct cfn_hal_dac_s     cfn_hal_dac_t;
typedef struct cfn_hal_dac_api_s cfn_hal_dac_api_t;

/**
 * @brief DAC callback signature.
 * @param driver Pointer to the DAC driver instance.
 * @param event_mask Mask of triggered nominal events.
 * @param error_mask Mask of triggered exception errors.
 * @param user_arg User-defined argument passed during registration.
 */
typedef void (*cfn_hal_dac_callback_t)(cfn_hal_dac_t *driver, uint32_t event_mask, uint32_t error_mask, void *user_arg);

/**
 * @brief DAC Virtual Method Table (VMT).
 */
struct cfn_hal_dac_api_s
{
    cfn_hal_api_base_t base;

    /* DAC Specific Extensions */
    cfn_hal_error_code_t (*set_value)(cfn_hal_dac_t *driver, uint32_t value);
    cfn_hal_error_code_t (*start)(cfn_hal_dac_t *driver);
    cfn_hal_error_code_t (*stop)(cfn_hal_dac_t *driver);
    cfn_hal_error_code_t (*write_dma)(cfn_hal_dac_t *driver, const uint32_t *data, size_t nbr_of_samples);
};

CFN_HAL_VMT_CHECK(struct cfn_hal_dac_api_s);

CFN_HAL_CREATE_DRIVER_TYPE(dac, cfn_hal_dac_config_t, cfn_hal_dac_api_t, cfn_hal_dac_phy_t, cfn_hal_dac_callback_t);

#define CFN_HAL_DAC_INITIALIZER(api_ptr, phy_ptr, config_ptr)                                                          \
    CFN_HAL_DRIVER_INITIALIZER(CFN_HAL_PERIPHERAL_TYPE_DAC, api_ptr, phy_ptr, config_ptr)

/* Functions inline ------------------------------------------------- */

/**
 * @brief Validates the DAC configuration.
 * @param config Pointer to the configuration structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_dac_config_validate(const cfn_hal_dac_t        *driver,
                                                                const cfn_hal_dac_config_t *config)
{
    if (driver == NULL || config == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    if ((config->resolution >= CFN_HAL_DAC_RESOLUTION_BIT_MAX) || (config->alignment >= CFN_HAL_DAC_ALIGN_MAX))
    {
        return CFN_HAL_ERROR_BAD_CONFIG;
    }

    return cfn_hal_base_config_validate(&driver->base, CFN_HAL_PERIPHERAL_TYPE_DAC, config);
}

/**
 * @brief Initializes the DAC driver.
 * @param driver Pointer to the DAC driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_dac_init(cfn_hal_dac_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->base.vmt           = (const struct cfn_hal_api_base_s *) driver->api;
    cfn_hal_error_code_t error = cfn_hal_dac_config_validate(driver, driver->config);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    return cfn_hal_base_init(&driver->base, CFN_HAL_PERIPHERAL_TYPE_DAC);
}

/**
 * @brief Deinitializes the DAC driver.
 * @param driver Pointer to the DAC driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_dac_deinit(cfn_hal_dac_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_deinit(&driver->base, CFN_HAL_PERIPHERAL_TYPE_DAC);
}

/**
 * @brief Sets the DAC configuration.
 * @param driver Pointer to the DAC driver instance.
 * @param config Pointer to the configuration structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_dac_config_set(cfn_hal_dac_t *driver, const cfn_hal_dac_config_t *config)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    cfn_hal_error_code_t error = cfn_hal_dac_config_validate(driver, config);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    {
        driver->config = config;
    }
    return cfn_hal_base_config_set(&driver->base, CFN_HAL_PERIPHERAL_TYPE_DAC, (const void *) config);
}

/**
 * @brief Gets the current DAC configuration.
 * @param driver Pointer to the DAC driver instance.
 * @param config [out] Pointer to store the configuration.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_dac_config_get(cfn_hal_dac_t *driver, cfn_hal_dac_config_t *config)
{
    if (!driver || !config || !driver->config)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    *config = *(driver->config);
    return CFN_HAL_ERROR_OK;
}

/**
 * @brief Registers a callback for DAC events and errors.
 * @param driver Pointer to the DAC driver instance.
 * @param callback The callback function to register.
 * @param user_arg User-defined argument passed to the callback.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_dac_callback_register(cfn_hal_dac_t               *driver,
                                                                  const cfn_hal_dac_callback_t callback,
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
        &driver->base, CFN_HAL_PERIPHERAL_TYPE_DAC, (cfn_hal_callback_t) callback, user_arg);
}

/**
 * @brief Sets the DAC power state.
 * @param driver Pointer to the DAC driver instance.
 * @param state Target power state.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_dac_power_state_set(cfn_hal_dac_t *driver, cfn_hal_power_state_t state)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_power_state_set(&driver->base, CFN_HAL_PERIPHERAL_TYPE_DAC, state);
}

/**
 * @brief Enables one or more DAC nominal events.
 * @param driver Pointer to the DAC driver instance.
 * @param event_mask Mask of events to enable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_dac_event_enable(cfn_hal_dac_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_enable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_DAC, event_mask);
}

/**
 * @brief Disables one or more DAC nominal events.
 * @param driver Pointer to the DAC driver instance.
 * @param event_mask Mask of events to disable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_dac_event_disable(cfn_hal_dac_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_disable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_DAC, event_mask);
}

/**
 * @brief Retrieves the current DAC nominal event status.
 * @param driver Pointer to the DAC driver instance.
 * @param event_mask [out] Pointer to store the event mask.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_dac_event_get(cfn_hal_dac_t *driver, uint32_t *event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_get(&driver->base, CFN_HAL_PERIPHERAL_TYPE_DAC, event_mask);
}

/**
 * @brief Enables one or more DAC exception errors.
 * @param driver Pointer to the DAC driver instance.
 * @param error_mask Mask of errors to enable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_dac_error_enable(cfn_hal_dac_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_enable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_DAC, error_mask);
}

/**
 * @brief Disables one or more DAC exception errors.
 * @param driver Pointer to the DAC driver instance.
 * @param error_mask Mask of errors to disable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_dac_error_disable(cfn_hal_dac_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_disable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_DAC, error_mask);
}

/**
 * @brief Retrieves the current DAC exception error status.
 * @param driver Pointer to the DAC driver instance.
 * @param error_mask [out] Pointer to store the error mask.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_dac_error_get(cfn_hal_dac_t *driver, uint32_t *error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_get(&driver->base, CFN_HAL_PERIPHERAL_TYPE_DAC, error_mask);
}

/* DAC Specific Functions ------------------------------------------- */

/**
 * @brief Sets a static output value for the DAC.
 * @param driver Pointer to the DAC driver instance.
 * @param value The digital value to convert to analog.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_dac_set_value(cfn_hal_dac_t *driver, uint32_t value)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_DAC, set_value, driver, error, value);
    return error;
}

/**
 * @brief Starts DAC conversion (e.g., enables output).
 * @param driver Pointer to the DAC driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_dac_start(cfn_hal_dac_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_DAC, start, driver, error);
    return error;
}

/**
 * @brief Stops DAC conversion (e.g., disables output).
 * @param driver Pointer to the DAC driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_dac_stop(cfn_hal_dac_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_DAC, stop, driver, error);
    return error;
}

/**
 * @brief Starts continuous DAC output using DMA.
 * @param driver Pointer to the DAC driver instance.
 * @param data Pointer to the buffer containing samples to write.
 * @param nbr_of_samples Number of samples to output.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_dac_write_dma(cfn_hal_dac_t  *driver,
                                                          const uint32_t *data,
                                                          size_t          nbr_of_samples)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_DAC, write_dma, driver, error, data, nbr_of_samples);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif // CAFFEINE_HAL_HAL_DAC_H
