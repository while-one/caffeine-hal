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
 * @file cfn_hal_rtc.h
 * @brief Real-Time Clock (RTC) HAL API.
 */

#ifndef CAFFEINE_HAL_HAL_RTC_H
#define CAFFEINE_HAL_HAL_RTC_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_hal.h"
#include "cfn_hal_base.h"
#include "cfn_hal_types.h"
#include <time.h>

/* Defines ----------------------------------------------------------*/

/* Types Enums ------------------------------------------------------*/

/**
 * @brief RTC nominal event flags.
 */
typedef enum
{
    CFN_HAL_RTC_EVENT_NONE      = 0,
    CFN_HAL_RTC_EVENT_ALARM     = CFN_HAL_BIT(0), /*!< Alarm time reached */
    CFN_HAL_RTC_EVENT_WAKEUP    = CFN_HAL_BIT(1), /*!< Periodic wakeup timer event */
    CFN_HAL_RTC_EVENT_TIMESTAMP = CFN_HAL_BIT(2), /*!< External timestamp capture event */
    CFN_HAL_RTC_EVENT_TAMPER    = CFN_HAL_BIT(3), /*!< Security tamper event */
} cfn_hal_rtc_event_t;

/**
 * @brief RTC exception error flags.
 */
typedef enum
{
    CFN_HAL_RTC_ERROR_NONE    = 0,
    CFN_HAL_RTC_ERROR_INIT    = CFN_HAL_BIT(0), /*!< Synchronization or oscillator error */
    CFN_HAL_RTC_ERROR_GENERAL = CFN_HAL_BIT(1), /*!< General hardware error */
} cfn_hal_rtc_error_t;

/**
 * @brief RTC time format.
 */
typedef enum
{
    CFN_HAL_RTC_CONFIG_FORMAT_12H, /*!< 12-hour clock (AM/PM) */
    CFN_HAL_RTC_CONFIG_FORMAT_24H, /*!< 24-hour clock */

    CFN_HAL_RTC_CONFIG_FORMAT_MAX
} cfn_hal_rtc_config_format_t;

/**
 * @brief RTC clocking mode.
 */
typedef enum
{
    CFN_HAL_RTC_CONFIG_MODE_BCD,        /*!< Binary Coded Decimal format */
    CFN_HAL_RTC_CONFIG_MODE_BINARY,     /*!< Standard binary format */
    CFN_HAL_RTC_CONFIG_MODE_BINARY_BCD, /*!< Mixed mode support */

    CFN_HAL_RTC_CONFIG_MODE_MAX
} cfn_hal_rtc_config_mode_t;

/* Types Structs ----------------------------------------------------*/

/**
 * @brief RTC time representation (std tm).
 */
typedef struct tm cfn_hal_rtc_time_t;

/**
 * @brief RTC hardware physical mapping.
 */
typedef struct
{
    void *instance; /*!< Peripheral base instance */
    void *user_arg; /*!< Peripheral instance user argument */
} cfn_hal_rtc_phy_t;

/**
 * @brief RTC configuration structure.
 */
typedef struct
{
    cfn_hal_rtc_config_format_t format;           /*!< Time notation format */
    cfn_hal_rtc_config_mode_t   mode;             /*!< Hardware clocking mode */
    uint32_t                    target_rtc_clock; /*!< Input clock source frequency */
    void                       *custom;           /*!< Vendor-specific custom configuration */
} cfn_hal_rtc_config_t;

typedef struct cfn_hal_rtc_s     cfn_hal_rtc_t;
typedef struct cfn_hal_rtc_api_s cfn_hal_rtc_api_t;

/**
 * @brief RTC callback signature.
 * @param driver Pointer to the RTC driver instance.
 * @param event_mask Mask of triggered nominal events.
 * @param error_mask Mask of triggered exception errors.
 * @param id Identifier of the source (e.g., Alarm index).
 * @param user_arg User-defined argument passed during registration.
 */
typedef void (*cfn_hal_rtc_callback_t)(
    cfn_hal_rtc_t *driver, uint32_t event_mask, uint32_t error_mask, uint32_t id, void *user_arg);

/**
 * @brief RTC Virtual Method Table (VMT).
 */
struct cfn_hal_rtc_api_s
{
    cfn_hal_api_base_t base;

    /* RTC Specific Extensions */
    cfn_hal_error_code_t (*set_time)(cfn_hal_rtc_t *driver, cfn_hal_rtc_time_t *time);
    cfn_hal_error_code_t (*get_time)(cfn_hal_rtc_t *driver, cfn_hal_rtc_time_t *time);
    cfn_hal_error_code_t (*set_alarm)(cfn_hal_rtc_t *driver, uint32_t id, cfn_hal_rtc_time_t *time);
    cfn_hal_error_code_t (*get_alarm)(cfn_hal_rtc_t *driver, uint32_t id, bool *elapsed, cfn_hal_rtc_time_t *time);
    cfn_hal_error_code_t (*stop_alarm)(cfn_hal_rtc_t *driver, uint32_t id);
};

CFN_HAL_VMT_CHECK(struct cfn_hal_rtc_api_s);

CFN_HAL_CREATE_DRIVER_TYPE(rtc, cfn_hal_rtc_config_t, cfn_hal_rtc_api_t, cfn_hal_rtc_phy_t, cfn_hal_rtc_callback_t);
/* Functions inline ------------------------------------------------- */
CFN_HAL_INLINE void cfn_hal_rtc_populate(cfn_hal_rtc_t              *driver,
                                         uint32_t                    peripheral_id,
                                         struct cfn_hal_clock_s     *clock,
                                         void                       *dependency,
                                         const cfn_hal_rtc_api_t    *api,
                                         const cfn_hal_rtc_phy_t    *phy,
                                         const cfn_hal_rtc_config_t *config,
                                         cfn_hal_rtc_callback_t      callback,
                                         void                       *user_arg)
{
    CFN_HAL_POPULATE_DRIVER(
        driver, CFN_HAL_PERIPHERAL_TYPE_RTC, peripheral_id, clock, dependency, api, phy, config, callback, user_arg);
}
/**
 * @brief Validates the RTC configuration.
 * @param driver Pointer to the RTC driver instance.
 * @param config Pointer to the configuration structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_rtc_config_validate(const cfn_hal_rtc_t        *driver,
                                                                const cfn_hal_rtc_config_t *config)
{
    if (driver == NULL || config == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    if (config->format >= CFN_HAL_RTC_CONFIG_FORMAT_MAX || config->mode >= CFN_HAL_RTC_CONFIG_MODE_MAX)
    {
        return CFN_HAL_ERROR_BAD_CONFIG;
    }

    return cfn_hal_base_config_validate(&driver->base, CFN_HAL_PERIPHERAL_TYPE_RTC, config);
    ;
}

/**
 * @brief Initializes the RTC driver.
 * @param driver Pointer to the RTC driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_rtc_init(cfn_hal_rtc_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->base.vmt           = (const struct cfn_hal_api_base_s *) driver->api;
    cfn_hal_error_code_t error = cfn_hal_rtc_config_validate(driver, driver->config);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    return cfn_hal_base_init(&driver->base, CFN_HAL_PERIPHERAL_TYPE_RTC);
}

/**
 * @brief Deinitializes the RTC driver.
 * @param driver Pointer to the RTC driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_rtc_deinit(cfn_hal_rtc_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_deinit(&driver->base, CFN_HAL_PERIPHERAL_TYPE_RTC);
}

/**
 * @brief Sets the RTC configuration.
 * @param driver Pointer to the RTC driver instance.
 * @param config Pointer to the configuration structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_rtc_config_set(cfn_hal_rtc_t *driver, const cfn_hal_rtc_config_t *config)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    cfn_hal_error_code_t error = cfn_hal_rtc_config_validate(driver, config);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    {
        driver->config = config;
    }
    return cfn_hal_base_config_set(&driver->base, CFN_HAL_PERIPHERAL_TYPE_RTC, (const void *) config);
}

/**
 * @brief Gets the current RTC configuration.
 * @param driver Pointer to the RTC driver instance.
 * @param config [out] Pointer to store the configuration.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_rtc_config_get(cfn_hal_rtc_t *driver, cfn_hal_rtc_config_t *config)
{
    if (!driver || !config || !driver->config)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    *config = *(driver->config);
    return CFN_HAL_ERROR_OK;
}

/**
 * @brief Registers a callback for RTC events and errors.
 * @param driver Pointer to the RTC driver instance.
 * @param callback The callback function to register.
 * @param user_arg User-defined argument passed to the callback.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_rtc_callback_register(cfn_hal_rtc_t               *driver,
                                                                  const cfn_hal_rtc_callback_t callback,
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
        &driver->base, CFN_HAL_PERIPHERAL_TYPE_RTC, (cfn_hal_callback_t) callback, user_arg);
}

/**
 * @brief Sets the RTC power state.
 * @param driver Pointer to the RTC driver instance.
 * @param state Target power state.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_rtc_power_state_set(cfn_hal_rtc_t *driver, cfn_hal_power_state_t state)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_power_state_set(&driver->base, CFN_HAL_PERIPHERAL_TYPE_RTC, state);
}

/**
 * @brief Enables one or more RTC nominal events.
 * @param driver Pointer to the RTC driver instance.
 * @param event_mask Mask of events to enable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_rtc_event_enable(cfn_hal_rtc_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_enable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_RTC, event_mask);
}

/**
 * @brief Disables one or more RTC nominal events.
 * @param driver Pointer to the RTC driver instance.
 * @param event_mask Mask of events to disable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_rtc_event_disable(cfn_hal_rtc_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_disable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_RTC, event_mask);
}

/**
 * @brief Retrieves the current RTC nominal event status.
 * @param driver Pointer to the RTC driver instance.
 * @param event_mask [out] Pointer to store the event mask.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_rtc_event_get(cfn_hal_rtc_t *driver, uint32_t *event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_get(&driver->base, CFN_HAL_PERIPHERAL_TYPE_RTC, event_mask);
}

/**
 * @brief Enables one or more RTC exception errors.
 * @param driver Pointer to the RTC driver instance.
 * @param error_mask Mask of errors to enable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_rtc_error_enable(cfn_hal_rtc_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_enable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_RTC, error_mask);
}

/**
 * @brief Disables one or more RTC exception errors.
 * @param driver Pointer to the RTC driver instance.
 * @param error_mask Mask of errors to disable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_rtc_error_disable(cfn_hal_rtc_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_disable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_RTC, error_mask);
}

/**
 * @brief Retrieves the current RTC exception error status.
 * @param driver Pointer to the RTC driver instance.
 * @param error_mask [out] Pointer to store the error mask.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_rtc_error_get(cfn_hal_rtc_t *driver, uint32_t *error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_get(&driver->base, CFN_HAL_PERIPHERAL_TYPE_RTC, error_mask);
}

/* RTC Specific Functions ------------------------------------------- */

/**
 * @brief Sets the RTC current wall-clock time.
 * @param driver Pointer to the RTC driver instance.
 * @param time Pointer to the structure containing the new time.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_rtc_set_time(cfn_hal_rtc_t *driver, cfn_hal_rtc_time_t *time)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_RTC, set_time, driver, error, time);
    return error;
}

/**
 * @brief Retrieves the current wall-clock time from the RTC.
 * @param driver Pointer to the RTC driver instance.
 * @param time [out] Pointer to store the current time.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_rtc_get_time(cfn_hal_rtc_t *driver, cfn_hal_rtc_time_t *time)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_RTC, get_time, driver, error, time);
    return error;
}

/**
 * @brief Configures an alarm event for a specific time.
 * @param driver Pointer to the RTC driver instance.
 * @param id Index of the alarm to set.
 * @param time Pointer to the structure containing the alarm time.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_rtc_set_alarm(cfn_hal_rtc_t *driver, uint32_t id, cfn_hal_rtc_time_t *time)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_RTC, set_alarm, driver, error, id, time);
    return error;
}

/**
 * @brief Retrieves configuration and status of a specific alarm.
 * @param driver Pointer to the RTC driver instance.
 * @param id Index of the alarm.
 * @param elapsed [out] Set to true if the alarm has triggered.
 * @param time [out] Pointer to store the configured alarm time.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_rtc_get_alarm(cfn_hal_rtc_t      *driver,
                                                          uint32_t            id,
                                                          bool               *elapsed,
                                                          cfn_hal_rtc_time_t *time)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_RTC, get_alarm, driver, error, id, elapsed, time);
    return error;
}

/**
 * @brief Disables a previously configured alarm.
 * @param driver Pointer to the RTC driver instance.
 * @param id Index of the alarm to stop.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_rtc_stop_alarm(cfn_hal_rtc_t *driver, uint32_t id)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_RTC, stop_alarm, driver, error, id);
    return error;
}
cfn_hal_error_code_t cfn_hal_rtc_construct(cfn_hal_rtc_t              *driver,
                                           const cfn_hal_rtc_config_t *config,
                                           const cfn_hal_rtc_phy_t    *phy,
                                           struct cfn_hal_clock_s     *clock,
                                           void                       *dependency,
                                           cfn_hal_rtc_callback_t      callback,
                                           void                       *user_arg);
cfn_hal_error_code_t cfn_hal_rtc_destruct(cfn_hal_rtc_t *driver);
#ifdef __cplusplus
}
#endif

#endif // CAFFEINE_HAL_HAL_RTC_H
