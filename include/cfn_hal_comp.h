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
 * @file cfn_hal_comp.h
 * @brief Analog Comparator (COMP) HAL API.
 */

#ifndef CAFFEINE_HAL_HAL_COMP_H
#define CAFFEINE_HAL_HAL_COMP_H

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
 * @brief COMP nominal event flags.
 */
typedef enum
{
    CFN_HAL_COMP_EVENT_NONE    = 0,
    CFN_HAL_COMP_EVENT_TRIGGER = CFN_HAL_BIT(0), /*!< Comparator output triggered (crossed threshold) */
} cfn_hal_comp_event_t;

/**
 * @brief COMP exception error flags.
 */
typedef enum
{
    CFN_HAL_COMP_ERROR_NONE    = 0,
    CFN_HAL_COMP_ERROR_GENERAL = CFN_HAL_BIT(0), /*!< General hardware error */
} cfn_hal_comp_error_t;

/**
 * @brief COMP output levels.
 */
typedef enum
{
    CFN_HAL_COMP_LEVEL_LOW = 0,
    CFN_HAL_COMP_LEVEL_HIGH
} cfn_hal_comp_level_t;

/* Types Structs ----------------------------------------------------*/

/**
 * @brief COMP hardware physical mapping.
 */
typedef struct
{
    void                      *instance;    /*!< Peripheral base instance */
    cfn_hal_gpio_pin_handle_t *input_plus;  /*!< Non-inverting input pin mapping */
    cfn_hal_gpio_pin_handle_t *input_minus; /*!< Inverting input pin mapping */
    cfn_hal_gpio_pin_handle_t *output;      /*!< Comparator output pin mapping */
    void                      *user_arg;    /*!< Peripheral instance user argument */
} cfn_hal_comp_phy_t;

/**
 * @brief COMP configuration structure.
 */
typedef struct
{
    void *user_config; /*!< Vendor-specific custom configuration */
} cfn_hal_comp_config_t;

typedef struct cfn_hal_comp_s     cfn_hal_comp_t;
typedef struct cfn_hal_comp_api_s cfn_hal_comp_api_t;

/**
 * @brief COMP callback signature.
 * @param driver Pointer to the COMP driver instance.
 * @param event_mask Mask of triggered nominal events.
 * @param error_mask Mask of triggered exception errors.
 * @param user_arg User-defined argument passed during registration.
 */
typedef void (*cfn_hal_comp_callback_t)(cfn_hal_comp_t *driver,
                                        uint32_t        event_mask,
                                        uint32_t        error_mask,
                                        void           *user_arg);

/**
 * @brief COMP Virtual Method Table (VMT).
 */
struct cfn_hal_comp_api_s
{
    cfn_hal_api_base_t base;

    /* COMP Specific Extensions */
    cfn_hal_error_code_t (*read_output)(cfn_hal_comp_t *driver, cfn_hal_comp_level_t *level);
    cfn_hal_error_code_t (*set_threshold)(cfn_hal_comp_t *driver, uint32_t threshold);
    cfn_hal_error_code_t (*start)(cfn_hal_comp_t *driver);
    cfn_hal_error_code_t (*stop)(cfn_hal_comp_t *driver);
};

CFN_HAL_VMT_CHECK(struct cfn_hal_comp_api_s);

CFN_HAL_CREATE_DRIVER_TYPE(
    comp, cfn_hal_comp_config_t, cfn_hal_comp_api_t, cfn_hal_comp_phy_t, cfn_hal_comp_callback_t);

#define CFN_HAL_COMP_INITIALIZER(api_ptr, phy_ptr, config_ptr)                                                         \
    CFN_HAL_DRIVER_INITIALIZER(CFN_HAL_PERIPHERAL_TYPE_COMP, api_ptr, phy_ptr, config_ptr)

/* Functions inline ------------------------------------------------- */

/**
 * @brief Initializes the Comparator driver.
 * @param driver Pointer to the COMP driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_comp_init(cfn_hal_comp_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->base.vmt = (const struct cfn_hal_api_base_s *) driver->api;
    return cfn_hal_base_init(&driver->base, CFN_HAL_PERIPHERAL_TYPE_COMP);
}

/**
 * @brief Deinitializes the Comparator driver.
 * @param driver Pointer to the COMP driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_comp_deinit(cfn_hal_comp_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_deinit(&driver->base, CFN_HAL_PERIPHERAL_TYPE_COMP);
}

/**
 * @brief Sets the Comparator configuration.
 * @param driver Pointer to the COMP driver instance.
 * @param config Pointer to the configuration structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_comp_config_set(cfn_hal_comp_t *driver, const cfn_hal_comp_config_t *config)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    {
        driver->config = config;
    }
    return cfn_hal_base_config_set(&driver->base, CFN_HAL_PERIPHERAL_TYPE_COMP, (const void *) config);
}

/**
 * @brief Gets the current Comparator configuration.
 * @param driver Pointer to the COMP driver instance.
 * @param config [out] Pointer to store the configuration.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_comp_config_get(cfn_hal_comp_t *driver, cfn_hal_comp_config_t *config)
{
    if (!driver || !config || !driver->config)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    *config = *(driver->config);
    return CFN_HAL_ERROR_OK;
}

/**
 * @brief Registers a callback for Comparator events and errors.
 * @param driver Pointer to the COMP driver instance.
 * @param callback The callback function to register.
 * @param user_arg User-defined argument passed to the callback.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_comp_callback_register(cfn_hal_comp_t               *driver,
                                                                   const cfn_hal_comp_callback_t callback,
                                                                   void                         *user_arg)
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
        &driver->base, CFN_HAL_PERIPHERAL_TYPE_COMP, (cfn_hal_callback_t) callback, user_arg);
}

/**
 * @brief Sets the Comparator power state.
 * @param driver Pointer to the COMP driver instance.
 * @param state Target power state.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_comp_power_state_set(cfn_hal_comp_t *driver, cfn_hal_power_state_t state)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_power_state_set(&driver->base, CFN_HAL_PERIPHERAL_TYPE_COMP, state);
}

/**
 * @brief Enables one or more Comparator nominal events.
 * @param driver Pointer to the COMP driver instance.
 * @param event_mask Mask of events to enable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_comp_event_enable(cfn_hal_comp_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_enable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_COMP, event_mask);
}

/**
 * @brief Disables one or more Comparator nominal events.
 * @param driver Pointer to the COMP driver instance.
 * @param event_mask Mask of events to disable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_comp_event_disable(cfn_hal_comp_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_disable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_COMP, event_mask);
}

/**
 * @brief Retrieves the current Comparator nominal event status.
 * @param driver Pointer to the COMP driver instance.
 * @param event_mask [out] Pointer to store the event mask.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_comp_event_get(cfn_hal_comp_t *driver, uint32_t *event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_get(&driver->base, CFN_HAL_PERIPHERAL_TYPE_COMP, event_mask);
}

/**
 * @brief Enables one or more Comparator exception errors.
 * @param driver Pointer to the COMP driver instance.
 * @param error_mask Mask of errors to enable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_comp_error_enable(cfn_hal_comp_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_enable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_COMP, error_mask);
}

/**
 * @brief Disables one or more Comparator exception errors.
 * @param driver Pointer to the COMP driver instance.
 * @param error_mask Mask of errors to disable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_comp_error_disable(cfn_hal_comp_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_disable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_COMP, error_mask);
}

/**
 * @brief Retrieves the current Comparator exception error status.
 * @param driver Pointer to the COMP driver instance.
 * @param error_mask [out] Pointer to store the error mask.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_comp_error_get(cfn_hal_comp_t *driver, uint32_t *error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_get(&driver->base, CFN_HAL_PERIPHERAL_TYPE_COMP, error_mask);
}

/* COMP Specific Functions ------------------------------------------ */

/**
 * @brief Reads the immediate logical output of the comparator.
 * @param driver Pointer to the COMP driver instance.
 * @param level [out] Pointer to store the current output level.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_comp_read_output(cfn_hal_comp_t *driver, cfn_hal_comp_level_t *level)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_COMP, read_output, driver, error, level);
    return error;
}

/**
 * @brief Dynamically adjusts the reference threshold (if supported by hardware).
 * @param driver Pointer to the COMP driver instance.
 * @param threshold The new threshold value to set.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_comp_set_threshold(cfn_hal_comp_t *driver, uint32_t threshold)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_COMP, set_threshold, driver, error, threshold);
    return error;
}

/**
 * @brief Enables/Starts the comparator operation.
 * @param driver Pointer to the COMP driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_comp_start(cfn_hal_comp_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_COMP, start, driver, error);
    return error;
}

/**
 * @brief Disables/Stops the comparator operation.
 * @param driver Pointer to the COMP driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_comp_stop(cfn_hal_comp_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_COMP, stop, driver, error);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif // CAFFEINE_HAL_HAL_COMP_H
