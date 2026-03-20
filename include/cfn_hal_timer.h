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
 * @file cfn_hal_timer.h
 * @brief General Purpose Timer HAL API.
 */

#ifndef CAFFEINE_HAL_HAL_TIMER_H
#define CAFFEINE_HAL_HAL_TIMER_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_hal_types.h"
#include "cfn_hal.h"
#include "cfn_hal_base.h"

/* Defines ----------------------------------------------------------*/

/* Types Enums ------------------------------------------------------*/

/**
 * @brief Timer nominal event flags.
 */
typedef enum
{
    CFN_HAL_TIMER_EVENT_NONE    = 0,
    CFN_HAL_TIMER_EVENT_UPDATE  = CFN_HAL_BIT(0), /*!< Counter rollover/update */
    CFN_HAL_TIMER_EVENT_CC1     = CFN_HAL_BIT(1), /*!< Capture/Compare channel 1 */
    CFN_HAL_TIMER_EVENT_CC2     = CFN_HAL_BIT(2), /*!< Capture/Compare channel 2 */
    CFN_HAL_TIMER_EVENT_CC3     = CFN_HAL_BIT(3), /*!< Capture/Compare channel 3 */
    CFN_HAL_TIMER_EVENT_CC4     = CFN_HAL_BIT(4), /*!< Capture/Compare channel 4 */
    CFN_HAL_TIMER_EVENT_TRIGGER = CFN_HAL_BIT(5), /*!< Trigger input event */
} cfn_hal_timer_event_t;

/**
 * @brief Timer exception error flags.
 */
typedef enum
{
    CFN_HAL_TIMER_ERROR_NONE    = 0,
    CFN_HAL_TIMER_ERROR_BREAK   = CFN_HAL_BIT(0), /*!< Safety break input triggered */
    CFN_HAL_TIMER_ERROR_GENERAL = CFN_HAL_BIT(1), /*!< General hardware error */
} cfn_hal_timer_error_t;

/* Types Structs ----------------------------------------------------*/

/**
 * @brief Timer configuration structure.
 */
typedef struct
{
    uint32_t prescaler; /*!< Clock division factor */
    uint32_t period;    /*!< Auto-reload value */
    void    *custom;    /*!< Vendor-specific custom configuration */
} cfn_hal_timer_config_t;

/**
 * @brief Timer hardware physical mapping.
 */
typedef struct
{
    void *instance; /*!< Peripheral base instance */
    void *user_arg; /*!< Peripheral instance user argument */
} cfn_hal_timer_phy_t;

/**
 * @brief Timer period and channel mapping.
 */
typedef struct
{
    uint32_t channel; /*!< Target channel index */
    uint64_t ticks;   /*!< Period in timer ticks */
} cfn_hal_timer_period_t;

typedef struct cfn_hal_timer_s     cfn_hal_timer_t;
typedef struct cfn_hal_timer_api_s cfn_hal_timer_api_t;

/**
 * @brief Timer callback signature.
 * @param driver Pointer to the Timer driver instance.
 * @param event_mask Mask of triggered nominal events.
 * @param error_mask Mask of triggered exception errors.
 * @param channel Source channel index (if applicable).
 * @param user_arg User-defined argument passed during registration.
 */
typedef void (*cfn_hal_timer_callback_t)(
    cfn_hal_timer_t *driver, uint32_t event_mask, uint32_t error_mask, uint32_t channel, void *user_arg);

/**
 * @brief Timer Virtual Method Table (VMT).
 */
struct cfn_hal_timer_api_s
{
    cfn_hal_api_base_t base;

    /* Timer Specific Extensions */
    cfn_hal_error_code_t (*start)(cfn_hal_timer_t *driver);
    cfn_hal_error_code_t (*stop)(cfn_hal_timer_t *driver);
    cfn_hal_error_code_t (*get_ticks)(cfn_hal_timer_t *driver, uint32_t ch, uint32_t *ticks);
    cfn_hal_error_code_t (*get_ticks_u64)(cfn_hal_timer_t *driver, uint32_t ch, uint64_t *ticks);
    cfn_hal_error_code_t (*set_period)(cfn_hal_timer_t *driver, const cfn_hal_timer_period_t *period);
};

CFN_HAL_VMT_CHECK(struct cfn_hal_timer_api_s);

CFN_HAL_CREATE_DRIVER_TYPE(
    timer, cfn_hal_timer_config_t, cfn_hal_timer_api_t, cfn_hal_timer_phy_t, cfn_hal_timer_callback_t);

#define CFN_HAL_TIMER_INITIALIZER(api_ptr, phy_ptr, config_ptr)                                                        \
    CFN_HAL_DRIVER_INITIALIZER(CFN_HAL_PERIPHERAL_TYPE_TIMER, api_ptr, phy_ptr, config_ptr)

/* Functions inline ------------------------------------------------- */

/**
 * @brief Validates the Timer configuration.
 * @param config Pointer to the configuration structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_timer_config_validate(const cfn_hal_timer_config_t *config)
{
    if (config == NULL)
    {
        return CFN_HAL_ERROR_BAD_CONFIG;
    }

    return CFN_HAL_ERROR_OK;
}

/**
 * @brief Initializes the Timer driver.
 * @param driver Pointer to the Timer driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_timer_init(cfn_hal_timer_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->base.vmt = (const struct cfn_hal_api_base_s *) driver->api;
    return cfn_hal_base_init(&driver->base, CFN_HAL_PERIPHERAL_TYPE_TIMER);
}

/**
 * @brief Deinitializes the Timer driver.
 * @param driver Pointer to the Timer driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_timer_deinit(cfn_hal_timer_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_deinit(&driver->base, CFN_HAL_PERIPHERAL_TYPE_TIMER);
}

/**
 * @brief Sets the Timer configuration.
 * @param driver Pointer to the Timer driver instance.
 * @param config Pointer to the configuration structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_timer_config_set(cfn_hal_timer_t              *driver,
                                                             const cfn_hal_timer_config_t *config)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    {
        driver->config = config;
    }
    return cfn_hal_base_config_set(&driver->base, CFN_HAL_PERIPHERAL_TYPE_TIMER, (const void *) config);
}

/**
 * @brief Gets the current Timer configuration.
 * @param driver Pointer to the Timer driver instance.
 * @param config [out] Pointer to store the configuration.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_timer_config_get(cfn_hal_timer_t *driver, cfn_hal_timer_config_t *config)
{
    if (!driver || !config || !driver->config)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    *config = *(driver->config);
    return CFN_HAL_ERROR_OK;
}

/**
 * @brief Registers a callback for Timer events and errors.
 * @param driver Pointer to the Timer driver instance.
 * @param callback The callback function to register.
 * @param user_arg User-defined argument passed to the callback.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_timer_callback_register(cfn_hal_timer_t               *driver,
                                                                    const cfn_hal_timer_callback_t callback,
                                                                    void                          *user_arg)
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
        &driver->base, CFN_HAL_PERIPHERAL_TYPE_TIMER, (cfn_hal_callback_t) callback, user_arg);
}

/**
 * @brief Sets the Timer power state.
 * @param driver Pointer to the Timer driver instance.
 * @param state Target power state.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_timer_power_state_set(cfn_hal_timer_t *driver, cfn_hal_power_state_t state)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_power_state_set(&driver->base, CFN_HAL_PERIPHERAL_TYPE_TIMER, state);
}

/**
 * @brief Enables one or more Timer nominal events.
 * @param driver Pointer to the Timer driver instance.
 * @param event_mask Mask of events to enable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_timer_event_enable(cfn_hal_timer_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_enable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_TIMER, event_mask);
}

/**
 * @brief Disables one or more Timer nominal events.
 * @param driver Pointer to the Timer driver instance.
 * @param event_mask Mask of events to disable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_timer_event_disable(cfn_hal_timer_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_disable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_TIMER, event_mask);
}

/**
 * @brief Retrieves the current Timer nominal event status.
 * @param driver Pointer to the Timer driver instance.
 * @param event_mask [out] Pointer to store the event mask.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_timer_event_get(cfn_hal_timer_t *driver, uint32_t *event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_get(&driver->base, CFN_HAL_PERIPHERAL_TYPE_TIMER, event_mask);
}

/**
 * @brief Enables one or more Timer exception errors.
 * @param driver Pointer to the Timer driver instance.
 * @param error_mask Mask of errors to enable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_timer_error_enable(cfn_hal_timer_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_enable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_TIMER, error_mask);
}

/**
 * @brief Disables one or more Timer exception errors.
 * @param driver Pointer to the Timer driver instance.
 * @param error_mask Mask of errors to disable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_timer_error_disable(cfn_hal_timer_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_disable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_TIMER, error_mask);
}

/**
 * @brief Retrieves the current Timer exception error status.
 * @param driver Pointer to the Timer driver instance.
 * @param error_mask [out] Pointer to store the error mask.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_timer_error_get(cfn_hal_timer_t *driver, uint32_t *error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_get(&driver->base, CFN_HAL_PERIPHERAL_TYPE_TIMER, error_mask);
}

/* Timer Specific Functions ----------------------------------------- */

/**
 * @brief Starts the timer counter.
 * @param driver Pointer to the Timer driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_timer_start(cfn_hal_timer_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_TIMER, start, driver, error);
    return error;
}

/**
 * @brief Stops the timer counter.
 * @param driver Pointer to the Timer driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_timer_stop(cfn_hal_timer_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_TIMER, stop, driver, error);
    return error;
}

/**
 * @brief Reads the current tick count for a specific channel (32-bit).
 * @param driver Pointer to the Timer driver instance.
 * @param ch Channel index.
 * @param ticks [out] Pointer to store the tick count.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_timer_get_ticks(cfn_hal_timer_t *driver, uint32_t ch, uint32_t *ticks)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_TIMER, get_ticks, driver, error, ch, ticks);
    return error;
}

/**
 * @brief Reads the current tick count for a specific channel (64-bit).
 * @param driver Pointer to the Timer driver instance.
 * @param ch Channel index.
 * @param ticks [out] Pointer to store the tick count.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_timer_get_ticks_u64(cfn_hal_timer_t *driver, uint32_t ch, uint64_t *ticks)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_TIMER, get_ticks_u64, driver, error, ch, ticks);
    return error;
}

/**
 * @brief Updates the timer period/reload value for a specific channel.
 * @param driver Pointer to the Timer driver instance.
 * @param period Pointer to the structure defining channel and new tick value.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_timer_set_period(cfn_hal_timer_t              *driver,
                                                             const cfn_hal_timer_period_t *period)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_TIMER, set_period, driver, error, period);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif // CAFFEINE_HAL_HAL_TIMER_H
