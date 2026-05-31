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
 * @file cfn_hal_adc.h
 * @brief Analog-to-Digital Converter (ADC) HAL API.
 */

#ifndef CAFFEINE_HAL_HAL_ADC_H
#define CAFFEINE_HAL_HAL_ADC_H

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
 * @brief ADC nominal event flags.
 */
typedef enum
{
    CFN_HAL_ADC_EVENT_NONE = 0,
    CFN_HAL_ADC_EVENT_EOC  = CFN_HAL_BIT(0), /*!< End of Conversion */
} cfn_hal_adc_event_t;

/**
 * @brief ADC exception error flags.
 */
typedef enum
{
    CFN_HAL_ADC_ERROR_NONE    = 0,
    CFN_HAL_ADC_ERROR_OVERRUN = CFN_HAL_BIT(0), /*!< Data Overrun */
    CFN_HAL_ADC_ERROR_GENERAL = CFN_HAL_BIT(1), /*!< General Hardware Error */
} cfn_hal_adc_error_t;

typedef enum
{
    CFN_HAL_ADC_RESOLUTION_BIT_4 = 0,
    CFN_HAL_ADC_RESOLUTION_BIT_6,
    CFN_HAL_ADC_RESOLUTION_BIT_8,
    CFN_HAL_ADC_RESOLUTION_BIT_10,
    CFN_HAL_ADC_RESOLUTION_BIT_12,
    CFN_HAL_ADC_RESOLUTION_BIT_14,
    CFN_HAL_ADC_RESOLUTION_BIT_16,
    CFN_HAL_ADC_RESOLUTION_BIT_24,

    CFN_HAL_ADC_RESOLUTION_BIT_MAX
} cfn_hal_adc_resolution_t;

typedef enum
{
    CFN_HAL_ADC_ALIGN_RIGHT = 0,
    CFN_HAL_ADC_ALIGN_LEFT,

    CFN_HAL_ADC_ALIGN_MAX
} cfn_hal_adc_align_t;

typedef enum
{
    CFN_HAL_SCAN_ENABLED = 0,
    CFN_HAL_SCAN_DISABLED,

    CFN_HAL_SCAN_MAX
} cfn_hal_adc_scan_t;

typedef enum
{
    CFN_HAL_ADC_MODE_SINGLE = 0,
    CFN_HAL_ADC_MODE_CONTINUOUS,

    CFN_HAL_ADC_MODE_MAX

} cfn_hal_adc_mode_t;

typedef enum
{
    CFN_HAL_ADC_EOC_SINGLE   = CFN_HAL_BIT(0),
    CFN_HAL_ADC_EOC_SEQUENCE = CFN_HAL_BIT(1),
} cfn_hal_adc_eoc_t;
/* Types Structs ----------------------------------------------------*/

typedef struct
{
    void                      *instance; /*!< Peripheral base instance */
    uint32_t                   channel;  /*!< ADC channel index */
    cfn_hal_gpio_pin_handle_t *gpio;     /*!< Analog input pin mapping */
    void                      *user_arg; /*!< Peripheral instance user argument */
} cfn_hal_adc_phy_t;

typedef struct
{
    cfn_hal_adc_resolution_t resolution;
    cfn_hal_adc_align_t      alignment;
    cfn_hal_adc_scan_t       scan;
    cfn_hal_adc_mode_t       mode;
    uint32_t                 eoc;
    void                    *custom;
} cfn_hal_adc_config_t;

typedef struct cfn_hal_adc_s     cfn_hal_adc_t;
typedef struct cfn_hal_adc_api_s cfn_hal_adc_api_t;

/**
 * @brief ADC callback signature.
 * @param p_driver Pointer to the ADC driver instance.
 * @param event_mask Mask of triggered nominal events.
 * @param error_mask Mask of triggered exception errors.
 * @param value Last conversion result (if applicable).
 * @param p_user_arg User-defined argument passed during registration.
 */
typedef void (*cfn_hal_adc_callback_t)(
    cfn_hal_adc_t *p_driver, uint32_t event_mask, uint32_t error_mask, uint32_t value, void *p_user_arg);

/**
 * @brief ADC Virtual Method Table (VMT).
 */
struct cfn_hal_adc_api_s
{
    cfn_hal_api_base_t base;

    /* ADC Specific Extensions */
    cfn_hal_error_code_t (*read)(cfn_hal_adc_t *p_driver, uint32_t *p_value, uint32_t timeout);
    cfn_hal_error_code_t (*start)(cfn_hal_adc_t *p_driver);
    cfn_hal_error_code_t (*stop)(cfn_hal_adc_t *p_driver);
    cfn_hal_error_code_t (*read_dma)(cfn_hal_adc_t *p_driver, uint32_t *p_data, size_t nbr_of_samples);
};

CFN_HAL_VMT_CHECK(struct cfn_hal_adc_api_s);

CFN_HAL_CREATE_DRIVER_TYPE(adc, cfn_hal_adc_config_t, cfn_hal_adc_api_t, cfn_hal_adc_phy_t, cfn_hal_adc_callback_t);
/* Functions inline ------------------------------------------------- */
CFN_HAL_INLINE void
cfn_hal_adc_populate (cfn_hal_adc_t              *p_driver,
                      uint32_t                    peripheral_id,
                      struct cfn_hal_clock_s     *p_clock,
                      void                       *p_dependency,
                      const cfn_hal_adc_api_t    *p_api,
                      const cfn_hal_adc_phy_t    *p_phy,
                      const cfn_hal_adc_config_t *p_config,
                      cfn_hal_adc_callback_t      p_callback,
                      void                       *p_user_arg)
{
    CFN_HAL_POPULATE_DRIVER(p_driver,
                            CFN_HAL_PERIPHERAL_TYPE_ADC,
                            peripheral_id,
                            p_clock,
                            p_dependency,
                            p_api,
                            p_phy,
                            p_config,
                            p_callback,
                            p_user_arg);
}

/**
 * @brief Validates the ADC configuration.
 * @param p_driver Pointer to the ADC driver instance.
 * @param p_config Pointer to the configuration structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_adc_config_validate (const cfn_hal_adc_t *p_driver, const cfn_hal_adc_config_t *p_config)
{
    if (p_driver == NULL || p_config == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    if ((p_config->resolution >= CFN_HAL_ADC_RESOLUTION_BIT_MAX) || (p_config->alignment >= CFN_HAL_ADC_ALIGN_MAX) ||
        (p_config->scan >= CFN_HAL_SCAN_MAX) || (p_config->mode >= CFN_HAL_ADC_MODE_MAX))
    {
        return CFN_HAL_ERROR_BAD_CONFIG;
    }

    return cfn_hal_base_config_validate(&p_driver->base, CFN_HAL_PERIPHERAL_TYPE_ADC, p_config);
}

/**
 * @brief Initializes the ADC driver.
 * @param p_driver Pointer to the ADC driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_adc_init (cfn_hal_adc_t *p_driver)
{
    if (!p_driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    p_driver->base.vmt         = (const struct cfn_hal_api_base_s *) p_driver->api;
    cfn_hal_error_code_t error = cfn_hal_adc_config_validate(p_driver, p_driver->config);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    return cfn_hal_base_init(&p_driver->base, CFN_HAL_PERIPHERAL_TYPE_ADC);
}

/**
 * @brief Deinitializes the ADC driver.
 * @param p_driver Pointer to the ADC driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_adc_deinit (cfn_hal_adc_t *p_driver)
{
    if (!p_driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_deinit(&p_driver->base, CFN_HAL_PERIPHERAL_TYPE_ADC);
}

/**
 * @brief Sets the ADC configuration.
 * @param p_driver Pointer to the ADC driver instance.
 * @param p_config Pointer to the configuration structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_adc_config_set (cfn_hal_adc_t *p_driver, const cfn_hal_adc_config_t *p_config)
{
    if (!p_driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    cfn_hal_error_code_t error = cfn_hal_adc_config_validate(p_driver, p_config);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    {
        p_driver->config = p_config;
    }
    return cfn_hal_base_config_set(&p_driver->base, CFN_HAL_PERIPHERAL_TYPE_ADC, (const void *) p_config);
}

/**
 * @brief Gets the current ADC configuration.
 * @param p_driver Pointer to the ADC driver instance.
 * @param p_config [out] Pointer to store the configuration.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_adc_config_get (cfn_hal_adc_t *p_driver, cfn_hal_adc_config_t *p_config)
{
    if (!p_driver || !p_config || !p_driver->config)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    *p_config = *(p_driver->config);
    return CFN_HAL_ERROR_OK;
}

/**
 * @brief Registers a callback for ADC events and errors.
 * @param p_driver Pointer to the ADC driver instance.
 * @param callback The callback function to register.
 * @param p_user_arg User-defined argument passed to the callback.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_adc_callback_register (cfn_hal_adc_t *p_driver, const cfn_hal_adc_callback_t callback, void *p_user_arg)
{
    if (!p_driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    {
        p_driver->cb          = callback;
        p_driver->cb_user_arg = p_user_arg;
    }
    return cfn_hal_base_callback_register(
        &p_driver->base, CFN_HAL_PERIPHERAL_TYPE_ADC, (cfn_hal_callback_t) callback, p_user_arg);
}

/**
 * @brief Sets the ADC power state.
 * @param p_driver Pointer to the ADC driver instance.
 * @param state Target power state.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_adc_power_state_set (cfn_hal_adc_t *p_driver, cfn_hal_power_state_t state)
{
    if (!p_driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_power_state_set(&p_driver->base, CFN_HAL_PERIPHERAL_TYPE_ADC, state);
}

/**
 * @brief Enables one or more ADC nominal events.
 * @param p_driver Pointer to the ADC driver instance.
 * @param event_mask Mask of events to enable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_adc_event_enable (cfn_hal_adc_t *p_driver, uint32_t event_mask)
{
    if (!p_driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_enable(&p_driver->base, CFN_HAL_PERIPHERAL_TYPE_ADC, event_mask);
}

/**
 * @brief Disables one or more ADC nominal events.
 * @param p_driver Pointer to the ADC driver instance.
 * @param event_mask Mask of events to disable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_adc_event_disable (cfn_hal_adc_t *p_driver, uint32_t event_mask)
{
    if (!p_driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_disable(&p_driver->base, CFN_HAL_PERIPHERAL_TYPE_ADC, event_mask);
}

/**
 * @brief Retrieves the current ADC nominal event status.
 * @param p_driver Pointer to the ADC driver instance.
 * @param p_event_mask [out] Pointer to store the event mask.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_adc_event_get (cfn_hal_adc_t *p_driver, uint32_t *p_event_mask)
{
    if (!p_driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_get(&p_driver->base, CFN_HAL_PERIPHERAL_TYPE_ADC, p_event_mask);
}

/**
 * @brief Enables one or more ADC exception errors.
 * @param p_driver Pointer to the ADC driver instance.
 * @param error_mask Mask of errors to enable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_adc_error_enable (cfn_hal_adc_t *p_driver, uint32_t error_mask)
{
    if (!p_driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_enable(&p_driver->base, CFN_HAL_PERIPHERAL_TYPE_ADC, error_mask);
}

/**
 * @brief Disables one or more ADC exception errors.
 * @param p_driver Pointer to the ADC driver instance.
 * @param error_mask Mask of errors to disable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_adc_error_disable (cfn_hal_adc_t *p_driver, uint32_t error_mask)
{
    if (!p_driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_disable(&p_driver->base, CFN_HAL_PERIPHERAL_TYPE_ADC, error_mask);
}

/**
 * @brief Retrieves the current ADC exception error status.
 * @param p_driver Pointer to the ADC driver instance.
 * @param p_error_mask [out] Pointer to store the error mask.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_adc_error_get (cfn_hal_adc_t *p_driver, uint32_t *p_error_mask)
{
    if (!p_driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_get(&p_driver->base, CFN_HAL_PERIPHERAL_TYPE_ADC, p_error_mask);
}

/* ADC Specific Functions ------------------------------------------- */

/**
 * @brief Reads a single ADC value (polling).
 * @param p_driver Pointer to the ADC driver instance.
 * @param p_value [out] Pointer to store the read value.
 * @param timeout Timeout duration in milliseconds.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_adc_read (cfn_hal_adc_t *p_driver, uint32_t *p_value, uint32_t timeout)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_ADC, read, p_driver, error, p_value, timeout);
    return error;
}

/**
 * @brief Starts continuous ADC conversion.
 * @param p_driver Pointer to the ADC driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_adc_start (cfn_hal_adc_t *p_driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_ADC, start, p_driver, error);
    return error;
}

/**
 * @brief Stops continuous ADC conversion.
 * @param p_driver Pointer to the ADC driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_adc_stop (cfn_hal_adc_t *p_driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_ADC, stop, p_driver, error);
    return error;
}

/**
 * @brief Reads multiple ADC values using DMA.
 * @param p_driver Pointer to the ADC driver instance.
 * @param p_data [out] Pointer to the buffer where samples will be stored.
 * @param nbr_of_samples Number of samples to read.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_adc_read_dma (cfn_hal_adc_t *p_driver, uint32_t *p_data, size_t nbr_of_samples)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_ADC, read_dma, p_driver, error, p_data, nbr_of_samples);
    return error;
}

cfn_hal_error_code_t cfn_hal_adc_construct (cfn_hal_adc_t              *p_driver,
                                            const cfn_hal_adc_config_t *p_config,
                                            const cfn_hal_adc_phy_t    *p_phy,
                                            struct cfn_hal_clock_s     *p_clock,
                                            void                       *p_dependency,
                                            cfn_hal_adc_callback_t      p_callback,
                                            void                       *p_user_arg);
cfn_hal_error_code_t cfn_hal_adc_destruct (cfn_hal_adc_t *p_driver);
#ifdef __cplusplus
}
#endif

#endif // CAFFEINE_HAL_HAL_ADC_H
