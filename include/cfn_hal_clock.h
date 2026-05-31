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
 * @file cfn_hal_clock.h
 * @brief Clock Management HAL API.
 */

#ifndef CAFFEINE_HAL_HAL_CLOCK_H
#define CAFFEINE_HAL_HAL_CLOCK_H

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
 * @brief Clock nominal event flags.
 */
typedef enum
{
    CFN_HAL_CLOCK_EVENT_NONE  = 0,
    CFN_HAL_CLOCK_EVENT_READY = CFN_HAL_BIT(0), /*!< Clock source is stable and ready */
} cfn_hal_clock_event_t;

/**
 * @brief Clock exception error flags.
 */
typedef enum
{
    CFN_HAL_CLOCK_ERROR_NONE     = 0,
    CFN_HAL_CLOCK_ERROR_LSE_FAIL = CFN_HAL_BIT(0), /*!< Low Speed External oscillator failure */
    CFN_HAL_CLOCK_ERROR_HSE_FAIL = CFN_HAL_BIT(1), /*!< High Speed External oscillator failure */
    CFN_HAL_CLOCK_ERROR_GENERAL  = CFN_HAL_BIT(2), /*!< General clock hardware error */
} cfn_hal_clock_error_t;

/* Types Structs ----------------------------------------------------*/

/**
 * @brief Clock configuration structure.
 */
typedef struct
{
    void *user_config; /*!< Vendor-specific clock tree configuration */
} cfn_hal_clock_config_t;

/**
 * @brief Clock hardware physical mapping.
 */
typedef struct
{
    void *instance; /*!< Peripheral base instance */
    void *user_arg; /*!< Peripheral instance user argument */
} cfn_hal_clock_phy_t;

typedef struct cfn_hal_clock_s     cfn_hal_clock_t;
typedef struct cfn_hal_clock_api_s cfn_hal_clock_api_t;

/**
 * @brief Clock callback signature.
 * @param driver Pointer to the Clock driver instance.
 * @param event_mask Mask of triggered nominal events.
 * @param error_mask Mask of triggered exception errors.
 * @param user_arg User-defined argument passed during registration.
 */
typedef void (*cfn_hal_clock_callback_t)(cfn_hal_clock_t *p_driver,
                                         uint32_t         event_mask,
                                         uint32_t         error_mask,
                                         void            *p_user_arg);

/**
 * @brief Clock Virtual Method Table (VMT).
 */
struct cfn_hal_clock_api_s
{
    cfn_hal_api_base_t base;

    /* Clock Specific Extensions */
    cfn_hal_error_code_t (*suspend_tick)(cfn_hal_clock_t *p_driver);
    cfn_hal_error_code_t (*resume_tick)(cfn_hal_clock_t *p_driver);
    cfn_hal_error_code_t (*get_system_freq)(cfn_hal_clock_t *p_driver, uint32_t *p_freq_hz);
    cfn_hal_error_code_t (*get_peripheral_freq)(cfn_hal_clock_t *p_driver, uint32_t peripheral_id, uint32_t *p_freq_hz);
    cfn_hal_error_code_t (*enable_gate)(cfn_hal_clock_t *p_driver, uint32_t peripheral_id);
    cfn_hal_error_code_t (*disable_gate)(cfn_hal_clock_t *p_driver, uint32_t peripheral_id);
};

CFN_HAL_VMT_CHECK(struct cfn_hal_clock_api_s);

CFN_HAL_CREATE_DRIVER_TYPE(
    clock, cfn_hal_clock_config_t, cfn_hal_clock_api_t, cfn_hal_clock_phy_t, cfn_hal_clock_callback_t);

/* Functions inline ------------------------------------------------- */
CFN_HAL_INLINE void
cfn_hal_clock_populate (cfn_hal_clock_t              *p_driver,
                        uint32_t                      peripheral_id,
                        struct cfn_hal_clock_s       *p_clock,
                        void                         *p_dependency,
                        const cfn_hal_clock_api_t    *p_api,
                        const cfn_hal_clock_phy_t    *p_phy,
                        const cfn_hal_clock_config_t *p_config,
                        cfn_hal_clock_callback_t      p_callback,
                        void                         *p_user_arg)
{
    CFN_HAL_POPULATE_DRIVER(p_driver,
                            CFN_HAL_PERIPHERAL_TYPE_CLOCK,
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
 * @brief Validates the Clock configuration.
 * @param driver Pointer to the CLOCK driver instance.
 * @param config Pointer to the configuration structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_clock_config_validate (const cfn_hal_clock_t *p_driver, const cfn_hal_clock_config_t *p_config)
{
    if (p_driver == NULL || p_config == NULL)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    return cfn_hal_base_config_validate(&p_driver->base, CFN_HAL_PERIPHERAL_TYPE_CLOCK, p_config);
}

/**
 * @brief Initializes the Clock driver.
 * @param driver Pointer to the Clock driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_clock_init (cfn_hal_clock_t *p_driver)
{
    if (!p_driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    p_driver->base.vmt         = (const struct cfn_hal_api_base_s *) p_driver->api;
    cfn_hal_error_code_t error = cfn_hal_clock_config_validate(p_driver, p_driver->config);
    if (error != CFN_HAL_ERROR_OK)
    {
        return error;
    }
    return cfn_hal_base_init(&p_driver->base, CFN_HAL_PERIPHERAL_TYPE_CLOCK);
}

/**
 * @brief Deinitializes the Clock driver.
 * @param driver Pointer to the Clock driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_clock_deinit (cfn_hal_clock_t *p_driver)
{
    if (!p_driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_deinit(&p_driver->base, CFN_HAL_PERIPHERAL_TYPE_CLOCK);
}

/**
 * @brief Sets the Clock configuration.
 * @param driver Pointer to the Clock driver instance.
 * @param config Pointer to the configuration structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_clock_config_set (cfn_hal_clock_t *p_driver, const cfn_hal_clock_config_t *p_config)
{
    if (!p_driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    {
        p_driver->config = p_config;
    }
    return cfn_hal_base_config_set(&p_driver->base, CFN_HAL_PERIPHERAL_TYPE_CLOCK, (const void *) p_config);
}

/**
 * @brief Gets the current Clock configuration.
 * @param driver Pointer to the Clock driver instance.
 * @param config [out] Pointer to store the configuration.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_clock_config_get (cfn_hal_clock_t *p_driver, cfn_hal_clock_config_t *p_config)
{
    if (!p_driver || !p_config || !p_driver->config)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    *p_config = *(p_driver->config);
    return CFN_HAL_ERROR_OK;
}

/**
 * @brief Registers a callback for Clock events and errors.
 * @param driver Pointer to the Clock driver instance.
 * @param callback The callback function to register.
 * @param user_arg User-defined argument passed to the callback.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_clock_callback_register (cfn_hal_clock_t *p_driver, const cfn_hal_clock_callback_t callback, void *p_user_arg)
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
        &p_driver->base, CFN_HAL_PERIPHERAL_TYPE_CLOCK, (cfn_hal_callback_t) callback, p_user_arg);
}

/**
 * @brief Sets the Clock power state.
 * @param driver Pointer to the Clock driver instance.
 * @param state Target power state.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_clock_power_state_set (cfn_hal_clock_t *p_driver, cfn_hal_power_state_t state)
{
    if (!p_driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_power_state_set(&p_driver->base, CFN_HAL_PERIPHERAL_TYPE_CLOCK, state);
}

/**
 * @brief Enables one or more Clock nominal events.
 * @param driver Pointer to the Clock driver instance.
 * @param event_mask Mask of events to enable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_clock_event_enable (cfn_hal_clock_t *p_driver, uint32_t event_mask)
{
    if (!p_driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_enable(&p_driver->base, CFN_HAL_PERIPHERAL_TYPE_CLOCK, event_mask);
}

/**
 * @brief Disables one or more Clock nominal events.
 * @param driver Pointer to the Clock driver instance.
 * @param event_mask Mask of events to disable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_clock_event_disable (cfn_hal_clock_t *p_driver, uint32_t event_mask)
{
    if (!p_driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_disable(&p_driver->base, CFN_HAL_PERIPHERAL_TYPE_CLOCK, event_mask);
}

/**
 * @brief Retrieves the current Clock nominal event status.
 * @param driver Pointer to the Clock driver instance.
 * @param event_mask [out] Pointer to store the event mask.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_clock_event_get (cfn_hal_clock_t *p_driver, uint32_t *p_event_mask)
{
    if (!p_driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_get(&p_driver->base, CFN_HAL_PERIPHERAL_TYPE_CLOCK, p_event_mask);
}

/**
 * @brief Enables one or more Clock exception errors.
 * @param driver Pointer to the Clock driver instance.
 * @param error_mask Mask of errors to enable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_clock_error_enable (cfn_hal_clock_t *p_driver, uint32_t error_mask)
{
    if (!p_driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_enable(&p_driver->base, CFN_HAL_PERIPHERAL_TYPE_CLOCK, error_mask);
}

/**
 * @brief Disables one or more Clock exception errors.
 * @param driver Pointer to the Clock driver instance.
 * @param error_mask Mask of errors to disable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_clock_error_disable (cfn_hal_clock_t *p_driver, uint32_t error_mask)
{
    if (!p_driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_disable(&p_driver->base, CFN_HAL_PERIPHERAL_TYPE_CLOCK, error_mask);
}

/**
 * @brief Retrieves the current Clock exception error status.
 * @param driver Pointer to the Clock driver instance.
 * @param error_mask [out] Pointer to store the error mask.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_clock_error_get (cfn_hal_clock_t *p_driver, uint32_t *p_error_mask)
{
    if (!p_driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_get(&p_driver->base, CFN_HAL_PERIPHERAL_TYPE_CLOCK, p_error_mask);
}

/* Clock Specific Functions ----------------------------------------- */

/**
 * @brief Suspends the system tick (e.g. before entering sleep).
 * @param driver Pointer to the Clock driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_clock_suspend_tick (cfn_hal_clock_t *p_driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_CLOCK, suspend_tick, p_driver, error);
    return error;
}

/**
 * @brief Resumes the system tick (e.g. after waking up).
 * @param driver Pointer to the Clock driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_clock_resume_tick (cfn_hal_clock_t *p_driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_CLOCK, resume_tick, p_driver, error);
    return error;
}

/**
 * @brief Gets the current main system clock frequency.
 * @param driver Pointer to the Clock driver instance.
 * @param freq_hz [out] Pointer to store the frequency in Hz.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_clock_get_system_freq (cfn_hal_clock_t *p_driver, uint32_t *p_freq_hz)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_CLOCK, get_system_freq, p_driver, error, p_freq_hz);
    return error;
}

/**
 * @brief Gets the clock frequency for a specific peripheral domain.
 * @param driver Pointer to the Clock driver instance.
 * @param peripheral_id ID or FourCC of the target peripheral.
 * @param freq_hz [out] Pointer to store the frequency in Hz.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_clock_get_peripheral_freq (cfn_hal_clock_t *p_driver, uint32_t peripheral_id, uint32_t *p_freq_hz)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(
        CFN_HAL_PERIPHERAL_TYPE_CLOCK, get_peripheral_freq, p_driver, error, peripheral_id, p_freq_hz);
    return error;
}

/**
 * @brief Enables the clock gate for a specific peripheral.
 * @param driver Pointer to the Clock driver instance.
 * @param peripheral_id ID or FourCC of the target peripheral.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_clock_enable_gate (cfn_hal_clock_t *p_driver, uint32_t peripheral_id)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_CLOCK, enable_gate, p_driver, error, peripheral_id);
    return error;
}

/**
 * @brief Disables the clock gate for a specific peripheral.
 * @param driver Pointer to the Clock driver instance.
 * @param peripheral_id ID or FourCC of the target peripheral.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t
cfn_hal_clock_disable_gate (cfn_hal_clock_t *p_driver, uint32_t peripheral_id)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_CLOCK, disable_gate, p_driver, error, peripheral_id);
    return error;
}
cfn_hal_error_code_t cfn_hal_clock_construct (cfn_hal_clock_t              *p_driver,
                                              const cfn_hal_clock_config_t *p_config,
                                              const cfn_hal_clock_phy_t    *p_phy,
                                              struct cfn_hal_clock_s       *p_clock,
                                              void                         *p_dependency,
                                              cfn_hal_clock_callback_t      p_callback,
                                              void                         *p_user_arg);
cfn_hal_error_code_t cfn_hal_clock_destruct (cfn_hal_clock_t *p_driver);
#ifdef __cplusplus
}
#endif

#endif // CAFFEINE_HAL_HAL_CLOCK_H
