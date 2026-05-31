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
#include "cfn_hal.h"
#include "cfn_hal_base.h"
#include "cfn_hal_types.h"

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
 * @param p_driver Pointer to the Timer driver instance.
 * @param event_mask Mask of triggered nominal events.
 * @param error_mask Mask of triggered exception errors.
 * @param channel Source channel index (if applicable).
 * @param p_user_arg User-defined argument passed during registration.
 */
typedef void (*cfn_hal_timer_callback_t)(
    cfn_hal_timer_t *p_driver, uint32_t event_mask, uint32_t error_mask, uint32_t channel, void *p_user_arg);

/**
 * @brief Timer Virtual Method Table (VMT).
 */
struct cfn_hal_timer_api_s
{
    cfn_hal_api_base_t base;

    /* Timer Specific Extensions */
    cfn_hal_error_code_t (*start)(cfn_hal_timer_t *p_driver);
    cfn_hal_error_code_t (*stop)(cfn_hal_timer_t *p_driver);
    cfn_hal_error_code_t (*get_ticks)(cfn_hal_timer_t *p_driver, uint32_t channel, uint32_t *p_ticks);
    cfn_hal_error_code_t (*get_ticks_u64)(cfn_hal_timer_t *p_driver, uint32_t channel, uint64_t *p_ticks);
    cfn_hal_error_code_t (*set_period)(cfn_hal_timer_t *p_driver, const cfn_hal_timer_period_t *p_period);
};

CFN_HAL_VMT_CHECK(struct cfn_hal_timer_api_s);

CFN_HAL_CREATE_DRIVER_TYPE(
    timer, cfn_hal_timer_config_t, cfn_hal_timer_api_t, cfn_hal_timer_phy_t, cfn_hal_timer_callback_t);

/* Functions inline ------------------------------------------------- */
CFN_HAL_INLINE void
cfn_hal_timer_populate (cfn_hal_timer_t              *p_driver,
                        uint32_t                      peripheral_id,
                        struct cfn_hal_clock_s       *p_clock,
                        void                         *p_dependency,
                        const cfn_hal_timer_api_t    *p_api,
                        const cfn_hal_timer_phy_t    *p_phy,
                        const cfn_hal_timer_config_t *p_config,
                        cfn_hal_timer_callback_t      p_callback,
                        void                         *p_user_arg)
{
    CFN_HAL_POPULATE_DRIVER(p_driver,
                            CFN_HAL_PERIPHERAL_TYPE_TIMER,
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
 * @brief Validates the Timer configuration.
 * @param p_driver Pointer to the TIMER driver instance.
 * @param p_config Pointer to the configuration structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_timer_config_validate (const cfn_hal_timer_t *p_driver, const cfn_hal_timer_config_t *p_config)
{
    if (p_driver == NULL || p_config == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    return cfn_hal_base_config_validate(&p_driver->base, CFN_HAL_PERIPHERAL_TYPE_TIMER, p_config);
}

/**
 * @brief Initializes the Timer driver.
 * @param p_driver Pointer to the Timer driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_timer_init (cfn_hal_timer_t *p_driver)
{
    if (!p_driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    p_driver->base.vmt         = (const struct cfn_hal_api_base_s *) p_driver->api;
    cfn_hal_error_code_t error = cfn_hal_timer_config_validate(p_driver, p_driver->config);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    return cfn_hal_base_init(&p_driver->base, CFN_HAL_PERIPHERAL_TYPE_TIMER);
}

/**
 * @brief Deinitializes the Timer driver.
 * @param p_driver Pointer to the Timer driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_timer_deinit (cfn_hal_timer_t *p_driver)
{
    if (!p_driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_deinit(&p_driver->base, CFN_HAL_PERIPHERAL_TYPE_TIMER);
}

/**
 * @brief Sets the Timer configuration.
 * @param p_driver Pointer to the Timer driver instance.
 * @param p_config Pointer to the configuration structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_timer_config_set (cfn_hal_timer_t *p_driver, const cfn_hal_timer_config_t *p_config)
{
    if (!p_driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    {
        p_driver->config = p_config;
    }
    return cfn_hal_base_config_set(&p_driver->base, CFN_HAL_PERIPHERAL_TYPE_TIMER, (const void *) p_config);
}

/**
 * @brief Gets the current Timer configuration.
 * @param p_driver Pointer to the Timer driver instance.
 * @param p_config [out] Pointer to store the configuration.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_timer_config_get (cfn_hal_timer_t *p_driver, cfn_hal_timer_config_t *p_config)
{
    if (!p_driver || !p_config || !p_driver->config)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    *p_config = *(p_driver->config);
    return CFN_HAL_ERROR_OK;
}

/**
 * @brief Registers a callback for Timer events and errors.
 * @param p_driver Pointer to the Timer driver instance.
 * @param callback The callback function to register.
 * @param p_user_arg User-defined argument passed to the callback.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_timer_callback_register (cfn_hal_timer_t *p_driver, const cfn_hal_timer_callback_t callback, void *p_user_arg)
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
        &p_driver->base, CFN_HAL_PERIPHERAL_TYPE_TIMER, (cfn_hal_callback_t) callback, p_user_arg);
}

/**
 * @brief Sets the Timer power state.
 * @param p_driver Pointer to the Timer driver instance.
 * @param state Target power state.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_timer_power_state_set (cfn_hal_timer_t *p_driver, cfn_hal_power_state_t state)
{
    if (!p_driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_power_state_set(&p_driver->base, CFN_HAL_PERIPHERAL_TYPE_TIMER, state);
}

/**
 * @brief Enables one or more Timer nominal events.
 * @param p_driver Pointer to the Timer driver instance.
 * @param event_mask Mask of events to enable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_timer_event_enable (cfn_hal_timer_t *p_driver, uint32_t event_mask)
{
    if (!p_driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_enable(&p_driver->base, CFN_HAL_PERIPHERAL_TYPE_TIMER, event_mask);
}

/**
 * @brief Disables one or more Timer nominal events.
 * @param p_driver Pointer to the Timer driver instance.
 * @param event_mask Mask of events to disable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_timer_event_disable (cfn_hal_timer_t *p_driver, uint32_t event_mask)
{
    if (!p_driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_disable(&p_driver->base, CFN_HAL_PERIPHERAL_TYPE_TIMER, event_mask);
}

/**
 * @brief Retrieves the current Timer nominal event status.
 * @param p_driver Pointer to the Timer driver instance.
 * @param p_event_mask [out] Pointer to store the event mask.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_timer_event_get (cfn_hal_timer_t *p_driver, uint32_t *p_event_mask)
{
    if (!p_driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_get(&p_driver->base, CFN_HAL_PERIPHERAL_TYPE_TIMER, p_event_mask);
}

/**
 * @brief Enables one or more Timer exception errors.
 * @param p_driver Pointer to the Timer driver instance.
 * @param error_mask Mask of errors to enable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_timer_error_enable (cfn_hal_timer_t *p_driver, uint32_t error_mask)
{
    if (!p_driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_enable(&p_driver->base, CFN_HAL_PERIPHERAL_TYPE_TIMER, error_mask);
}

/**
 * @brief Disables one or more Timer exception errors.
 * @param p_driver Pointer to the Timer driver instance.
 * @param error_mask Mask of errors to disable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_timer_error_disable (cfn_hal_timer_t *p_driver, uint32_t error_mask)
{
    if (!p_driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_disable(&p_driver->base, CFN_HAL_PERIPHERAL_TYPE_TIMER, error_mask);
}

/**
 * @brief Retrieves the current Timer exception error status.
 * @param p_driver Pointer to the Timer driver instance.
 * @param p_error_mask [out] Pointer to store the error mask.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_timer_error_get (cfn_hal_timer_t *p_driver, uint32_t *p_error_mask)
{
    if (!p_driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_get(&p_driver->base, CFN_HAL_PERIPHERAL_TYPE_TIMER, p_error_mask);
}

/* Timer Specific Functions ----------------------------------------- */

/**
 * @brief Starts the timer counter.
 * @param p_driver Pointer to the Timer driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_timer_start (cfn_hal_timer_t *p_driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_TIMER, start, p_driver, error);
    return error;
}

/**
 * @brief Stops the timer counter.
 * @param p_driver Pointer to the Timer driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_timer_stop (cfn_hal_timer_t *p_driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_TIMER, stop, p_driver, error);
    return error;
}

/**
 * @brief Reads the current tick count for a specific channel (32-bit).
 * @param p_driver Pointer to the Timer driver instance.
 * @param channel Channel index.
 * @param p_ticks [out] Pointer to store the tick count.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_timer_get_ticks (cfn_hal_timer_t *p_driver, uint32_t channel, uint32_t *p_ticks)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_TIMER, get_ticks, p_driver, error, channel, p_ticks);
    return error;
}

/**
 * @brief Reads the current tick count for a specific channel (64-bit).
 * @param p_driver Pointer to the Timer driver instance.
 * @param channel Channel index.
 * @param p_ticks [out] Pointer to store the tick count.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_timer_get_ticks_u64 (cfn_hal_timer_t *p_driver,
                             uint32_t         channel,
                             uint64_t        *p_ticks) // NOLINT(readability-non-const-parameter)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_TIMER, get_ticks_u64, p_driver, error, channel, p_ticks);
    return error;
}

/**
 * @brief Updates the timer period/reload value for a specific channel.
 * @param p_driver Pointer to the Timer driver instance.
 * @param p_period Pointer to the structure defining channel and new tick value.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_timer_set_period (cfn_hal_timer_t *p_driver, const cfn_hal_timer_period_t *p_period)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_TIMER, set_period, p_driver, error, p_period);
    return error;
}

cfn_hal_error_code_t cfn_hal_timer_construct (cfn_hal_timer_t              *p_driver,
                                              const cfn_hal_timer_config_t *p_config,
                                              const cfn_hal_timer_phy_t    *p_phy,
                                              struct cfn_hal_clock_s       *p_clock,
                                              void                         *p_dependency,
                                              cfn_hal_timer_callback_t      p_callback,
                                              void                         *p_user_arg);
cfn_hal_error_code_t cfn_hal_timer_destruct (cfn_hal_timer_t *p_driver);
#ifdef __cplusplus
}
#endif

#endif // CAFFEINE_HAL_HAL_TIMER_H
