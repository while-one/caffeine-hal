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
 * @file cfn_hal_irq.h
 * @brief Generic Interrupt Controller HAL API.
 */

#ifndef CAFFEINE_HAL_HAL_IRQ_H
#define CAFFEINE_HAL_HAL_IRQ_H

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
 * @brief IRQ nominal event flags.
 */
typedef enum
{
    CFN_HAL_IRQ_EVENT_NONE = 0,
} cfn_hal_irq_event_t;

/**
 * @brief IRQ exception error flags.
 */
typedef enum
{
    CFN_HAL_IRQ_ERROR_NONE = 0,
    CFN_HAL_IRQ_ERROR_GENERAL = CFN_HAL_BIT(0), /*!< Controller fault or access error */
} cfn_hal_irq_error_t;

/* Types Structs ----------------------------------------------------*/

/**
 * @brief IRQ configuration structure.
 */
typedef struct
{
    void *user_config; /*!< Vendor-specific controller configuration */
} cfn_hal_irq_config_t;

/**
 * @brief IRQ hardware physical mapping.
 */
typedef struct
{
    void *instance; /*!< Peripheral base instance */
    void *user_arg; /*!< Peripheral instance user argument */
} cfn_hal_irq_phy_t;

typedef struct cfn_hal_irq_s     cfn_hal_irq_t;
typedef struct cfn_hal_irq_api_s cfn_hal_irq_api_t;

/**
 * @brief IRQ callback signature.
 * @param driver Pointer to the IRQ driver instance.
 * @param event_mask Mask of triggered nominal events.
 * @param error_mask Mask of triggered exception errors.
 * @param user_arg User-defined argument passed during registration.
 */
typedef void (*cfn_hal_irq_callback_t)(cfn_hal_irq_t *driver, uint32_t event_mask, uint32_t error_mask, void *user_arg);

/**
 * @brief IRQ Virtual Method Table (VMT).
 */
struct cfn_hal_irq_api_s
{
    cfn_hal_api_base_t base;

    /* IRQ Specific Extensions */
    cfn_hal_error_code_t (*global_enable)(cfn_hal_irq_t *driver);
    cfn_hal_error_code_t (*global_disable)(cfn_hal_irq_t *driver);
    cfn_hal_error_code_t (*enable_vector)(cfn_hal_irq_t *driver, uint32_t irq_id);
    cfn_hal_error_code_t (*disable_vector)(cfn_hal_irq_t *driver, uint32_t irq_id);
    cfn_hal_error_code_t (*set_priority)(cfn_hal_irq_t *driver, uint32_t irq_id, uint32_t priority);
    cfn_hal_error_code_t (*clear_pending)(cfn_hal_irq_t *driver, uint32_t irq_id);
};
CFN_HAL_VMT_CHECK(struct cfn_hal_irq_api_s);

CFN_HAL_CREATE_DRIVER_TYPE(irq, cfn_hal_irq_config_t, cfn_hal_irq_api_t, cfn_hal_irq_phy_t, cfn_hal_irq_callback_t);

#define CFN_HAL_IRQ_INITIALIZER(api_ptr, phy_ptr, config_ptr)                                                          \
    CFN_HAL_DRIVER_INITIALIZER(CFN_HAL_PERIPHERAL_TYPE_IRQ, api_ptr, phy_ptr, config_ptr)

/* Functions inline ------------------------------------------------- */

/**
 * @brief Initializes the IRQ controller driver.
 * @param driver Pointer to the IRQ driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_irq_init(cfn_hal_irq_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    driver->base.vmt = (const void *) driver->api;
    return cfn_hal_base_init(&driver->base, CFN_HAL_PERIPHERAL_TYPE_IRQ);
}

/**
 * @brief Deinitializes the IRQ controller driver.
 * @param driver Pointer to the IRQ driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_irq_deinit(cfn_hal_irq_t *driver)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_deinit(&driver->base, CFN_HAL_PERIPHERAL_TYPE_IRQ);
}

/**
 * @brief Sets the IRQ configuration.
 * @param driver Pointer to the IRQ driver instance.
 * @param config Pointer to the configuration structure.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_irq_config_set(cfn_hal_irq_t *driver, const cfn_hal_irq_config_t *config)
{
    if (driver)
    {
        driver->config = config;
    }
    return cfn_hal_base_config_set(&driver->base, CFN_HAL_PERIPHERAL_TYPE_IRQ, (const void *) config);
}

/**
 * @brief Gets the current IRQ configuration.
 * @param driver Pointer to the IRQ driver instance.
 * @param config [out] Pointer to store the configuration.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_irq_config_get(cfn_hal_irq_t *driver, cfn_hal_irq_config_t *config)
{
    if (!driver || !config || !driver->config)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    *config = *(driver->config);
    return CFN_HAL_ERROR_OK;
}

/**
 * @brief Registers a callback for IRQ events and errors.
 * @param driver Pointer to the IRQ driver instance.
 * @param callback The callback function to register.
 * @param user_arg User-defined argument passed to the callback.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_irq_callback_register(cfn_hal_irq_t               *driver,
                                                                  const cfn_hal_irq_callback_t callback,
                                                                  void                        *user_arg)
{
    if (driver)
    {
        driver->cb = callback;
        driver->cb_user_arg = user_arg;
    }
    return cfn_hal_base_callback_register(
        &driver->base, CFN_HAL_PERIPHERAL_TYPE_IRQ, (cfn_hal_callback_t) callback, user_arg);
}

/**
 * @brief Sets the IRQ power state.
 * @param driver Pointer to the IRQ driver instance.
 * @param state Target power state.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_irq_power_state_set(cfn_hal_irq_t *driver, cfn_hal_power_state_t state)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_power_state_set(&driver->base, CFN_HAL_PERIPHERAL_TYPE_IRQ, state);
}

/**
 * @brief Enables one or more IRQ nominal events.
 * @param driver Pointer to the IRQ driver instance.
 * @param event_mask Mask of events to enable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_irq_event_enable(cfn_hal_irq_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_enable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_IRQ, event_mask);
}

/**
 * @brief Disables one or more IRQ nominal events.
 * @param driver Pointer to the IRQ driver instance.
 * @param event_mask Mask of events to disable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_irq_event_disable(cfn_hal_irq_t *driver, uint32_t event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_disable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_IRQ, event_mask);
}

/**
 * @brief Retrieves the current IRQ nominal event status.
 * @param driver Pointer to the IRQ driver instance.
 * @param event_mask [out] Pointer to store the event mask.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_irq_event_get(cfn_hal_irq_t *driver, uint32_t *event_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_event_get(&driver->base, CFN_HAL_PERIPHERAL_TYPE_IRQ, event_mask);
}

/**
 * @brief Enables one or more IRQ exception errors.
 * @param driver Pointer to the IRQ driver instance.
 * @param error_mask Mask of errors to enable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_irq_error_enable(cfn_hal_irq_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_enable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_IRQ, error_mask);
}

/**
 * @brief Disables one or more IRQ exception errors.
 * @param driver Pointer to the IRQ driver instance.
 * @param error_mask Mask of errors to disable.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_irq_error_disable(cfn_hal_irq_t *driver, uint32_t error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_disable(&driver->base, CFN_HAL_PERIPHERAL_TYPE_IRQ, error_mask);
}

/**
 * @brief Retrieves the current IRQ exception error status.
 * @param driver Pointer to the IRQ driver instance.
 * @param error_mask [out] Pointer to store the error mask.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_irq_error_get(cfn_hal_irq_t *driver, uint32_t *error_mask)
{
    if (!driver)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }
    return cfn_hal_base_error_get(&driver->base, CFN_HAL_PERIPHERAL_TYPE_IRQ, error_mask);
}

/* IRQ Specific Functions ------------------------------------------- */

/**
 * @brief Enables interrupts globally at the processor level.
 * @param driver Pointer to the IRQ driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_irq_global_enable(cfn_hal_irq_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_IRQ, global_enable, driver, error);
    return error;
}

/**
 * @brief Disables interrupts globally at the processor level.
 * @param driver Pointer to the IRQ driver instance.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_irq_global_disable(cfn_hal_irq_t *driver)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC(CFN_HAL_PERIPHERAL_TYPE_IRQ, global_disable, driver, error);
    return error;
}

/**
 * @brief Enables a specific interrupt vector.
 * @param driver Pointer to the IRQ driver instance.
 * @param irq_id Numeric identifier of the target interrupt.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_irq_enable_vector(cfn_hal_irq_t *driver, uint32_t irq_id)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_IRQ, enable_vector, driver, error, irq_id);
    return error;
}

/**
 * @brief Disables a specific interrupt vector.
 * @param driver Pointer to the IRQ driver instance.
 * @param irq_id Numeric identifier of the target interrupt.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_irq_disable_vector(cfn_hal_irq_t *driver, uint32_t irq_id)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_IRQ, disable_vector, driver, error, irq_id);
    return error;
}

/**
 * @brief Sets the priority level for a specific interrupt vector.
 * @param driver Pointer to the IRQ driver instance.
 * @param irq_id Numeric identifier of the target interrupt.
 * @param priority Target priority level (architecture dependent).
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_irq_set_priority(cfn_hal_irq_t *driver, uint32_t irq_id, uint32_t priority)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_IRQ, set_priority, driver, error, irq_id, priority);
    return error;
}

/**
 * @brief Clears the pending status of a specific interrupt vector.
 * @param driver Pointer to the IRQ driver instance.
 * @param irq_id Numeric identifier of the target interrupt.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
CFN_HAL_INLINE cfn_hal_error_code_t cfn_hal_irq_clear_pending(cfn_hal_irq_t *driver, uint32_t irq_id)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;
    CFN_HAL_CHECK_AND_CALL_FUNC_VARG(CFN_HAL_PERIPHERAL_TYPE_IRQ, clear_pending, driver, error, irq_id);
    return error;
}

#ifdef __cplusplus
}
#endif

#endif // CAFFEINE_HAL_HAL_IRQ_H
