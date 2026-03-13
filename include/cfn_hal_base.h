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
 * @file cfn_hal_base.h
 * @brief Base driver Hardware Abstraction Layer implementation.
 */

#ifndef CAFFEINE_HAL_HAL_BASE_H
#define CAFFEINE_HAL_HAL_BASE_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------------*/
#include "cfn_hal.h"
#include "cfn_hal_types.h"

/* Types Structs ----------------------------------------------------*/

/**
 * @brief Base API structure for all peripheral drivers.
 * Every peripheral-specific API struct MUST have this as its first member.
 */
typedef struct
{
    cfn_hal_error_code_t (*init)(cfn_hal_driver_t *base);
    cfn_hal_error_code_t (*deinit)(cfn_hal_driver_t *base);

    cfn_hal_error_code_t (*power_state_set)(cfn_hal_driver_t *base, cfn_hal_power_state_t state);
    cfn_hal_error_code_t (*config_set)(cfn_hal_driver_t *base, const void *config);
    cfn_hal_error_code_t (*callback_register)(cfn_hal_driver_t *base, const void *callback, void *user_arg);

    cfn_hal_error_code_t (*event_enable)(cfn_hal_driver_t *base, uint32_t event_mask);
    cfn_hal_error_code_t (*event_disable)(cfn_hal_driver_t *base, uint32_t event_mask);
    cfn_hal_error_code_t (*event_get)(cfn_hal_driver_t *base, uint32_t *event_mask);

    cfn_hal_error_code_t (*error_enable)(cfn_hal_driver_t *base, uint32_t error_mask);
    cfn_hal_error_code_t (*error_disable)(cfn_hal_driver_t *base, uint32_t error_mask);
    cfn_hal_error_code_t (*error_get)(cfn_hal_driver_t *base, uint32_t *error_mask);

#if (CFN_HAL_USE_LOCK == 1)
    cfn_hal_error_code_t (*lock)(cfn_hal_driver_t *base, uint32_t timeout);
    cfn_hal_error_code_t (*unlock)(cfn_hal_driver_t *base);
#endif
} cfn_hal_api_base_t;

/* Functions inline  ---------------------------------------------*/

/**
 * @brief Generic initialization for any driver.
 */
/**
 * @brief Generic initialization for any driver.
 * Handles board-level hooks, type validation, and hardware-specific commitment.
 * @param base Pointer to the base driver structure.
 * @param expected_type FourCC code for peripheral type validation.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_base_init(cfn_hal_driver_t *base, cfn_hal_peripheral_type_t expected_type)
{
    if (!base || base->type != expected_type || !base->vmt)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    if (base->status == CFN_HAL_DRIVER_STATUS_INITIALIZED)
    {
        return CFN_HAL_ERROR_DRIVER_ALREADY_INIT;
    }

    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;

    /* Board-level Initialization Hook (Phase A)
     * This must happen BEFORE the VMT init. It typically handles clock gating
     * and pin muxing via the 'on_config' callback provided by the BSP.
     */
    if (base->on_config)
    {
        error = base->on_config(base, base->on_config_arg, DRIVER_CONFIG_INIT);
        if (error != CFN_HAL_ERROR_OK)
        {
            return error;
        }
    }

    /* Hardware-specific Peripheral Initialization (Phase B)
     * We cast the generic VMT to the base API type to access the init hook.
     */
    const cfn_hal_api_base_t *api = (const cfn_hal_api_base_t *) base->vmt;
    if (api->init)
    {
        error = api->init(base);
    }

    /* 4. Final State Update (Phase C)
     * If all stages passed, we mark the driver as ready for functional use.
     */
    if (error == CFN_HAL_ERROR_OK)
    {
        base->status = CFN_HAL_DRIVER_STATUS_INITIALIZED;
    }
    else if (base->on_config)
    {
        /* Roll back board-level config if hardware init failed */
        (void) base->on_config(base, base->on_config_arg, DRIVER_CONFIG_DEINIT);
    }

    return error;
}

/**
 * @brief Generic deinitialization for any driver.
 * Returns the driver to the CONSTRUCTED state and releases board resources.
 * @param base Pointer to the base driver structure.
 * @param expected_type FourCC code for peripheral type validation.
 * @return CFN_HAL_ERROR_OK on success, or a specific error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_base_deinit(cfn_hal_driver_t *base, cfn_hal_peripheral_type_t expected_type)
{
    if (!base || base->type != expected_type || !base->vmt)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    if (base->status == CFN_HAL_DRIVER_STATUS_CONSTRUCTED)
    {
        return CFN_HAL_ERROR_OK;
    }

    if (base->status == CFN_HAL_DRIVER_STATUS_UNKNOWN)
    {
        return CFN_HAL_ERROR_DRIVER_NOT_INIT;
    }

    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;

    /* Hardware-specific Peripheral Deinitialization (Phase A)
     * We trigger the VMT deinit first while the clocks/pins are still active.
     */
    const cfn_hal_api_base_t *api = (const cfn_hal_api_base_t *) base->vmt;
    if (api->deinit)
    {
        error = api->deinit(base);
    }

    /* Board-level Deinitialization Hook (Phase B)
     * Only after the peripheral logic is stopped do we release clocks and pins.
     * We use DRIVER_CONFIG_DEINIT to signify teardown to the hook.
     */
    if (error == CFN_HAL_ERROR_OK)
    {
        /* Mark as constructed (not live) before releasing clocks to prevent race conditions */
        base->status = CFN_HAL_DRIVER_STATUS_CONSTRUCTED;

        if (base->on_config)
        {
            /* We ignore this return code to ensure we finish the teardown,
               but we propagate the failure if hardware deinit failed above. */
            (void) base->on_config(base, base->on_config_arg, DRIVER_CONFIG_DEINIT);
        }
    }

    return error;
}

/**
 * @brief Generic configuration setter for any driver.
 * @param base Pointer to the base driver structure.
 * @param expected_type FourCC code for peripheral type validation.
 * @param config Pointer to the peripheral-specific configuration structure.
 * @return cfn_hal_error_code_t status code.
 */
static inline cfn_hal_error_code_t
cfn_hal_base_config_set(cfn_hal_driver_t *base, cfn_hal_peripheral_type_t expected_type, const void *config)
{
    if (!base || base->type != expected_type || !config)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;

    /*
     * If the hardware is not yet initialized, we do NOT trigger the VMT.
     * The config has already been "shadowed" (stored) in the peripheral-specific
     * wrapper before calling this base function.
     */
    if (base->status == CFN_HAL_DRIVER_STATUS_INITIALIZED)
    {
        const cfn_hal_api_base_t *api = (const cfn_hal_api_base_t *) base->vmt;
        if (api && api->config_set)
        {
            error = api->config_set(base, config);
        }
        else
        {
            error = CFN_HAL_ERROR_NOT_SUPPORTED;
        }
    }

    return error;
}

/**
 * @brief Generic callback registration for any driver.
 * @param base Pointer to the base driver structure.
 * @param expected_type FourCC code for peripheral type validation.
 * @param cb Pointer to the callback function (type erased to void*).
 * @param user_arg User-defined argument passed to the callback during events.
 * @return cfn_hal_error_code_t status code.
 */
static inline cfn_hal_error_code_t cfn_hal_base_callback_register(cfn_hal_driver_t         *base,
                                                                  cfn_hal_peripheral_type_t expected_type,
                                                                  const void               *callback,
                                                                  void                     *user_arg)
{
    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;

    /*
     * If the hardware is already INITIALIZED, we must notify the VMT.
     * The callback has already been stored in the peripheral-specific
     * This is critical for peripherals that enable/disable interrupts or
     * DMA request lines based on whether a callback is present.
     */
    if (base->status == CFN_HAL_DRIVER_STATUS_INITIALIZED)
    {
        const cfn_hal_api_base_t *api = (const cfn_hal_api_base_t *) base->vmt;

        if (api && api->callback_register)
        {
            error = api->callback_register(base, callback, user_arg);
        }
        else
        {
            error = CFN_HAL_ERROR_NOT_SUPPORTED;
        }
    }

    return error;
}
/**
 * @brief Generic power state transition for any driver.
 * @param base Pointer to the base driver structure.
 * @param expected_type FourCC code for peripheral type validation.
 * @param state The target power state to transition to.
 * @return cfn_hal_error_code_t status code.
 */
static inline cfn_hal_error_code_t
cfn_hal_power_state_set(cfn_hal_driver_t *base, cfn_hal_peripheral_type_t expected_type, cfn_hal_power_state_t state)
{
    if (!base || base->type != expected_type)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    /* No-op if already in the target state */
    if (base->power_state == state)
    {
        return CFN_HAL_ERROR_OK;
    }

    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;

    /*
     * Power states are strictly hardware-active operations.
     * We only trigger the VMT if the driver is INITIALIZED.
     */
    if (base->status == CFN_HAL_DRIVER_STATUS_INITIALIZED)
    {
        const cfn_hal_api_base_t *api = (const cfn_hal_api_base_t *) base->vmt;

        if (api && api->power_state_set)
        {
            error = api->power_state_set(base, state);
            if (error == CFN_HAL_ERROR_OK)
            {
                base->power_state = state; /* Update shadow state on success */
            }
        }
        else
        {
            error = CFN_HAL_ERROR_NOT_SUPPORTED;
        }
    }
    else
    {
        /* If hardware isn't initialized, we just update the shadow intent for when init() is called */
        base->power_state = state;
    }

    return error;
}
/**
 * @brief Generic power state getter.
 * Returns the current power state from the software shadow.
 *
 * @param base Pointer to the base driver structure.
 * @return Current cfn_hal_power_state_t.
 */
static inline cfn_hal_power_state_t cfn_hal_power_state_get(const cfn_hal_driver_t *base)
{
    if (!base)
    {
        return CFN_HAL_POWER_STATE_UNKNOWN;
    }

    return base->power_state;
}

/**
 * @brief Generic event enable for any driver.
 * Activates nominal hardware triggers based on the provided mask.
 *
 * @param base Pointer to the base driver structure.
 * @param expected_type FourCC code for peripheral type validation.
 * @param event_mask Pointer to a peripheral-specific event mask.
 * @return cfn_hal_error_code_t status code.
 */
static inline cfn_hal_error_code_t
cfn_hal_base_event_enable(cfn_hal_driver_t *base, cfn_hal_peripheral_type_t expected_type, uint32_t event_mask)
{
    if (!base || base->type != expected_type || !base->vmt)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    /*
     * Events represent live hardware interrupts. We cannot enable them
     * if the hardware is not initialized.
     */
    if (base->status != CFN_HAL_DRIVER_STATUS_INITIALIZED)
    {
        return CFN_HAL_ERROR_DRIVER_NOT_INIT;
    }

    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;

    const cfn_hal_api_base_t *api = (const cfn_hal_api_base_t *) base->vmt;
    if (api->event_enable)
    {
        error = api->event_enable(base, event_mask);
    }
    else
    {
        error = CFN_HAL_ERROR_NOT_SUPPORTED;
    }

    return error;
}

/**
 * @brief Generic event disable for any driver.
 * Deactivates nominal hardware triggers based on the provided mask.
 *
 * @param base Pointer to the base driver structure.
 * @param expected_type FourCC code for peripheral type validation.
 * @param event_mask Pointer to a peripheral-specific event mask.
 * @return cfn_hal_error_code_t status code.
 */
static inline cfn_hal_error_code_t
cfn_hal_base_event_disable(cfn_hal_driver_t *base, cfn_hal_peripheral_type_t expected_type, uint32_t event_mask)
{
    if (!base || base->type != expected_type || !base->vmt)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    /* State Check
     * Unlike configuration shadowing, event disabling is a hardware-active task.
     * We return an error if the hardware is not initialized.
     */
    if (base->status != CFN_HAL_DRIVER_STATUS_INITIALIZED)
    {
        return CFN_HAL_ERROR_DRIVER_NOT_INIT;
    }

    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;

    const cfn_hal_api_base_t *api = (const cfn_hal_api_base_t *) base->vmt;
    if (api->event_disable)
    {
        error = api->event_disable(base, event_mask);
    }
    else
    {
        error = CFN_HAL_ERROR_NOT_SUPPORTED;
    }

    return error;
}

/**
 * @brief Generic event status getter for any driver.
 * Retrieves the current nominal hardware triggers/flags.
 *
 * @param base Pointer to the base driver structure.
 * @param expected_type FourCC code for peripheral type validation.
 * @param event_mask [out] Pointer to a buffer to receive the event status.
 * @return cfn_hal_error_code_t status code.
 */
static inline cfn_hal_error_code_t
cfn_hal_base_event_get(cfn_hal_driver_t *base, cfn_hal_peripheral_type_t expected_type, uint32_t *event_mask)
{
    if (!base || base->type != expected_type || !event_mask || !base->vmt)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    /* State Check
     * Event status resides in physical registers. If the hardware is not
     * initialized, the status is logically undefined/inaccessible.
     */
    if (base->status != CFN_HAL_DRIVER_STATUS_INITIALIZED)
    {
        return CFN_HAL_ERROR_DRIVER_NOT_INIT;
    }

    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;

    const cfn_hal_api_base_t *api = (const cfn_hal_api_base_t *) base->vmt;
    if (api->event_get)
    {
        error = api->event_get(base, event_mask);
    }
    else
    {
        error = CFN_HAL_ERROR_NOT_SUPPORTED;
    }

    return error;
}
/**
 * @brief Generic error enable for any driver.
 * Activates exception-flow hardware triggers based on the provided mask.
 *
 * @param base Pointer to the base driver structure.
 * @param expected_type FourCC code for peripheral type validation.
 * @param error_mask Pointer to a peripheral-specific error mask.
 * @return cfn_hal_error_code_t status code.
 */
static inline cfn_hal_error_code_t
cfn_hal_base_error_enable(cfn_hal_driver_t *base, cfn_hal_peripheral_type_t expected_type, uint32_t error_mask)
{
    if (!base || base->type != expected_type || !base->vmt)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    /*
     * Errors represent hardware exceptions. We cannot enable them
     * if the hardware is not initialized.
     */
    if (base->status != CFN_HAL_DRIVER_STATUS_INITIALIZED)
    {
        return CFN_HAL_ERROR_DRIVER_NOT_INIT;
    }

    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;

    const cfn_hal_api_base_t *api = (const cfn_hal_api_base_t *) base->vmt;
    if (api->error_enable)
    {
        error = api->error_enable(base, error_mask);
    }
    else
    {
        error = CFN_HAL_ERROR_NOT_SUPPORTED;
    }

    return error;
}

/**
 * @brief Generic error disable for any driver.
 * Deactivates exception-flow hardware triggers based on the provided mask.
 *
 * @param base Pointer to the base driver structure.
 * @param expected_type FourCC code for peripheral type validation.
 * @param error_mask Pointer to a peripheral-specific error mask.
 * @return cfn_hal_error_code_t status code.
 */
static inline cfn_hal_error_code_t
cfn_hal_base_error_disable(cfn_hal_driver_t *base, cfn_hal_peripheral_type_t expected_type, uint32_t error_mask)
{
    if (!base || base->type != expected_type || !base->vmt)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    /*
     * Hardware must be live to modify error interrupt enable bits.
     */
    if (base->status != CFN_HAL_DRIVER_STATUS_INITIALIZED)
    {
        return CFN_HAL_ERROR_DRIVER_NOT_INIT;
    }

    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;

    const cfn_hal_api_base_t *api = (const cfn_hal_api_base_t *) base->vmt;
    if (api->error_disable)
    {
        error = api->error_disable(base, error_mask);
    }
    else
    {
        error = CFN_HAL_ERROR_NOT_SUPPORTED;
    }

    return error;
}
/**
 * @brief Generic error status getter for any driver.
 * Retrieves current exception-flow hardware flags/errors.
 *
 * @param base Pointer to the base driver structure.
 * @param expected_type FourCC code for peripheral type validation.
 * @param error_mask [out] Pointer to a buffer to receive the error status.
 * @return cfn_hal_error_code_t status code.
 */
static inline cfn_hal_error_code_t
cfn_hal_base_error_get(cfn_hal_driver_t *base, cfn_hal_peripheral_type_t expected_type, uint32_t *error_mask)
{
    if (!base || base->type != expected_type || !error_mask || !base->vmt)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    /*
     * Error registers are part of the physical peripheral.
     * If not initialized, the state is logically inaccessible.
     */
    if (base->status != CFN_HAL_DRIVER_STATUS_INITIALIZED)
    {
        return CFN_HAL_ERROR_DRIVER_NOT_INIT;
    }

    cfn_hal_error_code_t error = CFN_HAL_ERROR_OK;

    const cfn_hal_api_base_t *api = (const cfn_hal_api_base_t *) base->vmt;
    if (api->error_get)
    {
        error = api->error_get(base, error_mask);
    }
    else
    {
        error = CFN_HAL_ERROR_NOT_SUPPORTED;
    }

    return error;
}

#if (CFN_HAL_USE_LOCK == 1)
/**
 * @brief Type-validated concurrency lock.
 * @param base Pointer to the base driver structure.
 * @param expected_type FourCC code for peripheral type validation.
 * @param timeout Lock acquisition timeout in milliseconds.
 * @return CFN_HAL_ERROR_OK on success, or an error code on failure.
 */
static inline cfn_hal_error_code_t
cfn_hal_base_lock(cfn_hal_driver_t *base, uint32_t timeout)
{
    if (!base || !base->vmt)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    const cfn_hal_api_base_t *api = (const cfn_hal_api_base_t *) base->vmt;
    if (!api->lock)
    {
        return CFN_HAL_ERROR_OK; /* Safe bare-metal fallback */
    }

    return api->lock(base, timeout);
}

/**
 * @brief Type-validated concurrency unlock.
 * @param base Pointer to the base driver structure.
 * @param expected_type FourCC code for peripheral type validation.
 * @return CFN_HAL_ERROR_OK on success, or an error code on failure.
 */
static inline cfn_hal_error_code_t cfn_hal_base_unlock(cfn_hal_driver_t *base)
{
    if (!base|| !base->vmt)
    {
        return CFN_HAL_ERROR_BAD_PARAM;
    }

    const cfn_hal_api_base_t *api = (const cfn_hal_api_base_t *) base->vmt;
    if (!api->unlock)
    {
        return CFN_HAL_ERROR_OK; /* Safe bare-metal fallback */
    }

    return api->unlock(base);
}
#endif

#ifdef __cplusplus
}
#endif

#endif // CAFFEINE_HAL_HAL_BASE_H
