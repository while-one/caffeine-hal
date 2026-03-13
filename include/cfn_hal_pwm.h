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
 * @file cfn_hal_pwm.h
 * @brief Pulse Width Modulation (PWM) HAL API.
 */

#ifndef CAFFEINE_HAL_HAL_PWM_H
#define CAFFEINE_HAL_HAL_PWM_H

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
 * @brief PWM nominal event flags.
 */
typedef enum
{
    CFN_HAL_PWM_EVENT_NONE = 0,
    CFN_HAL_PWM_EVENT_PERIOD_MATCH = CFN_HAL_BIT(0), /*!< Timer period reached (cycle end) */
    CFN_HAL_PWM_EVENT_PULSE_MATCH = CFN_HAL_BIT(1),  /*!< Compare match reached (duty transition) */
} cfn_hal_pwm_event_t;

/**
 * @brief PWM exception error flags.
 */
typedef enum
{
    CFN_HAL_PWM_ERROR_NONE = 0,
    CFN_HAL_PWM_ERROR_FAULT = CFN_HAL_BIT(0),   /*!< External break or safety fault */
    CFN_HAL_PWM_ERROR_GENERAL = CFN_HAL_BIT(1), /*!< General hardware error */
} cfn_hal_pwm_error_t;

/**
 * @brief PWM output polarity.
 */
typedef enum
{
    CFN_HAL_PWM_CONFIG_POLARITY_NORMAL,   /*!< High during pulse, Low during remainder */
    CFN_HAL_PWM_CONFIG_POLARITY_INVERTED, /*!< Low during pulse, High during remainder */
} cfn_hal_pwm_config_polarity_t;

/* Types Structs ----------------------------------------------------*/

/**
 * @brief PWM hardware physical mapping.
 */
typedef struct
{
    void                      *instance; /*!< Peripheral base instance */
    uint32_t                   channel;  /*!< PWM output channel index */
    cfn_hal_gpio_pin_handle_t *pin;      /*!< PWM output pin mapping */
    void                      *user_arg; /*!< Peripheral instance user argument */
} cfn_hal_pwm_phy_t;

/**
 * @brief PWM configuration structure.
 */
typedef struct
{
    uint32_t                      frequency_hz;       /*!< Target PWM frequency in Hz */
    uint32_t                      duty_cycle_percent; /*!< Duty cycle (0 to 100) */
    cfn_hal_pwm_config_polarity_t polarity;           /*!< Output signal polarity */
    void                         *custom;             /*!< Vendor-specific custom configuration */
} cfn_hal_pwm_config_t;

typedef struct cfn_hal_pwm_s     cfn_hal_pwm_t;
typedef struct cfn_hal_pwm_api_s cfn_hal_pwm_api_t;

/**
 * @brief PWM callback signature.
 * @param driver Pointer to the PWM driver instance.
 * @param event_mask Mask of triggered nominal events.
 * @param error_mask Mask of triggered exception errors.
 * @param user_arg User-defined argument passed during registration.
 */
typedef void (*cfn_hal_pwm_callback_t)(cfn_hal_pwm_t *driver, uint32_t event_mask, uint32_t error_mask, void *user_arg);

/**
 * @brief PWM Virtual Method Table (VMT).
 */
struct cfn_hal_pwm_api_s
{
    cfn_hal_api_base_t base;

    /* PWM Specific Extensions */
    cfn_hal_error_code_t (*start)(cfn_hal_pwm_t *driver);
    cfn_hal_error_code_t (*stop)(cfn_hal_pwm_t *driver);
    cfn_hal_error_code_t (*set_frequency)(cfn_hal_pwm_t *driver, uint32_t frequency_hz);
    cfn_hal_error_code_t (*set_duty_cycle)(cfn_hal_pwm_t *driver, uint32_t duty_percent);
};

CFN_HAL_VMT_CHECK(struct cfn_hal_pwm_api_s);

CFN_HAL_CREATE_DRIVER_TYPE(pwm, cfn_hal_pwm_config_t, cfn_hal_pwm_api_t, cfn_hal_pwm_phy_t, cfn_hal_pwm_callback_t);

#define CFN_HAL_PWM_INITIALIZER(api_ptr, phy_ptr, config_ptr)                                                          \
    CFN_HAL_DRIVER_INITIALIZER(CFN_HAL_PERIPHERAL_TYPE_PWM, api_ptr, phy_ptr, config_ptr)

/* Functions inline ------------------------------------------------- */

/**
 * @brief Initializes the PWM driver.
 * @param driver Pointer to the PWM driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_pwm_init(cfn_hal_pwm_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->base.vmt = (const struct cfn_hal_api_base_s *) driver->api;
    return cfn_hal_base_init(&driver->base, CFN_HAL_PERIPHERAL_TYPE_PWM);
}

/**
 * @brief Deinitializes the PWM driver.
 * @param driver Pointer to the PWM driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_pwm_deinit(cfn_hal_pwm_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_deinit(&driver->base, CFN_HAL_PERIPHERAL_TYPE_PWM);
}

/**
 * @brief Sets the PWM configuration.
 * @param driver Pointer to the PWM driver instance.
 * @param config Pointer to the configuration structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_pwm_config_set(cfn_hal_pwm_t *driver, const cfn_hal_pwm_config_t *config)
{
    if (driver)
    {
        driver->config = config;
    }
    return cfn_hal_base_config_set(&driver->base, CFN_HAL_PERIPHERAL_TYPE_PWM, (const void *) config);
}

/**
 * @brief Gets the current PWM configuration.
 * @param driver Pointer to the PWM driver instance.
 * @param config [out] Pointer to store the configuration.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_pwm_config_get(cfn_hal_pwm_t *driver, cfn_hal_pwm_config_t *config)
{
    if (!driver || !config || !driver->config)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    *config = *(driver->config);
    return CFN_HAL_ERROR_OK;
}

/**
 * @brief Registers a callback for PWM events and errors.
 * @param driver Pointer to the PWM driver instance.
 * @param callback The callback function to register.
 * @param user_arg User-defined argument passed to the callback.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_pwm_callback_register(cfn_hal_pwm_t               *driver,
                                                                  const cfn_hal_pwm_callback_t callback,
                                                                  void                        *user_arg)
{
    if (driver)
    {
        driver->cb = callback;
        driver->cb_user_arg = user_arg;
    }
    return cfn_hal_base_callback_register(
        &driver->base, CFN_HAL_PERIPHERAL_TYPE_PWM, (cfn_hal_callback_t) callback, user_arg);
}

/**
 * @brief Sets the PWM power state.
 * @param driver Pointer to the PWM driver instance.
 * @param state Target power state.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_pwm_power_state_set(cfn_hal_pwm_t *driver, cfn_hal_power_state_t state)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_power_state_set(&driver->base, CFN_HAL_PERIPHERAL_TYPE_PWM, state);
}

/**
 * @brief Enables one or more PWM nominal events.
 * @param driver Pointer to the PWM driver instance.
 * @param event_mask Mask of events to enable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_pwm_event_enable(cfn_hal_pwm_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_enable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_PWM, event_mask);
}

/**
 * @brief Disables one or more PWM nominal events.
 * @param driver Pointer to the PWM driver instance.
 * @param event_mask Mask of events to disable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_pwm_event_disable(cfn_hal_pwm_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_disable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_PWM, event_mask);
}

/**
 * @brief Retrieves the current PWM nominal event status.
 * @param driver Pointer to the PWM driver instance.
 * @param event_mask [out] Pointer to store the event mask.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_pwm_event_get(cfn_hal_pwm_t *driver, uint32_t *event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_get(&driver->base, CFN_HAL_PERIPHERAL_TYPE_PWM, event_mask);
}

/**
 * @brief Enables one or more PWM exception errors.
 * @param driver Pointer to the PWM driver instance.
 * @param error_mask Mask of errors to enable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_pwm_error_enable(cfn_hal_pwm_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_enable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_PWM, error_mask);
}

/**
 * @brief Disables one or more PWM exception errors.
 * @param driver Pointer to the PWM driver instance.
 * @param error_mask Mask of errors to disable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_pwm_error_disable(cfn_hal_pwm_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_disable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_PWM, error_mask);
}

/**
 * @brief Retrieves the current PWM exception error status.
 * @param driver Pointer to the PWM driver instance.
 * @param error_mask [out] Pointer to store the error mask.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_pwm_error_get(cfn_hal_pwm_t *driver, uint32_t *error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_get(&driver->base, CFN_HAL_PERIPHERAL_TYPE_PWM, error_mask);
}

/* PWM Specific Functions ------------------------------------------- */

/**
 * @brief Starts PWM signal generation.
 * @param driver Pointer to the PWM driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_pwm_start(cfn_hal_pwm_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_PWM, start, driver, error);
    return error;
}

/**
 * @brief Stops PWM signal generation.
 * @param driver Pointer to the PWM driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_pwm_stop(cfn_hal_pwm_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_PWM, stop, driver, error);
    return error;
}

/**
 * @brief Updates the PWM output frequency.
 * @param driver Pointer to the PWM driver instance.
 * @param frequency_hz The new target frequency in Hertz.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_pwm_set_frequency(cfn_hal_pwm_t *driver, uint32_t frequency_hz)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_PWM, set_frequency, driver, error, frequency_hz);
    return error;
}

/**
 * @brief Updates the PWM duty cycle.
 * @param driver Pointer to the PWM driver instance.
 * @param duty_percent The new target duty cycle (0 to 100).
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_pwm_set_duty_cycle(cfn_hal_pwm_t *driver, uint32_t duty_percent)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_PWM, set_duty_cycle, driver, error, duty_percent);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif // CAFFEINE_HAL_HAL_PWM_H
