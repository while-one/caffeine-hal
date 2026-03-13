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
 * @file cfn_hal_wdt.h
 * @brief Watchdog Timer (WDT) HAL API.
 */

#ifndef CAFFEINE_HAL_HAL_WDT_H
#define CAFFEINE_HAL_HAL_WDT_H

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
 * @brief WDT nominal event flags.
 */
typedef enum
{
    CFN_HAL_WDT_EVENT_NONE = 0,
    CFN_HAL_WDT_EVENT_WARNING = CFN_HAL_BIT(0), /*!< Early warning interrupt (before reset) */
} cfn_hal_wdt_event_t;

/**
 * @brief WDT exception error flags.
 */
typedef enum
{
    CFN_HAL_WDT_ERROR_NONE = 0,
    CFN_HAL_WDT_ERROR_GENERAL = CFN_HAL_BIT(0), /*!< General hardware error */
} cfn_hal_wdt_error_t;

/**
 * @brief WDT sleep behavior.
 */
typedef enum
{
    CFN_HAL_WDT_CONFIG_SLEEP_RUN,   /*!< Continue counting during sleep */
    CFN_HAL_WDT_CONFIG_SLEEP_PAUSE, /*!< Freeze counter during sleep */
} cfn_hal_wdt_config_sleep_t;

/**
 * @brief WDT reset behavior.
 */
typedef enum
{
    CFN_HAL_WDT_CONFIG_RESET_NONE, /*!< No reset (interrupt only) */
    CFN_HAL_WDT_CONFIG_RESET_IRQ,  /*!< Generate interrupt then reset */
    CFN_HAL_WDT_CONFIG_RESET_CPU,  /*!< Immediate CPU reset */
} cfn_hal_wdt_config_reset_t;

/* Types Structs ----------------------------------------------------*/

/**
 * @brief WDT hardware physical mapping.
 */
typedef struct
{
    void *instance; /*!< Peripheral base instance */
    void *user_arg; /*!< Peripheral instance user argument */
} cfn_hal_wdt_phy_t;

/**
 * @brief WDT configuration structure.
 */
typedef struct
{
    cfn_hal_wdt_config_sleep_t sleep;  /*!< Behavior during system sleep */
    cfn_hal_wdt_config_reset_t reset;  /*!< Reset strategy on timeout */
    void                      *custom; /*!< Vendor-specific custom configuration */
} cfn_hal_wdt_config_t;

typedef struct cfn_hal_wdt_s     cfn_hal_wdt_t;
typedef struct cfn_hal_wdt_api_s cfn_hal_wdt_api_t;

/**
 * @brief WDT callback signature.
 * @param driver Pointer to the WDT driver instance.
 * @param event_mask Mask of triggered nominal events.
 * @param error_mask Mask of triggered exception errors.
 * @param user_arg User-defined argument passed during registration.
 */
typedef void (*cfn_hal_wdt_callback_t)(cfn_hal_wdt_t *driver, uint32_t event_mask, uint32_t error_mask, void *user_arg);

/**
 * @brief WDT Virtual Method Table (VMT).
 */
struct cfn_hal_wdt_api_s
{
    cfn_hal_api_base_t base;

    /* WDT Specific Extensions */
    cfn_hal_error_code_t (*start)(cfn_hal_wdt_t *driver);
    cfn_hal_error_code_t (*stop)(cfn_hal_wdt_t *driver);
    cfn_hal_error_code_t (*feed)(cfn_hal_wdt_t *driver);
};
CFN_HAL_VMT_CHECK(struct cfn_hal_wdt_api_s);

CFN_HAL_CREATE_DRIVER_TYPE(wdt, cfn_hal_wdt_config_t, cfn_hal_wdt_api_t, cfn_hal_wdt_phy_t, cfn_hal_wdt_callback_t);

#define CFN_HAL_WDT_INITIALIZER(api_ptr, phy_ptr, config_ptr)                                                          \
    CFN_HAL_DRIVER_INITIALIZER(CFN_HAL_PERIPHERAL_TYPE_WDT, api_ptr, phy_ptr, config_ptr)

/* Functions inline ------------------------------------------------- */

/**
 * @brief Initializes the Watchdog driver.
 * @param driver Pointer to the WDT driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_wdt_init(cfn_hal_wdt_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->base.vmt = (const void *) driver->api;
    return cfn_hal_base_init(&driver->base, CFN_HAL_PERIPHERAL_TYPE_WDT);
}

/**
 * @brief Deinitializes the Watchdog driver.
 * @param driver Pointer to the WDT driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_wdt_deinit(cfn_hal_wdt_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_deinit(&driver->base, CFN_HAL_PERIPHERAL_TYPE_WDT);
}

/**
 * @brief Sets the WDT configuration.
 * @param driver Pointer to the WDT driver instance.
 * @param config Pointer to the configuration structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_wdt_config_set(cfn_hal_wdt_t *driver, const cfn_hal_wdt_config_t *config)
{
    if (driver)
    {
        driver->config = config;
    }
    return cfn_hal_base_config_set(&driver->base, CFN_HAL_PERIPHERAL_TYPE_WDT, (const void *) config);
}

/**
 * @brief Gets the current WDT configuration.
 * @param driver Pointer to the WDT driver instance.
 * @param config [out] Pointer to store the configuration.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_wdt_config_get(cfn_hal_wdt_t *driver, cfn_hal_wdt_config_t *config)
{
    if (!driver || !config || !driver->config)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    *config = *(driver->config);
    return CFN_HAL_ERROR_OK;
}

/**
 * @brief Registers a callback for WDT events and errors.
 * @param driver Pointer to the WDT driver instance.
 * @param callback The callback function to register.
 * @param user_arg User-defined argument passed to the callback.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_wdt_callback_register(cfn_hal_wdt_t               *driver,
                                                                  const cfn_hal_wdt_callback_t callback,
                                                                  void                        *user_arg)
{
    if (driver)
    {
        driver->cb = callback;
        driver->cb_user_arg = user_arg;
    }
    return cfn_hal_base_callback_register(
        &driver->base, CFN_HAL_PERIPHERAL_TYPE_WDT, (cfn_hal_callback_t) callback, user_arg);
}

/**
 * @brief Sets the WDT power state.
 * @param driver Pointer to the WDT driver instance.
 * @param state Target power state.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_wdt_power_state_set(cfn_hal_wdt_t *driver, cfn_hal_power_state_t state)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_power_state_set(&driver->base, CFN_HAL_PERIPHERAL_TYPE_WDT, state);
}

/**
 * @brief Enables one or more WDT nominal events.
 * @param driver Pointer to the WDT driver instance.
 * @param event_mask Mask of events to enable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_wdt_event_enable(cfn_hal_wdt_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_enable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_WDT, event_mask);
}

/**
 * @brief Disables one or more WDT nominal events.
 * @param driver Pointer to the WDT driver instance.
 * @param event_mask Mask of events to disable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_wdt_event_disable(cfn_hal_wdt_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_disable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_WDT, event_mask);
}

/**
 * @brief Retrieves the current WDT nominal event status.
 * @param driver Pointer to the WDT driver instance.
 * @param event_mask [out] Pointer to store the event mask.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_wdt_event_get(cfn_hal_wdt_t *driver, uint32_t *event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_get(&driver->base, CFN_HAL_PERIPHERAL_TYPE_WDT, event_mask);
}

/**
 * @brief Enables one or more WDT exception errors.
 * @param driver Pointer to the WDT driver instance.
 * @param error_mask Mask of errors to enable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_wdt_error_enable(cfn_hal_wdt_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_enable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_WDT, error_mask);
}

/**
 * @brief Disables one or more WDT exception errors.
 * @param driver Pointer to the WDT driver instance.
 * @param error_mask Mask of errors to disable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_wdt_error_disable(cfn_hal_wdt_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_disable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_WDT, error_mask);
}

/**
 * @brief Retrieves the current WDT exception error status.
 * @param driver Pointer to the WDT driver instance.
 * @param error_mask [out] Pointer to store the error mask.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_wdt_error_get(cfn_hal_wdt_t *driver, uint32_t *error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_get(&driver->base, CFN_HAL_PERIPHERAL_TYPE_WDT, error_mask);
}

/* WDT Specific Functions ------------------------------------------- */

/**
 * @brief Starts the watchdog timer.
 * @param driver Pointer to the WDT driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_wdt_start(cfn_hal_wdt_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_WDT, start, driver, error);
    return error;
}

/**
 * @brief Stops the watchdog timer (if supported by hardware).
 * @param driver Pointer to the WDT driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_wdt_stop(cfn_hal_wdt_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_WDT, stop, driver, error);
    return error;
}

/**
 * @brief Feeds (refreshes) the watchdog timer to prevent system reset.
 * @param driver Pointer to the WDT driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_wdt_feed(cfn_hal_wdt_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_WDT, feed, driver, error);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif // CAFFEINE_HAL_HAL_WDT_H
